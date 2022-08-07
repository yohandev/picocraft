/**
 * [draw.h] Provides graphic utility functions.
 */
#pragma once

#include "num.h"

#define WIDTH 240
#define HEIGHT 135

extern rgb565 frame[WIDTH*HEIGHT];

inline rgb565* frame_get(usize x, usize y) {
    return &frame[y * WIDTH + x];
}

void frame_fill(rgb565 c);
void frame_copy(rgb565 buf[WIDTH*HEIGHT]);
void frame_draw_line(vec2 a, vec2 b, rgb565 c);