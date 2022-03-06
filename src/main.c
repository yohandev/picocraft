#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

#include "util.h"
#include "st7735.h"
#include "test_img.h"

#define WIDTH ST7735_WIDTH
#define HEIGHT ST7735_HEIGHT

LCD lcd;

int main() {
    stdio_init_all();

    lcd_init(&lcd);

    u16 img[WIDTH * HEIGHT];
    // Generate red/blue linear gradient
    for (usize i = 0; i < WIDTH * HEIGHT; i++) {
        // (!!!) Framebuffer is column major
        u16 x = i / HEIGHT;
        u16 y = i % HEIGHT;
        // 0..160 -> 0..32(R)
        u16 u = (x * 31) / WIDTH;
        // 0..128 -> 0..32(B)
        u16 v = (y * 31) / HEIGHT;

        img[i] = (u << 11) | (v);
    }
    // Flip endianess(TODO use PIO)
    for (usize i = 0; i < WIDTH * HEIGHT; i++) {
        img[i] = (img[i] >> 8) | (img[i] << 8);
        TEST_IMAGE[i] = (TEST_IMAGE[i] >> 8) | (TEST_IMAGE[i] << 8);
    }

    while (true) {
        lcd_draw(&lcd, TEST_IMAGE);
        sleep_ms(500);
        lcd_draw(&lcd, img);
        sleep_ms(500);
    }
    while (true);
    return 0;
}