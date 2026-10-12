// gx misc: flush/abort, draw sync and draw done handling, the pe poke/peek helpers, and the pe interrupt setup.
// (register poking here is mostly direct pe reg access through __peReg)
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"
#include "revolution/os.h"
#include "revolution/base/PPCArch.h"

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)

typedef void (*GXDrawSyncCallback)(u16 token);
typedef void (*GXDrawDoneCallback)(void);

extern volatile void* __piReg;
extern void __GXCleanGPFifo(void);

// declared backwards on purpose: the c compiler lays statics out in reverse order
static GXDrawSyncCallback TokenCB;
static GXDrawDoneCallback DrawDoneCB;
static volatile u8 DrawDone;
static OSThreadQueue FinishQueue;

void GXSetMisc(GXMiscToken token, u32 val) {
    switch (token) {
    case GX_MT_NULL:
        break;
    case GX_MT_XF_FLUSH:
        gx->vNum = val;
        gx->vNumNot = !gx->vNum;
        gx->bpSentNot = GX_TRUE;
        if (gx->vNum) {
            gx->dirtyState |= 8;
        }
        break;
    case GX_MT_DL_SAVE_CONTEXT:
        gx->dlSaveContext = (val != 0);
        break;
    case GX_MT_ABORT_WAIT_COPYOUT:
        gx->abtWaitPECopy = (val != 0);
        break;
    }
}

void GXFlush(void) {
    if (gx->dirtyState) {
        __GXSetDirtyState();
    }

    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    GX_WRITE_U32(0);
    PPCSync();
}

void GXResetWriteGatherPipe(void) {
    while (PPCMfwpar() & 1) {
        ;
    }
    PPCMtwpar(0x0C008000);
}

static inline u32 ReadMemCtr_i(u32 regAddrL, u32 regAddrH) {
    u32 ctrL;
    u32 ctrH1;
    u32 ctrH0;
    ctrH0 = GX_MEM_REG_READ_U16(regAddrH);
    do {
        ctrH1 = ctrH0;
        ctrL = GX_MEM_REG_READ_U16(regAddrL);
        ctrH0 = GX_MEM_REG_READ_U16(regAddrH);
    } while (ctrH0 != ctrH1);
    return (ctrH0 << 16) | ctrL;
}

static inline void Abort_i(void) {
    u32 reg;
    u32 reg2;
    OSTime time;

    if (gx->abtWaitPECopy && __GXIsGPFifoReady()) {
        reg = ReadMemCtr_i(MEM_PE_REQCOUNTL_IDX, MEM_PE_REQCOUNTH_IDX);
        do {
            time = OSGetTime();
            while (OSGetTime() - time <= 8) {
                ;
            }
            reg2 = reg;
            reg = ReadMemCtr_i(MEM_PE_REQCOUNTL_IDX, MEM_PE_REQCOUNTH_IDX);
        } while (reg != reg2);
    }

    *(vu32*)0xCC003018 = 1;
    time = OSGetTime();
    while (OSGetTime() - time <= 50) {
        ;
    }
    *(vu32*)0xCC003018 = 0;
    time = OSGetTime();
    while (OSGetTime() - time <= 5) {
        ;
    }
}

void fn_801F05C0(void) {
    u32 reg;
    u32 reg2;
    OSTime time;

    if (gx->abtWaitPECopy && __GXIsGPFifoReady()) {
        reg = ReadMemCtr_i(MEM_PE_REQCOUNTL_IDX, MEM_PE_REQCOUNTH_IDX);
        do {
            time = OSGetTime();
            while (OSGetTime() - time <= 8) {
                ;
            }
            reg2 = reg;
            reg = ReadMemCtr_i(MEM_PE_REQCOUNTL_IDX, MEM_PE_REQCOUNTH_IDX);
        } while (reg != reg2);
    }

    *(vu32*)0xCC003018 = 1;
    time = OSGetTime();
    while (OSGetTime() - time <= 50) {
        ;
    }
    *(vu32*)0xCC003018 = 0;
    time = OSGetTime();
    while (OSGetTime() - time <= 5) {
        ;
    }
}

void GXAbortFrame(void) {
    Abort_i();

    if (__GXIsGPFifoReady()) {
        __GXCleanGPFifo();
        __GXInitRevisionBits();
        gx->dirtyState = 0;
        GXFlush();
    }
}

void GXSetDrawSync(u16 token) {
    u32 reg;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    reg = token | 0x48000000;
    GX_WRITE_RA_REG(reg);

    SET_REG_FIELD(reg, 16, 0, token);
    SET_REG_FIELD(reg, 8, 24, 0x47);
    GX_WRITE_RA_REG(reg);

    GXFlush();
    OSRestoreInterrupts(enabled);
    gx->bpSentNot = GX_FALSE;
}

u16 GXReadDrawSync(void) {
    return GX_PE_REG_READ_U16(7);
}

void GXSetDrawDone(void) {
    BOOL enabled;

    enabled = OSDisableInterrupts();
    GX_WRITE_RA_REG(0x45000002);
    GXFlush();
    DrawDone = 0;
    OSRestoreInterrupts(enabled);
}

void GXDrawDone(void) {
    BOOL enabled;

    GXSetDrawDone();

    enabled = OSDisableInterrupts();
    while (!DrawDone) {
        OSSleepThread(&FinishQueue);
    }
    OSRestoreInterrupts(enabled);
}

