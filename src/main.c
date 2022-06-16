#include "hardware/gpio.h"

#include "st7789.h"

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
rgb565 frame[WIDTH*HEIGHT] = {0};

int main() {
    st7789_init(&lcd);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // Draw
    while (true) {
        st7789_draw(&lcd, frame);

        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(100);
        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(100);
    }

    st7789_drop(&lcd);
    return 0;
}