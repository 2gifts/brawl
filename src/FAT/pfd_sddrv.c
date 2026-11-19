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

// TODO: clean up, give variables real names
extern char lbl_8041F3B8[];

struct fn_803EE8E0_obj {
    void *unk0;
    int unk4;
};

// TODO: clean up, give variables real names
int fn_803EE8E0(struct fn_803EE8E0_obj *self, int arg) {
    self->unk4 = arg;
    self->unk0 = lbl_8041F3B8;
    return 0;
}


// TODO: clean up, give variables real names
int fn_803EE5A4(int arg0) {
    if (arg0 == 0) {
        return 1;
    }
    return ((int*)&lbl_80599680)[4] ? 0 : 6;
}

// TODO: clean up, give variables real names
/* jj_803EE760 */
#define lbl_80599680 ((int*)&lbl_80599680)
extern char lbl_804931C0[];
extern int fn_803CB314(int, int *);
extern int fn_803EED98(void *, void *);
extern void OSReport(const char *, ...);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803EE760(int a, int *b)
{
    char *s = lbl_804931C0;
    int sp8;
    int e;
    int r;
    if (a == 0 || b == 0) return 1;
    if (!(lbl_80599680[0] & 2)) return 15;
    if (lbl_80599680[5] != 0) return 15;
    if (lbl_80599680[4] == 0) return 6;
    e = fn_803CB314(lbl_80599680[2], &sp8);
    if (e != 0) {
        OSReport(s + 0x130, e);
        return 0x15;
    }
    if (!(sp8 & 1)) {
        OSReport(s + 0x318);
        return 6;
    }
    if (sp8 & 2) {
        OSReport(s + 0x350);
        return 6;
    }
    if (!(sp8 & 0x10000)) {
        OSReport(s + 0x384);
        return 0x15;
    }
    b[3] = 4;
    if (sp8 & 4) b[3] |= 1;
    r = fn_803EED98(b, b + 2);
    if (r != 0) {
        OSReport(s + 0x3C4);
        return r;
    }
    *(short *)((char *)b + 4) = 0;
    *((char *)b + 6) = 0;
    *((char *)b + 7) = 0;
    b[4] = 0;
    return 0;
}
#undef lbl_80599680

/* jj_803EE3E0 */
#define lbl_80599680 ((int*)&lbl_80599680)
extern char lbl_804931C0[];
extern int fn_803CB83C(int);
extern int fn_803CC360(int, int, void *, int);
extern void VFipf_memset(void *, int, unsigned int);
extern void OSReport(const char *, ...);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_803EE3E0(int a)
{
    char *s;
    unsigned int count;
    int r;
    int spC;
    int sp10[4];
    short sp8;

    s = lbl_804931C0;
    if (a == 0) return 1;
    lbl_80599680[5] = 0;
    if (lbl_80599680[4 + lbl_80599680[5]] == 0) return 6;
    if (lbl_80599680[0] & 2) {
        OSReport(s + 0x1A4);
        return 0;
    }
    count = 0;
    for (;;) {
        if (lbl_80599680[4] == 0) return 6;
        r = fn_803CB83C(lbl_80599680[2]);
        if (r == 0) break;
        OSReport(s + 0x1D4, r);
        count++;
        if (count >= 5) break;
    }
    if (count == 5) return 0x15;
    if (lbl_80599680[5] != 0) return 6;
    spC = 0;
    if (fn_803CC360(lbl_80599680[2], 0x29, &spC, 4) != 0) {
        OSReport(s + 0x214);
        return 0x15;
    }
    VFipf_memset(sp10, 0, 0x10);
    if (fn_803CC360(lbl_80599680[2], 0xA, sp10, 0x10) != 0) {
        OSReport(s + 0x244);
        return 0x15;
    }
    sp8 = 0;
    if (fn_803CC360(lbl_80599680[2], 3, &sp8, 2) != 0) {
        OSReport(s + 0x274);
        return 0x15;
    }
    lbl_80599680[0] |= 2;
    return 0;
}
#undef lbl_80599680

