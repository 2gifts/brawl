#include "revolution.h"

extern u8 lbl_804E89E0[];
extern u32 lbl_805A0AD8;
extern u32 lbl_805A0ADC;
extern u32 lbl_805A0AE0;
extern u32 lbl_805A0AE4;
extern u32 lbl_805A0AE8;
extern u32 lbl_805A0AEC;
extern u32 lbl_805A0AF0;
extern u32 lbl_805A0AF4;
extern u32 lbl_805A0AF8;
extern u32 lbl_805A0AFC;
extern u32 lbl_805A0B00;
extern u32 lbl_805A0B04;
extern u32 lbl_805A0B08;
extern u32 lbl_805A0B0C;
extern u32 lbl_805A0B10;
extern u32 lbl_805A0B14;
extern u32 lbl_805A0B18;
extern u32 lbl_805A0B1C;
extern u32 lbl_805A0B20;
extern u32 lbl_805A0B24;
extern u32 lbl_805A0B38;
extern u32 lbl_805A0B28;

void* __AXGetStudio(void) {
    return lbl_804E89E0;
}

void __AXSPBInit(void) {
    lbl_805A0AD8 = 0;
    lbl_805A0ADC = 0;
    lbl_805A0AE0 = 0;
    lbl_805A0AE4 = 0;
    lbl_805A0AE8 = 0;
    lbl_805A0AEC = 0;
    lbl_805A0AF0 = 0;
    lbl_805A0AF4 = 0;
    lbl_805A0AF8 = 0;
    lbl_805A0AFC = 0;
    lbl_805A0B00 = 0;
    lbl_805A0B04 = 0;
    lbl_805A0B08 = 0;
    lbl_805A0B0C = 0;
    lbl_805A0B10 = 0;
    lbl_805A0B14 = 0;
    lbl_805A0B18 = 0;
    lbl_805A0B1C = 0;
    lbl_805A0B20 = 0;
    lbl_805A0B24 = 0;
}

void __AXSPBQuit(void) {}

u32 __AXGetNumVoices(void) {
    return lbl_805A0B38;
}

u32 __AXGetPBs(void) {
    return lbl_805A0B28;
}

// TODO: clean up, give variables real names
/* jj_802012DC */
void __AXDepopFadeRmt(int *arg0, int *arg1, short *arg2) {
    int q;

    q = *arg0 / 18;
    if (q != 0) {
        if (q > 20) {
            q = 20;
        }
        if (q < -20) {
            q = -20;
        }
        *arg1 = *arg0;
        *arg0 -= q * 18;
        *arg2 = -q;
        return;
    }
    *arg0 = 0;
    *arg1 = 0;
    *arg2 = 0;
}

// TODO: clean up, give variables real names
/* jj_8020151C */
#define lbl_805A0AD8 (*(int*)&lbl_805A0AD8)
#define lbl_805A0ADC (*(int*)&lbl_805A0ADC)
#define lbl_805A0AE0 (*(int*)&lbl_805A0AE0)
#define lbl_805A0AE4 (*(int*)&lbl_805A0AE4)
#define lbl_805A0AE8 (*(int*)&lbl_805A0AE8)
#define lbl_805A0AEC (*(int*)&lbl_805A0AEC)
#define lbl_805A0AF0 (*(int*)&lbl_805A0AF0)
#define lbl_805A0AF4 (*(int*)&lbl_805A0AF4)
#define lbl_805A0AF8 (*(int*)&lbl_805A0AF8)
#define lbl_805A0AFC (*(int*)&lbl_805A0AFC)
#define lbl_805A0B00 (*(int*)&lbl_805A0B00)
#define lbl_805A0B04 (*(int*)&lbl_805A0B04)
#define lbl_805A0B08 (*(int*)&lbl_805A0B08)
#define lbl_805A0B0C (*(int*)&lbl_805A0B0C)
#define lbl_805A0B10 (*(int*)&lbl_805A0B10)
#define lbl_805A0B14 (*(int*)&lbl_805A0B14)
#define lbl_805A0B18 (*(int*)&lbl_805A0B18)
#define lbl_805A0B1C (*(int*)&lbl_805A0B1C)
#define lbl_805A0B20 (*(int*)&lbl_805A0B20)
#define lbl_805A0B24 (*(int*)&lbl_805A0B24)

