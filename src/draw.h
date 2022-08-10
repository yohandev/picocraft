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
inline rgb565* frame_get2(vec2 p) {
    return &frame[fxfloor(p.y) * WIDTH + fxfloor(p.x)];
}

inline bool frame_in_bounds(fixed x, fixed y) {
    return cmp(x, >=, fixed(0))
        && cmp(y, >=, fixed(0))
        && cmp(x, <, fixed(WIDTH))
        && cmp(y, <, fixed(HEIGHT));
}

void frame_fill(rgb565 c);
void frame_copy(rgb565 buf[WIDTH*HEIGHT]);
void frame_draw_line(vec2 a, vec2 b, rgb565 c);