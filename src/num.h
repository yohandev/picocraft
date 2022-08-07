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
    i32 v[1];
} fixed;

// 2D vector of `fixed`
typedef union {
    struct {
        fixed x;
        fixed y;
    };
    i32 v[2];
} vec2;
// Fixed 3D vector
typedef union {
    struct {
        fixed x;
        fixed y;
        fixed z;
    };
    i32 v[3];
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

// // Integer 2D vector
// typedef union {
//     struct {
//         i32 x;
//         i32 y;
//     };
//     i32 vec[2];
// } vec2i;
// // Integer 3D vector
// typedef union {
//     struct {
//         i32 x;
//         i32 y;
//         i32 z;
//     };
//     i32 vec[3];
// } vec3i;

#define FIXED_FRAC_BITS 8

// Reduces a type to a primitive by reinterpreting
#define num_inner(z) _Generic((z),                  \
    fixed: (((union {                               \
        __typeof__(z) from;                         \
        i32 into;                                   \
    }){.from=(z)}).into),                           \
    default: (z)                                    \
)
// Casts between numeric types
#define num_cast(T, y) _Generic((y),                \
    fixed: ((T)num_inner((y)) >> FIXED_FRAC_BITS),  \
    default: ((T)num_inner((y)))                    \
)

// 8-bit
#define u8(x) num_cast(u8, (x))
#define i8(x) num_cast(i8, (x))
// 16-bit
#define u16(x) num_cast(u16, (x))
#define i16(x) num_cast(i16, (x))
// 32-bit
#define u32(x) num_cast(u32, (x))
#define i32(x) num_cast(i32, (x))
// 64-bit
#define u64(x) num_cast(u64, (x))
#define i64(x) num_cast(i64, (x))
// fixed
#define fixed(x) _Generic((x),                      \
    int: ((fixed){((i32)(x) << FIXED_FRAC_BITS)}),  \
    i32: ((fixed){((i32)(x) << FIXED_FRAC_BITS)}),  \
    f32: ((fixed){((x) * (1 << FIXED_FRAC_BITS))}), \
    f64: ((fixed){((x) * (1 << FIXED_FRAC_BITS))})  \
)

#define add(a, b) _Generic((a),                     \
    fixed: ((fixed){(a).v[0] + (b).v[1]}),          \
    vec2: ((vec2){                                  \
        (a).v[0] + (b).v[0],                        \
        (a).v[1] + (b).v[1],                        \
    }),                                             \
    vec3: ((vec3){                                  \
        (a).v[0] + (b).v[0],                        \
        (a).v[1] + (b).v[1],                        \
        (a).v[2] + (b).v[2],                        \
    })                                              \
)
// #define add(a, b) ({                                \
//     __typeof__(a) _a = (a);                         \
//     __typeof__(a) _b = (b);                         \
//     _Generic((a),                                   \
//         fixed: ((fixed){num_inner((a)) + num_inner((b))}),  \
//         vec2: ((vec2){                              \
//             num_inner((a).vec[0]) + num_inner((b).vec[0]),                \
//             num_inner((a).vec[1]) + num_inner((b).vec[1]),                \
//         }),                                         \
//         vec2i: ((vec2i){                            \
//             num_inner((a).vec[0]) + num_inner((b).vec[0]),                \
//             num_inner((a).vec[1]) + num_inner((b).vec[1]),                \
//         }),                                         \
//         vec3: ((vec3){                              \
//             num_inner((a).vec[0]) + num_inner((b).vec[0]),                \
//             num_inner((a).vec[1]) + num_inner((b).vec[1]),                \
//             num_inner((a).vec[2]) + num_inner((b).vec[2]),                \
//         }),                                         \
//         vec2: ((vec2){                              \
//             num_inner((a).vec[0]) + num_inner((b).vec[0]),                \
//             num_inner((a).vec[1]) + num_inner((b).vec[1]),                \
//             num_inner((a).vec[2]) + num_inner((b).vec[2]),                \
//         })                                          \
//     )                                               \
// })
// #define add(a, b) ((a) + (b))
// #define sub(a, b) ((a) - (b))
// #define mul(a, b) ({                                \
//     __typeof__(a) _a = (a);                         \
//     __typeof__(a) _b = (b);                         \
//     _Generic((a),                                   \
//         fixed: ((fixed){((inner((a)) * inner((b))) >> FIXED_FRAC_BITS)}),\
//         default: (inner((a)) * inner((b)))                            \
//     );                                              \
// })
// inline fixed fadd(fixed lhs, fixed rhs) { return (lhs + rhs); }
// inline fixed fsub(fixed lhs, fixed rhs) { return (lhs + rhs); }
// inline fixed fmul(fixed lhs, fixed rhs) { return (lhs * rhs) >> FIXED_FRAC_BITS; }
// inline fixed fdiv(fixed lhs, fixed rhs) { return (lhs << FIXED_FRAC_BITS) / rhs; }

// inline fixed fixed_from_i32(i32 i) { return (fixed){(i << FIXED_FRAC_BITS)}; }
// inline fixed fixed_from_f32(f32 f) { return (fixed){(f * (1 << FIXED_FRAC_BITS))}; }
// inline i32 fixed_into_i32(fixed self) { return self.i >> FIXED_FRAC_BITS; }
// inline f32 fixed_into_f32(fixed self) { return (f32)self / (1 << FIXED_FRAC_BITS); }

// inline fixed fixed_max(fixed a, fixed b) { return a > b ? a : b; }
// inline fixed fixed_min(fixed a, fixed b) { return a < b ? a : b; }

inline rgba32 rgb_into(rgb565 self) {
    u8 i = self.rgb & 0x1f;
    u8 j = (self.rgb >> 5) & 0x3f;
    u8 k = self.rgb >> 11;
    return (rgba32){
        .r = (i << 3) | (i >> 2),   // ~0.25% error linear interpolation
        .g = (j << 2) | (j >> 4),   // ~0.27% error linear interpolation
        .b = (k << 3) | (k >> 2),   // ~0.25% error linear interpolation
        .a = 0xff,
    };
}
inline rgb565 rgb_from(rgba32 c) {
    return (rgb565){
        .rgb = ((c.r >> 3) << 11) | ((c.g >> 2) << 5) | (c.b >> 3)
    };
}