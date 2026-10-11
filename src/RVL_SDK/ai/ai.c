// ai: audio interface. dma of the output buffer is driven through the dsp block's registers, the
// interrupt handler runs the user callback on its own stack if one was handed to AIInit.
#include "revolution/ai.h"
#include "revolution/os.h"

// dsp interface registers (half words) and ai registers (words)
#define DSP_HW_REGS ((volatile u16*)0xCC005000)
#define AI_HW_REGS ((volatile u32*)0xCD006C00)

#define DSP_CSR 5
#define DSP_AI_DMA_START_H 24
#define DSP_AI_DMA_START_L 25
#define DSP_AI_DMA_CSR 27
#define DSP_AI_DMA_BYTES_LEFT 29

#define DSP_CSR_AIDINT (1 << 3)
#define DSP_CSR_DSPINT (1 << 7)
#define DSP_CSR_ARINT (1 << 5)
#define DSP_AI_DMA_CSR_PLAY (1 << 15)

#define AI_AICR 0
#define AI_AIVR 1
#define AI_AISCNT 2
#define AI_AIIT 3

#define AI_AICR_SAMPLERATE (1 << 6)
#define AI_AICR_SCRESET (1 << 5)
#define AI_AICR_AIINTVLD (1 << 4)
#define AI_AICR_AIINTMSK (1 << 2)
#define AI_AICR_AFR (1 << 1)
#define AI_AICR_PSTAT (1 << 0)

#define AI_DSP_32KHZ 0

typedef void (*AIDMACallback)(void);

const char* __AIVersion = "<< RVL_SDK - AI \trelease build: May  8 2007 12:54:34 (0x4199_60831) >>";

static AIDMACallback __AID_Callback;

static void* __CallbackStack;
static void* __OldStack;

static s64 bound_32KHz;
static s64 bound_48KHz;

static s64 min_wait;
static s64 max_wait;
static s64 buffer;

static BOOL __AID_Active;
static BOOL __AI_init_flag;

extern void __AIDHandler(s16 intr, OSContext* ctx);
static void __AICallbackStackSwitch(AIDMACallback callback);
static void __AI_SRC_INIT(void);

AIDMACallback AIRegisterDMACallback(AIDMACallback callback) {
    BOOL enabled;
    AIDMACallback old;

    old = __AID_Callback;
    enabled = OSDisableInterrupts();

    __AID_Callback = callback;

    OSRestoreInterrupts(enabled);
    return old;
}

void AIInitDMA(u32 buffer, u32 length) {
    BOOL enabled;

    enabled = OSDisableInterrupts();

    DSP_HW_REGS[DSP_AI_DMA_START_H] = (DSP_HW_REGS[DSP_AI_DMA_START_H] & ~0x1FFF) | ((buffer) >> 16 & 0xFFFF);

    DSP_HW_REGS[DSP_AI_DMA_START_L] = (DSP_HW_REGS[DSP_AI_DMA_START_L] & ~0xFFE0) | ((buffer) & 0xFFFF);

    DSP_HW_REGS[DSP_AI_DMA_CSR] = (DSP_HW_REGS[DSP_AI_DMA_CSR] & ~0x7FFF) | ((length / 32) & 0xFFFF);

    OSRestoreInterrupts(enabled);
}

void AIStartDMA(void) {
    DSP_HW_REGS[DSP_AI_DMA_CSR] |= DSP_AI_DMA_CSR_PLAY;
}

void AIStopDMA(void) {
    DSP_HW_REGS[DSP_AI_DMA_CSR] &= ~DSP_AI_DMA_CSR_PLAY;
}

u32 AIGetDMABytesLeft(void) {
    return (DSP_HW_REGS[DSP_AI_DMA_BYTES_LEFT] & 0x7FFF) * 32;
}

u32 AIGetDMAStartAddr(void) {
    return (DSP_HW_REGS[DSP_AI_DMA_START_H] & 0x1FFF) << 16 | (DSP_HW_REGS[DSP_AI_DMA_START_L] & 0xFFE0);
}

u32 AIGetDMALength(void) {
    return (DSP_HW_REGS[DSP_AI_DMA_CSR] & 0x7FFF) * 32;
}

BOOL AICheckInit(void) {
    return __AI_init_flag;
}

static void AISetDSPSampleRate(u32 rate);
static u32 AIGetDSPSampleRate(void);

