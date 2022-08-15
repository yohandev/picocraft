#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/regs/rosc.h"
#include "hardware/regs/addressmap.h"

#include "st7789.h"
#include "draw.h"
#include "fmt.h"
#include "test.bmp.h"
#include "world.h"

ST7789 lcd = {
    .width=WIDTH, .height=HEIGHT,
    .sck=18, .mosi=19, .dc=3, .rst=2,
    .spi=spi0, .baudrate=125000000,
};
rgb565 frame[WIDTH*HEIGHT] = {0};

World world;

const ivec3 BENCH_POS[CHUNK_SLOTS] = {
    {-16, -16, -16}, {0, -16, -16}, {16, -16, -16},
    {-16, 0, -16}, {0, 0, -16}, {16, 0, -16},
    {-16, 16, -16}, {0, 16, -16}, {16, 16, -16},
    {-16, -16, 0}, {0, -16, 0}, {16, -16, 0},
    {-16, 0, 0}, {0, 0, 0}, {16, 0, 0},
    {-16, 16, 0}, {0, 16, 0}, {16, 16, 0},
    {-16, -16, 16}, {0, -16, 16}, {16, -16, 16},
    {-16, 0, 16}, {0, 0, 16}, {16, 0, 16},
    {-16, 16, 16}, {0, 16, 16}, {16, 16, 16},
    {32, 0, 0}, {32, 32, 0}, {32, 32, 32},
    {32, -32, 0}, {64, 0, 0},
};

// Get an `n` bits random number
usize randn(usize n) {
    volatile usize* reg = (usize*)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);

    usize out = 0;
    while (n--) {
        out = (out << 1) | (*reg & 1);
    }
    return out;
}

void benchmark(void) {
    #define NUM_TESTS 1000000

    world_init(&world);

    for (usize i = 0; i < CHUNK_SLOTS; i++) {
        world_load(&world, BENCH_POS[i]);
    }

    // Bench array lookup
    u32 time = time_us_32();
    for (usize i = 0; i < NUM_TESTS; i++) {
        world_get_chunk(&world, BENCH_POS[randn(5)]);
    }
    time = time_us_32() - time;

    print("{u32} lookups: {u32}us", (u32)NUM_TESTS, time);
}

int main() {
    stdout_init();
    st7789_init(&lcd);

    // Draw
    frame_fill(rgb(0, 100, 150));
    frame_copy((rgb565*)TEST_BMP);
    st7789_draw(&lcd, frame);

    // Bench
    while (true) {
        benchmark();
        sleep_ms(16);
    }
    st7789_drop(&lcd);
}