#include "st7735.h"

void lcd_init(LCD* self) {
    // Initialize SPI at 16MHz
    spi_init(SPI_PORT, 16000000);

    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);

    // RST pin
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    // DC pin
    gpio_init(PIN_DC);
    gpio_set_dir(PIN_DC, GPIO_OUT);

    // Claim DMA channel
    self->dma = dma_claim_unused_channel(true);
    self->dma_cfg = dma_channel_get_default_config(self->dma);
    // Transfer to SPI's TX FIFO 8 bits at a time
    channel_config_set_transfer_data_size(&self->dma_cfg, DMA_SIZE_8);
    // Transmit speed managed by SPI's DREQ
    channel_config_set_dreq(&self->dma_cfg, spi_get_dreq(SPI_PORT, true));

    // First reset
    lcd_reset(self);
}

void lcd_drop(LCD* self) {
    // TODO
}

void lcd_write_command(LCD* self, const u8* cmd, usize argc) {
    // Command
    gpio_put(PIN_DC, 0);
    spi_write_blocking(SPI_PORT, cmd, 1);
    // Arguments
    if (argc) {
        gpio_put(PIN_DC, 1);
        spi_write_blocking(SPI_PORT, ++cmd, argc);
    }
}

void lcd_reset(LCD* self) {
    // Reset instruction buffer
    // [argc], [sleep(ms)], [cmd], [argv...]
    static const u8 buf[] = {
        // Software reset
        0, 150, ST7735_SWRESET,
        // Exit sleep mode
        0, 255, ST7735_SLPOUT,
        // Frame rate control(normal mode)
        3,   0, ST7735_FRMCTR1, 0x01, 0x2C, 0x2D,
        // Frame rate control(idle mode)
        3,   0, ST7735_FRMCTR2, 0x01, 0x2C, 0x2D,
        // Frame rate control(partial mode)
        6,   0, ST7735_FRMCTR3, 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D,
        // Inversion control(none)
        1,   0, ST7735_INVCTR, 0x07,
        // Power control(-4.6V, AUTO mode)
        3,   0, ST7735_PWCTR1, 0xA2, 0x02, 0x84,
        // Power control
        1,   0, ST7735_PWCTR2, 0xC5,
        // Power control
        2,   0, ST7735_PWCTR3, 0x0A, 0x00,
        // Power control
        2,   0, ST7735_PWCTR4, 0x8A, 0x2A,
        // Power control
        2,   0, ST7735_PWCTR5, 0x8A, 0xEE,
        // Power control
        1,   0, ST7735_VMCTR1, 0x0E,
        // Inversion off
        0,   0, ST7735_INVOFF,
        // Write direction(top-left origin(0x40 works too), RGB)
        1,   0, ST7735_MADCTL, 0x80,
        // Set color mode(16-bit)
        1,  10, ST7735_COLMOD, 0x05,
        // Colum address set(x: 0..=159)
        4,   0, ST7735_CASET, 0x00, 0x00, 0x00, 0x7F,
        // Row address set(y: 0..=127)
        4,   0, ST7735_RASET, 0x00, 0x00, 0x00, 0x9F,
        // ???
        16,  0, ST7735_GMCTRP1, 0x02, 0x1c, 0x07, 0x12,
                                0x37, 0x32, 0x29, 0x2d,
                                0x29, 0x25, 0x2B, 0x39,
                                0x00, 0x01, 0x03, 0x10,
        // ???
        16, 0, ST7735_GMCTRN1,  0x03, 0x1d, 0x07, 0x06,
                                0x2E, 0x2C, 0x29, 0x2D,
                                0x2E, 0x2E, 0x37, 0x3F,
                                0x00, 0x00, 0x02, 0x10,
        // Normal display on
        0,  10, ST7735_NORON,
        // Screen on
        0, 100, ST7735_DISPON,
    };

    // Hard reset
    gpio_put(PIN_RST, 1);
    sleep_ms(10);
    gpio_put(PIN_RST, 0);
    sleep_ms(10);
    gpio_put(PIN_RST, 1);

    // Reset software
    for (usize i = 0; i < sizeof(buf); i += buf[i] + 3) {
        lcd_write_command(self, &buf[i + 2], buf[i]);
        if (buf[i + 1]) {
            sleep_ms(buf[i + 1]);
        }
    }
}

void lcd_draw(LCD* self, const u16* buf) {
    u8 cmd = ST7735_RAMWR;

    // Await DMA
    while (dma_channel_is_busy(self->dma));
    // Await SPI FIFO to flush
    while(spi_get_hw(SPI_PORT)->sr & SPI_SSPSR_BSY_BITS);
    // hw_write_masked(&spi_get_hw(SPI_PORT)->cr0, (16 - 1) << SPI_SSPCR0_DSS_LSB, SPI_SSPCR0_DSS_BITS);

    // Data/Command -> Command(Memory access)
    lcd_write_command(self, &cmd, 0);
    // Data/Command -> Data
    gpio_put(PIN_DC, 1);

    // Start DMA transfer
    dma_channel_configure(self->dma, &self->dma_cfg,
        // Write addr
        &spi_get_hw(SPI_PORT)->dr,
        // Read addr
        (u8*)buf,
        // Number of transfers
        ST7735_WIDTH * ST7735_HEIGHT * 2,
        // Start immediately
        true);

    // // Correct endianess on per-pixel basis(TODO do this in PIO) 
    // for (usize i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++) {
    //     u8 a = buf[i] >> 8;
    //     u8 b = buf[i] & 0xff;
    //     spi_write_blocking(SPI_PORT, &a, 1);
    //     spi_write_blocking(SPI_PORT, &b, 1);
    // }
}