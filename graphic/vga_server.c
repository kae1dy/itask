#include <inc/x86.h>
#include <inc/string.h>
#include <inc/graphic.h>

#include "fs/pci.h"
#include "virtio.h"

/* Virtual address at which to receive graphics mappings containing client requests. */
union Gsipc *gsreq = (union Gsipc*)0x0FFFF000;

/* Open req->req_path in mode req->req_omode, storing the Fd page and
 * permissions to return to the calling environment in *pg_store and
 * *perm_store respectively. */
int serve_create_window(union Gsipc *ipc) {
    window_d window = calloc(1, sizeof(* window));
    if (!window) {
        panic("Window wasn't created");
        return -GRAPHIC_WINDOW_CREATE_ERROR;
    }

    struct Gsreq_create_window * req = &ipc->create_window;
    if (req->req_width <= 0 || req->req_height <= 0) {
        panic("Window dimensions can't be 0");
        return -GRAPHIC_WINDOW_CREATE_ERROR;
    }

    window->width = req->req_width;
    window->height = req->req_height;
    window->x_offset = 0;
    window->y_offset = 0;
    window->mode = req->req_mode;

    struct GraphicsAdapter * svga = get_svga();

    if (window->mode == WINDOW_MODE_CENTERED) {
        window->x_offset = svga->width / 2 - window->width / 2;
        window->y_offset = svga->height / 2 - window->height / 2;
    }

    req->res_window = window;
    svga_fill(svga, 0x00000000);

    return GRAPHIC_OK;
}

int serve_destroy_window(union Gsipc *ipc) {
    window_d window = ipc->destroy_window.req_window;

    if (window)
        free(window);

    return GRAPHIC_OK;
}

int serve_create_texture(envid_t dst_envid, union Gsipc *ipc) {
    texture_d texture = calloc(1, sizeof(* texture));
    if (!texture) {
        panic("Texture wasn't created");
        return -GRAPHIC_TEXTURE_CREATE_ERROR;
    }

    struct Gsreq_create_texture *req = &ipc->create_texture;
    if (req->req_width <= 0 || req->req_height <= 0) {
        panic("Texture dimensions can't be 0");
        return -GRAPHIC_TEXTURE_CREATE_ERROR;
    }

    texture->width = req->req_width;
    texture->height = req->req_height;
    texture->buf = calloc(texture->width * texture->height, sizeof(* texture->buf));
    if (!texture->buf) {
        panic("Texture wasn't created");
        return -GRAPHIC_TEXTURE_CREATE_ERROR;
    }

    if (req->req_need_mapping) {
        uint32_t * texture_map = (uint32_t*)(GRAPHIC_MAP_TOP + ((char*)texture->buf - USER_HEAP_TOP));
        int res = sys_map_region(0, texture->buf, dst_envid, texture_map, sizeof(uint32_t) * texture->width * texture->height, PROT_RW);
        if (res) {
            free(texture->buf);
            free(texture);
            panic("Texture wasn't created");
            return res;
        }

        texture->user_buf_map = texture_map;
        req->res_buffer_map = texture_map;
    }

    req->res_texture = texture;

    return GRAPHIC_OK;
}

int serve_destroy_texture(envid_t dst_envid, union Gsipc *ipc) {
    texture_d texture = ipc->destroy_texture.req_texture;

    if (texture->user_buf_map) {
        int res = sys_unmap_region(dst_envid, texture->user_buf_map,
                sizeof(* texture->user_buf_map) * texture->height * texture->width);
        if (res)
            return res;
    }

    if (texture->buf)
        free(texture->buf);

    if (texture)
        free(texture);

    return GRAPHIC_OK;
}

int serve_create_renderer(union Gsipc *ipc) {
    renderer_d renderer = calloc(1, sizeof(* renderer));
    if (!renderer) {
        panic("Renderer wasn't created");
        return -GRAPHIC_RENDERER_CREATE_ERROR;
    }

    struct Gsreq_create_renderer *req = &ipc->create_renderer;
    renderer->window = req->req_window;
    renderer->back_buffer = calloc(req->req_window->width * req->req_window->height, sizeof(* renderer->back_buffer));

    if (!renderer->back_buffer) {
        panic("Renderer wasn't created");
        return -GRAPHIC_RENDERER_CREATE_ERROR;
    }

    req->res_renderer = renderer;

    return GRAPHIC_OK;
}

int serve_destroy_renderer(union Gsipc *ipc) {
    renderer_d renderer = ipc->destroy_renderer.req_renderer;

    if (req->back_buffer)
        free(req->back_buffer);

    if (req)
        free(req);

    return GRAPHIC_OK;
}

