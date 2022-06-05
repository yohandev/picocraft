#include "texture.h"

void texture_init(Texture* self) {
    self->buf = malloc(self->width * self->height * sizeof(rgb565));
}

void texture_drop(Texture* self) {
    free(self->buf);
}

void texture_fill(Texture* self, rgb565 c) {
    for (usize i = 0; i < self->width * self->height; i++) {
        self->buf[i] = c;
    }
}

void texture_line(Texture* self, rgb565 c, i32 x0, i32 y0, i32 x1, i32 y1) {
    i32 m = 2 * (y1 - y0);
    i32 e = m - (x1 - x0);

    for (i32 x = x0, y = y0; x <= x1; x++) {
        
        
        e += m;
        if (e >= 0) {
            y++;
            e -= 2 * (x1 - x0);
        }
    }
}