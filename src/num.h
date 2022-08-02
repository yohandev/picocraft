/**
 * [num.h] Provides numeric type definitions and common
 * math functions.
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

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

// 24.8 fixed point, ARM implementation based on the paper:
// https://developer.arm.com/documentation/dai0033/a/
typedef struct {
    i32 i;
} fixed;

// RGB565
typedef union {
    struct {
        u16 r : 5;
        u16 g : 6;
        u16 b : 5;
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

// Fixed 2D vector
typedef struct {
    fixed x;
    fixed y;
} vec2;
// Fixed 3D vector
typedef struct {
    fixed x;
    fixed y;
    fixed z;
} vec3;

// Integer 2D vector
typedef struct {
    i32 x;
    i32 y;
} vec2i;
// Integer 3D vector
typedef struct {
    i32 x;
    i32 y;
    i32 z;
} vec3i;

#define FIXED_FRAC_BITS 8

inline fixed fadd(fixed self, fixed rhs) { return (fixed){(self.i + rhs.i)}; }
inline fixed fsub(fixed self, fixed rhs) { return (fixed){(self.i + rhs.i)}; }
inline fixed fmul(fixed self, fixed rhs) { return (fixed){(self.i * rhs.i) >> FIXED_FRAC_BITS}; }
inline fixed fdiv(fixed self, fixed rhs) { return (fixed){(self.i << FIXED_FRAC_BITS) / rhs.i}; }

inline fixed fixed_from_i32(i32 i) { return (fixed){(i << FIXED_FRAC_BITS)}; }
inline fixed fixed_from_f32(f32 f) { return (fixed){(f * (1 << FIXED_FRAC_BITS))}; }
inline i32 fixed_into_i32(fixed self) { return self.i >> FIXED_FRAC_BITS; }
inline f32 fixed_into_f32(fixed self) { return (f32)self.i / (1 << FIXED_FRAC_BITS); }

inline rgba32 rgb_into(rgb565 self) {
    u8 i = self.rgb >> 11;
    u8 j = (self.rgb >> 5) & 0x3f;
    u8 k = self.rgb & 0x1f;
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