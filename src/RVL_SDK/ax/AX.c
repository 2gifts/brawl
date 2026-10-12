// AX: audio engine front door. brings the sub modules (voice allocator, voice/param blocks, aux buffers,
// command list, output) up in order and tears them down in the opposite one.
#include "revolution/ax.h"
#include "revolution/os.h"

extern void __AXAllocInit(void);
extern void __AXAllocQuit(void);
extern void __AXVPBInit(u32 num, void* mem);
extern void __AXVPBQuit(void);
extern void __AXSPBInit(void);
extern void __AXSPBQuit(void);
extern void __AXAuxInit(void);
extern void __AXAuxQuit(void);
extern void __AXClInit(void);
extern void __AXClQuit(void);
extern void __AXOutInit(u32 mode);
extern void __AXOutQuit(void);

const char* __AXVersion = "<< RVL_SDK - AX \trelease build: May  8 2007 12:54:39 (0x4199_60831) >>";

static BOOL __init = FALSE;

// HYPOTHESIS: two argument init, the pair goes straight to the voice block setup (count + user memory)
void AXInitSpecifyMem(u32 num, void* mem) {
    if (!__init) {
        OSRegisterVersion(__AXVersion);

        __AXAllocInit();
        __AXVPBInit(num, mem);
        __AXSPBInit();
        __AXAuxInit();
        __AXClInit();
        __AXOutInit(0);

        __init = TRUE;
    }
}

void AXQuit(void) {
    if (__init) {
        __AXOutQuit();
        __AXAllocQuit();
        __AXVPBQuit();
        __AXSPBQuit();
        __AXAuxQuit();
        __AXClQuit();

        __init = FALSE;
    }
}

BOOL AXIsInit(void) {
    return __init;
}
