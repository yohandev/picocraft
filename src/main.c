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

const ivec3 BENCH_POS[MAX_CHUNKS] = {
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

typedef struct {
    ivec3 pos;
} MockChunk;

struct {
    MockChunk chunks[MAX_CHUNKS];
    // Marks chunks that are free
    enum {
        FREE,
        OCCUPIED,
    } slots[MAX_CHUNKS];
} world;

inline usize hash_ivec3(ivec3 v) {
    u32 hash = 0;
    // A few assumptions to make hashing better:
    // 1. Index needs to fit in 5 bits(32 slots)
    // 2. Chunks loaded will usually be close to each other,
    //    so their least significant position bits matter
    // 3. Chunk positions are aligned to CHUNK_SIZE, so their
    //    lower 3 position bits can be discarded
    // hash ^= (v.x >> 3) * 0x633767;
    // hash ^= (v.y >> 3) * 0x2634bf;
    // hash ^= (v.z >> 3) * 0x5aa6a3;
    // hash ^= (v.x >> 4);
    // hash |= (v.y >> 3) & 0b00110;
    // hash |= (v.z >> 4) & 0b1;

    return hash;
}

static usize collisions = 0;

bool hash_insert(MockChunk chunk) {
    usize idx = hash_ivec3(chunk.pos);

    // print("{u32}", (u32)idx & (MAX_CHUNKS - 1));

    // Linear probing
    usize j = MAX_CHUNKS;
    for (
        idx &= (MAX_CHUNKS - 1);
        world.slots[idx] != FREE && j--;
        idx = (idx + 1) % MAX_CHUNKS
    ) {
        // Already in collection
        if (cmp(world.chunks[idx].pos, ==, chunk.pos)) {
            return true;
        }
        collisions += 1;
    }
    // All slots occupied
    if (!j) return false;

    world.chunks[idx] = chunk;
    world.slots[idx] = OCCUPIED;

    return true;
}

MockChunk* hash_lookup(ivec3 pos) {
    usize idx = hash_ivec3(pos);

    // Linear probing
    usize j = MAX_CHUNKS;
    for (
        idx &= (MAX_CHUNKS - 1);
        world.slots[idx] != FREE && j--;
        idx = (idx + 1) % MAX_CHUNKS   
    ) {
        // Found
        if (cmp(world.chunks[idx].pos, ==, pos)) {
            return &world.chunks[idx];
        }
    }
    return NULL;
}
MockChunk* array_lookup(ivec3 pos) {
    for (usize i = 0; i < MAX_CHUNKS; i++) {
        if (world.slots[i] != FREE && cmp(world.chunks[i].pos, ==, pos)) {
            return &world.chunks[i];
        }
    }
    return NULL;
}

usize random_num(usize bits) {
    volatile usize* reg = (usize*)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);

    usize out = 0;
    while (bits--) {
        out = (out << 1) | (*reg & 1);
    }
    return out;
}

void benchmark(void) {
    #define NUM_TESTS 10000

    for (usize i = 0; i < MAX_CHUNKS; i++) {
        world.slots[i] = OCCUPIED;
        world.chunks[i] = (MockChunk){BENCH_POS[i]};
    }
    // Bench array lookup
    u32 array_time = time_us_32();
    for (usize i = 0; i < NUM_TESTS; i++) {
        array_lookup(BENCH_POS[random_num(5)]);
    }
    array_time = time_us_32() - array_time;

    // Reset mock world
    collisions = 0;
    for (usize i = 0; i < MAX_CHUNKS; i++) world.slots[i] = FREE;
    for (usize i = 0; i < MAX_CHUNKS; i++) {
        if (!hash_insert((MockChunk){BENCH_POS[i]})) {
            print("insertion failed!");
        }
    }

    // Bench hash lookup
    u32 hash_time = time_us_32();
    for (usize i = 0; i < NUM_TESTS; i++) {
        hash_lookup(BENCH_POS[random_num(5)]);
    }
    hash_time = time_us_32() - hash_time;

    print("hash collisions: {u32}", (u32)collisions);
    print("linear lookup: {u32} | hash lookup: {u32}", array_time, hash_time);
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