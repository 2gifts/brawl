// DWC NP utility functions (not decompiled yet).

#include <revolution/OS/OS.h>
// TODO: clean up, give variables real names
/* jj_8034F020 */
extern int lbl_805A0FB8;

void fn_8034F020(int arg0) {
    lbl_805A0FB8 = arg0;
}

// TODO: clean up, give variables real names
/* jj_8034F684 */
extern int VFIsAvailable(void);
extern int DWC_AllocEx(int, int, int);
extern void VFInitEx(int, int);
extern int NWC24GetMyUserId(int *);
extern void DWC_Printf(int, const char *, ...);
extern void VFFinalize(void);
extern void DWC_Free(int, int, int);
extern int lbl_805A0FBC;
extern int lbl_805A0FC0;
extern int lbl_805A0FC4;
extern const char lbl_8048D2A0[];
extern const char lbl_8048D2C4[];

unsigned long long DWCi_Np_GetConsoleId(void) {
    int r30, r29, r28;
    if (lbl_805A0FBC == 0) {
        r30 = VFIsAvailable();
        r29 = DWC_AllocEx(3, 0x4000, 0x20);
        r28 = DWC_AllocEx(3, 0x8000, 0x20);
        lbl_805A0FBC = 1;
        if (r30 != 1)
            VFInitEx(r28, 0x8000);
        if (NWC24GetMyUserId(&lbl_805A0FC0) == 0)
            DWC_Printf(0x08000000, lbl_8048D2A0, lbl_805A0FC0, lbl_805A0FC4);
        else {
            DWC_Printf(0x08000000, lbl_8048D2C4);
            lbl_805A0FC4 = 0;
            lbl_805A0FC0 = 0;
        }
        if (r30 != 1)
            VFFinalize();
        DWC_Free(3, r29, 0);
        DWC_Free(3, r28, 0);
    }
    return ((unsigned long long)(unsigned int)lbl_805A0FC0 << 32) | (unsigned int)lbl_805A0FC4;
}

// TODO: clean up, give variables real names
/* jj_8034F79C */
extern int lbl_8059F1B8;

int* DWCi_Np_GetConsoleName(void) {
    return &lbl_8059F1B8;
}

// TODO: clean up, give variables real names
/* jj_8034F7A4 */

const char* DWCi_Np_GetAppGameName(void) {
    return OSGetAppGamename();
}

// TODO: clean up, give variables real names
/* jj_8034F028 */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long size_t;

void* memcpy(void*, const void*, size_t);

#define CP(d, s) { u32 t = *(s)++; *(d)++ = t; }

void DWCi_Np_CPUCopyFast(void* dst, const void* src, u32 n) {
    s32 hw;
    s32 i;
    u16* dh;
    const u16* sh;
    u32* dw;
    const u32* sw;
    u8* db;
    const u8* sb;

    if (n & 0x80000000) {
        memcpy(dst, src, n);
    }
    if (((u32)dst & 3) || ((u32)src & 3)) {
        if (((u32)dst & 1) || ((u32)src & 1)) {
            memcpy(dst, src, n);
            return;
        }
        dh = (u16*)dst;
        sh = (const u16*)src;
        hw = n >> 1;
        if (hw != 0) {
            for (i = hw >> 4; i > 0; i--) {
                *dh++ = *sh++; *dh++ = *sh++; *dh++ = *sh++; *dh++ = *sh++;
                *dh++ = *sh++; *dh++ = *sh++; *dh++ = *sh++; *dh++ = *sh++;
                *dh++ = *sh++; *dh++ = *sh++; *dh++ = *sh++; *dh++ = *sh++;
                *dh++ = *sh++; *dh++ = *sh++; *dh++ = *sh++; *dh++ = *sh++;
            }
            switch (hw & 0xF) {
            case 15: *dh++ = *sh++;
            case 14: *dh++ = *sh++;
            case 13: *dh++ = *sh++;
            case 12: *dh++ = *sh++;
            case 11: *dh++ = *sh++;
            case 10: *dh++ = *sh++;
            case 9: *dh++ = *sh++;
            case 8: *dh++ = *sh++;
            case 7: *dh++ = *sh++;
            case 6: *dh++ = *sh++;
            case 5: *dh++ = *sh++;
            case 4: *dh++ = *sh++;
            case 3: *dh++ = *sh++;
            case 2: *dh++ = *sh++;
            case 1: *dh++ = *sh++;
            case 0: break;
            }
        }
        db = (u8*)dh;
        sb = (const u8*)sh;
        switch (n & 1) {
        case 1: *db = *sb;
        }
        return;
    }

    dw = (u32*)dst;
    sw = (const u32*)src;
    hw = n >> 2;
    if (hw != 0) {
        for (i = hw >> 3; i > 0; i--) {
            CP(dw, sw) CP(dw, sw) CP(dw, sw) CP(dw, sw)
            CP(dw, sw) CP(dw, sw) CP(dw, sw) CP(dw, sw)
        }
        switch (hw & 7) {
        case 7: *dw++ = *sw++;
        case 6: *dw++ = *sw++;
        case 5: *dw++ = *sw++;
        case 4: *dw++ = *sw++;
        case 3: *dw++ = *sw++;
        case 2: *dw++ = *sw++;
        case 1: *dw++ = *sw++;
        case 0: break;
        }
    }
    db = (u8*)dw;
    sb = (const u8*)sw;
    switch (n & 3) {
    case 3: *db++ = *sb++;
    case 2: *db++ = *sb++;
    case 1: *db++ = *sb++;
    }
}

// TODO: clean up, give variables real names
/* jj_8034F57C */
typedef unsigned long long u64;

extern u64 OSGetTime(void);
extern u64 __div2i(u64 a, u64 b);

u64 fn_8034F57C(void) {
    u64 t = OSGetTime();
    u32 d = *(u32 *)0x800000F8 >> 2;
    return __div2i(t, (u64)d) + 0x386D4380ULL;
}

