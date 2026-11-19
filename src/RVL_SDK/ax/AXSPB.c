#include "revolution/ax.h"
#include "revolution/os.h"

#pragma pack(push, 1)
typedef struct _AXSTUDIO {
    s32 L;
    s16 dL;

    s32 R;
    s16 dR;

    s32 S;
    s16 dS;

    s32 AuxAL;
    s16 dAuxAL;

    s32 AuxAR;
    s16 dAuxAR;

    s32 AuxAS;
    s16 dAuxAS;

    s32 AuxBL;
    s16 dAuxBL;

    s32 AuxBR;
    s16 dAuxBR;

    s32 AuxBS;
    s16 dAuxBS;

    s32 AuxCL;
    s16 dAuxCL;

    s32 AuxCR;
    s16 dAuxCR;

    s32 AuxCS;
    s16 dAuxCS;

    s32 Main0;
    s16 dMain0;

    s32 Aux0;
    s16 dAux0;

    s32 Main1;
    s16 dMain1;

    s32 Aux1;
    s16 dAux1;

    s32 Main2;
    s16 dMain2;

    s32 Aux2;
    s16 dAux2;

    s32 Main3;
    s16 dMain3;

    s32 Aux3;
    s16 dAux3;
} AXSTUDIO;
#pragma pack(pop)

#define AX_SAMPLES_PER_FRAME 96
#define AX_SAMPLES_PER_FRAME_RMT 18

void DCFlushRange(void* addr, u32 nBytes);

#define DEPOP_MIN (-20)
#define DEPOP_MAX 20

extern AXSTUDIO lbl_804E89E0;
#define __AXStudio lbl_804E89E0
extern s32 lbl_805A0AD8;
#define __AXSpbAux3 lbl_805A0AD8
extern s32 lbl_805A0ADC;
#define __AXSpbAux2 lbl_805A0ADC
extern s32 lbl_805A0AE0;
#define __AXSpbAux1 lbl_805A0AE0
extern s32 lbl_805A0AE4;
#define __AXSpbAux0 lbl_805A0AE4
extern s32 lbl_805A0AE8;
#define __AXSpbMain3 lbl_805A0AE8
extern s32 lbl_805A0AEC;
#define __AXSpbMain2 lbl_805A0AEC
extern s32 lbl_805A0AF0;
#define __AXSpbMain1 lbl_805A0AF0
extern s32 lbl_805A0AF4;
#define __AXSpbMain0 lbl_805A0AF4
extern s32 lbl_805A0AF8;
#define __AXSpbACS lbl_805A0AF8
extern s32 lbl_805A0AFC;
#define __AXSpbACR lbl_805A0AFC
extern s32 lbl_805A0B00;
#define __AXSpbACL lbl_805A0B00
extern s32 lbl_805A0B04;
#define __AXSpbABS lbl_805A0B04
extern s32 lbl_805A0B08;
#define __AXSpbABR lbl_805A0B08
extern s32 lbl_805A0B0C;
#define __AXSpbABL lbl_805A0B0C
extern s32 lbl_805A0B10;
#define __AXSpbAAS lbl_805A0B10
extern s32 lbl_805A0B14;
#define __AXSpbAAR lbl_805A0B14
extern s32 lbl_805A0B18;
#define __AXSpbAAL lbl_805A0B18
extern s32 lbl_805A0B1C;
#define __AXSpbAS lbl_805A0B1C
extern s32 lbl_805A0B20;
#define __AXSpbAR lbl_805A0B20
extern s32 lbl_805A0B24;
#define __AXSpbAL lbl_805A0B24








AXSTUDIO* __AXGetStudio(void) {
    return &__AXStudio;
}

