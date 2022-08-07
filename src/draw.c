#include <string.h>

#include "draw.h"

void frame_fill(rgb565 c) {
    for (usize i = 0; i < WIDTH*HEIGHT; i++) {
        frame[i] = c;
    }
}

void frame_copy(rgb565 buf[WIDTH*HEIGHT]) {
    memcpy(frame, buf, sizeof(frame));
}

void frame_draw_line(vec2 a, vec2 b, rgb565 c) {
    // DDA algorithm
    vec2 d = sub(b, a);
    fixed steps = fxmax(fxabs(d.x), fxabs(d.y));

    if (cmp(steps, ==, fixed(0))) {
        return;
    }
    d = div(d, steps);

    for (usize i = 0; i < fxfloor(steps); i++) {
        if (cmp(a.x, >=, fixed(0)) && cmp(a.y, >=, fixed(0))
        && cmp(a.x, <, fixed(WIDTH)) && cmp(a.y, <, fixed(HEIGHT))) {
            *frame_get(fxfloor(a.x), fxfloor(a.y)) = c;
        }
        a = add(a, d);
    }
}