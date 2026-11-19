#include "revolution/ax.h"
#include "revolution/os.h"

// TODO: clean up - the AXVPB statics are referenced through their symbol-map names (lbl_*) instead of real static variables
typedef struct _AXITD {
    u8 UNK_0x0[0x40];
} AXITD;

extern AXPB* lbl_805A0B28;
#define __AXPB lbl_805A0B28
extern AXITD* lbl_805A0B2C;
#define __AXITD lbl_805A0B2C
extern AXVPB* lbl_805A0B30;
#define __AXVPB lbl_805A0B30
extern s32 lbl_805A0B34;
#define __AXMaxVoices lbl_805A0B34
extern u32 lbl_805A0B3C;
#define __AXRecDspCycles lbl_805A0B3C
extern u32 lbl_805A0B40;
#define __AXMaxDspCycles lbl_805A0B40

#define AX_VOICE_MAX 96
#define ADDRHI(x) (((u32)x) >> 16 & 0xFFFF)
#define ADDRLO(x) (((u32)x) & 0xFFFF)
#define AX_PRIORITY_MIN 1
#define AX_VOICE_STOP 0
#define AX_SAMPLE_RATE 32000
#define OS_BUS_CLOCK_SPEED (*(u32*)0x800000F8)
#define TRUE 1
#define FALSE 0
#define AX_SRC_TYPE_NONE 0
#define AX_SRC_TYPE_LINEAR 1
#define AX_SRC_TYPE_4TAP_8K 2
#define AX_SRC_TYPE_4TAP_12K 3
#define AX_SRC_TYPE_4TAP_16K 4
#define AX_SAMPLE_FORMAT_DSP_ADPCM 0
#define AX_SAMPLE_FORMAT_PCM_S16 10
#define AX_SAMPLE_FORMAT_PCM_S8 25

void DCFlushRange(void* addr, u32 nBytes);
void __AXPushFreeStack(AXVPB* vpb);
double cos(double x);
double sqrt(double x);

// TODO: clean up - same body as __AXSetPBDefault (AXVPB.c); copied so the compiler inlines it like the retail build did
static void AXSetPBDefaultInline(AXVPB* vpb) {
    vpb->pb.state = 0;
    vpb->pb.itd.flag = 0;
    vpb->sync = 0x18A80024;
    vpb->pb.lpf.on = 0;
    vpb->pb.biquad.on = 0;
    vpb->pb.remote = 0;
    vpb->pb.rmtIIR.lpf.on = 0;
    vpb->pb.rmtSrc.currentAddressFrac = 0;
    vpb->pb.rmtSrc.last_samples[0] = 0;
    vpb->pb.rmtSrc.last_samples[1] = 0;
    vpb->pb.rmtSrc.last_samples[2] = 0;
    vpb->pb.rmtSrc.last_samples[3] = 0;
}

void __AXVPBInitCommon(void);

void __AXVPBInit(u32 num, void* mem) {
    if (num > AX_VOICE_MAX) {
        return;
    }
    if (mem == NULL) {
        return;
    }
    if (((u32)mem & 31) == 0) {
        u8* itd;
        __AXMaxVoices = num;
        __AXPB = (AXPB*)mem;
        itd = (u8*)mem + num * sizeof(AXPB);
        __AXITD = (AXITD*)itd;
        itd += num * sizeof(AXITD);
        __AXVPB = (AXVPB*)itd;
        __AXVPBInitCommon();
        return;
    }
    return;
}

void __AXVPBInitCommon(void) {
    u32 i;
    u32* dst;

    __AXMaxDspCycles = OS_BUS_CLOCK_SPEED / 667;
    __AXRecDspCycles = 0;

    for (dst = (u32*)__AXPB, i = __AXMaxVoices * (sizeof(AXPB) / sizeof(u32));
         i > 0; i--) {
        *dst++ = 0;
    }

    for (dst = (u32*)__AXITD, i = __AXMaxVoices * (sizeof(AXITD) / sizeof(u32));
         i > 0; i--) {
        *dst++ = 0;
    }

    for (dst = (u32*)__AXVPB, i = __AXMaxVoices * (sizeof(AXVPB) / sizeof(u32));
         i > 0; i--) {
        *dst++ = 0;
    }

    for (i = 0; i < __AXMaxVoices; i++) {
        AXPB* pb = &__AXPB[i];
        AXITD* itd = &__AXITD[i];
        AXVPB* vpb = &__AXVPB[i];

        vpb->index = i;
        vpb->itdBuffer = itd;
        AXSetPBDefaultInline(vpb);

        if (i == __AXMaxVoices - 1) {
            pb->nextHi = pb->nextLo = 0;
            vpb->pb.nextHi = vpb->pb.nextLo = 0;
        } else {
            vpb->pb.nextHi = ADDRHI((u32)pb + 320);
            vpb->pb.nextLo = ADDRLO((u32)pb + 320);

            pb->nextHi = ADDRHI((u32)pb + 320);
            pb->nextLo = ADDRLO((u32)pb + 320);
        }

        vpb->pb.currHi = ADDRHI(pb);
        vpb->pb.currLo = ADDRLO(pb);

        pb->currHi = ADDRHI(pb);
        pb->currLo = ADDRLO(pb);

        vpb->pb.itd.bufferHi = ADDRHI(itd);
        vpb->pb.itd.bufferLo = ADDRLO(itd);

        pb->itd.bufferHi = ADDRHI(itd);
        pb->itd.bufferLo = ADDRLO(itd);

        vpb->priority = AX_PRIORITY_MIN;
        __AXPushFreeStack(vpb);
    }

    DCFlushRange(__AXPB, __AXMaxVoices * sizeof(AXPB));
}


