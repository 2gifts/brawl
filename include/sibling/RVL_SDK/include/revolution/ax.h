#ifndef AX_H
#define AX_H

#include "revolution/types.h"

typedef struct _AXPBMIX {
    u16 vL;           // at 0x0
    u16 vDeltaL;      // at 0x2
    u16 vR;           // at 0x4
    u16 vDeltaR;      // at 0x6
    u16 vAuxAL;       // at 0x8
    u16 vDeltaAuxAL;  // at 0xA
    u16 vAuxAR;       // at 0xC
    u16 vDeltaAuxAR;  // at 0xE
    u16 vAuxBL;       // at 0x10
    u16 vDeltaAuxBL;  // at 0x12
    u16 vAuxBR;       // at 0x14
    u16 vDeltaAuxBR;  // at 0x16
    u16 vAuxCL;       // at 0x18
    u16 vDeltaAuxCL;  // at 0x1A
    u16 vAuxCR;       // at 0x1C
    u16 vDeltaAuxCR;  // at 0x1E
    u16 vS;           // at 0x20
    u16 vDeltaS;      // at 0x22
    u16 vAuxAS;       // at 0x24
    u16 vDeltaAuxAS;  // at 0x26
    u16 vAuxBS;       // at 0x28
    u16 vDeltaAuxBS;  // at 0x2A
    u16 vAuxCS;       // at 0x2C
    u16 vDeltaAuxCS;  // at 0x2E
} AXPBMIX;

typedef struct _AXPBITD {
    u16 flag;          // at 0x0
    u16 bufferHi;      // at 0x2
    u16 bufferLo;      // at 0x4
    u16 shiftL;        // at 0x6
    u16 shiftR;        // at 0x8
    u16 targetShiftL;  // at 0xA
    u16 targetShiftR;  // at 0xC
} AXPBITD;

typedef struct _AXPBDPOP {
    s16 aL;      // at 0x0
    s16 aAuxAL;  // at 0x2
    s16 aAuxBL;  // at 0x4
    s16 aAuxCL;  // at 0x6
    s16 aR;      // at 0x8
    s16 aAuxAR;  // at 0xA
    s16 aAuxBR;  // at 0xC
    s16 aAuxCR;  // at 0xE
    s16 aS;      // at 0x10
    s16 aAuxAS;  // at 0x12
    s16 aAuxBS;  // at 0x14
    s16 aAuxCS;  // at 0x16
} AXPBDPOP;

typedef struct _AXPBVE {
    u16 currentVolume;  // at 0x0
    s16 currentDelta;   // at 0x2
} AXPBVE;

typedef struct _AXPBADDR {
    u16 loopFlag;          // at 0x0
    u16 format;            // at 0x2
    u16 loopAddressHi;     // at 0x4
    u16 loopAddressLo;     // at 0x6
    u16 endAddressHi;      // at 0x8
    u16 endAddressLo;      // at 0xA
    u16 currentAddressHi;  // at 0xC
    u16 currentAddressLo;  // at 0xE
} AXPBADDR;

typedef struct _AXPBADPCM {
    u16 a[8][2];     // at 0x0
    u16 gain;        // at 0x20
    u16 pred_scale;  // at 0x22
    u16 yn1;         // at 0x24
    u16 yn2;         // at 0x26
} AXPBADPCM;

typedef struct _AXPBSRC {
    u16 ratioHi;             // at 0x0
    u16 ratioLo;             // at 0x2
    u16 currentAddressFrac;  // at 0x4
    u16 last_samples[4];     // at 0x6
} AXPBSRC;

typedef struct _AXPBADPCMLOOP {
    u16 loop_pred_scale;  // at 0x0
    u16 loop_yn1;         // at 0x2
    u16 loop_yn2;         // at 0x4
} AXPBADPCMLOOP;

typedef struct _AXPBLPF {
    u16 on;   // at 0x0
    u16 yn1;  // at 0x2
    u16 a0;   // at 0x4
    u16 b0;   // at 0x6
} AXPBLPF;

typedef struct _AXPBBIQUAD {
    u16 on;   // at 0x0
    u16 xn1;  // at 0x2
    u16 xn2;  // at 0x4
    u16 yn1;  // at 0x6
    u16 yn2;  // at 0x8
    u16 b0;   // at 0xA
    u16 b1;   // at 0xC
    u16 b2;   // at 0xE
    u16 a1;   // at 0x10
    u16 a2;   // at 0x12
} AXPBBIQUAD;

