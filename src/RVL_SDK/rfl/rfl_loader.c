#include <types.h>

extern BOOL RFLiGetWorking(void);
extern void RFLiSetWorking(BOOL working);
extern void* RFLiGetManager(void);
extern void* RFLiGetAccInfo(u16 index);
extern BOOL RFLAvailable(void);
extern void* RFLiGetLoader(void);
extern int OSDisableInterrupts(void);
extern void OSRestoreInterrupts(int level);
extern void OSCancelAlarm(void* alarm);

BOOL RFLIsResourceCached(void) {
    u8* loader;
    if (!RFLAvailable()) {
        return FALSE;
    }
    loader = RFLiGetLoader();
    if (loader == NULL) {
        return FALSE;
    }
    return *(u32*)(loader + 0x98);
}

void RFLiExitAccessInfo(void) {
    u32 i;
    for (i = 0; i < 2; i++) {
        u8* info = RFLiGetAccInfo(i);
        OSCancelAlarm(info + 0x198);
    }
}

BOOL RFLiIsWorking(void) {
    return RFLiGetWorking();
}

void RFLiStartWorking(void) {
    int level = OSDisableInterrupts();
    int state;
    RFLiSetWorking(TRUE);
    state = 6;
    *(int*)((u8*)RFLiGetManager() + 0x1B40) = state;
    OSRestoreInterrupts(level);
}

extern void* MEMAllocFromExpHeapEx(void* heap, u32 size, int align);
extern void OSCreateAlarm(void* alarm);
extern void* memset(void*, int, unsigned long);

BOOL RFLFreeCachedResource(void) {
    u32* loader;
    if (!RFLAvailable()) {
        return TRUE;
    }
    if (RFLiIsWorking()) {
        return TRUE;
    }
    loader = RFLiGetLoader();
    if (loader == NULL) {
        return TRUE;
    }
    loader[0x98 / 4] = 0;
    loader[0x94 / 4] = 0;
    loader[0x90 / 4] = 0;
    return FALSE;
}

void RFLiInitAccessInfo(void* heap) {
    u32 i;
    for (i = 0; i < 2; i++) {
        u8* info = RFLiGetAccInfo(i);
        memset(info, 0, 0x1E0);
        *(void**)(info + 0x1D4) = MEMAllocFromExpHeapEx(heap, 0x2000, 0x20);
        OSCreateAlarm(info + 0x198);
    }
}

void RFLiEndWorking(int state) {
    int level;
    int cur = *(int*)((u8*)RFLiGetManager() + 0x1B40);
    switch (cur) {
    case 0:
    case 6:
        level = OSDisableInterrupts();
        RFLiSetWorking(FALSE);
        *(int*)((u8*)RFLiGetManager() + 0x1B40) = state;
        {
            int zero = 0;
            *(int*)((u8*)RFLiGetManager() + 0x1B48) = zero;
        }
        OSRestoreInterrupts(level);
        break;
    }
}
