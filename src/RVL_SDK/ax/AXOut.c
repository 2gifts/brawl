// AXOut: builds the dsp command list for every audio frame (the "cl" half) and drives the output side, i.e.
// the dsp task, the ai dma ring and the remote (wiimote speaker) sample buffers. the two used to be separate
// source files, they share one split here.
#include "revolution/ax.h"
#include "revolution/os.h"
#include "revolution/ai.h"
#include "revolution/dsp.h"

// MATCH-ONLY: pointer compares against NULL come out as cmpwi in retail, not cmplwi
#undef NULL
#define NULL 0

#define AX_SAMPLE_DEPTH_BYTES 4
#define AX_SAMPLES_PER_FRAME 96
#define AX_FRAME_SIZE (AX_SAMPLES_PER_FRAME * AX_SAMPLE_DEPTH_BYTES)
#define AX_SAMPLES_PER_FRAME_RMT 18
#define AX_RMT_MAX 4
#define AX_MAX_VOLUME 0x8000

#define AX_OUTPUT_STEREO 0
#define AX_OUTPUT_SURROUND 1
#define AX_OUTPUT_DPL2 2

// compression parameters
#define THRESHOLD 32768
#define RELEASE_FRAMES 10

// two command lists are kept, one is being built while the dsp reads the other
#define LIST_MAX 2
#define AX_CL_SIZE 0x80

// output uses a ring of frame buffers
#define OUT_RING_MAX 3
// every remote output buffer holds 10 frames
#define RMT_FRAME_MAX 10

#define DSP_MAIL_CLSIZE(size) (0xBABE << 16 | (size))
// where the ram image of the task lives in dsp memory
#define DRAM_IMAGE_DSP_ADDR 0x0CD2

typedef void (*AXOutCallback)(void);
typedef void (*AXExceedCallback)(u32 cycles);

typedef struct AXProfile {
    OSTime timeBegin;
    OSTime timeProcessAuxBegin;
    OSTime timeProcessAuxEnd;
    OSTime timeUserFrameBegin;
    OSTime timeUserFrameEnd;
    OSTime timeEnd;
    u32 numVoices;
    u32 pad;
} AXProfile;

// the dsp microcode and its entry points, those live in .data/.sdata of this split
extern u8 axDspSlave[];
extern u16 axDspInitVector;
extern u16 axDspResumeVector;
extern u16 axDspSlaveLength;
extern u16 __AXCompressorTable[];

extern void *__AXGetStudio(void);
extern void *__AXGetPBs(void);
extern void __AXSyncPBs(u32 baseSample);
extern void __AXPrintStudio(void);
extern void __AXServiceCallbackStack(void);
extern void __AXProcessAux(void);
extern u32 __AXGetNumVoices(void);
extern void *__AXGetCurrentProfile(void);
extern void __AXGetAuxAInput(void **out);
extern void __AXGetAuxAInputDpl2(void **out);
extern void __AXGetAuxAOutput(void **out);
extern void __AXGetAuxAOutputDpl2R(void **out);
extern void __AXGetAuxAOutputDpl2Ls(void **out);
extern void __AXGetAuxAOutputDpl2Rs(void **out);
extern void __AXGetAuxBInput(void **out);
extern void __AXGetAuxBInputDpl2(void **out);
extern void __AXGetAuxBOutput(void **out);
extern void __AXGetAuxBOutputDpl2R(void **out);
extern void __AXGetAuxBOutputDpl2Ls(void **out);
extern void __AXGetAuxBOutputDpl2Rs(void **out);
extern void __AXGetAuxCInput(void **out);
extern void __AXGetAuxCOutput(void **out);
extern void DSPAddTask(DSPTaskInfo *task);
extern void DSPAssertTask(DSPTaskInfo *task);
extern void DSPCancelTask(DSPTaskInfo *task);
extern BOOL DSPCheckInit(void);
extern u32 AIGetDMABytesLeft(void);

