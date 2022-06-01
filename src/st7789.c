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

    st7789_reset(self);
}

void st7789_drop(st7789* self) {
    spi_deinit(self->spi);
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
    gpio_put(self->dc, 1);
    spi_set_format(self->spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    spi_write16_blocking(self->spi, (u8*)buf, self->width * self->height);
}