typedef struct _AXPBRMTMIX {
    u16 vMain0;       // at 0x0
    u16 vDeltaMain0;  // at 0x2
    u16 vAux0;        // at 0x4
    u16 vDeltaAux0;   // at 0x6
    u16 vMain1;       // at 0x8
    u16 vDeltaMain1;  // at 0xA
    u16 vAux1;        // at 0xC
    u16 vDeltaAux1;   // at 0xE
    u16 vMain2;       // at 0x10
    u16 vDeltaMain2;  // at 0x12
    u16 vAux2;        // at 0x14
    u16 vDeltaAux2;   // at 0x16
    u16 vMain3;       // at 0x18
    u16 vDeltaMain3;  // at 0x1A
    u16 vAux3;        // at 0x1C
    u16 vDeltaAux3;   // at 0x1E
} AXPBRMTMIX;

typedef struct _AXPBRMTDPOP {
    s16 aMain0;  // at 0x0
    s16 aMain1;  // at 0x2
    s16 aMain2;  // at 0x4
    s16 aMain3;  // at 0x6
    s16 aAux0;   // at 0x8
    s16 aAux1;   // at 0xA
    s16 aAux2;   // at 0xC
    s16 aAux3;   // at 0xE
} AXPBRMTDPOP;

typedef struct _AXPBRMTSRC {
    u16 currentAddressFrac;  // at 0x0
    u16 last_samples[4];     // at 0x2
} AXPBRMTSRC;

typedef union __AXPBRMTIIR {
    AXPBLPF lpf;
    AXPBBIQUAD biquad;
} AXPBRMTIIR;

typedef struct _AXPB {
    u16 nextHi;                 // at 0x0
    u16 nextLo;                 // at 0x2
    u16 currHi;                 // at 0x4
    u16 currLo;                 // at 0x6
    u16 srcSelect;              // at 0x8
    u16 coefSelect;             // at 0xA
    u32 mixerCtrl;              // at 0xC
    u16 state;                  // at 0x10
    u16 type;                   // at 0x12
    AXPBMIX mix;                // at 0x14
    AXPBITD itd;                // at 0x44
    AXPBDPOP dpop;              // at 0x52
    AXPBVE ve;                  // at 0x6A
    AXPBADDR addr;              // at 0x6E
    AXPBADPCM adpcm;            // at 0x7E
    AXPBSRC src;                // at 0xA6
    AXPBADPCMLOOP adpcmLoop;    // at 0xB4
    AXPBLPF lpf;                // at 0xBA
    AXPBBIQUAD biquad;          // at 0xC2
    u16 remote;                 // at 0xD6
    u16 rmtMixerCtrl;           // at 0xD8
    AXPBRMTMIX rmtMix;          // at 0xDA
    AXPBRMTDPOP rmtDpop;        // at 0xFA
    AXPBRMTSRC rmtSrc;          // at 0x10A
    AXPBRMTIIR rmtIIR;          // at 0x114
    u8 padding[0x140 - 0x128];  // at 0x128
} AXPB;

typedef void (*AXVoiceCallback)(void* vpb);

// Mixer-control and sync flags (from mscharged-decomp, for mix.c / remote.c)
typedef enum {
    AX_MIXER_CTRL_L = (1 << 0),
    AX_MIXER_CTRL_R = (1 << 1),
    AX_MIXER_CTRL_DELTA = (1 << 2),
    AX_MIXER_CTRL_S = (1 << 3),
    AX_MIXER_CTRL_DELTA_S = (1 << 4),

    AX_MIXER_CTRL_AL = (1 << 16),
    AX_MIXER_CTRL_AR = (1 << 17),
    AX_MIXER_CTRL_DELTA_A = (1 << 18),
    AX_MIXER_CTRL_AS = (1 << 19),
    AX_MIXER_CTRL_DELTA_AS = (1 << 20),

    AX_MIXER_CTRL_BL = (1 << 21),
    AX_MIXER_CTRL_BR = (1 << 22),
    AX_MIXER_CTRL_DELTA_B = (1 << 23),
    AX_MIXER_CTRL_BS = (1 << 24),
    AX_MIXER_CTRL_DELTA_BS = (1 << 25),

    AX_MIXER_CTRL_CL = (1 << 26),
    AX_MIXER_CTRL_CR = (1 << 27),
    AX_MIXER_CTRL_DELTA_C = (1 << 28),
    AX_MIXER_CTRL_CS = (1 << 29),
    AX_MIXER_CTRL_DELTA_CS = (1 << 30)
};