void __AXDepopVoice(void *arg0) {
    char *p = (char *)arg0;
    lbl_805A0B24 += *(short *)(p + 0x52);
    lbl_805A0B18 += *(short *)(p + 0x54);
    lbl_805A0B0C += *(short *)(p + 0x56);
    lbl_805A0B00 += *(short *)(p + 0x58);
    lbl_805A0B20 += *(short *)(p + 0x5A);
    lbl_805A0B14 += *(short *)(p + 0x5C);
    lbl_805A0B08 += *(short *)(p + 0x5E);
    lbl_805A0AFC += *(short *)(p + 0x60);
    lbl_805A0B1C += *(short *)(p + 0x62);
    lbl_805A0B10 += *(short *)(p + 0x64);
    lbl_805A0B04 += *(short *)(p + 0x66);
    lbl_805A0AF8 += *(short *)(p + 0x68);
    lbl_805A0AF4 += *(short *)(p + 0xFA);
    lbl_805A0AF0 += *(short *)(p + 0xFC);
    lbl_805A0AEC += *(short *)(p + 0xFE);
    lbl_805A0AE8 += *(short *)(p + 0x100);
    lbl_805A0AE4 += *(short *)(p + 0x102);
    lbl_805A0AE0 += *(short *)(p + 0x104);
    lbl_805A0ADC += *(short *)(p + 0x106);
    lbl_805A0AD8 += *(short *)(p + 0x108);
}
#undef lbl_805A0AD8
#undef lbl_805A0ADC
#undef lbl_805A0AE0
#undef lbl_805A0AE4
#undef lbl_805A0AE8
#undef lbl_805A0AEC
#undef lbl_805A0AF0
#undef lbl_805A0AF4
#undef lbl_805A0AF8
#undef lbl_805A0AFC
#undef lbl_805A0B00
#undef lbl_805A0B04
#undef lbl_805A0B08
#undef lbl_805A0B0C
#undef lbl_805A0B10
#undef lbl_805A0B14
#undef lbl_805A0B18
#undef lbl_805A0B1C
#undef lbl_805A0B20
#undef lbl_805A0B24

// TODO: clean up, give variables real names
/* jj_80201BE0 */
#define __AXServiceVPB __AXServiceVPB_jj80201BE0
typedef unsigned short u16;

extern s32 lbl_804780E0[];
extern s32 lbl_80478160[];
#define lbl_805A0B28 (*(s32*)&lbl_805A0B28)
extern s32 lbl_805A0B2C;
extern s32 lbl_805A0B34;
#define lbl_805A0B38 (*(s32*)&lbl_805A0B38)
extern u32 lbl_805A0B3C;
extern u32 lbl_805A0B40;

void DCFlushRange(void *buf, u32 len);
void DCInvalidateRange(void *dst, u32 len);
void *__AXGetStackHead(s32 idx);
void __AXPushCallbackStack(void *node);
s32 __AXGetCommandListCycles(void);
void __AXDepopVoice(void *obj);
void __AXServiceVPB(void *node);

