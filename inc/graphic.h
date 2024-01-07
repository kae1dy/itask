#ifndef JOS_INC_GS_H
#define JOS_INC_GS_H

/* Definitions for entities states. */
enum {
    GRAPHIC_OK = 0,
    GRAPHIC_WINDOW_CREATE_ERROR,
    GRAPHIC_TEXTURE_CREATE_ERROR,
    GRAPHIC_TEXTURE_UPDATE_ERROR,
    GRAPHIC_RENDERER_CREATE_ERROR,
    GRAPHIC_INVAL
};

/* Definitions for requests from clients to graphic server. */
enum {
    GSREQ_CREATE_WINDOW = 1,
    GSREQ_DESTROY_WINDOW,
    GSREQ_CREATE_TEXTURE,
    GSREQ_DESTROY_TEXTURE,
    GSREQ_CREATE_RENDERER,
    GSREQ_DESTROY_RENDERER,
    GSREQ_UPDATE_TEXTURE,
    GSREQ_COPY_TEXTURE,
    GSREQ_DISPLAY,
    GSREQ_GET_DISPLAY_INFO,
    GSREQ_CLEAR,
};

/* Definitions for window states. */
enum {
    WINDOW_MODE_CORNER = 0,
    WINDOW_MODE_FULLSCREEN,
    WINDOW_MODE_CENTERED,
    WINDOW_MODE_2XSCALE
};

/* A part of the user's screen, characterized by:
1. Offset - pixels shift from left-rop corner,
2. Size - amount of pixels in the current window,
3. Mode - window mode (cornered/fullscreen/centered/scaled). */
struct Window {
    uint32_t width;
    uint32_t height;
    uint32_t x_offset;
    uint32_t y_offset;
    int mode;
};

/* An entity for updating user screen, characterized by:
1. Size - amount of pixels in the current texture,
3. Mode - window mode (cornered/fullscreen/centered/scaled). */
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

/* Definitions of parameters for all entities. */
union Gsipc {
    struct Gsreq_create_window {
        uint32_t req_width;
        uint32_t req_height;
        int req_mode;
        window_d res_window;
    } create_window;
    struct Gsreq_destroy_window {
        window_d req_window;
    } destroy_window;
    struct Gsreq_create_texture {
        uint32_t  req_width;
        uint32_t  req_height;
        bool req_need_mapping;
        texture_d res_texture;
        uint32_t * res_buffer_map;
    } create_texture;
    struct Gsreq_destroy_texture {
        texture_d req_texture;
    } destroy_texture;
    struct Gsreq_create_renderer {
        window_d req_window;
        renderer_d res_renderer;
    } create_renderer;
    struct Gsreq_destroy_renderer {
        renderer_d req_renderer;
    } destroy_renderer;
    struct Gsreq_copy_texture {
        texture_d req_texture;
        renderer_d req_renderer;
    } copy_texture;
    struct Gsreq_display {
        renderer_d req_renderer;
    } display;
    struct Gsreq_get_display_info {
        uint32_t res_display_width;
        uint32_t res_display_height;
    } get_display_info;
    struct Gsreq_clear {
        renderer_d req_renderer;
    } clear;

    /* Ensure Fsipc is one page */
    char _pad[PAGE_SIZE];
};

#endif /* !JOS_INC_GS_H */
