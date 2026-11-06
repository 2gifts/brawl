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

u8 THPPlayerGetState() {
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
