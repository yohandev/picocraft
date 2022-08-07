/**
 * [num.h] Provides numeric type definitions and common
 * math functions.
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

typedef size_t usize;
// 8-bit
typedef uint8_t u8;
typedef int8_t i8;
// 16-bit
typedef uint16_t u16;
typedef int16_t i16;
// 32-bit
typedef uint32_t u32;
typedef int32_t i32;
// 64-bit
typedef uint64_t u64;
typedef int64_t i64;
// Floating point
typedef float f32;
typedef double f64;

// 24.8 fixed point, ARM implementation based on the paper:
// https://developer.arm.com/documentation/dai0033/a/
typedef struct {
    i32 _v[1];
} fixed;

// 2D vector of `fixed`
typedef union {
    struct {
        fixed x;
        fixed y;
    };
    i32 _v[2];
} vec2;
// Fixed 3D vector
typedef union {
    struct {
        fixed x;
        fixed y;
        fixed z;
    };
    i32 _v[3];
} vec3;

// RGB565
typedef union {
    struct {
        u16 b : 5;
        u16 g : 6;
        u16 r : 5;
    };
    u16 rgb;
} rgb565;

// RGBA32
typedef union {
    struct {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };
    u32 rgba;
} rgba32;

#define FIXED_FRAC_BITS 8

#define fixed(x) _Generic((x),                      \
    int: ((fixed){((i32)(x) << FIXED_FRAC_BITS)}),  \
    i32: ((fixed){((i32)(x) << FIXED_FRAC_BITS)}),  \
    f32: ((fixed){((x) * (1 << FIXED_FRAC_BITS))}), \
    f64: ((fixed){((x) * (1 << FIXED_FRAC_BITS))}), \
    fixed: (x)                                      \
)
#define vec2(x, y) ((vec2){                         \
    fixed(x),                                       \
    fixed(y),                                       \
})
#define vec3(x, y, z) ((vec3){                      \
    fixed(x),                                       \
    fixed(y),                                       \
    fixed(z)                                        \
})
#define rgb(r, g, b) rgb_from((rgba32){r, g, b})

// C++ : boring, repetitve boilerplate
// C : complicated, *cool* boilerplate
#define add(a, b) _Generic((a),                     \
    fixed: ((fixed){                                \
        (a)._v[0] + (b)._v[0],                      \
    }),                                             \
    vec2: ((vec2){                                  \
        (a)._v[0] + (b)._v[0],                      \
        (a)._v[1] + (b)._v[1],                      \
    }),                                             \
    vec3: ((vec3){                                  \
        (a)._v[0] + (b)._v[0],                      \
        (a)._v[1] + (b)._v[1],                      \
        (a)._v[2] + (b)._v[2],                      \
    })                                              \
)
#define sub(a, b) _Generic((a),                     \
    fixed: ((fixed){                                \
        (a)._v[0] - (b)._v[0],                      \
    }),                                             \
    vec2: ((vec2){                                  \
        (a)._v[0] - (b)._v[0],                      \
        (a)._v[1] - (b)._v[1],                      \
    }),                                             \
    vec3: ((vec3){                                  \
        (a)._v[0] - (b)._v[0],                      \
        (a)._v[1] - (b)._v[1],                      \
        (a)._v[2] - (b)._v[2],                      \
    })                                              \
)
#define mul(a, b) _Generic((a),                     \
    fixed: ((fixed){                                \
        ((a)._v[0] * (b)._v[0]) >> FIXED_FRAC_BITS, \
    }),                                             \
    vec2: ((vec2){                                  \
        ((a)._v[0] * (b)._v[0]) >> FIXED_FRAC_BITS, \
        ((a)._v[1] * (b)._v[0]) >> FIXED_FRAC_BITS, \
    }),                                             \
    vec3: ((vec3){                                  \
        ((a)._v[0] * (b)._v[0]) >> FIXED_FRAC_BITS, \
        ((a)._v[1] * (b)._v[0]) >> FIXED_FRAC_BITS, \
        ((a)._v[2] * (b)._v[0]) >> FIXED_FRAC_BITS, \
    })                                              \
)
#define div(a, b) _Generic((a),                     \
    fixed: ((fixed){                                \
        ((a)._v[0] << FIXED_FRAC_BITS) / (b)._v[0], \
    }),                                             \
    vec2: ((vec2){                                  \
        ((a)._v[0] << FIXED_FRAC_BITS) / (b)._v[0], \
        ((a)._v[1] << FIXED_FRAC_BITS) / (b)._v[0], \
    }),                                             \
    vec3: ((vec3){                                  \
        ((a)._v[0] << FIXED_FRAC_BITS) / (b)._v[0], \
        ((a)._v[1] << FIXED_FRAC_BITS) / (b)._v[0], \
        ((a)._v[2] << FIXED_FRAC_BITS) / (b)._v[0], \
    })                                              \
)
#define cmp(a, op, b) _Generic((a),                 \
    fixed: ((a)._v[0] op (b)._v[0]),                \
    vec2: (                                         \
        ((a)._v[0] op (b)._v[0]) &&                 \
        ((a)._v[1] op (b)._v[1])                    \
    ),                                              \
    vec3: (                                         \
        ((a)._v[0] op (b)._v[0]) &&                 \
        ((a)._v[1] op (b)._v[1]) &&                 \
        ((a)._v[2] op (b)._v[2])                    \
    )                                               \
)

inline fixed fxmax(fixed a, fixed b) { return cmp(a, >, b) ? a : b; }
inline fixed fxmin(fixed a, fixed b) { return cmp(a, <, b) ? a : b; }
inline fixed fxabs(fixed x) { return (fixed){abs(x._v[0])}; }

inline i32 fxfloor(fixed x) { return x._v[0] >> FIXED_FRAC_BITS; }
inline i32 fxceil(fixed x) { return (x._v[0] >> FIXED_FRAC_BITS) + 1; }

inline rgba32 rgb_into(rgb565 c) {
    return (rgba32){
        .r = (c.r << 3) | (c.r >> 2),   // ~0.25% error linear interpolation
        .g = (c.g << 2) | (c.g >> 4),   // ~0.27% error linear interpolation
        .b = (c.b << 3) | (c.b >> 2),   // ~0.25% error linear interpolation
        .a = 0xff,
    };
}
inline rgb565 rgb_from(rgba32 c) {
    return (rgb565){
        .rgb = ((c.r >> 3) << 11) | ((c.g >> 2) << 5) | (c.b >> 3)
    };
}