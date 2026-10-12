// gx fifo management: the cpu-side and gp-side fifo copies, linking them when both point at the same ring, and the
// cp interrupt (overflow/underflow/breakpoint) handling. everything goes through the shadow copies in CPUFifo/GPFifo.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"
#include "revolution/gd/GDGeometry.h"
#include "revolution/os.h"
#include "revolution/base/PPCArch.h"

#define SET_REG_FIELD(reg, size, shift, val) FAST_FLAG_SET(reg, val, shift, size)

extern volatile void* __piReg;
extern void* memset(void*, int, unsigned long);

// declared backwards on purpose: the c compiler lays statics out in reverse order
static GXBool CPGPLinked;
static OSThread* __GXCurrentThread;
static BOOL GXOverflowSuspendInProgress;
static GXBreakPtCallback BreakPointCB;
static u32 sbss_805A08A0;
static u32 __GXOverflowCount;
static GXBool GPFifoReady;
static GXBool CPUFifoReady;

static __GXFifoObj CPUFifo;
static __GXFifoObj GPFifo;

static void GXCPInterruptHandler(__OSInterrupt interrupt, OSContext* context) {
    OSContext exceptContext;

    gx->cpStatus = GX_CP_REG_READ_U16(0);

    if (((gx->cpEnable & 8) >> 3) && ((gx->cpStatus & 2) >> 1)) {
        OSResumeThread(__GXCurrentThread);
        GXOverflowSuspendInProgress = GX_FALSE;
        SC_CP_REG_CLR_SET_OVFLINT(gx->cpClr, 1);
        SC_CP_REG_CLR_SET_UNFLINT(gx->cpClr, 1);
        GX_CP_REG_WRITE_U16(2, gx->cpClr);
        SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 1);
        SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
    }

    if (((gx->cpEnable & 4) >> 2) && (gx->cpStatus & 1)) {
        __GXOverflowCount++;
        SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 0);
        SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 1);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
        SC_CP_REG_CLR_SET_OVFLINT(gx->cpClr, 1);
        SC_CP_REG_CLR_SET_UNFLINT(gx->cpClr, 0);
        GX_CP_REG_WRITE_U16(2, gx->cpClr);
        GXOverflowSuspendInProgress = GX_TRUE;
        OSSuspendThread(__GXCurrentThread);
    }

    if (((gx->cpEnable & 0x20) >> 5) && ((gx->cpStatus & 0x10) >> 4)) {
        SC_CP_REG_ENABLE_SET_FIFOBRKINT(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);

        if (BreakPointCB) {
            OSClearContext(&exceptContext);
            OSSetCurrentContext(&exceptContext);
            BreakPointCB();
            OSClearContext(&exceptContext);
            OSSetCurrentContext(context);
        }
    }
}

void GXInitFifoBase(GXFifoObj* fifo, void* base, u32 size) {
    __GXFifoObj* realFifo = (__GXFifoObj*)fifo;
    BOOL interrupts;

    realFifo->base = base;
    realFifo->top = (u8*)base + size - 4;
    realFifo->size = size;
    realFifo->count = 0;

    // limits: high at size-0x4000, low at half the size rounded to 32 bytes
    realFifo->hiWatermark = size - 0x4000;
    realFifo->loWatermark = (size >> 1) & ~0x1F;

    // ptrs start out empty
    interrupts = OSDisableInterrupts();
    realFifo->rdPtr = base;
    realFifo->wrPtr = base;
    realFifo->count = (u8*)base - (u8*)base;
    if (realFifo->count < 0) {
        realFifo->count += realFifo->size;
    }
    OSRestoreInterrupts(interrupts);
}

static GXBool CPGPLinkCheck(void) {
    u32 cnt = 0;
    s32 d1;
    BOOL bad;
    s32 d2;

    if (!CPUFifoReady || !GPFifoReady) {
        return FALSE;
    }

    if (CPUFifo.base == GPFifo.base) {
        cnt++;
    }
    if (CPUFifo.top == GPFifo.top) {
        cnt++;
    }

    if (cnt == 2) {
        return TRUE;
    }

    d1 = (s32)CPUFifo.top - (s32)GPFifo.base;
    d2 = (s32)GPFifo.top - (s32)CPUFifo.base;
    bad = FALSE;
    if (d1 > 0 && d2 > 0 || d1 < 0 && d2 < 0) {
        bad = TRUE;
    }

    if (bad) {
        OSReport("CPUFifo: %08X - %08X\n", CPUFifo.base, CPUFifo.top);
        OSReport("GP Fifo: %08X - %08X\n", GPFifo.base, GPFifo.top);
    }

    return FALSE;
}

