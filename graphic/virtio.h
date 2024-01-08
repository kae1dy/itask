#ifndef VIRTIO_VIDEO_H
#define VIRTIO_VIDEO_H

#include "fs/pci.h"
#include "fs/pci_classes.h"

#include <inc/x86.h>
#include <inc/string.h>
#include <inc/lib.h>
#include <inc/video.h>

// Virtio Video specific ports
#define VIRTIO_VIDEO_INDEX (0)
#define VIRTIO_VIDEO_VALUE (1)
#define VIRTIO_VIDEO_BIOS (2)
#define VIRTIO_VIDEO_IRQSTATUS (8)

// Virtio Video internal ports
#define VIRTIO_VIDEO_REG_ID (0) // register used to negociate specification ID
#define VIRTIO_VIDEO_REG_ENABLE (1) // flag set by the driver when the device should enter Virtio Video mode
#define VIRTIO_VIDEO_REG_WIDTH (2) // current screen width
#define VIRTIO_VIDEO_REG_HEIGHT (3) // current screen height
#define VIRTIO_VIDEO_REG_MAX_WIDTH (4) // maximum supported screen width
#define VIRTIO_VIDEO_REG_MAX_HEIGHT (5) // maximum supported screen height
#define VIRTIO_VIDEO_REG_BPP (7) // current screen bits per pixel
#define VIRTIO_VIDEO_REG_FB_START (13) // address in system memory of the frame buffer
#define VIRTIO_VIDEO_REG_FB_OFFSET (14) // offset in the frame buffer to the visible pixel data
#define VIRTIO_VIDEO_REG_VRAM_SIZE (15) // size of the video RAM
#define VIRTIO_VIDEO_REG_FB_SIZE (16) // size of the frame buffer
#define VIRTIO_VIDEO_REG_CAPABILITIES (17) // device capabilities
#define VIRTIO_VIDEO_REG_FIFO_START (18) // address in system memory of the FIFO
#define VIRTIO_VIDEO_REG_FIFO_SIZE (19) // FIFO size
#define VIRTIO_VIDEO_REG_CONFIG_DONE (20) // flag to enable FIFO operation
#define VIRTIO_VIDEO_REG_SYNC (21) // flag set by the driver to flush FIFO changes
#define VIRTIO_VIDEO_REG_BUSY (22) // flag set by the FIFO when it's processed

// Virtio Video specific capabilities
#define VIRTIO_VIDEO_CAP_NONE               0x00000000
#define VIRTIO_VIDEO_CAP_RECT_COPY          0x00000002
// Add more capabilities as needed

// Virtio Video specific error codes
enum virtio_video_error {
    VIRTIO_VIDEO_OK = 0,
    VIRTIO_VIDEO_MAP_ERR = 1,
    VIRTIO_VIDEO_PORT_ERR = 2,
    VIRTIO_VIDEO_FIFO_ERR = 3,
    VIRTIO_PCI_NOT_FOUND = 4
};

// Virtio Video specific structure
struct GraphicAdapter {
    struct PciDevice * pcidev;

    uintptr_t io_base;
    volatile uint32_t * fb_base_addr;
    volatile uint32_t * fifo;
    volatile uint32_t * fb;

    uint32_t framebuffer_size;
    uint32_t fifo_size;

    uint32_t width;
    uint32_t height;
    uint32_t bits_per_pixel;
    uint32_t capabilities;
};

// Virtio Video specific functions
int virtio_video_read_io_bar(struct VirtioVideoAdapter * adapter);
uint32_t virtio_video_read(struct VirtioVideoAdapter * adapter, int reg);
void virtio_video_write(struct VirtioVideoAdapter * adapter, int reg, int value);
int virtio_video_map(struct VirtioVideoAdapter * adapter);
int virtio_video_read_resolution(struct VirtioVideoAdapter * adapter);
int virtio_video_set_resolution(struct VirtioVideoAdapter * adapter, uint32_t w, uint32_t h);
int virtio_video_set_spec_id(struct VirtioVideoAdapter * adapter);
void virtio_video_fill(struct VirtioVideoAdapter * adapter, uint32_t col);
int virtio_video_fifo_init(struct VirtioVideoAdapter * adapter);
int virtio_video_init();
struct VirtioVideoAdapter * get_virtio_video();

#endif //VIRTIO_VIDEO_H