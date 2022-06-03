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