void GXSetCPUFifo(const GXFifoObj* fifo) {
    __GXFifoObj* realFifo;
    BOOL enabled;
    u32 reg;

    realFifo = (__GXFifoObj*)fifo;
    enabled = OSDisableInterrupts();

    if (!fifo) {
        CPUFifoReady = GX_FALSE;
        CPGPLinked = GX_FALSE;
        CPUFifo.bind_gp = GX_FALSE;
        CPUFifo.bind_cpu = GX_FALSE;
        OSRestoreInterrupts(enabled);
        return;
    }

    CPUFifo = *realFifo;
    CPUFifoReady = GX_TRUE;
    CPUFifo.bind_cpu = GX_TRUE;

    if (CPGPLinkCheck()) {
        reg = 0;
        CPGPLinked = GX_TRUE;
        CPUFifo.bind_gp = GX_TRUE;
        GX_PI_REG_WRITE_U32(0xC, (u32)CPUFifo.base & 0x3FFFFFFF);
        GX_PI_REG_WRITE_U32(0x10, (u32)CPUFifo.top & 0x3FFFFFFF);
        reg = __rlwimi(reg, (u32)CPUFifo.wrPtr, 0, 3, 26);
        GX_PI_REG_WRITE_U32(0x14, reg);

        SC_CP_REG_CLR_SET_OVFLINT(gx->cpClr, 1);
        SC_CP_REG_CLR_SET_UNFLINT(gx->cpClr, 1);
        GX_CP_REG_WRITE_U16(2, gx->cpClr);
        SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 1);
        SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
        SC_CP_REG_ENABLE_SET_WRPTRINC(gx->cpEnable, 1);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
    } else {
        reg = 0;
        CPUFifo.bind_gp = GX_FALSE;

        if (CPGPLinked) {
            SC_CP_REG_ENABLE_SET_WRPTRINC(gx->cpEnable, 0);
            GX_CP_REG_WRITE_U16(1, gx->cpEnable);
            CPGPLinked = GX_FALSE;
        }

        SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 0);
        SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);

        GX_PI_REG_WRITE_U32(0xC, (u32)CPUFifo.base & 0x3FFFFFFF);
        GX_PI_REG_WRITE_U32(0x10, (u32)CPUFifo.top & 0x3FFFFFFF);
        reg = __rlwimi(reg, (u32)CPUFifo.wrPtr, 0, 3, 26);
        GX_PI_REG_WRITE_U32(0x14, reg);
    }

    PPCSync();
    OSRestoreInterrupts(enabled);
}