void AIInit(u8* stack) {
    if (__AI_init_flag != TRUE) {
        OSRegisterVersion(__AIVersion);

        bound_32KHz = OSNanosecondsToTicks(31524);
        bound_48KHz = OSNanosecondsToTicks(42024);
        min_wait = OSNanosecondsToTicks(42000);
        max_wait = OSNanosecondsToTicks(63000);
        buffer = OSNanosecondsToTicks(3000);

        AI_HW_REGS[AI_AICR] &= ~(AI_AICR_PSTAT | AI_AICR_AIINTMSK | AI_AICR_AIINTVLD);

        AI_HW_REGS[AI_AIVR] = 0;
        AI_HW_REGS[AI_AIIT] = 0;

        AI_HW_REGS[AI_AICR] = (AI_HW_REGS[AI_AICR] & ~AI_AICR_SCRESET) | AI_AICR_SCRESET;

        AISetDSPSampleRate(AI_DSP_32KHZ);

        __AID_Callback = NULL;
        __CallbackStack = stack;

        __OSSetInterruptHandler(__OS_INTERRUPT_DSP_AI, __AIDHandler);
        __OSUnmaskInterrupts(OS_INTERRUPTMASK_DSP_AI);

        __AI_init_flag = TRUE;
    }
}

void __AIDHandler(s16 intr, OSContext* ctx) {
#pragma unused(intr)

    OSContext tempCtx;

    DSP_HW_REGS[DSP_CSR] = (DSP_HW_REGS[DSP_CSR] & ~(DSP_CSR_ARINT | DSP_CSR_DSPINT)) | DSP_CSR_AIDINT;

    OSClearContext(&tempCtx);
    OSSetCurrentContext(&tempCtx);

    if (__AID_Callback != NULL && !__AID_Active) {
        __AID_Active = TRUE;

        if (__CallbackStack != NULL) {
            __AICallbackStackSwitch(__AID_Callback);
        } else {
            __AID_Callback();
        }

        __AID_Active = FALSE;
    }

    OSClearContext(&tempCtx);
    OSSetCurrentContext(ctx);
}

static asm void __AICallbackStackSwitch(AIDMACallback callback) {
    fralloc
    mr r31, r3

    lis r5, __OldStack@ha
    addi r5, r5, __OldStack@l
    stw r1, 0(r5)

    lis r5, __CallbackStack@ha
    addi r5, r5, __CallbackStack@l
    lwz r1, 0(r5)

    subi r1, r1, 0x8
    mtlr r31
    blrl

    lis r5, __OldStack@ha
    addi r5, r5, __OldStack@l
    lwz r1, 0(r5)

    frfree
    blr
}

static u32 AIGetDSPSampleRate(void) {
    return ((AI_HW_REGS[AI_AICR] & AI_AICR_SAMPLERATE) >> 6) ^ 1;
}

static void AISetDSPSampleRate(u32 rate) {
    BOOL enabled;

    if (rate != AIGetDSPSampleRate()) {
        AI_HW_REGS[AI_AICR] &= ~AI_AICR_SAMPLERATE;

        if (rate == AI_DSP_32KHZ) {
            enabled = OSDisableInterrupts();

            __AI_SRC_INIT();
            AI_HW_REGS[AI_AICR] |= AI_AICR_SAMPLERATE;

            OSRestoreInterrupts(enabled);
        }
    }
}

static void __AI_SRC_INIT(void) {
    s64 start = 0;
    s64 end = 0;
    BOOL exit = FALSE;
    s64 diff = 0;
    s64 wait = 0;
    u32 samples = 0;

    while (!exit) {
        AI_HW_REGS[AI_AICR] = (AI_HW_REGS[AI_AICR] & ~AI_AICR_SCRESET) | AI_AICR_SCRESET;

        AI_HW_REGS[AI_AICR] &= ~AI_AICR_AFR;

        AI_HW_REGS[AI_AICR] = (AI_HW_REGS[AI_AICR] & ~AI_AICR_PSTAT) | AI_AICR_PSTAT;

        // wait for the counter to tick over once
        samples = AI_HW_REGS[AI_AISCNT] & ~0x80000000;
        while (samples == (AI_HW_REGS[AI_AISCNT] & ~0x80000000)) {
            ;
        }

        start = OSGetTime();

        AI_HW_REGS[AI_AICR] = (AI_HW_REGS[AI_AICR] & ~AI_AICR_AFR) | AI_AICR_AFR;

        AI_HW_REGS[AI_AICR] = (AI_HW_REGS[AI_AICR] & ~AI_AICR_PSTAT) | AI_AICR_PSTAT;

        samples = AI_HW_REGS[AI_AISCNT] & ~0x80000000;
        while (samples == (AI_HW_REGS[AI_AISCNT] & ~0x80000000)) {
            ;
        }

        end = OSGetTime();
        diff = end - start;

        AI_HW_REGS[AI_AICR] &= ~AI_AICR_AFR;
        AI_HW_REGS[AI_AICR] &= ~AI_AICR_PSTAT;

        if (diff < bound_32KHz - buffer) {
            wait = min_wait;
            exit = TRUE;
        } else if (diff >= bound_32KHz + buffer && diff < bound_48KHz - buffer) {
            wait = max_wait;
            exit = TRUE;
        } else {
            exit = FALSE;
        }
    }

    while (end + wait > OSGetTime()) {
        ;
    }
}
