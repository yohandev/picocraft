#include "draw.h"

#define FWIDTH fixed_from_i32(WIDTH)
#define FHEIGHT fixed_from_i32(HEIGHT)

void frame_fill(rgb565 c) {
    for (usize i = 0; i < WIDTH*HEIGHT; i++) {
        frame[i] = c;
    }
}

void frame_draw_line(vec2 a, vec2 b, rgb565 c) {
    // DDA algorithm
    fixed dx = fsub(b.x, a.x);
    fixed dy = fsub(b.y, a.y);

    fixed steps;
    if (!(steps = fixed_max(abs(dx), abs(dy)))) {
        return;
    }
    dx = fdiv(dx, steps);
    dy = fdiv(dy, steps);

    fixed x = a.x;
    fixed y = a.y;
    for (usize i = 0; i < 100; i++) {
        frame[i + 30 * WIDTH] = (rgb565){0xFFFF};
    }
    for (usize i = 0; i < fixed_into_i32(steps); i++) {
        if (x >= 0 && y >= 0 && x < FWIDTH && y < FHEIGHT) {
            *pixel_at(fixed_into_i32(x), fixed_into_i32(y)) = c;
        }
        frame[i + 10 * WIDTH] = c;
        x += dx;
        y += dy;
    }
}