void GXSetGPFifo(const GXFifoObj* fifo) {
    __GXFifoObj* realFifo = (__GXFifoObj*)fifo;
    BOOL enabled;
    u32 reg;

    enabled = OSDisableInterrupts();

    SC_CP_REG_ENABLE_SET_FIFORD(gx->cpEnable, 0);
    GX_CP_REG_WRITE_U16(1, gx->cpEnable);
    SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 0);
    SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 0);
    GX_CP_REG_WRITE_U16(1, gx->cpEnable);

    if (!fifo) {
        GPFifoReady = GX_FALSE;
        CPGPLinked = GX_FALSE;
        SC_CP_REG_ENABLE_SET_WRPTRINC(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
        GPFifo.bind_cpu = GX_FALSE;
        GPFifo.bind_gp = GX_FALSE;
        OSRestoreInterrupts(enabled);
        return;
    }

    GPFifo = *realFifo;
    GPFifoReady = GX_TRUE;
    GPFifo.bind_gp = GX_TRUE;

    GX_CP_REG_WRITE_U16(0x10, (u32)GPFifo.base);
    GX_CP_REG_WRITE_U16(0x12, (u32)GPFifo.top);
    GX_CP_REG_WRITE_U16(0x18, GPFifo.count);
    GX_CP_REG_WRITE_U16(0x1A, (u32)GPFifo.wrPtr);
    GX_CP_REG_WRITE_U16(0x1C, (u32)GPFifo.rdPtr);
    GX_CP_REG_WRITE_U16(0x14, GPFifo.hiWatermark);
    GX_CP_REG_WRITE_U16(0x16, GPFifo.loWatermark);
    GX_CP_REG_WRITE_U16(0x11, ((u32)GPFifo.base >> 16) & 0x3FFF);
    GX_CP_REG_WRITE_U16(0x13, ((u32)GPFifo.top >> 16) & 0x3FFF);
    GX_CP_REG_WRITE_U16(0x19, GPFifo.count >> 16);
    GX_CP_REG_WRITE_U16(0x1B, ((u32)GPFifo.wrPtr >> 16) & 0x3FFF);
    GX_CP_REG_WRITE_U16(0x1D, ((u32)GPFifo.rdPtr >> 16) & 0x3FFF);
    GX_CP_REG_WRITE_U16(0x15, GPFifo.hiWatermark >> 16);
    GX_CP_REG_WRITE_U16(0x17, GPFifo.loWatermark >> 16);

    PPCSync();

    if (CPGPLinkCheck()) {
        CPGPLinked = GX_TRUE;
        GPFifo.bind_cpu = GX_TRUE;
        SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 1);
        SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
        SC_CP_REG_ENABLE_SET_WRPTRINC(gx->cpEnable, 1);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
    } else {
        CPGPLinked = GX_FALSE;
        GPFifo.bind_cpu = GX_FALSE;
        SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 0);
        SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
        SC_CP_REG_ENABLE_SET_WRPTRINC(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
    }

    reg = gx->cpEnable;
    SC_CP_REG_ENABLE_SET_FIFOBRK(reg, 0);
    SC_CP_REG_ENABLE_SET_FIFOBRKINT(reg, 0);
    GX_CP_REG_WRITE_U16(1, reg);
    GX_CP_REG_WRITE_U16(1, gx->cpEnable);

    SC_CP_REG_CLR_SET_OVFLINT(gx->cpClr, 1);
    SC_CP_REG_CLR_SET_UNFLINT(gx->cpClr, 1);
    GX_CP_REG_WRITE_U16(2, gx->cpClr);

    SC_CP_REG_ENABLE_SET_FIFORD(gx->cpEnable, 1);
    GX_CP_REG_WRITE_U16(1, gx->cpEnable);

    OSRestoreInterrupts(enabled);
}

// pulls the live wr/rd/count state out of the pi and cp regs back into the shadow fifos
void fn_801EEE74(void) {
    BOOL enabled;
    u32 reg;
    u32 hi;
    u32 lo;

    enabled = OSDisableInterrupts();

    if (CPUFifoReady) {
        reg = GX_PI_REG_READ_U32(0x14);
        CPUFifo.wrap = (reg >> 29) & 1;
        CPUFifo.wrPtr = (void*)((reg & 0x1FFFFFE0) + 0x80000000);
    }

    if (GPFifoReady) {
        hi = GX_CP_REG_READ_U16(0x1D);
        lo = GX_CP_REG_READ_U16(0x1C);
        GPFifo.rdPtr = (void*)(((hi << 16) | lo) + 0x80000000);
        hi = GX_CP_REG_READ_U16(0x19);
        lo = GX_CP_REG_READ_U16(0x18);
        GPFifo.count = (hi << 16) | lo;
    }

    if (CPGPLinked) {
        CPUFifo.rdPtr = GPFifo.rdPtr;
        CPUFifo.count = GPFifo.count;
        GPFifo.wrPtr = CPUFifo.wrPtr;
        GPFifo.wrap = CPUFifo.wrap;
    } else if (CPUFifoReady) {
        CPUFifo.count = (u8*)CPUFifo.wrPtr - (u8*)CPUFifo.rdPtr;
        if (CPUFifo.count < 0) {
            CPUFifo.count += CPUFifo.size;
        }
    }

    OSRestoreInterrupts(enabled);
}

GXBool __GXIsGPFifoReady(void) {
    return GPFifoReady;
}