int serve_copy_texture(union Gsipc *ipc) {
    texture_d texture = ipc->copy_texture.req_texture;
    renderer_d renderer = ipc->copy_texture.req_renderer;

    memcpy(renderer->back_buffer, texture->buf, renderer->window->width * renderer->window->height * sizeof(uint32_t));

    return GRAPHIC_OK;
}

int serve_get_display_info(union Gsipc *ipc) {
    struct GraphicsAdapter * adapter = get_svga();

    ipc->get_display_info.res_display_height = adapter->width;
    ipc->get_display_info.res_display_width = adapter->height;

    return GRAPHIC_OK;
}

int serve_display(union Gsipc *ipc) {
    renderer_d renderer = ipc->display.req_renderer;
    struct GraphicsAdapter * adapter = get_svga();
    window_d window = renderer->window;

    if (window->mode == WINDOW_MODE_CORNER || window->mode == WINDOW_MODE_CENTERED) {
        for (uint32_t y = 0; y < window->height; ++y) {
            for (uint32_t x = 0; x < window->width; ++x) {
                uint32_t adapter_x = window->x_offset + x;
                uint32_t adapter_y = window->y_offset + y;
                adapter->fb[adapter_y * adapter->width + adapter_x] = renderer->back_buffer[y * window->width + x];
            }
        }
    } else if (window->mode == WINDOW_MODE_FULLSCREEN) {
        for (uint32_t y = 0; y < adapter->height; ++y) {
            for (uint32_t x = 0; x < adapter->width; ++x) {
                uint32_t w_x = (uint32_t)((double)x / adapter->width * window->width);
                uint32_t w_y = (uint32_t)((double)y / adapter->height * window->height);
                adapter->fb[y * adapter->width + x] = renderer->back_buffer[w_y * window->width + w_x];
            }
        }
    } else if (window->mode == WINDOW_MODE_2XSCALE) {
        for (uint32_t y = 0; y < window->height * 2; ++y) {
            for (uint32_t x = 0; x < window->width * 2; ++x) {
                uint32_t w_x = x / 2;
                uint32_t w_y = y / 2;
                adapter->fb[y * adapter->width + x] = renderer->back_buffer[w_y * window->width + w_x];
            }
        }
    }

    return GRAPHIC_OK;
}

int serve_clear(union Gsipc *ipc) {
    renderer_d renderer = ipc->clear.req_renderer;

    memset(renderer->back_buffer, 0, (renderer->window->width * renderer->window->height)
                                    * sizeof(* (renderer->back_buffer)));
    
    return GRAPHIC_OK;
}


typedef int (*vshandler) (union Gsipc *req);

vshandler handlers[] = {
        /* Create_and Destroy texture are handled specially,
         * because memory is allocatedin sender's memory region (game region). */
        [GSREQ_CREATE_WINDOW] = serve_create_window,
        [GSREQ_DESTROY_WINDOW] = serve_destroy_window,
        [GSREQ_CREATE_RENDERER] = serve_create_renderer,
        [GSREQ_DESTROY_RENDERER] = serve_destroy_renderer,
        // [GSREQ_CREATE_TEXTURE] = serve_create_texture,
        // [GSREQ_DESTROY_TEXTURE] = serve_destroy_texture,
        [GSREQ_COPY_TEXTURE] = serve_copy_texture,
        [GSREQ_DISPLAY] = serve_display,
        [GSREQ_CLEAR] = serve_clear
};
#define NHANDLERS (sizeof(handlers) / sizeof(handlers[0]))


void serve(void) {
    uint32_t req, whom;
    int perm, res;

    while (1) {
        perm = 0;
        size_t sz = PAGE_SIZE;
        req = ipc_recv((int32_t *)&whom, gsreq, &sz, &perm);

        if (!(perm & PROT_R)) {
            continue; /* Just leave it hanging... */
        }

        if (req == GSREQ_CREATE_TEXTURE) {
            res = serve_create_texture(whom, gsreq);
        } else if (req == GSREQ_DESTROY_TEXTURE) {
            res = serve_destroy_texture(whom, gsreq);
        }
        else if (req < NHANDLERS && handlers[req]) {
            res = handlers[req](gsreq);
        }
        else {
            res = -GRAPHIC_INVAL;
        }

        ipc_send(whom, res, NULL, PAGE_SIZE, perm);
        sys_unmap_region(0, gsreq, PAGE_SIZE);
    }
}

void
umain(int argc, char **argv) {
    cprintf("GS is running\n");
    pci_init(argv);
    virtio_init(640, 400);
    serve();
}