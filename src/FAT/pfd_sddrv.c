#include <types.h>

// HYPOTHESIS: SD driver state; field meanings guessed from the offsets used.
typedef struct SdDrvState {
    u32 flags;
    u32 unk4;
    int unk8;
    u32* unkC;
    u32 unk10;
    u32 unk14;
    s8 mediaChar;
    u8 unk19[7];
} SdDrvState;

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
SdDrvState lbl_80599680 __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805996A0[0x40] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805996E0[0x200] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
void* lbl_805A12A0 __attribute__((aligned(4))) = 0;
void* lbl_805A12A4 __attribute__((aligned(4))) = 0;
unsigned char lbl_805A12A8[0x18] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A12C0[0x8] __attribute__((aligned(4))) = { 0 };

extern int pfd_sddrv_physical_read(int sector, void* buffer, int arg2, u32 drive, int count);
extern int pfd_sddrv_physical_write(int sector, void* buffer, int arg2, u32 drive, int count);
extern int pfd_get_media_drv_char(u32* handle, u8* out, int count);
// pdm_disk_notify_media_insert (the map name); src/FAT/pf_driver.c calls it by this stand-in name.
extern void fn_803D80C4(u32* self);

int pfd_sddrv_registar_callback(void* callback, void* userData) {
    lbl_805A12A0 = callback;
    lbl_805A12A4 = userData;
    return 0;
}

int pfd_sddrv_pread(void* handle, void* buffer, int arg2, int sector, int count) {
    if (handle == 0 || buffer == 0 || count == 0) {
        return 1;
    }
    return pfd_sddrv_physical_read(sector, buffer, arg2, lbl_80599680.unk4, count);
}

int pfd_sddrv_pwrite(void* handle, void* buffer, int arg2, int sector, int count) {
    if (handle == 0 || buffer == 0 || count == 0) {
        return 1;
    }
    return pfd_sddrv_physical_write(sector, buffer, arg2, lbl_80599680.unk4, count);
}

int pfd_sddrv_is_media_insert(void) {
    u8 mediaChar;
    if (lbl_80599680.flags & 1) {
        if (lbl_80599680.mediaChar == 0) {
            if ((u8)pfd_get_media_drv_char(lbl_80599680.unkC, &mediaChar, 1) == 1) {
                lbl_80599680.mediaChar = mediaChar;
            }
        }
        if (lbl_80599680.unk10 != 0) {
            fn_803D80C4(lbl_80599680.unkC);
            return 1;
        }
        return 0;
    }
    return 0;
}