GXBool GXGetCPUFifo(GXFifoObj* fifo) {
    __GXFifoObj* realFifo = (__GXFifoObj*)fifo;

    if (!CPUFifoReady) {
        return GX_FALSE;
    }

    GXFlush();
    fn_801EEE74();
    *realFifo = CPUFifo;
    return GX_TRUE;
}

u32 GXGetFifoCount(const GXFifoObj* fifo) {
    return ((__GXFifoObj*)fifo)->count;
}

GXBool GXGetFifoWrap(const GXFifoObj* fifo) {
    return ((__GXFifoObj*)fifo)->wrap;
}

GXBreakPtCallback GXSetBreakPtCallback(GXBreakPtCallback cb) {
    GXBreakPtCallback old = BreakPointCB;
    BOOL enabled = OSDisableInterrupts();
    BreakPointCB = cb;
    OSRestoreInterrupts(enabled);
    return old;
}

void __GXFifoInit(void) {
    __OSSetInterruptHandler(17, GXCPInterruptHandler);
    __OSUnmaskInterrupts(0x4000);
    __GXCurrentThread = OSGetCurrentThread();
    GXOverflowSuspendInProgress = GX_FALSE;
    memset(&CPUFifo, 0, sizeof(__GXFifoObj));
    memset(&GPFifo, 0, sizeof(__GXFifoObj));
    CPUFifoReady = GX_FALSE;
    GPFifoReady = GX_FALSE;
}

void __GXCleanGPFifo(void) {
    BOOL enabled;
    u32 reg;

    if (!GPFifoReady) {
        return;
    }

    enabled = OSDisableInterrupts();

    SC_CP_REG_ENABLE_SET_FIFORD(gx->cpEnable, 0);
    GX_CP_REG_WRITE_U16(1, gx->cpEnable);
    SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 0);
    SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 0);
    GX_CP_REG_WRITE_U16(1, gx->cpEnable);

    GPFifo.rdPtr = GPFifo.wrPtr;
    GPFifo.count = 0;

    GX_CP_REG_WRITE_U16(0x18, GPFifo.count);
    GX_CP_REG_WRITE_U16(0x1A, (u32)GPFifo.wrPtr);
    GX_CP_REG_WRITE_U16(0x1C, (u32)GPFifo.rdPtr);
    GX_CP_REG_WRITE_U16(0x19, GPFifo.count >> 16);
    GX_CP_REG_WRITE_U16(0x1B, ((u32)GPFifo.wrPtr >> 16) & 0x3FFF);
    GX_CP_REG_WRITE_U16(0x1D, ((u32)GPFifo.rdPtr >> 16) & 0x3FFF);

    PPCSync();

    if (CPGPLinked) {
        reg = 0;
        CPUFifo.rdPtr = GPFifo.rdPtr;
        CPUFifo.wrPtr = GPFifo.wrPtr;
        CPUFifo.count = GPFifo.count;
        reg = __rlwimi(reg, (u32)GPFifo.wrPtr, 0, 3, 26);
        GX_PI_REG_WRITE_U32(0x14, reg);
        SC_CP_REG_ENABLE_SET_OVFLINT(gx->cpEnable, 1);
        SC_CP_REG_ENABLE_SET_UNFLINT(gx->cpEnable, 0);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
        SC_CP_REG_ENABLE_SET_WRPTRINC(gx->cpEnable, 1);
        GX_CP_REG_WRITE_U16(1, gx->cpEnable);
    }

    SC_CP_REG_ENABLE_SET_FIFOBRK(gx->cpEnable, 0);
    SC_CP_REG_ENABLE_SET_FIFOBRKINT(gx->cpEnable, 0);
    GX_CP_REG_WRITE_U16(1, gx->cpEnable);

    sbss_805A08A0 = 0;
    SC_CP_REG_CLR_SET_OVFLINT(gx->cpClr, 1);
    SC_CP_REG_CLR_SET_UNFLINT(gx->cpClr, 1);
    GX_CP_REG_WRITE_U16(2, gx->cpClr);

    SC_CP_REG_ENABLE_SET_FIFORD(gx->cpEnable, 1);
    GX_CP_REG_WRITE_U16(1, gx->cpEnable);

    OSRestoreInterrupts(enabled);
}
