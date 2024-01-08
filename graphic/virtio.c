#include "video.h"
#include "fs/pci.h"

static struct GraphicsAdapter virtio;

uint16_t virtio_read(uint16_t index) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    return inw(VBE_DISPI_IOPORT_DATA);
}

void virtio_write(uint16_t index, uint16_t data) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, data);
}

int virtio_get_version(void) {
    return virtio_read(VBE_DISPI_INDEX_ID);
}

int virtio_set_display(struct GraphicsAdapter * adapter, uint16_t width, uint16_t height, uint32_t bit_depth, uint32_t use_linear_framebuffer, uint32_t clear_video_memory) {
    virtio_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    virtio_write(VBE_DISPI_INDEX_XRES, width);
    virtio_write(VBE_DISPI_INDEX_YRES, height);
    virtio_write(VBE_DISPI_INDEX_BPP, bit_depth);
    virtio_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
                                             (use_linear_framebuffer ? VBE_DISPI_LFB_ENABLED : 0) |
                                             (clear_video_memory ? 0 : VBE_DISPI_NOCLEARMEM));

    adapter->width = width;
    adapter->height = height;
    adapter->bits_per_pixel = bit_depth;

    return SVGA_OK;
}

int virtio_map_fb(struct GraphicsAdapter * adapter) {
    adapter->fb_base_addr =  adapter->fb = (volatile uint32_t *) VGA_FB_VADDR;

    const size_t bar_size = get_bar_size(adapter->pcidev, 0);
    const uintptr_t fb_base = get_bar_address(adapter->pcidev, 0);

    cprintf("bar_size = %lu, fb_base = %p\n", bar_size, (void*)fb_base);

    if (!bar_size || !fb_base)
        return -SVGA_MAP_ERR;

    int res = sys_map_physical_region(fb_base, 0, (void*) adapter->fb_base_addr, bar_size, PROT_RW);
    if (res)
        return -SVGA_MAP_ERR;

    return SVGA_OK;
}

int virtio_init(uint32_t width, uint32_t hegiht) {
    int version = virtio_get_version();
    struct GraphicsAdapter * adapter = &virtio;
    struct PciDevice * pcidevice = find_pci_dev(PCI_CLASS_DISPLAY, 0);
    int res;
    adapter->pcidev = pcidevice;

    cprintf("VIRTIO version: 0x%X\n", version);
    res = virtio_set_display(adapter, 640, 400, 32, 1, 1);
    if (res)
        return res;

    res = bga_map_fb(adapter);
    if (res)
        return res;

    return 0;
}