void __AXVPBQuit(void) {
    __AXPB = NULL;
    __AXITD = NULL;
    __AXVPB = NULL;
    __AXMaxVoices = 0;
}

void AXSetVoiceSrcType(AXVPB* vpb, u32 type) {
    BOOL old;
    AXPB* ppb;

    old = OSDisableInterrupts();
    ppb = &vpb->pb;
    switch (type) {
    case AX_SRC_TYPE_NONE:
        ppb->srcSelect = 2;
        break;
    case AX_SRC_TYPE_LINEAR:
        ppb->srcSelect = 1;
        break;
    case AX_SRC_TYPE_4TAP_8K:
        ppb->srcSelect = 0;
        ppb->coefSelect = 0;
        break;
    case AX_SRC_TYPE_4TAP_12K:
        ppb->srcSelect = 0;
        ppb->coefSelect = 1;
        break;
    case AX_SRC_TYPE_4TAP_16K:
        ppb->srcSelect = 0;
        ppb->coefSelect = 2;
        break;
    }

    vpb->sync |= AX_PBSYNC_SELECT;
    OSRestoreInterrupts(old);
}

void AXSetVoiceState(AXVPB* vpb, u16 state) {
    BOOL enabled = OSDisableInterrupts();

    if (vpb->pb.state == state) {
        OSRestoreInterrupts(enabled);
        return;
    }

    vpb->pb.state = state;
    vpb->sync |= AX_PBSYNC_STATE;

    if (state == AX_VOICE_STOP) {
        vpb->depop = TRUE;
    }

    OSRestoreInterrupts(enabled);
}

void AXSetVoiceAddr(AXVPB* vpb, AXPBADDR* addr) {
    BOOL enabled = OSDisableInterrupts();

    u32* dst = (u32*)&vpb->pb.addr;
    const u32* src = (u32*)addr;

    // :(
    *dst = *src;
    dst++;
    src++;

    *dst = *src;
    dst++;
    src++;

    *dst = *src;
    dst++;
    src++;

    *dst = *src;

    switch (addr->format) {
    case AX_SAMPLE_FORMAT_PCM_S16:
        dst++;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0x08000000;
        *dst = 0;
        break;
    case AX_SAMPLE_FORMAT_PCM_S8:
        dst++;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = 0x01000000;
        *dst = 0;
        break;
    case AX_SAMPLE_FORMAT_DSP_ADPCM:
    default:
        break;
    }

    vpb->sync &= ~(AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR |
                   AX_PBSYNC_END_ADDR | AX_PBSYNC_CURR_ADDR);
    vpb->sync |= AX_PBSYNC_ADDR | AX_PBSYNC_ADPCM;

    OSRestoreInterrupts(enabled);
}

void AXSetVoiceCurrentAddr(AXVPB* p, u32 addr) {
    BOOL old;

    old = OSDisableInterrupts();
    p->pb.addr.currentAddressHi = addr >> 16;
    p->pb.addr.currentAddressLo = addr;
    p->sync |= AX_PBSYNC_CURR_ADDR;
    OSRestoreInterrupts(old);
}

void AXSetVoiceSrcRatio(AXVPB* vpb, float ratio) {
    u32 r;
    BOOL old;

    old = OSDisableInterrupts();
    r = 65536.0f * ratio;
    vpb->pb.src.ratioHi = r >> 16;
    vpb->pb.src.ratioLo = r;
    vpb->sync |= AX_PBSYNC_SRC_RATIO;
    OSRestoreInterrupts(old);
}

#define fM_PI 3.14159265358979323846f

void AXGetLpfCoefs(u16 freq, u16* a, u16* b) {
    f32 rf31 = 2.0f - (f32)cos(2 * fM_PI * freq / AX_SAMPLE_RATE);
    f32 rf30 = (f32)sqrt(rf31 * rf31 - 1.0f) - rf31;

    *b = 32768 * -rf30;
    *a = 32767 - *b;
}

s32 AXGetMaxVoices(void) {
    return __AXMaxVoices;
}