void GXPixModeSync(void) {
    GX_WRITE_RA_REG(gx->peCtrl);
    gx->bpSentNot = GX_FALSE;
}

void GXPokeAlphaMode(GXCompare func, u8 threshold) {
    u32 reg = threshold;
    reg = __rlwimi(reg, func, 8, 0, 23);
    GX_PE_REG_WRITE_U16(3, reg);
}

void GXPokeAlphaRead(GXAlphaReadMode mode) {
    u32 reg = 0;
    SET_REG_FIELD(reg, 2, 0, mode);
    reg |= 4;
    GX_PE_REG_WRITE_U16(4, reg);
}

void GXPokeAlphaUpdate(GXBool update) {
    u32 reg = GX_PE_REG_READ_U16(1);
    SET_REG_FIELD(reg, 1, 4, update);
    GX_PE_REG_WRITE_U16(1, reg);
}

void GXPokeBlendMode(GXBlendMode type, GXBlendFactor src, GXBlendFactor dst, GXLogicOp op) {
    u32 reg = GX_PE_REG_READ_U16(1);
    u32 blend = (type == GX_BM_BLEND) || (type == GX_BM_SUBTRACT);
    u32 logic = (type == GX_BM_LOGIC);
    u32 sub = (type == GX_BM_SUBTRACT);

    SET_REG_FIELD(reg, 1, 0, blend);
    SET_REG_FIELD(reg, 1, 11, sub);
    SET_REG_FIELD(reg, 1, 1, logic);
    SET_REG_FIELD(reg, 4, 12, op);
    SET_REG_FIELD(reg, 3, 8, src);
    SET_REG_FIELD(reg, 3, 5, dst);
    SET_REG_FIELD(reg, 8, 24, 0x41);
    GX_PE_REG_WRITE_U16(1, reg);
}

void GXPokeColorUpdate(GXBool update) {
    u32 reg = GX_PE_REG_READ_U16(1);
    SET_REG_FIELD(reg, 1, 3, update);
    GX_PE_REG_WRITE_U16(1, reg);
}

void GXPokeDstAlpha(GXBool enable, u8 alpha) {
    u32 reg = 0;
    SET_REG_FIELD(reg, 8, 0, alpha);
    SET_REG_FIELD(reg, 1, 8, enable);
    GX_PE_REG_WRITE_U16(2, reg);
}

void GXPokeDither(GXBool dither) {
    u32 reg = GX_PE_REG_READ_U16(1);
    SET_REG_FIELD(reg, 1, 2, dither);
    GX_PE_REG_WRITE_U16(1, reg);
}

void GXPokeZMode(GXBool enable, GXCompare func, GXBool update) {
    u32 reg = 0;
    SET_REG_FIELD(reg, 1, 0, enable);
    SET_REG_FIELD(reg, 3, 1, func);
    SET_REG_FIELD(reg, 1, 4, update);
    GX_PE_REG_WRITE_U16(0, reg);
}

void GXPeekZ(u16 x, u16 y, u32* z) {
    u32 addr = 0xC8000000;
    SET_REG_FIELD(addr, 10, 2, x);
    SET_REG_FIELD(addr, 10, 12, y);
    *z = *(vu32*)addr;
}

GXDrawSyncCallback GXSetDrawSyncCallback(GXDrawSyncCallback cb) {
    GXDrawSyncCallback old = TokenCB;
    BOOL enabled = OSDisableInterrupts();
    TokenCB = cb;
    OSRestoreInterrupts(enabled);
    return old;
}

void GXTokenInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    u16 token = GX_PE_REG_READ_U16(7);

    if (TokenCB) {
        OSContext exceptContext;
        OSClearContext(&exceptContext);
        OSSetCurrentContext(&exceptContext);
        TokenCB(token);
        OSClearContext(&exceptContext);
        OSSetCurrentContext(context);
    }

    GX_PE_REG_WRITE_U16(5, GX_PE_REG_READ_U16(5) | 4);
}

GXDrawDoneCallback GXSetDrawDoneCallback(GXDrawDoneCallback cb) {
    GXDrawDoneCallback old = DrawDoneCB;
    BOOL enabled = OSDisableInterrupts();
    DrawDoneCB = cb;
    OSRestoreInterrupts(enabled);
    return old;
}

void GXFinishInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptContext;

    GX_PE_REG_WRITE_U16(5, GX_PE_REG_READ_U16(5) | 8);
    DrawDone = 1;

    if (DrawDoneCB) {
        OSClearContext(&exceptContext);
        OSSetCurrentContext(&exceptContext);
        DrawDoneCB();
        OSClearContext(&exceptContext);
        OSSetCurrentContext(context);
    }

    OSWakeupThread(&FinishQueue);
}

void __GXPEInit(void) {
    __OSSetInterruptHandler(18, GXTokenInterruptHandler);
    __OSSetInterruptHandler(19, GXFinishInterruptHandler);
    OSInitThreadQueue(&FinishQueue);
    __OSUnmaskInterrupts(0x2000);
    __OSUnmaskInterrupts(0x1000);
    GX_PE_REG_WRITE_U16(5, GX_PE_REG_READ_U16(5) | 0xF);
}