// TODO: clean up - dont_inline is a match workaround (the retail build left these as out-of-line calls)
#pragma dont_inline on
void __AXDepopFadeMain(s32* all, s32* value, s16* depop) {
    if (*all / AX_SAMPLES_PER_FRAME != 0) {
        // Average depop across the channel?
        s32 avg = *all / AX_SAMPLES_PER_FRAME;

        if (avg > DEPOP_MAX) {
            avg = DEPOP_MAX;
        }

        if (avg < DEPOP_MIN) {
            avg = DEPOP_MIN;
        }

        *value = *all;
        *all -= avg * AX_SAMPLES_PER_FRAME;
        *depop = -avg;
    } else {
        *all = 0;
        *value = 0;
        *depop = 0;
    }
}

void __AXDepopFadeRmt(s32* all, s32* value, s16* depop) {
    if (*all / AX_SAMPLES_PER_FRAME_RMT != 0) {
        // Average depop across the channel?
        s32 avg = *all / AX_SAMPLES_PER_FRAME_RMT;

        if (avg > DEPOP_MAX) {
            avg = DEPOP_MAX;
        }

        if (avg < DEPOP_MIN) {
            avg = DEPOP_MIN;
        }

        *value = *all;
        *all -= avg * AX_SAMPLES_PER_FRAME_RMT;
        *depop = -avg;
    } else {
        *all = 0;
        *value = 0;
        *depop = 0;
    }
}

#pragma dont_inline reset

void __AXPrintStudio(void) {
    __AXDepopFadeMain(&__AXSpbAL, &__AXStudio.L, &__AXStudio.dL);
    __AXDepopFadeMain(&__AXSpbAR, &__AXStudio.R, &__AXStudio.dR);
    __AXDepopFadeMain(&__AXSpbAS, &__AXStudio.S, &__AXStudio.dS);

    __AXDepopFadeMain(&__AXSpbAAL, &__AXStudio.AuxAL, &__AXStudio.dAuxAL);
    __AXDepopFadeMain(&__AXSpbAAR, &__AXStudio.AuxAR, &__AXStudio.dAuxAR);
    __AXDepopFadeMain(&__AXSpbAAS, &__AXStudio.AuxAS, &__AXStudio.dAuxAS);

    __AXDepopFadeMain(&__AXSpbABL, &__AXStudio.AuxBL, &__AXStudio.dAuxBL);
    __AXDepopFadeMain(&__AXSpbABR, &__AXStudio.AuxBR, &__AXStudio.dAuxBR);
    __AXDepopFadeMain(&__AXSpbABS, &__AXStudio.AuxBS, &__AXStudio.dAuxBS);

    __AXDepopFadeMain(&__AXSpbACL, &__AXStudio.AuxCL, &__AXStudio.dAuxCL);
    __AXDepopFadeMain(&__AXSpbACR, &__AXStudio.AuxCR, &__AXStudio.dAuxCR);
    __AXDepopFadeMain(&__AXSpbACS, &__AXStudio.AuxCS, &__AXStudio.dAuxCS);

    __AXDepopFadeRmt(&__AXSpbMain0, &__AXStudio.Main0, &__AXStudio.dMain0);
    __AXDepopFadeRmt(&__AXSpbMain1, &__AXStudio.Main1, &__AXStudio.dMain1);
    __AXDepopFadeRmt(&__AXSpbMain2, &__AXStudio.Main2, &__AXStudio.dMain2);
    __AXDepopFadeRmt(&__AXSpbMain3, &__AXStudio.Main3, &__AXStudio.dMain3);

    __AXDepopFadeRmt(&__AXSpbAux0, &__AXStudio.Aux0, &__AXStudio.dAux0);
    __AXDepopFadeRmt(&__AXSpbAux1, &__AXStudio.Aux1, &__AXStudio.dAux1);
    __AXDepopFadeRmt(&__AXSpbAux2, &__AXStudio.Aux2, &__AXStudio.dAux2);
    __AXDepopFadeRmt(&__AXSpbAux3, &__AXStudio.Aux3, &__AXStudio.dAux3);

    DCFlushRange(&__AXStudio, sizeof(AXSTUDIO));
}

