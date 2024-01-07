#include <inc/lib.h>

union Gsipc gsipcbuf __attribute__((aligned(PAGE_SIZE)));

/* Send an inter-environment request to the graphic server, and wait for
 * a reply.  The request body should be in gsipcbuf, and parts of the
 * response may be written back to fsipcbuf.
 * type: request code, passed as the simple integer IPC value.
 * dstva: virtual address at which to receive reply page, 0 if none.
 * Returns result from the file server. */
static int gsipc(unsigned type, void *dstva) {
    static envid_t gsenv;

    if (!gsenv) gsenv = ipc_find_env(ENV_TYPE_GS);

    static_assert(sizeof(gsipcbuf) == PAGE_SIZE, "Invalid gsipcbuf size");

    ipc_send(gsenv, type, &gsipcbuf, PAGE_SIZE, PROT_RW);
    size_t maxsz = PAGE_SIZE;
    return ipc_recv(NULL, dstva, &maxsz, NULL);
}

/* Create a new window.
 *
 * Returns:
 *  The window descriptor on success
 *  NULL for errors. */
window_d create_window(uint32_t width, uint32_t height, int mode) {
    gsipcbuf.create_window.req_width = width;
    gsipcbuf.create_window.req_height = height;
    gsipcbuf.create_window.req_mode = mode;

    int res = gsipc(GSREQ_CREATE_WINDOW, &gsipcbuf);
    if (res)
        return NULL;

    return gsipcbuf.create_window.res_window;
}

/* Destroy the window.
 *
 * Returns:
 *  GRAPHIC_OK on success
 *  <0 for errors. */
int destroy_window(window_d window) {
    gsipcbuf.destroy_window.req_window = window;

    return gsipc(GSREQ_DESTROY_WINDOW, &gsipcbuf);
}

/* Create a new texture.
 *
 * Returns:
 *  The texture descriptor on success
 *  NULL for errors. */
texture_d create_texture(uint32_t width, uint32_t height, bool need_mapping, uint32_t **buffer_map) {
    gsipcbuf.create_texture.req_width = width;
    gsipcbuf.create_texture.req_height = height;
    gsipcbuf.create_texture.req_need_mapping = need_mapping;

    int res = gsipc(GSREQ_CREATE_TEXTURE, &gsipcbuf);
    if (res)
        return NULL;

    if (need_mapping) {
        *buffer_map = gsipcbuf.create_texture.res_buffer_map;
#ifdef SANITIZE_USER_SHADOW_BASE
        platform_asan_unpoison(*buffer_map, sizeof(uint32_t) * width * height);
#endif
    }

    return gsipcbuf.create_texture.res_texture;
}

/* Destroy a texture.
 *
 * Returns:
 *  GRAPHIC_OK on success
 *  < 0 for errors. */
int destroy_texture(texture_d texture) {
    gsipcbuf.destroy_texture.req_texture = texture;
#ifdef SANITIZE_USER_SHADOW_BASE
    platform_asan_poison(texture->user_buf_map, sizeof(uint32_t) * texture->width * texture->height);
#endif
    return gsipc(GSREQ_DESTROY_TEXTURE, &gsipcbuf);
}

/* Create a new renderer.
 *
 * Returns:
 *  The renderer descriptor on success
 *  NULL for errors. */
renderer_d create_renderer(window_d window) {
    gsipcbuf.create_renderer.req_window = window;
    int res =  gsipc(GSREQ_CREATE_RENDERER, &gsipcbuf);
    if (res)
        return NULL;

    return gsipcbuf.create_renderer.res_renderer;
}

/* Destroy a renderer.
 *
 * Returns:
 *  GRAPHIC_OK on success
 *  < 0 for errors. */
int destroy_renderer(renderer_d renderer) {
    gsipcbuf.destroy_renderer.req_renderer = renderer;
    return gsipc(GSREQ_DESTROY_RENDERER, &gsipcbuf);
}

/* Copy a texture to the renderer's back_buffer
 *
 * Returns:
 *  GRAPHIC_OK on success
 *  < 0 for errors. */
int copy_texture(renderer_d renderer, texture_d texture) {
    gsipcbuf.copy_texture.req_texture = texture;
    gsipcbuf.copy_texture.req_renderer = renderer;
    return gsipc(GSREQ_COPY_TEXTURE, &gsipcbuf);
}

/* Copy the renderer's back_buffer to the game texture buffer.
 *
 * Returns:
 *  GRAPHIC_OK on success
 *  < 0 for errors. */
int display(renderer_d renderer) {
    gsipcbuf.display.req_renderer = renderer;
    return gsipc(GSREQ_DISPLAY, &gsipcbuf);
}

/* Clear the renderer's back_buffer.
 *
 * Returns:
 *  GRAPHIC_OK on success
 *  < 0 for errors. */
int clear(renderer_d renderer) {
    gsipcbuf.clear.req_renderer = renderer;
    return gsipc(GSREQ_CLEAR, &gsipcbuf);
}