void __AXSyncPBs(s32 arg0)
{
    s32 total;
    u32 i;
    char *node;
    char *item;
    u32 v;
    u16 x;
    u32 f;
    u32 r4v;
    s32 var;
    s32 sumA;
    s32 sumB;
    s32 sumC;
    s32 sumD;
    s32 sumE;
    s32 sumF;
    s32 sumG;

    lbl_805A0B38 = 0;
    DCInvalidateRange((void *)lbl_805A0B28, lbl_805A0B34 * 0x140);
    DCInvalidateRange((void *)lbl_805A0B2C, lbl_805A0B34 << 6);
    v = (u32)((char *)__AXGetCommandListCycles() + lbl_805A0B34 * 0x258);
    v = (u32)((char *)v + arg0);
    total = v + 0x20;
    i = 0x1F;
    do {
        node = __AXGetStackHead(i);
        while (node != 0) {
            if (*(u16 *)(node + 0x6C) == 1) {
                total += 0x81;
            }
            if (*(s32 *)(node + 0x20) != 0) {
                __AXDepopVoice((void *)(lbl_805A0B28 + *(s32 *)(node + 0x18) * 0x140));
            }
            if (*(u16 *)(node + 0x38) == 1) {
                total += 0x183;
                if (*(u16 *)(node + 0xE2) != 0) {
                    total += 0x135;
                }
                if (*(u16 *)(node + 0xEA) != 0) {
                    total += 0x400;
                }
                if (*(u16 *)(node + 0x6C) == 1) {
                    total += 0x1B;
                }
                f = *(u16 *)(node + 0x30);
                r4v = ((u32)*(u16 *)(node + 0xCE) << 16) | *(u16 *)(node + 0xD0);
                if (f == 0) {
                    var = (s32)((((r4v << 9) + 0x8000) >> 16) + 0x619);
                } else {
                    var = 0x25D;
                    if (f == 1) {
                        var = (s32)((((r4v << 9) + 0x8000) >> 16) + 0x5BA);
                    }
                }
                v = *(u32 *)(node + 0x34);
                total += var;
                total += lbl_804780E0[v & 0x1F] + lbl_804780E0[(v >> 16) & 0x1F] + lbl_804780E0[(v >> 21) & 0x1F] + lbl_804780E0[(v >> 26) & 0x1F];
                if (*(u16 *)(node + 0xFE) == 1) {
                    total += 0x265;
                    f = *(u16 *)(node + 0x13C);
                    if (f == 1) {
                        total += 0x76;
                    } else if (f == 2) {
                        total += 0x342;
                    }
                    x = *(u16 *)(node + 0x100);
                    total += lbl_80478160[x & 3] + lbl_80478160[(x >> 2) & 3] + lbl_80478160[(x >> 4) & 3] + lbl_80478160[(x >> 6) & 3] + lbl_80478160[(x >> 8) & 3] + lbl_80478160[(x >> 10) & 3] + lbl_80478160[(x >> 12) & 3] + lbl_80478160[(x >> 14) & 3];
                }
                if (lbl_805A0B40 > (u32)total) {
                    __AXServiceVPB(node);
                } else {
                    item = (char *)lbl_805A0B28 + *(s32 *)(node + 0x18) * 0x140;
                    if (*(u16 *)(item + 0x10) == 1) {
                        __AXDepopVoice(item);
                    }
                    *(u16 *)(node + 0x38) = 0;
                    *(u16 *)(item + 0x10) = 0;
                    __AXPushCallbackStack(node);
                }
            } else {
                __AXServiceVPB(node);
            }
            *(s32 *)(node + 0x1C) = 0;
            *(s32 *)(node + 0x20) = 0;
            node = *(char **)node;
        }
        i--;
    } while (i != 0);
    lbl_805A0B3C = total;
    {
        char *last = __AXGetStackHead(0);
        while (last != 0) {
            if (*(s32 *)(last + 0x20) != 0) {
                __AXDepopVoice((void *)(lbl_805A0B28 + *(s32 *)(last + 0x18) * 0x140));
            }
            *(s32 *)(last + 0x20) = 0;
            *(u16 *)((char *)lbl_805A0B28 + *(s32 *)(last + 0x18) * 0x140 + 0x10) = 0;
            last = *(char **)last;
        }
    }
    DCFlushRange((void *)lbl_805A0B28, lbl_805A0B34 * 0x140);
    DCFlushRange((void *)lbl_805A0B2C, lbl_805A0B34 << 6);
}
#undef lbl_805A0B28
#undef lbl_805A0B38
#undef __AXServiceVPB

