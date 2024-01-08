#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "fs/pci.h"
#include "fs/pci_classes.h"

#include <inc/x86.h>
#include <inc/string.h>
#include <inc/lib.h>
#include <inc/graphic.h>

// VIRTIO external ports
#define VIRTIO_INDEX (0)
#define VIRTIO_VALUE (1)
#define VIRTIO_BIOS (2)
#define VIRTIO_IRQSTATUS (8)

// VIRTIO internal ports
#define VIRTIO_REG_ID (0) // register used to negociate specification ID
#define VIRTIO_REG_ENABLE (1) // flag set by the driver when the device should enter Virtio Video mode
#define VIRTIO_REG_WIDTH (2) // current screen width
#define VIRTIO_REG_HEIGHT (3) // current screen height
#define VIRTIO_REG_MAX_WIDTH (4) // maximum supported screen width
#define VIRTIO_REG_MAX_HEIGHT (5) // maximum supported screen height
#define VIRTIO_REG_BPP (7) // current screen bits per pixel
#define VIRTIO_REG_FB_START (13) // address in system memory of the frame buffer
#define VIRTIO_REG_FB_OFFSET (14) // offset in the frame buffer to the visible pixel data
#define VIRTIO_REG_VRAM_SIZE (15) // size of the video RAM
#define VIRTIO_REG_FB_SIZE (16) // size of the frame buffer
#define VIRTIO_REG_CAPABILITIES (17) // device capabilities
#define VIRTIO_REG_FIFO_START (18) // address in system memory of the FIFO
#define VIRTIO_REG_FIFO_SIZE (19) // FIFO size
#define VIRTIO_REG_CONFIG_DONE (20) // flag to enable FIFO operation
#define VIRTIO_REG_SYNC (21) // flag set by the driver to flush FIFO changes
#define VIRTIO_REG_BUSY (22) // flag set by the FIFO when it's processed

#define SVGA_FIFO_MIN 0
#define SVGA_FIFO_MAX 1
#define SVGA_FIFO_NEXT_CMD 2
#define SVGA_FIFO_STOP 3
#define SVGA_FIFO_CAPABILITIES 4
#define SVGA_FIFO_FLAGS 5

#define SVGA_FIFO_REGISTERS_SIZE 293

#define SVGA_CAP_NONE               0x00000000
#define SVGA_CAP_RECT_COPY          0x00000002
#define SVGA_CAP_CURSOR             0x00000020
#define SVGA_CAP_CURSOR_BYPASS      0x00000040   // Legacy (Use Cursor Bypass 3 instead)
#define SVGA_CAP_CURSOR_BYPASS_2    0x00000080   // Legacy (Use Cursor Bypass 3 instead)
#define SVGA_CAP_8BIT_EMULATION     0x00000100
#define SVGA_CAP_ALPHA_CURSOR       0x00000200
#define SVGA_CAP_3D                 0x00004000
#define SVGA_CAP_EXTENDED_FIFO      0x00008000
#define SVGA_CAP_MULTIMON           0x00010000   // Legacy multi-monitor support
#define SVGA_CAP_PITCHLOCK          0x00020000
#define SVGA_CAP_IRQMASK            0x00040000
#define SVGA_CAP_DISPLAY_TOPOLOGY   0x00080000   // Legacy multi-monitor support
#define SVGA_CAP_GMR                0x00100000
#define SVGA_CAP_TRACES             0x00200000
#define SVGA_CAP_GMR2               0x00400000
#define SVGA_CAP_SCREEN_OBJECT_2    0x00800000

#define MAX_OPEN_WINDOWS 128
#define MAX_OPEN_RENDERERS 128
#define MAX_OPEN_TEXTURES 128


#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA 0x01CF

#define VBE_DISPI_INDEX_ID (0)
#define VBE_DISPI_INDEX_XRES (1)
#define VBE_DISPI_INDEX_YRES (2)
#define VBE_DISPI_INDEX_BPP (3)
#define VBE_DISPI_INDEX_ENABLE (4)
#define VBE_DISPI_INDEX_BANK (5)
#define VBE_DISPI_INDEX_VIRT_WIDTH (6)
#define VBE_DISPI_INDEX_VIRT_HEIGHT (7)
#define VBE_DISPI_INDEX_X_OFFSET (8)
#define VBE_DISPI_INDEX_Y_OFFSET (9)

#define VBE_DISPI_DISABLED 0x00
#define VBE_DISPI_ENABLED 0x01
#define VBE_DISPI_LFB_ENABLED 0x40
#define VBE_DISPI_NOCLEARMEM 0x80

enum virtio_error {
    VIRTIO_OK = 0,
    VIRTIO_MAP_ERR = 1,
    VIRTIO_PORT_ERR = 2,
    VIRTIO_FIFO_ERR = 3,
    VIRTIO_PCI_NOT_FOUND = 4
};

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

struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

int virtio_init(uint32_t width, uint32_t height);

#endif //GRAPHIC_H