#define DSP_SEND_MAIL_SYNC(mail)    \
    do {                            \
        DSPSendMailToDSP(mail);     \
        while (DSPCheckMailToDSP()) \
            ;                       \
    } while (0)

// every write goes through the global cursor, the compiler never keeps it in a register
#define LIST_WRITE_16(x)         \
    do {                         \
        *__AXClWrite = (x);      \
        __AXClWrite++;           \
    } while (0)

#define LIST_WRITE_32(x)               \
    do {                               \
        LIST_WRITE_16(((u32)(x)) >> 16); \
        LIST_WRITE_16((x));            \
    } while (0)

// opcodes of the command list the dsp interprets
enum {
    COMMAND_SETUP,
    COMMAND_ADD_TO_LR,
    COMMAND_SUB_TO_LR,
    COMMAND_ADD_SUB_TO_LR,
    COMMAND_PROCESS,
    COMMAND_MIX_AUXA,
    COMMAND_MIX_AUXB,
    COMMAND_MIX_AUXC,
    COMMAND_UPL_AUXA_MIX_LRSC,
    COMMAND_UPL_AUXB_MIX_LRSC,
    COMMAND_COMPRESSOR,
    COMMAND_OUTPUT,
    COMMAND_OUTPUT_DPL2,
    COMMAND_WM_OUTPUT,
    COMMAND_END
};

static u16 __AXAuxAVolume;
static u16 __AXAuxBVolume;
static u16 __AXAuxCVolume;
static volatile u16 __AXMasterVolume;
static BOOL __AXCompressor;
static u32 __AXCommandListCycles;
static u16 *__AXClWrite;
static u32 __AXCommandListPosition;
u32 __AXClMode;

static u8 __AXCommandList[AX_CL_SIZE * LIST_MAX];

u32 __AXGetCommandListCycles(void) {
    return __AXCommandListCycles;
}

void *__AXGetCommandListAddress(void) __attribute__((never_inline));
void *__AXGetCommandListAddress(void) {
    void *list = __AXCommandList + __AXCommandListPosition * AX_CL_SIZE;

    __AXCommandListPosition++;
    __AXCommandListPosition %= LIST_MAX;
    __AXClWrite = (u16 *)(__AXCommandList + __AXCommandListPosition * AX_CL_SIZE);

    return list;
}

