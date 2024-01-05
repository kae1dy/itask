#ifndef JOS_GRAPHICS_H
#define JOS_GRAPHICS_H

/* A part of the user's screen, characterized by:
1. Offset - pixels shift from left-rop corner,
2. Size - amount of pixels in the current window. */
struct Window {
    uint32_t width;
    uint32_t height;
    uint32_t x_offset;
    uint32_t y_offset;
    int mode;
};

/* Texture description. */
struct Texture {
    uint32_t width;
    uint32_t height;
    uint32_t *buf;
    uint32_t *user_buf_map;
};

/* Renderer description. */
struct Renderer {
    struct Window *window;
    uint32_t *back_buffer;
};

typedef struct Window * window_d;
typedef struct Texture * texture_d;
typedef struct Renderer * renderer_d;

#endif
