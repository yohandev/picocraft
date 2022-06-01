#include <stdio.h>
#include "pico/stdlib.h"

#include "st7789.h"
#include "bigsur.h"

#define WIDTH 240
#define HEIGHT 135

st7789 lcd = {
    .width=WIDTH, .height=HEIGHT,
    .sck=2, .mosi=3, .dc=6, .rst=7,
    .spi=spi0, .baudrate=125000000,
};
rgb16 frame[WIDTH*HEIGHT] = {0};

int main() {
    stdio_init_all();
    st7789_init(&lcd);

    // Draw
    while (true) {
        st7789_draw(&lcd, (rgb16*)bigsur_bmp);
        sleep_ms(500);
        st7789_draw(&lcd, frame);
        sleep_ms(500);
    }

    st7789_drop(&lcd);
    return 0;
}