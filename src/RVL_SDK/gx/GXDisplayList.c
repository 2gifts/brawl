// gx display lists: redirect the cpu fifo into a caller buffer, then hand the buffer back.
#include "revolution/gx.h"
#include "revolution/gx/GXRegs.h"
#include "revolution/os.h"
extern void* memcpy(void*, const void*, unsigned long);

// helpers that live in the other gx units (names guessed from the standard gx api)
extern void GXResetWriteGatherPipe(void);
extern void __GXSetDirtyState(void);
extern void __GXSendFlushPrim(void);
extern u32 GXGetFifoCount(const GXFifoObj* fifo);

static GXFifoObj OldCPUFifo;
static GXData __savedGXdata;
static GXFifoObj DisplayListFifo;

void GXBeginDisplayList(void* list, u32 size) {
    __GXFifoObj* impl = (__GXFifoObj*)&DisplayListFifo;

    if (gx->dirtyState != 0) {
        __GXSetDirtyState();
    }

    if (gx->dlSaveContext) {
        memcpy(&__savedGXdata, gx, sizeof(GXData));
    }

    impl->base = list;
    impl->top = (u8*)list + size - 4;
    impl->size = size;
    impl->count = 0;
    impl->rdPtr = list;
    impl->wrPtr = list;

    gx->inDispList = GX_TRUE;

    GXGetCPUFifo(&OldCPUFifo);
    GXSetCPUFifo(&DisplayListFifo);
    GXResetWriteGatherPipe();
}

u32 GXEndDisplayList(void) {
    u8 wrap;
    BOOL enabled;
    u32 cpEnable;

    GXGetCPUFifo(&DisplayListFifo);
    wrap = GXGetFifoWrap(&DisplayListFifo);
    GXSetCPUFifo(&OldCPUFifo);

    if (gx->dlSaveContext) {
        enabled = OSDisableInterrupts();

        cpEnable = gx->cpEnable;
        memcpy(gx, &__savedGXdata, sizeof(GXData));
        gx->cpEnable = cpEnable;

        OSRestoreInterrupts(enabled);
    }

    gx->inDispList = GX_FALSE;

    if (!wrap) {
        return GXGetFifoCount(&DisplayListFifo);
    }

    return 0;
}

void GXCallDisplayList(const void* list, u32 size) {
    if (gx->dirtyState != 0) {
        __GXSetDirtyState();
    }

    if (*(u32*)gx == 0) {
        __GXSendFlushPrim();
    }

    GX_WRITE_U8(0x40);
    GX_WRITE_U32(list);
    GX_WRITE_U32(size);
}
