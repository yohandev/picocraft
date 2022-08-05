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
    // frame_fill((rgb565){.r=0, .g=5, .b=0});
    memcpy(frame, TEST_BMP, WIDTH*HEIGHT*sizeof(rgb565));
    while (true) {
        frame_draw_line(
            (vec2){.x=fixed_from_i32(10), .y=fixed_from_i32(10)},
            (vec2){.x=fixed_from_i32(200), .y=fixed_from_i32(100)},
            (rgb565){.r=0, .g=63, .b=31}
        );
        st7789_draw(&lcd, frame);
    }

    st7789_drop(&lcd);
}