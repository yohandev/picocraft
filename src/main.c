#include "hardware/gpio.h"

#include "st7789.h"
#include "draw.h"
#include "test.bmp.h"

ST7789 lcd = {
    .width=WIDTH, .height=HEIGHT,
    .sck=18, .mosi=19, .dc=3, .rst=2,
    .spi=spi0, .baudrate=125000000,
};
rgb565 frame[WIDTH*HEIGHT] = {0};

int main() {
    st7789_init(&lcd);

    // Draw
    frame_fill(rgb(0, 100, 150));
    frame_copy(TEST_BMP);
    while (true) {
        frame_draw_line(vec2(0, 3), vec2(100, 3), rgb(255, 0, 255));
        st7789_draw(&lcd, frame);
    }
    st7789_drop(&lcd);
}