#include <stdio.h>
#include "pico/stdlib.h"

#include "st7789.h"

#define WIDTH 135
#define HEIGHT 240

st7789 lcd = {
    .width=WIDTH, .height=HEIGHT,
    .sck=2, .mosi=3, .dc=6, .rst=7,
    .spi=spi0, .baudrate=31250000,
};
rgb16 frame[WIDTH*HEIGHT] = {0};

int main() {
    stdio_init_all();
    st7789_init(&lcd);

    // Fill frame buffer with red
    for (usize i = 0; i < WIDTH*HEIGHT; i++) {
        frame[i] = (rgb16){ .r=0, .g=63, .b=31 };
    }

    // Draw
    st7789_draw(&lcd, (u8*)frame);

    while (true) {

    }

    st7789_drop(&lcd);
    return 0;
}