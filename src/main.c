#include <stdio.h>
#include "pico/stdlib.h"

#include "st7789.h"
#include "texture.h"

#define WIDTH 240
#define HEIGHT 135

const rgb565 COLORS[] = {
    { .r=31, .g=63, .b=31 }, // White
    { .r=31, .g=0,  .b=0 },  // Red
    { .r=0,  .g=63, .b=0 },  // Green
    { .r=0,  .g=0,  .b=31 }, // Blue
    { .r=31, .g=0,  .b=31 }, // Purple
    { .r=31, .g=63, .b=0 },  // Yellow
    { .r=0,  .g=63, .b=31 }, // Cyan
    { .r=0,  .g=0,  .b=0 },  // Black
};

ST7789 lcd = {
    .width=WIDTH, .height=HEIGHT,
    .sck=2, .mosi=3, .dc=6, .rst=7,
    .spi=spi0, .baudrate=125000000,
};
Texture frame = {
    .width=WIDTH, .height=HEIGHT
};

int main() {
    stdio_init_all();
    st7789_init(&lcd);
    texture_init(&frame);

    // Draw
    while (true) {
        for (usize i = 0; i < 8; i++) {
            texture_fill(&frame, COLORS[i]);
            st7789_draw(&lcd, frame.buf);
            sleep_ms(100);
        }
        printf("boop\n");
    }

    st7789_drop(&lcd);
    texture_drop(&frame);
    return 0;
}