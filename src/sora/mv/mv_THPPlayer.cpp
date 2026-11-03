#include <mv/mv_THPAudioDecode.h>
#include <mv/mv_THPDraw.h>
#include <mv/mv_THPPlayer.h>
#include <mv/mv_THPRead.h>
#include <mv/mv_THPVideoDecode.h>
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
        u32 unk7F8;
        u32 unk7FC;
        u32 unk800;
        u32 unk804;
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
