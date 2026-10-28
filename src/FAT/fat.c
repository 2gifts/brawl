#include <types.h>

// HYPOTHESIS: unnamed zero-initialized FAT driver buffers; sizes from the
// original .bss layout. Names are the address labels until their users are
// identified.
u8 lbl_8053A6E0[0xB68] = { 0 };
u8 lbl_8053B248[0x27FB8] = { 0 };
u8 lbl_80563200[0x18] = { 0 };
u8 lbl_80563218[0x68] = { 0 };
u8 lbl_80563280[0xD0] = { 0 };
u8 lbl_80563350[0x10] = { 0 };
u8 lbl_80563360[0x230] = { 0 };
u8 lbl_80563590[0x208] = { 0 };

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1200[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1208[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A120C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1210[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1214[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1218[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A121C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1220[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1224[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl, where each
// one was matched in a translation unit of its own. HYPOTHESIS: this unit spans
// several original source files, so its functions are not auto-inlined into one
// another; helpers the original inlined are marked inline.
#pragma auto_inline off

int fn_803A962C(int a) {
    if (a < 0x61)
        return a;
    if (a <= 0x7A)
        return a - 0x20;
    return a;
}

s32 fn_803A98A8(u8 *arg0, u8 *arg1) {
    u8 c;
    u8 d;
    while ((c = *arg0) != 0 && (d = *arg1) != 0 && c == d) {
        arg0++;
        arg1++;
    }
    return c - *arg1;
}

s32 fn_803A98E0(u8 *s1, u8 *s2, u32 n) {
    u32 i;
    u8 c1;

    for (i = n; i != 0; i--) {
        c1 = *s1;
        if (c1 == 0 || *s2 == 0 || c1 != *s2) {
            return c1 - *s2;
        }
        s1++;
        s2++;
    }
    return 0;
}

int VFiPFCODE_Combine_Width(int arg0, int arg1) {
    return (arg0 << 0x10) + arg1;
}

void VFiPFCODE_Divide_Width(unsigned int arg0, short *arg1, short *arg2) {
    *arg1 = arg0 >> 16;
    *arg2 = arg0;
}

s32 fn_803AA040(u16 *arg0, u16 *arg1, s32 arg2) {
    u16 *s1 = arg0;
    u16 *s2 = arg1;
    s32 n = arg2;

    while (n != 0) {
        u16 c1 = *s1;
        if (c1 == 0 || *s2 == 0 || c1 != *s2) {
            return c1 - *s2;
        }
        s1++;
        s2++;
        n--;
    }
    return 0;
}

int VFiPFDRV_finalize(int arg0) {
    return arg0 ? 0 : 10;
}

extern int fn_803ABA30(int);

int VFipdm_close_disk(int a) {
    if (a == 0) {
        return 1;
    }
    return fn_803ABA30(a);
}

void fn_803B2968(unsigned char *p)
{
    int i;

    p[0] = 0x2E;
    for (i = 1; i < 11; i++) {
        p[i] = 0x20;
    }
}

void fn_803B29A0(unsigned char *p)
{
    int i;

    for (i = 0; i < 2; i++) {
        p[i] = 0x2E;
    }
    for (i = 2; i < 11; i++) {
        p[i] = 0x20;
    }
}

extern void fn_803B4910(int, int, int, int, int, int);

void VFiPFENT_ITER_GetEntryOfPath(int a1, int a2, int a3, int a4, int a5) {
    fn_803B4910(a1, a2, a3, a4, 0, a5);
}

void fn_803B4910(int a, int b, int c, int d, int e, int f);

void fn_803B5644(int a, int b, int c, int d) {
    fn_803B4910(a, b, c, d, 1, 1);
}

void fn_803BD16C(void*, int, int, int);

void VFiPFPATH_SplitPath(void* p1, int p2, int p3) {
    fn_803BD16C(p1, p2, p3, 0);
}

void fn_803BD16C(void*, int, int, int);

void fn_803BE3D0(void* p1, int p2, int p3) {
    fn_803BD16C(p1, p2, p3, 1);
}

static inline int update_suffix(char *buf, u32 num, int i, int j,
                                u32 ext, u32 end, u32 *count,
                                u32 *position)
{
    char digits[8];
    char *p = digits;
    char *dst, *src;

    *count = 0;
    while (num != 0) {
        *p++ = num % 10 + '0';
        num /= 10;
        (*count)++;
    }

    if (i + (*count) >= ext) {
        (*position) = i + (*count) + 1;
        if ((*position) > 8)
            (*position) = 8;

        *(buf + (*position) + 4) = buf[end];
        *(buf + (*position) + 3) = *(buf + ext + 2);
        *(buf + (*position) + 2) = *(buf + ext + 1);
        *(buf + (*position) + 1) = buf[ext];
        buf[(*position)] = buf[j];
    }

    if (i + (*count) >= 8)
        i = 7 - (*count);

    buf[i++] = 0x7E;
    src = digits + (*count);
    dst = buf + i;

    while ((*count) > 0) {
        *dst++ = *--src;
        (*count)--;
    }

    return 0;
}

int VFiPFPATH_parseShortNameNumeric(char *buf, u32 num)
{
    int i, j;
    u32 n, ext, end, pos;

    if (num == 0)
        return 0;

    i = 1;
    while (buf[i] != 0x7E)
        i++;

    j = i + 1;
    while (buf[j] != '.' && buf[j] != 0)
        j++;

    ext = j + 1;
    if (buf[j] == '.' && buf[ext] != 0) {
        end = ext + 1;
        while (buf[end] != 0)
            end++;
    } else {
        end = ext;
    }

    return update_suffix(buf, num, i, j, ext, end, &n, &pos);
}

s32 VFiPFCODE_CP932_isOEMMBchar(u32 value, s32 type)
{
    u8 c = value;

    switch (type) {
    case 1:
        return (c >= 0x81U && c <= 0x9FU) ||
               (c >= 0xE0U && c <= 0xFCU);
    case 2:
        return (c >= 0x40U && c <= 0x7EU) ||
               (c >= 0x80U && c <= 0xFCU);
    default:
        return 0;
    }
}

int VFiPFAPI_convertReturnValue2NULL(int arg0, void *arg1) {
    if (arg0 != 0) {
        return 0;
    }
    return (int)arg1;
}

s32 VFiPFAPI_convertReturnValue4unmount(s32 arg0) {
    if (arg0 == 0) {
        return 0;
    }
    return (arg0 == 1) ? 1 : -1;
}

int VFiPFSYS_GetCurrentContextID(int *arg0) {
    *arg0 = 0;
    return 0;
}

int dCommon_GetPhysicalOffset(int arg0, int arg1, int arg2) {
    return arg1 * (arg0 - arg2) + 0x20;
}

s32 dCommon_GetNiceFatType(u32 *arg0, u32 arg1, u32 arg2, u32 arg3) {
    u32 q;

    q = arg1 / arg2;
    if (q < 0xFF5U) {
        if (arg0 != 0) {
            *arg0 = (arg3 + ((q * 4 - q) >> 1) - 1) / arg3;
        }
        return 0;
    }
    if (q < 0xFFF5U) {
        if (arg0 != 0) {
            *arg0 = (arg3 + (q * 2) - 1) / arg3;
        }
        return 1;
    }
    if (arg0 != 0) {
        *arg0 = (arg3 + (q * 4) - 1) / arg3;
    }
    return 2;
}

int dCommon_GetReservedSecFromFatType(int arg0)
{
    if (arg0 == 2) {
        return 0x20;
    }
    return 1;
}

int dCommon_GetRootEntNumFromFatType(int arg0) {
    return arg0 == 2 ? 0 : 0x80;
}