void __AXNextFrame(void *surround, void *lr, void *rmt) {
    void *imm;
    void *backup;

    __AXCommandListCycles = 7811;
    backup = __AXClWrite;
    imm = __AXGetStudio();

    LIST_WRITE_16(COMMAND_SETUP);
    LIST_WRITE_32((u32)imm);
    __AXCommandListCycles += 4126;

    switch (__AXClMode) {
    case AX_OUTPUT_STEREO:
        LIST_WRITE_16(COMMAND_ADD_TO_LR);
        LIST_WRITE_32((u32)surround);
        __AXCommandListCycles += 733;
        break;
    case AX_OUTPUT_SURROUND:
        LIST_WRITE_16(COMMAND_SUB_TO_LR);
        LIST_WRITE_32((u32)surround);
        __AXCommandListCycles += 829;
        break;
    case AX_OUTPUT_DPL2:
        LIST_WRITE_16(COMMAND_ADD_SUB_TO_LR);
        LIST_WRITE_32((u32)surround);
        __AXCommandListCycles += 925;
        break;
    }

    imm = __AXGetPBs();
    LIST_WRITE_16(COMMAND_PROCESS);
    LIST_WRITE_32((u32)imm);

    if (__AXClMode == AX_OUTPUT_DPL2) {
        __AXGetAuxAInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_UPL_AUXA_MIX_LRSC);
            LIST_WRITE_16(__AXAuxAVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAInputDpl2(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutputDpl2R(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutputDpl2Ls(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutputDpl2Rs(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 3036;
        }

        __AXGetAuxBInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_UPL_AUXB_MIX_LRSC);
            LIST_WRITE_16(__AXAuxBVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBInputDpl2(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutputDpl2R(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutputDpl2Ls(&imm);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutputDpl2Rs(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 3036;
        }
    } else {
        __AXGetAuxAInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_MIX_AUXA);
            LIST_WRITE_16(__AXAuxAVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxAOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 2235;
        }

        __AXGetAuxBInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_MIX_AUXB);
            LIST_WRITE_16(__AXAuxBVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxBOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 2235;
        }

        __AXGetAuxCInput(&imm);
        if (imm != NULL) {
            LIST_WRITE_16(COMMAND_MIX_AUXC);
            LIST_WRITE_16(__AXAuxCVolume);
            LIST_WRITE_32((u32)imm);

            __AXGetAuxCOutput(&imm);
            LIST_WRITE_32((u32)imm);

            __AXCommandListCycles += 2235;
        }
    }

    if (__AXCompressor) {
        LIST_WRITE_16(COMMAND_COMPRESSOR);
        LIST_WRITE_16(THRESHOLD);
        LIST_WRITE_16(RELEASE_FRAMES);
        LIST_WRITE_32((u32)__AXCompressorTable);
        __AXCommandListCycles += 1850;
    }

    LIST_WRITE_16(COMMAND_WM_OUTPUT);
    LIST_WRITE_32(((u32 *)rmt)[0]);
    LIST_WRITE_32(((u32 *)rmt)[1]);
    LIST_WRITE_32(((u32 *)rmt)[2]);
    LIST_WRITE_32(((u32 *)rmt)[3]);
    __AXCommandListCycles += 409;

    if (__AXClMode == AX_OUTPUT_DPL2) {
        LIST_WRITE_16(COMMAND_OUTPUT_DPL2);
        LIST_WRITE_16(__AXMasterVolume);
        LIST_WRITE_32((u32)surround);
        LIST_WRITE_32((u32)lr);
        __AXCommandListCycles += 1195;
    } else {
        LIST_WRITE_16(COMMAND_OUTPUT);
        LIST_WRITE_16(__AXMasterVolume);
        LIST_WRITE_32((u32)surround);
        LIST_WRITE_32((u32)lr);
        __AXCommandListCycles += 1172;
    }

    LIST_WRITE_16(COMMAND_END);
    __AXCommandListCycles += 30;

    DCFlushRange(backup, AX_CL_SIZE);
}

void __AXClInit(void) {
    __AXClMode = AX_OUTPUT_STEREO;
    __AXCommandListPosition = 0;
    __AXClWrite = (u16 *)__AXCommandList;
    __AXCompressor = TRUE;

    __AXAuxCVolume = __AXAuxBVolume = __AXAuxAVolume = __AXMasterVolume = AX_MAX_VOLUME;
}

void __AXClQuit(void) {
}

void AXSetMode(u32 mode) {
    __AXClMode = mode;
}

u16 AXGetAuxAReturnVolume(void) {
    return __AXAuxAVolume;
}

u16 AXGetAuxBReturnVolume(void) {
    return __AXAuxBVolume;
}

u16 AXGetAuxCReturnVolume(void) {
    return __AXAuxCVolume;
}

void AXSetMasterVolume(u16 volume) {
    if (volume > AX_MAX_VOLUME) {
        volume = AX_MAX_VOLUME;
    }

    __AXMasterVolume = volume;
}

void AXSetAuxAReturnVolume(u16 volume) {
    __AXAuxAVolume = volume;
}

void AXSetAuxBReturnVolume(u16 volume) {
    __AXAuxBVolume = volume;
}

void AXSetAuxCReturnVolume(u16 volume) {
    __AXAuxCVolume = volume;
}

AXProfile __AXLocalProfile;

static volatile u32 __AXOutFrame;
static u32 __AXAiDmaFrame;
static u32 __AXOutDspReady;
static AXOutCallback __AXUserFrameCallback;
static volatile BOOL __AXDSPInitFlag;
static volatile BOOL __AXDSPDoneFlag;
static BOOL __AXDebugSteppingMode;
static OSThreadQueue __AXOutThreadQueue;
static u32 __AXOutputBufferMode;
static s32 __AXRmtBuffLen;
static s32 __AXRmtDspPtr;
static s32 __AXRmtCpuPtr;
static AXExceedCallback __AXExceedCallback;

static s16 __AXRmtOutBuffer[AX_RMT_MAX][AX_SAMPLES_PER_FRAME_RMT * RMT_FRAME_MAX] __attribute__((aligned(32)));
static u8 __AXOutSBuffer[768] __attribute__((aligned(32)));
static u32 __AXOutBuffer[OUT_RING_MAX][AX_SAMPLES_PER_FRAME] __attribute__((aligned(32)));
static DSPTaskInfo __AXDSPTask __attribute__((aligned(32)));
static u8 __AXDramImage[64] __attribute__((aligned(32)));

static void __AXDSPInitCallback(void *task);
static void __AXDSPResumeCallback(void *task);
static void __AXDSPDoneCallback(void *task);
static void __AXDSPRequestCallback(void *task);

// a plain byte copy that the compiler turns into the unrolled word loop seen in retail
#define FUNNY_COPY(dst, src, type, n)                  \
    do {                                               \
        type *__dst = (type *)dst;                     \
        type *__src = (type *)src;                     \
        int i;                                         \
                                                       \
        for (i = 0; i < n / sizeof(type); i++) {       \
            *__dst = *__src;                           \
            __dst++;                                   \
            __src++;                                   \
        }                                              \
    } while (0)

u32 __AXOutNewFrame(void) {
    AXProfile *current;
    u32 cycles;
    void *list;
    void *rmt[AX_RMT_MAX];
    s32 ptr;

    __AXLocalProfile.timeBegin = OSGetTime();

    cycles = (AX_SAMPLES_PER_FRAME - (AIGetDMABytesLeft() / AX_SAMPLE_DEPTH_BYTES)) * 3797;

    if (__AXOutputBufferMode == 1) {
        __AXSyncPBs(0);
    } else {
        __AXSyncPBs(24400);
    }

    __AXPrintStudio();

    list = __AXGetCommandListAddress();
    DSP_SEND_MAIL_SYNC(DSP_MAIL_CLSIZE(AX_CL_SIZE));
    DSP_SEND_MAIL_SYNC((u32)list);

    __AXServiceCallbackStack();

    __AXLocalProfile.timeProcessAuxBegin = OSGetTime();
    __AXProcessAux();
    __AXLocalProfile.timeProcessAuxEnd = OSGetTime();

    __AXLocalProfile.timeUserFrameBegin = OSGetTime();
    if (__AXUserFrameCallback != NULL) {
        __AXUserFrameCallback();
    }
    __AXLocalProfile.timeUserFrameEnd = OSGetTime();

    rmt[0] = &__AXRmtOutBuffer[0][__AXRmtDspPtr];
    rmt[1] = &__AXRmtOutBuffer[1][__AXRmtDspPtr];
    rmt[2] = &__AXRmtOutBuffer[2][__AXRmtDspPtr];
    rmt[3] = &__AXRmtOutBuffer[3][__AXRmtDspPtr];

    ptr = __AXRmtDspPtr + AX_SAMPLES_PER_FRAME_RMT;
    if (ptr >= __AXRmtBuffLen) {
        ptr = 0;
    }

    if (__AXRmtCpuPtr >= __AXRmtDspPtr && __AXRmtCpuPtr < __AXRmtDspPtr + AX_SAMPLES_PER_FRAME_RMT) {
        __AXRmtCpuPtr = ptr;
    }

    __AXRmtDspPtr = ptr;
    __AXNextFrame(__AXOutSBuffer, __AXOutBuffer[__AXOutFrame], rmt);
    __AXOutFrame++;

    if (__AXOutputBufferMode == 1) {
        __AXOutFrame %= OUT_RING_MAX;
    } else {
        __AXOutFrame &= 1;
        AIInitDMA((u32)__AXOutBuffer[__AXOutFrame], AX_FRAME_SIZE);
    }

    __AXLocalProfile.timeEnd = OSGetTime();
    __AXLocalProfile.numVoices = __AXGetNumVoices();

    current = (AXProfile *)__AXGetCurrentProfile();
    if (current != NULL) {
        FUNNY_COPY(current, &__AXLocalProfile, u8, sizeof(AXProfile));
    }

    return cycles;
}

void __AXOutAiCallback(void) {
    u32 frame;

    if (__AXOutDspReady == 1) {
        __AXOutDspReady = 0;
        __AXOutNewFrame();
    } else {
        __AXOutDspReady = 2;
        DSPAssertTask(&__AXDSPTask);
    }

    if (__AXOutputBufferMode == 1) {
        AIInitDMA((u32)__AXOutBuffer[__AXAiDmaFrame], AX_FRAME_SIZE);

        frame = __AXAiDmaFrame + 1;
        frame %= OUT_RING_MAX;

        if (frame != __AXOutFrame) {
            __AXAiDmaFrame = frame;
        }
    }
}

static void __AXDSPInitCallback(void *task) {
    __AXDSPInitFlag = TRUE;
}

static void __AXDSPResumeCallback(void *task) {
    u32 cycles;

    if (__AXOutDspReady == 2) {
        __AXOutDspReady = 0;

        cycles = __AXOutNewFrame();

        if (__AXExceedCallback != NULL) {
            __AXExceedCallback(cycles);
        }
    } else {
        __AXOutDspReady = 1;
    }
}

static void __AXDSPDoneCallback(void *task) {
    __AXDSPDoneFlag = TRUE;
    OSWakeupThread(&__AXOutThreadQueue);
}

static void __AXDSPRequestCallback(void *task) {
}

void __AXOutInitDSP(void) {
    __AXDSPTask.iram_mmem_addr = (u16 *)axDspSlave;
    __AXDSPTask.iram_length = axDspSlaveLength;
    __AXDSPTask.iram_addr = NULL;

    __AXDSPTask.dram_mem_addr = (u16 *)__AXDramImage;
    __AXDSPTask.dram_len = sizeof(__AXDramImage);
    __AXDSPTask.dram_addr = DRAM_IMAGE_DSP_ADDR;

    __AXDSPTask.dsp_vector = axDspInitVector;
    __AXDSPTask.dsp_res_vector = axDspResumeVector;

    __AXDSPTask.init_cb = __AXDSPInitCallback;
    __AXDSPTask.res_cb = __AXDSPResumeCallback;
    __AXDSPTask.done_cb = __AXDSPDoneCallback;
    __AXDSPTask.req_cb = __AXDSPRequestCallback;

    __AXDSPTask.priority = 0;

    __AXDSPInitFlag = FALSE;
    __AXDSPDoneFlag = FALSE;

    OSInitThreadQueue(&__AXOutThreadQueue);

    if (!DSPCheckInit()) {
        DSPInit();
    }

    DSPAddTask(&__AXDSPTask);
    while (!__AXDSPInitFlag) {
    }
}

void __AXOutInit(u32 mode) {
    int i;
    u32 *dst;
    u32 *dst2;
    void *rmt[AX_RMT_MAX];

    __AXOutFrame = 0;
    __AXAiDmaFrame = 0;
    __AXOutputBufferMode = mode;
    __AXDebugSteppingMode = 0;

    for (dst = (u32 *)__AXOutBuffer, i = 0; i < sizeof(__AXOutBuffer) / sizeof(u32); i++) {
        *dst++ = 0;
    }
    DCFlushRange(__AXOutBuffer, sizeof(__AXOutBuffer));

    for (dst2 = (u32 *)__AXOutSBuffer, i = 0; i < sizeof(__AXOutSBuffer) / sizeof(u32); i++) {
        *dst2++ = 0;
    }
    DCFlushRange(__AXOutSBuffer, sizeof(__AXOutSBuffer));

    for (dst2 = (u32 *)__AXRmtOutBuffer, i = 0; i < sizeof(__AXRmtOutBuffer) / sizeof(u32); i++) {
        *dst2++ = 0;
    }
    DCFlushRange(__AXRmtOutBuffer, sizeof(__AXRmtOutBuffer));

    __AXOutInitDSP();
    AIRegisterDMACallback(__AXOutAiCallback);

    rmt[0] = __AXRmtOutBuffer[0];
    rmt[1] = __AXRmtOutBuffer[1];
    rmt[2] = __AXRmtOutBuffer[2];
    rmt[3] = __AXRmtOutBuffer[3];

    __AXRmtCpuPtr = AX_SAMPLES_PER_FRAME_RMT;
    __AXRmtDspPtr = AX_SAMPLES_PER_FRAME_RMT;
    __AXRmtBuffLen = AX_SAMPLES_PER_FRAME_RMT * RMT_FRAME_MAX;

    if (__AXOutputBufferMode == 1) {
        __AXNextFrame(__AXOutSBuffer, __AXOutBuffer[2], rmt);
    } else {
        __AXNextFrame(__AXOutSBuffer, __AXOutBuffer[1], rmt);
    }

    __AXOutDspReady = 1;
    __AXUserFrameCallback = NULL;

    if (__AXOutputBufferMode == 1) {
        AIInitDMA((u32)__AXOutBuffer[__AXAiDmaFrame], AX_FRAME_SIZE);
        __AXAiDmaFrame++;
    } else {
        AIInitDMA((u32)__AXOutBuffer[__AXOutFrame], AX_FRAME_SIZE);
    }

    AIStartDMA();
    __AXExceedCallback = NULL;
}

// HYPOTHESIS: not in the open sdk sources i have, written from the retail code. waits for the dsp task to be
// cancelled and silences the ai dma so the audio engine can be shut down
void __AXOutQuit(void) {
    BOOL enabled = OSDisableInterrupts();

    __AXUserFrameCallback = NULL;
    DSPCancelTask(&__AXDSPTask);
    OSSleepThread(&__AXOutThreadQueue);
    AIStopDMA();
    __AXOutputBufferMode = 0;

    OSRestoreInterrupts(enabled);
}

AXOutCallback AXRegisterCallback(AXOutCallback callback) {
    AXOutCallback old = __AXUserFrameCallback;
    BOOL enabled = OSDisableInterrupts();

    __AXUserFrameCallback = callback;

    OSRestoreInterrupts(enabled);
    return old;
}

s32 AXRmtGetSamplesLeft(void) {
    s32 samples = __AXRmtDspPtr - (AX_SAMPLES_PER_FRAME_RMT * 2);

    if (samples < 0) {
        samples += __AXRmtBuffLen;
    }

    samples -= __AXRmtCpuPtr;

    if (samples < 0) {
        samples += __AXRmtBuffLen;
    }

    return samples;
}

s32 AXRmtGetSamples(s32 chan, s16 *out, s32 num) {
    s32 avail = AXRmtGetSamplesLeft();
    s32 actual = num > avail ? avail : num;
    s32 ptr = __AXRmtCpuPtr;
    int i;

    for (i = 0; i < actual; i++) {
        *out++ = __AXRmtOutBuffer[chan][ptr];

        if (++ptr >= __AXRmtBuffLen) {
            ptr = 0;
        }
    }

    DCInvalidateRange(__AXRmtOutBuffer[chan], __AXRmtBuffLen * sizeof(s16));
    return actual;
}

s32 AXRmtAdvancePtr(s32 num) {
    s32 avail = AXRmtGetSamplesLeft();
    s32 actual = num > avail ? avail : num;

    __AXRmtCpuPtr += actual;

    if (__AXRmtCpuPtr >= __AXRmtBuffLen) {
        __AXRmtCpuPtr -= __AXRmtBuffLen;
    }

    return actual;
}
