#include <revolution/DVD/dvd.h>
#include <revolution/OS/OSMessage.h>
#include <mv/mv_THPAudioDecode.h>
#include <mv/mv_THPDraw.h>
#include <mv/mv_THPPlayer.h>
#include <mv/mv_THPRead.h>
#include <mv/mv_THPVideoDecode.h>
#include <revolution/AX/AXVPB.h>
#include <revolution/GX/GXMisc.h>
#include <revolution/OS/OSCache.h>
#include <revolution/VI/vi.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
s32 lbl_805A01E8 __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805A01EC[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A01F0[0x4] __attribute__((aligned(4))) = { 0 };
s32 lbl_805A01F4 __attribute__((aligned(4))) = { 0 };
// HYPOTHESIS: work pointer views and unknown work fields follow observed accesses.
union {
    void* unk0;
    struct {
        s32 unk0;
        u8 unk4[0x79C];
        OSMessageQueue unk7A0;
        OSMessageQueue unk7C0;
        OSMessage unk7E0[2];
        OSMessage unk7E8[3];
        u32 unk7F4;
        union { u32 unk7F8; _AXVPB* unk7F8Typed; };
        union { u32 unk7FC; _AXVPB* unk7FCTyped; };
        u32 unk800;
        u32 unk804;
        u64 unk808[5];
        u32 unk830;
        s32 unk834;
        u8 unk838[0x10];
        u64 unk848;
    } *unk0Typed;
} lbl_805A01F8[2] = { 0 };

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
UnkTHPClass g_thpObject __attribute__((aligned(8))) = { 0 };

BOOL THPPlayerInit() {
    memset(&g_thpObject, 0, 0x1B0);
    LCEnable();
    OSInitMessageQueue(&lbl_805A01F8[0].unk0Typed->unk7C0, lbl_805A01F8[0].unk0Typed->unk7E8, 3);
    if (!THPInit()) return FALSE;
    lbl_805A01F8[0].unk0Typed->unk0 = 1;
    return TRUE;
}

BOOL THPPlayerOpen(const char* path, s32 mode) {
    if (!lbl_805A01F8[0].unk0Typed->unk0) return FALSE;
    if (g_thpObject.unkA0) return FALSE;
    memset(&g_thpObject.unk80Width, 0, 12);
    memset(&g_thpObject.unk8C, 0, 16);
    if (!DVDOpen(path, &g_thpObject.m_fInfo)) return FALSE;
    lbl_805A01F4 = mode;
    lbl_805A01E8 = 0;
    return TRUE;
}

void InitAllMessageQueue() {
    u32 (*readBuffer)[3];
    THPTextureSet* textureBuffer;
    THPAudioBuffer* audioBuffer;
    int i;
    if (!g_thpObject.unkB0) {
        readBuffer = g_thpObject.unkF0;
        for (i = 0; i < 6; ++i, ++readBuffer) PushFreeReadBuffer(readBuffer);
    }
    textureBuffer = g_thpObject.unk138;
    for (i = 0; i < 3; ++i, ++textureBuffer) PushFreeTextureSet(textureBuffer);
    if (g_thpObject.unkA7Flag) {
        audioBuffer = g_thpObject.unk168;
        for (i = 0; i < 6; ++i, ++audioBuffer) PushFreeAudioBuffer(audioBuffer);
    }
    OSInitMessageQueue(&lbl_805A01F8[0].unk0Typed->unk7A0, lbl_805A01F8[0].unk0Typed->unk7E0, 2);
}

BOOL ProperTimingForGettingNextFrame() {
    if (g_thpObject.unk88 & 1) {
        if (VIGetNextField() == 0) return TRUE;
    } else if (g_thpObject.unk88 & 2) {
        if (VIGetNextField() == 1) return TRUE;
    } else {
        s32 rate = 100.0f * g_thpObject.unk4C;
        if ((u32)VIGetTvFormat() == 1)
            g_thpObject.unkD4 = (g_thpObject.unkC8 * rate) / 5000;
        else
            g_thpObject.unkD4 = (g_thpObject.unkC8 * rate) / 5994;
        s32 frame = g_thpObject.unkD4;
        if (g_thpObject.unkD0 != frame) {
            g_thpObject.unkD0 = frame;
            return TRUE;
        }
    }
    return FALSE;
}

s32 THPPlayerDrawCurrentFrame(int width, int height, int x, int y, int polygonWidth, int polygonHeight) {
    if (g_thpObject.unkA0 && g_thpObject.unkA4 && g_thpObject.unkE8) {
        THPGXYuv2RgbSetup(width, height);
        THPTextureSet* texture = g_thpObject.unkE8;
        THPGXYuv2RgbDraw(texture->ytexture, texture->utexture,
            texture->vtexture, x, y, g_thpObject.unk80Width, g_thpObject.unk84,
            polygonWidth, polygonHeight);
        THPGXRestore();
        return (g_thpObject.unkE8->frameNumber + g_thpObject.unkC0) % g_thpObject.unk50;
    }
    return -1;
}

void SetTHPPlayerWork(void* work) {
    if (lbl_805A01F8[0].unk0Typed) return;
    lbl_805A01F8[0].unk0 = work;
    lbl_805A01F8[0].unk0Typed->unk0 = 0;
    lbl_805A01F8[0].unk0Typed->unk7F4 = 0;
    lbl_805A01F8[0].unk0Typed->unk7F8 = 0;
    lbl_805A01F8[0].unk0Typed->unk7FC = 0;
    lbl_805A01F8[0].unk0Typed->unk800 = 0;
    lbl_805A01F8[0].unk0Typed->unk804 = 0;
}

void movieOpen::freeMovieBuffer() {
    LCDisable();
    if (lbl_805A01F8[0].unk0Typed) lbl_805A01F8[0].unk0Typed->unk0 = 0;
}

BOOL THPPlayerGetVideoInfo(void* info) {
    if (g_thpObject.unkA0) {
        memcpy(info, &g_thpObject.unk80Width, 12);
        return TRUE;
    }
    return FALSE;
}

BOOL THPPlayerGetAudioInfo(void* info) {
    if (g_thpObject.unkA0) {
        memcpy(info, &g_thpObject.unk8C, 16);
        return TRUE;
    }
    return FALSE;
}

u32 THPPlayerGetTotalFrame() {
    if (g_thpObject.unkA0) return g_thpObject.unk50;
    return 0;
}

void THPPlayerDrawDone() {
    GXDrawDone();
    if (lbl_805A01F8[0].unk0Typed->unk0) {
        while (true) {
            OSMessage message;
            void* texture;
            if (OSReceiveMessage(&lbl_805A01F8[0].unk0Typed->unk7C0, &message, 0) == TRUE)
                texture = message;
            else
                texture = 0;
            if (!texture) break;
            PushFreeTextureSet(texture);
        }
    }
}

void FillStreamBuffer(s16* right, s16* left, u32 count) {
    u64 total = lbl_805A01F8[0].unk0Typed->unk848;
    u32 remaining = count;
    s16* rightCursor = right;
    s16* leftCursor = left;
    while (true) {
        s32 status;
        u32 copied = GetAudioSample(rightCursor, leftCursor, remaining, &status);
        total += copied;
        if (status == 0) break;
        if (status == 1) {
            remaining -= copied;
            rightCursor += copied;
            if (leftCursor) leftCursor += copied;
            s32& index = lbl_805A01F8[0].unk0Typed->unk834;
            lbl_805A01F8[0].unk0Typed->unk808[index] = total;
            ++index;
            if (lbl_805A01F8[0].unk0Typed->unk834 >= 5)
                lbl_805A01F8[0].unk0Typed->unk834 = 0;
        } else {
            memset(rightCursor, 0, remaining * sizeof(s16));
            if (leftCursor) memset(leftCursor, 0, remaining * sizeof(s16));
            break;
        }
    }
    lbl_805A01F8[0].unk0Typed->unk848 += count;
}

void THPPlayerGetStreamAXPB(_AXVPB** right, _AXVPB** left) {
    *right = lbl_805A01F8[0].unk0Typed->unk7F8Typed;
    *left = lbl_805A01F8[0].unk0Typed->unk7FCTyped;
}

u32 GetSizeTHPPlayerWork() {
    return 0x860;
}

void FreeTHPPlayerWork() {
    lbl_805A01F8[0].unk0 = 0;
}

s32 THPPlayerGetState() {
    return g_thpObject.unkA4;
}

BOOL THPPlayerClose() {
    if (g_thpObject.unkA0 && !g_thpObject.unkA4) {
        g_thpObject.unkA0 = 0;
        DVDClose(&g_thpObject.m_fInfo);
        return TRUE;
    }
    return FALSE;
}

BOOL THPPlayerPause() {
    if (g_thpObject.unkA0 && g_thpObject.unkA4 == 2) {
        g_thpObject.unkA5 = 4;
        g_thpObject.unkA4 = 4;
        if (g_thpObject.unkA7Flag) {
            if (lbl_805A01F8[0].unk0Typed->unk7F8Typed) {
                AXSetVoiceState(lbl_805A01F8[0].unk0Typed->unk7F8Typed, 0);
            }
            if (lbl_805A01F8[0].unk0Typed->unk7FCTyped) {
                AXSetVoiceState(lbl_805A01F8[0].unk0Typed->unk7FCTyped, 0);
            }
        }
        return TRUE;
    }
    return FALSE;
}

BOOL THPPlayerPlay() {
    if (g_thpObject.unkA0) {
        u8 state = g_thpObject.unkA4;
        if (state == 1 || state == 4) {
            if (state == 4) {
                if (g_thpObject.unkA7Flag) {
                    StreamReInit();
                }
            }
            g_thpObject.unkA4 = 2;
            g_thpObject.unkD0 = 0;
            g_thpObject.unkD4 = 0;
            g_thpObject.unkC8 = -1;
            return TRUE;
        }
    }
    return FALSE;
}

// TODO: clean up, give variables real names
namespace jj_8007C9DC {

struct Work {
    char pad[0x7c0];
    OSMessageQueue queue;
    char pad2[0x7f4 - 0x7c0 - sizeof(OSMessageQueue)];
    int value;
    int* first;
    int* second;
};
extern Work* lbl_805A01F8;
extern "C" int fn_801E8078(int);
extern "C" int fn_801FB0F8(void*);
extern "C" void fn_8007E4C4();
extern "C" void fn_80207BC0(int*);
extern "C" void fn_801FF4CC(void*);

// TODO: clean up, give variables real names
extern "C" void fn_8007C9DC() {
    if (*(int*)&g_thpObject.unk80[0x20] && g_thpObject.unk80[0x24]) {
        g_thpObject.unk80[0x25] = 0;
        g_thpObject.unk80[0x24] = 0;
        fn_801E8078(lbl_805A01F8->value);
        if (!*(int*)&g_thpObject.unkAC[4]) {
            fn_801FB0F8(&g_thpObject);
            ReadThreadCancel();
        }
        fn_8007E4C4();
        if (g_thpObject.unkA7[0]) {
            if (lbl_805A01F8->first) {
                fn_80207BC0(lbl_805A01F8->first);
                fn_801FF4CC(lbl_805A01F8->first);
                lbl_805A01F8->first = 0;
            }
            if (lbl_805A01F8->second) {
                fn_80207BC0(lbl_805A01F8->second);
                fn_801FF4CC(lbl_805A01F8->second);
                lbl_805A01F8->second = 0;
            }
            AudioDecodeThreadCancel();
        }
        OSMessage msg;
        void* result;
        do {
            if (OSReceiveMessage(&lbl_805A01F8->queue, &msg, 0) == 1)
                result = msg;
            else
                result = 0;
        } while (result);
        g_thpObject.unkA8 = 0;
        *(int*)&g_thpObject.unkAC[0] = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8007BE4C {

extern "C" {
extern char* lbl_805A01F8;
extern int lbl_805A01E8;
extern int lbl_805A01EC;
extern unsigned int lbl_805A01F0;
extern int lbl_805A01F4;
extern char lbl_8059DB98;
extern char g_thpObject[];
void* memcpy(void*, const void*, unsigned long);
int strcmp(const char*, const char*);
int fn_801F6708(void*, void*, unsigned int, unsigned int,
                void (*)(void*, void*), void*);
int fn_801F6524(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
int fn_8007BE4C(void) {
    switch (lbl_805A01E8) {
    case 0:
        fn_801F6708(g_thpObject, lbl_805A01F8 + 0x20, 0x40, 0, 0, (void*)2);
        lbl_805A01E8 = lbl_805A01E8 + 1;
        break;
    case 1:
        if (DVDGetCommandBlockStatus((DVDCommandBlock*)g_thpObject) == 0) {
            lbl_805A01E8 = lbl_805A01E8 + 1;
        }
        break;
    case 2:
        memcpy(g_thpObject + 0x3C, lbl_805A01F8 + 0x20, 0x30);
        if (strcmp(g_thpObject + 0x3C, &lbl_8059DB98) != 0) {
            fn_801F6524(g_thpObject);
            return -1;
        }
        if ((*(unsigned int*)(g_thpObject + 0x40) - 0x10000) != 0x1000) {
            fn_801F6524(g_thpObject);
            return -1;
        }
        lbl_805A01EC = *(int*)(g_thpObject + 0x5C);
        fn_801F6708(g_thpObject, lbl_805A01F8 + 0x20, 0x20, lbl_805A01EC, 0,
                    (void*)2);
        lbl_805A01E8 = lbl_805A01E8 + 1;
        break;
    case 3:
        if (DVDGetCommandBlockStatus((DVDCommandBlock*)g_thpObject) == 0) {
            lbl_805A01E8 = lbl_805A01E8 + 1;
        }
        break;
    case 4:
        memcpy(g_thpObject + 0x6C, lbl_805A01F8 + 0x20, 0x14);
        lbl_805A01EC = lbl_805A01EC + 0x14;
        *(unsigned char*)(g_thpObject + 0xA7) = 0;
        lbl_805A01F0 = 0;
        lbl_805A01E8 = lbl_805A01E8 + 1;
        break;
    case 5:
        if (lbl_805A01F0 < *(unsigned int*)(g_thpObject + 0x6C)) {
            unsigned char b = *(unsigned char*)(g_thpObject + 0x70 + lbl_805A01F0);
            switch (b) {
            case 0:
                fn_801F6708(g_thpObject, lbl_805A01F8 + 0x20, 0x20,
                            lbl_805A01EC, 0, (void*)2);
                break;
            case 1:
                fn_801F6708(g_thpObject, lbl_805A01F8 + 0x20, 0x20,
                            lbl_805A01EC, 0, (void*)2);
                break;
            default:
                fn_801F6524(g_thpObject);
                return -1;
            }
            lbl_805A01E8 = lbl_805A01E8 + 1;
        } else {
            lbl_805A01E8 = lbl_805A01E8 + 2;
        }
        break;
    case 6:
        if (DVDGetCommandBlockStatus((DVDCommandBlock*)g_thpObject) == 0) {
            unsigned char b = *(unsigned char*)(g_thpObject + 0x70 + lbl_805A01F0);
            switch (b) {
            case 0:
                memcpy(g_thpObject + 0x80, lbl_805A01F8 + 0x20, 0xC);
                lbl_805A01EC = lbl_805A01EC + 0xC;
                break;
            case 1:
                memcpy(g_thpObject + 0x8C, lbl_805A01F8 + 0x20, 0x10);
                *(unsigned char*)(g_thpObject + 0xA7) = 1;
                lbl_805A01EC = lbl_805A01EC + 0x10;
                break;
            }
            lbl_805A01F0 = lbl_805A01F0 + 1;
            lbl_805A01E8 = lbl_805A01E8 - 1;
        }
        break;
    case 7:
        *(unsigned char*)(g_thpObject + 0xA5) = 0;
        *(unsigned char*)(g_thpObject + 0xA4) = 0;
        *(unsigned char*)(g_thpObject + 0xA6) = 0;
        *(int*)(g_thpObject + 0xB0) = lbl_805A01F4;
        *(int*)(g_thpObject + 0xA0) = 1;
        return 1;
    }
    return 0;
}
}
}

namespace jj_8007CB98 {
#pragma opt_common_subs off
extern int* lbl_805A01F8;
extern "C" int fn_801E9AB0(void);
extern "C" int fn_8007E958(int);
extern "C" void fn_802022DC(int, int);
extern "C" int fn_8007CF00(void);
#define P ((char*)&g_thpObject)
#define I(n) (*(int*)(P + (n)))
#define B(n) (*(u8*)(P + (n)))
#define Q (*(u64*)(P + 0xC8))
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8007CB98(int arg) {
    int r31;
    void (*cb)(int) = (void (*)(int))*(int*)((char*)lbl_805A01F8 + 0x7F4);
    if (cb) cb(arg);
    r31 = -1;
    if (I(0xA0) != 0 && B(0xA4) == 2) {
        if (I(0xA8) != 0 || I(0xAC) != 0) {
            B(0xA5) = 5; B(0xA4) = 5; return;
        }
        ++Q;
        if ((I(0xCC) | I(0xC8)) == 0) {
            int flags = I(0x88);
            int r;
            if (flags & 1) {
                if (fn_801E9AB0() == 0) r = 1;
                else goto fail;
            } else if (flags & 2) {
                if ((u32)fn_801E9AB0() == 1) r = 1;
                else goto fail;
            } else r = 1;
            goto done;
        fail: r = 0;
        done:
            if (r != 0) {
                if (B(0xA7) != 0) {
                    if (I(0xE0) - I(0xE4) <= 1) {
                        r31 = fn_8007E958(0);
                        I(0xE0) = I(0xE0) + 1;
                    } else {
                        int v = *(int*)((char*)lbl_805A01F8 + 0x7F8);
                        if (v != 0) fn_802022DC(v, 1);
                        v = *(int*)((char*)lbl_805A01F8 + 0x7FC);
                        if (v != 0) fn_802022DC(v, 1);
                        B(0xA5) = 2;
                    }
                } else r31 = fn_8007E958(0);
            } else { I(0xCC) = -1; I(0xC8) = -1; }
        } else {
            if (B(0xA7) != 0 && ((I(0xCC) ^ 1) | I(0xC8)) == 0 && B(0xA5) != 2) {
                int v = *(int*)((char*)lbl_805A01F8 + 0x7F8);
                if (v != 0) fn_802022DC(v, 1);
                v = *(int*)((char*)lbl_805A01F8 + 0x7FC);
                if (v != 0) fn_802022DC(v, 1);
                B(0xA5) = 2;
            }
            if (fn_8007CF00() != 0) {
                if (B(0xA7) != 0) {
                    if (I(0xE0) - I(0xE4) <= 1) {
                        r31 = fn_8007E958(0);
                        I(0xE0) = I(0xE0) + 1;
                    }
                } else r31 = fn_8007E958(0);
            }
        }
        if (r31 != 0 && (u32)r31 != 0xffffffffUL) {
            if (I(0xE8) != 0)
                OSSendMessage((OSMessageQueue*)((char*)lbl_805A01F8 + 0x7C0), (void*)I(0xE8), 0);
            I(0xE8) = r31;
        }
        if ((B(0xA6) & 1) == 0) {
            if (B(0xA7) != 0) {
                u32 count = (u32)(I(0xE4) + I(0xC0));
                if (count == (u32)I(0x50) && I(0xEC) == 0) {
                    B(0xA5) = 3; B(0xA4) = 3;
                }
            } else {
                u32 target;
                if (I(0xE8) != 0) target = *(u32*)(I(0xE8) + 0xC) + (u32)I(0xC0);
                else target = (u32)I(0xC0) - 1;
                if (target == (u32)(I(0x50) - 1) && r31 == 0) {
                    B(0xA5) = 3; B(0xA4) = 3;
                }
            }
        }
    }
}
}