void __AXSPBInit(void) {
    __AXSpbAux3 = 0;
    __AXSpbAux2 = 0;
    __AXSpbAux1 = 0;
    __AXSpbAux0 = 0;

    __AXSpbMain3 = 0;
    __AXSpbMain2 = 0;
    __AXSpbMain1 = 0;
    __AXSpbMain0 = 0;

    __AXSpbACS = 0;
    __AXSpbACR = 0;
    __AXSpbACL = 0;

    __AXSpbABS = 0;
    __AXSpbABR = 0;
    __AXSpbABL = 0;

    __AXSpbAAS = 0;
    __AXSpbAAR = 0;
    __AXSpbAAL = 0;

    __AXSpbAS = 0;
    __AXSpbAR = 0;
    __AXSpbAL = 0;
}

void __AXSPBQuit(void) {}

void __AXDepopVoice(AXPB* pb) {
    __AXSpbAL += pb->dpop.aL;
    __AXSpbAAL += pb->dpop.aAuxAL;
    __AXSpbABL += pb->dpop.aAuxBL;
    __AXSpbACL += pb->dpop.aAuxCL;

    __AXSpbAR += pb->dpop.aR;
    __AXSpbAAR += pb->dpop.aAuxAR;
    __AXSpbABR += pb->dpop.aAuxBR;
    __AXSpbACR += pb->dpop.aAuxCR;

    __AXSpbAS += pb->dpop.aS;
    __AXSpbAAS += pb->dpop.aAuxAS;
    __AXSpbABS += pb->dpop.aAuxBS;
    __AXSpbACS += pb->dpop.aAuxCS;

    __AXSpbMain0 += pb->rmtDpop.aMain0;
    __AXSpbMain1 += pb->rmtDpop.aMain1;
    __AXSpbMain2 += pb->rmtDpop.aMain2;
    __AXSpbMain3 += pb->rmtDpop.aMain3;

    __AXSpbAux0 += pb->rmtDpop.aAux0;
    __AXSpbAux1 += pb->rmtDpop.aAux1;
    __AXSpbAux2 += pb->rmtDpop.aAux2;
    __AXSpbAux3 += pb->rmtDpop.aAux3;
}



void* memcpy(void* dst, const void* src, unsigned long n);

typedef struct _AXITD {
    u8 UNK_0x0[0x40];
} AXITD;

// TODO: clean up - the AXVPB statics are referenced through their symbol-map names (lbl_*) instead of real static variables
extern AXPB* lbl_805A0B28;
#define __AXPB lbl_805A0B28
extern AXITD* lbl_805A0B2C;
#define __AXITD lbl_805A0B2C
extern s32 lbl_805A0B34;
#define __AXMaxVoices lbl_805A0B34
extern s32 lbl_805A0B38;
#define __AXNumVoices lbl_805A0B38
extern u32 lbl_805A0B3C;
#define __AXRecDspCycles lbl_805A0B3C
extern u32 lbl_805A0B40;
#define __AXMaxDspCycles lbl_805A0B40
extern u32 lbl_80478160[];
#define __AXMixCycles lbl_804780E0
extern u32 lbl_804780E0[];
#define __AXRmtMixCycles lbl_80478160

#define AX_PRIORITY_MAX 31
#define AX_PRIORITY_FREE 0
#define AX_VOICE_RUN 1
#define AX_VOICE_STOP 0
#define AX_PB_LPF_ON 1
#define AX_PB_BIQUAD_ON 2
#define TRUE 1
#define FALSE 0

void DCInvalidateRange(void* addr, u32 nBytes);
u32 __AXGetCommandListCycles(void);
AXVPB* __AXGetStackHead(u32 prio);
void __AXPushCallbackStack(AXVPB* vpb);

#define FUNNY_COPY(dst, src, type, n)                                          \
    do {                                                                       \
                                                                               \
        type* __dst = (type*)dst;                                              \
        type* __src = (type*)src;                                              \
        u16 i;                                                                 \
                                                                               \
        for (i = 0; i < n / sizeof(type); i++) {                               \
            *__dst = *__src;                                                   \
            __dst++;                                                           \
            __src++;                                                           \
        }                                                                      \
                                                                               \
    } while (0)


