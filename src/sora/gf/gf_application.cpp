#include <gf/gf_application.h>
#include <types.h>

extern "C" {
void fn_801DF434(s32 arg);
void fn_801F47D8(s32 a, s32 b);
void fn_801F4880(s32 a);
void fn_80016F8C(gfApplication* self);
void GXSetScissor(u32 left, u32 top, u32 wd, u32 ht);
void GXSetViewport(f32 x, f32 y, f32 wd, f32 ht, f32 nearZ, f32 farZ);
void GXSetClipMode(u32 mode);
void GXSetCullMode(u32 mode);
void VISetBlack(BOOL black);
void VIFlush();
void VIWaitForRetrace();
void fn_801FF1AC();
}

extern f32 lbl_805A13C8;
extern f32 lbl_805A13DC;

struct gfApplicationFields {
    u8 unk0[0x7C];
    s32 unk7C;
    s32 unk80;
    u8 unk84[0x8];
    u16 unk8C;
    u16 unk8E;
};

gfApplication::~gfApplication() { }

void gfApplication::reset() {
    VISetBlack(TRUE);
    VIFlush();
    fn_801FF1AC();
    VIWaitForRetrace();
    VIWaitForRetrace();
}

void gfApplication::restart() {
    VISetBlack(TRUE);
    VIFlush();
    fn_801FF1AC();
    VIWaitForRetrace();
    VIWaitForRetrace();
    fn_801DF434(2);
}

void gfApplication::mainLoop() {
    gfApplicationFields* self = (gfApplicationFields*)this;

    fn_801F47D8(self->unk7C, self->unk80);
    GXSetScissor(0, 0, self->unk8C, self->unk8E);
    GXSetViewport(lbl_805A13C8, lbl_805A13C8, (f32)self->unk8C, (f32)self->unk8E, lbl_805A13C8, lbl_805A13DC);
    fn_801F4880(0);
    GXSetClipMode(0);
    GXSetCullMode(0);
    fn_80016F8C(this);
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_80494890[0x10] __attribute__((aligned(8))) = { 0 };

void gfApplication::setPause(int value, int index) {
    m_pauseFlags = (m_pauseFlags & ~(1 << index)) | (value << index);
}

void gfApplication::clearPause() {
    m_pauseFlags = 0;
}

void gfApplication::setSkipLowEffect(u8 level) {
    if (m_lowEffectEnabled) {
        if (m_lowEffectActive) {
            if (level < 3) {
                m_skipCounter += 1;
                if (m_skipCounter > 30u) {
                    m_lowEffectActive = 0;
                }
            } else {
                m_skipCounter = 0;
            }
        } else if (level >= 3) {
            m_skipCounter += 1;
            if (m_skipCounter > 10u) {
                m_skipCounter = 0;
                m_lowEffectActive = 1;
            }
        } else if (m_skipCounter != 0) {
            m_skipCounter -= 1;
        }
    } else {
        m_skipLowEffect = 0;
        m_skipCounter = 0;
        m_lowEffectActive = 0;
    }
    m_skipLowEffect = m_lowEffectActive;
}
