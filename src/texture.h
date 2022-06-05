/**
 * [image.h] Framebuffer and rasterizing utilities
 */
#pragma once

#include "num.h"

typedef struct {
    usize width;    // Framebuffer width(px)
    usize height;   // Framebuffer height(px)

    rgb565* buf;    // Framebuffer
} Texture;

void texture_init(Texture* self);
void texture_drop(Texture* self);

void texture_fill(Texture* self, rgb565 c);
void texture_line(Texture* self, rgb565 c, i32 x0, i32 y0, i32 x1, i32 y1);

static inline rgb565 texture_get_unchecked(Texture* self, i32 x, i32 y) {
    return self->buf[self->width * y + x];
}

static inline rgb565 texture_get(Texture* self, i32 x, i32 y) {
    assert(x >= 0 && x < self->width);
    assert(y >= 0 && y < self->height);

    return texture_get_unchecked(x, y);
}

static inline void texture_set_unchecked(Texture* self, i32 x, i32 y, rgb565 c) {
    self->buf[self->width * y + x] = c;
}

static inline void texture_set(Texture* self, i32 x, i32 y, rgb565 c) {
    assert(x >= 0 && x < self->width);
    assert(y >= 0 && y < self->height);

    texture_set_unchecked(x, y, c);
}