static u32 __AXGetSrcCycles(u16 select, const AXPBSRC* src) {
    u32 ratio = src->ratioHi << 16 | src->ratioLo;

    if (select == 0) {
        return (ratio * 512 + 32768) / 65536 + 1561;
    }

    if (select == 1) {
        return (ratio * 512 + 32768) / 65536 + 1466;
    }

    return 605;
}


void __AXServiceVPB(AXVPB* vpb) {
    AXPB* dst;
    AXPB* src;
    u32 sync;

    __AXNumVoices++;

    dst = &__AXPB[vpb->index];
    src = &vpb->pb;
    sync = vpb->sync;

    if (sync == 0) {
        src->state = dst->state;
        src->ve.currentVolume = dst->ve.currentVolume;
        src->addr.currentAddressHi = dst->addr.currentAddressHi;
        src->addr.currentAddressLo = dst->addr.currentAddressLo;
        return;
    }

    if (sync & AX_PBSYNC_ALL) {
        memcpy(dst, src, sizeof(AXPB));
        return;
    }

    if (sync & AX_PBSYNC_SELECT) {
        dst->srcSelect = src->srcSelect;
        dst->coefSelect = src->coefSelect;
    }

    if (sync & AX_PBSYNC_MIXER_CTRL) {
        dst->mixerCtrl = src->mixerCtrl;
    }

    if (sync & AX_PBSYNC_STATE) {
        dst->state = src->state;
    } else {
        src->state = dst->state;
    }

    if (sync & AX_PBSYNC_TYPE) {
        dst->type = src->type;
    }

    if (sync & AX_PBSYNC_MIX) {
        memcpy(&dst->mix, &src->mix, sizeof(AXPBMIX));
    }

    if (sync & AX_PBSYNC_ITD_SHIFT) {
        dst->itd.targetShiftL = src->itd.targetShiftL;
        dst->itd.targetShiftR = src->itd.targetShiftR;
    } else if (sync & AX_PBSYNC_ITD) {
        FUNNY_COPY(&dst->itd, &src->itd, u16, sizeof(AXPBITD));
        __memclr(vpb->itdBuffer, sizeof(AXITD));
    }

    if (sync & AX_PBSYNC_DPOP) {
        memcpy(&dst->dpop, &src->dpop, sizeof(AXPBDPOP));
    }

    if (sync & AX_PBSYNC_VE_DELTA) {
        src->ve.currentVolume = dst->ve.currentVolume;
        dst->ve.currentDelta = src->ve.currentDelta;
    } else if (sync & AX_PBSYNC_VE) {
        dst->ve.currentVolume = src->ve.currentVolume;
        dst->ve.currentDelta = src->ve.currentDelta;
    }

    if (sync & (AX_PBSYNC_LOOP_FLAG | AX_PBSYNC_LOOP_ADDR | AX_PBSYNC_END_ADDR |
                AX_PBSYNC_CURR_ADDR)) {
        if (sync & AX_PBSYNC_LOOP_FLAG) {
            dst->addr.loopFlag = src->addr.loopFlag;
        }

        if (sync & AX_PBSYNC_LOOP_ADDR) {
            *(u32*)&dst->addr.loopAddressHi = *(u32*)&src->addr.loopAddressHi;
        }

        if (sync & AX_PBSYNC_END_ADDR) {
            *(u32*)&dst->addr.endAddressHi = *(u32*)&src->addr.endAddressHi;
        }

        if (sync & AX_PBSYNC_CURR_ADDR) {
            *(u32*)&dst->addr.currentAddressHi =
                *(u32*)&src->addr.currentAddressHi;
        } else {
            *(u32*)&src->addr.currentAddressHi =
                *(u32*)&dst->addr.currentAddressHi;
        }
    } else if (sync & AX_PBSYNC_ADDR) {
        FUNNY_COPY(&dst->addr, &src->addr, u32, sizeof(AXPBADDR));
    } else {
        src->addr.currentAddressHi = dst->addr.currentAddressHi;
        src->addr.currentAddressLo = dst->addr.currentAddressLo;
    }

    if (sync & AX_PBSYNC_ADPCM) {
        FUNNY_COPY(&dst->adpcm, &src->adpcm, u32, sizeof(AXPBADPCM));
    }

    if (sync & AX_PBSYNC_SRC_RATIO) {
        dst->src.ratioHi = src->src.ratioHi;
        dst->src.ratioLo = src->src.ratioLo;
    } else if (sync & AX_PBSYNC_SRC) {
        FUNNY_COPY(&dst->src, &src->src, u16, sizeof(AXPBSRC));
    }

    if (sync & AX_PBSYNC_ADPCM_LOOP) {
        FUNNY_COPY(&dst->adpcmLoop, &src->adpcmLoop, u16,
                   sizeof(AXPBADPCMLOOP));
    }

    if (sync & AX_PBSYNC_LPF_COEFS) {
        dst->lpf.a0 = src->lpf.a0;
        dst->lpf.b0 = src->lpf.b0;
    } else if (sync & AX_PBSYNC_LPF) {
        FUNNY_COPY(&dst->lpf, &src->lpf, u16, sizeof(AXPBLPF));
    }

    if (sync & AX_PBSYNC_BIQUAD_COEFS) {
        dst->biquad.b0 = src->biquad.b0;
        dst->biquad.b1 = src->biquad.b1;
        dst->biquad.b2 = src->biquad.b2;
        dst->biquad.a1 = src->biquad.a1;
        dst->biquad.a2 = src->biquad.a2;
    } else if (sync & AX_PBSYNC_BIQUAD) {
        FUNNY_COPY(&dst->biquad, &src->biquad, u16, sizeof(AXPBBIQUAD));
    }

    if (sync & AX_PBSYNC_REMOTE) {
        dst->remote = src->remote;
    }

    if (sync & AX_PBSYNC_RMT_MIXER_CTRL) {
        dst->rmtMixerCtrl = src->rmtMixerCtrl;
    }

    if (sync & AX_PBSYNC_RMTMIX) {
        memcpy(&dst->rmtMix, &src->rmtMix, sizeof(AXPBRMTMIX));
    }

    if (sync & AX_PBSYNC_RMTDPOP) {
        memcpy(&dst->rmtDpop, &src->rmtDpop, sizeof(AXPBRMTDPOP));
    }

    if (sync & AX_PBSYNC_RMTSRC) {
        memcpy(&dst->rmtSrc, &src->rmtSrc, sizeof(AXPBRMTSRC));
    }

    if (sync & AX_PBSYNC_RMTIIR_LPF_COEFS) {
        dst->rmtIIR.lpf.a0 = src->rmtIIR.lpf.a0;
        dst->rmtIIR.lpf.b0 = src->rmtIIR.lpf.b0;
    } else if (sync & AX_PBSYNC_RMTIIR_BIQUAD_COEFS) {
        dst->rmtIIR.biquad.b0 = src->rmtIIR.biquad.b0;
        dst->rmtIIR.biquad.b1 = src->rmtIIR.biquad.b1;
        dst->rmtIIR.biquad.b2 = src->rmtIIR.biquad.b2;
        dst->rmtIIR.biquad.a1 = src->rmtIIR.biquad.a1;
        dst->rmtIIR.biquad.a2 = src->rmtIIR.biquad.a2;
    } else if (sync & AX_PBSYNC_RMTIIR) {
        FUNNY_COPY(&dst->rmtIIR, &src->rmtIIR, u16, sizeof(AXPBRMTIIR));
    }
}

