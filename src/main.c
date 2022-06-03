#include <stdio.h>
#include "pico/stdlib.h"

#include "st7789.h"
#include "texture.h"

#define WIDTH 240
#define HEIGHT 135

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
        
    }

    st7789_drop(&lcd);
    texture_drop(&frame);
    return 0;
}