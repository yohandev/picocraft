#include "st7789.h"

void st7789_init(st7789* self) {
    spi_init(self->spi, self->baudrate);
    spi_set_format(self->spi, 8, SPI_CPOL_1, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_set_function(self->sck, GPIO_FUNC_SPI);
    gpio_set_function(self->mosi, GPIO_FUNC_SPI);

    gpio_init(self->dc);
    gpio_init(self->rst);
    gpio_set_dir(self->dc, GPIO_OUT);
    gpio_set_dir(self->rst, GPIO_OUT);

    self->dma = dma_claim_unused_channel(true);
    self->dma_cfg = dma_channel_get_default_config(self->dma);

    channel_config_set_transfer_data_size(&self->dma_cfg, DMA_SIZE_16);
    channel_config_set_dreq(&self->dma_cfg, spi_get_dreq(self->spi, true));

    st7789_reset(self);
}

void st7789_drop(st7789* self) {
    spi_deinit(self->spi);
    dma_channel_unclaim(self->dma);
}

void st7789_write(st7789* self, const u8* cmd, usize argc) {
    // 8-bit SPI
    // spi_set_format(self->spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);

    // Command
    gpio_put(self->dc, 0);
    spi_write_blocking(self->spi, cmd, 1);
    // Data
    if (argc) {
        gpio_put(self->dc, 1);
        spi_write_blocking(self->spi, ++cmd, argc);
    }
}

void st7789_reset(st7789* self) {
    // [argc], [sleep(ms * 10)], [cmd], [argv...]
    u8 inst[] = {
        // Software reset
        0,  15, ST7789_SWRESET,
        // Exit sleep mode
        0,   0, ST7789_SLPOUT,
        // Color mode
        1,   5, ST7789_COLMOD, (ST7789_COLMOD_65K | ST7789_COLMOD_16BIT),
        // Rotation(landscape)
        1,   0, ST7789_MADCTL, (ST7789_MADCTL_RGB | ST7789_MADCTL_MV),
        // Inversion mode
        0,   1, ST7789_INVON,
        // Normal mode
        0,   1, ST7789_NORON,
        // Column address set
        4,   0, ST7789_CASET, 0, 40, (self->width + 40 - 1) >> 8, (self->width + 40 - 1) & 0xFF,
        // Row address set
        4,   0, ST7789_RASET, 0, 53, (self->height + 53 - 1) >> 8, (self->height + 53 - 1) & 0xFF,
        // Display on
        0,  50, ST7789_DISPON,
        // Memory write
        0,   0, ST7789_RAMWR,
    };

    // Hard reset
    gpio_put(self->rst, 1);
    sleep_ms(50);
    gpio_put(self->rst, 0);
    sleep_ms(50);
    gpio_put(self->rst, 1);
    sleep_ms(150);

    // Soft reset
    for (usize i = 0; i < sizeof(inst); i += inst[i] + 3) {
        st7789_write(self, &inst[i + 2], inst[i]);
        sleep_ms(inst[i + 1] * 10);
    }
}

void st7789_draw(st7789* self, const rgb16* buf) {
    // Data/Command -> Command
    gpio_put(self->dc, 1);
    // SPI -> 16 bit
    spi_set_format(self->spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // Await DMA
    while (dma_channel_is_busy(self->dma));
    // Await SPI FIFO
    while (spi_get_hw(self->spi)->sr & SPI_SSPSR_BSY_BITS);

    // Write: DMA -> SPI -> LCD!
    dma_channel_configure(self->dma, &self->dma_cfg,
        // Write addr
        &spi_get_hw(self->spi)->dr,
        // Read addr
        (u8*)buf,
        // Number of transfers
        self->width * self->height,
        // Start immediately
        true
    );

    // spi_write16_blocking(self->spi, (u8*)buf, self->width * self->height);
}