static void __AXDumpVPB(AXVPB* vpb) {
    AXPB* pb = &__AXPB[vpb->index];

    if (pb->state == AX_VOICE_RUN) {
        __AXDepopVoice(pb);
    }

    vpb->pb.state = AX_VOICE_STOP;
    pb->state = AX_VOICE_STOP;

    __AXPushCallbackStack(vpb);
}

void __AXSyncPBs(u32 baseCycles) {
    u32 cycles;
    u32 prio;
    AXVPB* head;

    __AXNumVoices = 0;

    DCInvalidateRange(__AXPB, __AXMaxVoices * sizeof(AXPB));
    DCInvalidateRange(__AXITD, __AXMaxVoices * sizeof(AXITD));

    cycles = 32 + __AXGetCommandListCycles() + __AXMaxVoices * 600 + baseCycles;

    for (prio = AX_PRIORITY_MAX; prio > AX_PRIORITY_FREE; prio--) {
        for (head = __AXGetStackHead(prio); head != NULL; head = head->next) {
            if (head->pb.itd.flag == 1) {
                cycles += 129;
            }

            if (head->depop) {
                __AXDepopVoice(&__AXPB[head->index]);
            }

            if (head->pb.state == AX_VOICE_RUN) {
                cycles += 387;

                if (head->pb.lpf.on) {
                    cycles += 309;
                }

                if (head->pb.biquad.on) {
                    cycles += 1024;
                }

                if (head->pb.itd.flag == 1) {
                    cycles += 27;
                }

                cycles += __AXGetSrcCycles(head->pb.srcSelect, &head->pb.src);
                cycles += __AXMixCycles[head->pb.mixerCtrl >> 0 & 31] +
                          __AXMixCycles[head->pb.mixerCtrl >> 16 & 31] +
                          __AXMixCycles[head->pb.mixerCtrl >> 21 & 31] +
                          __AXMixCycles[head->pb.mixerCtrl >> 26 & 31];

                if (head->pb.remote == TRUE) {
                    cycles += 613;

                    if (head->pb.rmtIIR.lpf.on == AX_PB_LPF_ON) {
                        cycles += 118;
                    } else if (head->pb.rmtIIR.biquad.on == AX_PB_BIQUAD_ON) {
                        cycles += 834;
                    }

                    cycles +=
                        __AXRmtMixCycles[head->pb.rmtMixerCtrl >> 0 & 3] +
                        __AXRmtMixCycles[head->pb.rmtMixerCtrl >> 2 & 3] +
                        __AXRmtMixCycles[head->pb.rmtMixerCtrl >> 4 & 3] +
                        __AXRmtMixCycles[head->pb.rmtMixerCtrl >> 6 & 3] +
                        __AXRmtMixCycles[head->pb.rmtMixerCtrl >> 8 & 3] +
                        __AXRmtMixCycles[head->pb.rmtMixerCtrl >> 10 & 3] +
                        __AXRmtMixCycles[head->pb.rmtMixerCtrl >> 12 & 3] +
                        __AXRmtMixCycles[head->pb.rmtMixerCtrl >> 14 & 3];
                }

                if (__AXMaxDspCycles > cycles) {
                    __AXServiceVPB(head);
                } else {
                    __AXDumpVPB(head);
                }

            } else {
                __AXServiceVPB(head);
            }

            head->sync = 0;
            head->depop = FALSE;
        }
    }

    __AXRecDspCycles = cycles;

    for (head = __AXGetStackHead(AX_PRIORITY_FREE); head != NULL;
         head = head->next) {
        if (head->depop) {
            __AXDepopVoice(&__AXPB[head->index]);
        }

        head->depop = FALSE;
        __AXPB[head->index].state = AX_VOICE_STOP;
    }

    DCFlushRange(__AXPB, __AXMaxVoices * sizeof(AXPB));
    DCFlushRange(__AXITD, __AXMaxVoices * sizeof(AXITD));
}

AXPB* __AXGetPBs(void) {
    return __AXPB;
}

s32 __AXGetNumVoices(void) {
    return __AXNumVoices;
}
