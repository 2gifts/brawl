#include <types.h>

int RFLiGetMaxMaskRsl(int size) {
    switch (size) {
    case 0x40:
    case 0x60:
        return 0x40;
    case 0x80:
    case 0xE0:
        return 0x80;
    case 0x100:
    case 0x1E0:
        return 0x100;
    default:
        return 0x40;
    }
}

u32 RFLiGetMaskBufSize(u32 flags) {
    u32 size = 0;
    if (flags & 0x20) {
        size = 0x800;
    }
    if (flags & 0x40) {
        size += 0x2000;
    }
    if (flags & 0x80) {
        size += 0x8000;
    }
    if (flags & 0x100) {
        size += 0x20000;
    }
    return size;
}