typedef enum {
    AX_MIXER_CTRL_RMT_M0 = (1 << 0),
    AX_MIXER_CTRL_RMT_DELTA_M0 = (1 << 1),
    AX_MIXER_CTRL_RMT_A0 = (1 << 2),
    AX_MIXER_CTRL_RMT_DELTA_A0 = (1 << 3),

    AX_MIXER_CTRL_RMT_M1 = (1 << 4),
    AX_MIXER_CTRL_RMT_DELTA_M1 = (1 << 5),
    AX_MIXER_CTRL_RMT_A1 = (1 << 6),
    AX_MIXER_CTRL_RMT_DELTA_A1 = (1 << 7),

    AX_MIXER_CTRL_RMT_M2 = (1 << 8),
    AX_MIXER_CTRL_RMT_DELTA_M2 = (1 << 9),
    AX_MIXER_CTRL_RMT_A2 = (1 << 10),
    AX_MIXER_CTRL_RMT_DELTA_A2 = (1 << 11),

    AX_MIXER_CTRL_RMT_M3 = (1 << 12),
    AX_MIXER_CTRL_RMT_DELTA_M3 = (1 << 13),
    AX_MIXER_CTRL_RMT_A3 = (1 << 14),
    AX_MIXER_CTRL_RMT_DELTA_A3 = (1 << 15)
};

typedef enum {
    AX_PBSYNC_SELECT = (1 << 0),
    AX_PBSYNC_MIXER_CTRL = (1 << 1),
    AX_PBSYNC_STATE = (1 << 2),
    AX_PBSYNC_TYPE = (1 << 3),
    AX_PBSYNC_MIX = (1 << 4),
    AX_PBSYNC_ITD = (1 << 5),
    AX_PBSYNC_ITD_SHIFT = (1 << 6),
    AX_PBSYNC_DPOP = (1 << 7),
    AX_PBSYNC_VE = (1 << 8),
    AX_PBSYNC_VE_DELTA = (1 << 9),
    AX_PBSYNC_ADDR = (1 << 10),
    AX_PBSYNC_LOOP_FLAG = (1 << 11),
    AX_PBSYNC_LOOP_ADDR = (1 << 12),
    AX_PBSYNC_END_ADDR = (1 << 13),
    AX_PBSYNC_CURR_ADDR = (1 << 14),
    AX_PBSYNC_ADPCM = (1 << 15),
    AX_PBSYNC_SRC = (1 << 16),
    AX_PBSYNC_SRC_RATIO = (1 << 17),
    AX_PBSYNC_ADPCM_LOOP = (1 << 18),
    AX_PBSYNC_LPF = (1 << 19),
    AX_PBSYNC_LPF_COEFS = (1 << 20),
    AX_PBSYNC_BIQUAD = (1 << 21),
    AX_PBSYNC_BIQUAD_COEFS = (1 << 22),
    AX_PBSYNC_REMOTE = (1 << 23),
    AX_PBSYNC_RMT_MIXER_CTRL = (1 << 24),
    AX_PBSYNC_RMTMIX = (1 << 25),
    AX_PBSYNC_RMTDPOP = (1 << 26),
    AX_PBSYNC_RMTSRC = (1 << 27),
    AX_PBSYNC_RMTIIR = (1 << 28),
    AX_PBSYNC_RMTIIR_LPF_COEFS = (1 << 29),
    AX_PBSYNC_RMTIIR_BIQUAD_COEFS = (1 << 30),
    AX_PBSYNC_ALL = (1 << 31),
};

#define AX_VOICE_MAX 96

typedef struct _AXVPB {
    void* next;                // at 0x0
    void* prev;                // at 0x4
    void* next1;               // at 0x8
    u32 priority;              // at 0xC
    AXVoiceCallback callback;  // at 0x10
    u32 userContext;           // at 0x14
    u32 index;                 // at 0x18
    u32 sync;                  // at 0x1C
    u32 depop;                 // at 0x20
    void* itdBuffer;           // at 0x24
    AXPB pb;                   // at 0x28
} AXVPB;

typedef void (*AXAuxCallback)(void* chans, void* context);

void __AXSetPBDefault(AXVPB* vpb);

#endif  // AX_H
