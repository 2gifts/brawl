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

// TODO: clean up, give variables real names
/* jj_803CD830 */
extern void* RFLExit(void);

void* errclosecallback_(void) {
    return RFLExit();
}

/* jj_803CD9C4 */
extern int RFLGetAsyncStatus(void);
extern void *RFLiGetLoader(void);
extern int fn_803CFF50(int a, void *b, void *c);
extern void fn_803CFBE4(int a, void *b);
extern void loadResGetlengthcallback_(void);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void loadResOpencallback_(void) {
    int r;
    char *p;

    if (RFLGetAsyncStatus() == 0) {
        p = (char *)RFLiGetLoader();
        *(int *)(p + 0x90) = 0;
        r = fn_803CFF50(1, p + 0x90, (void *)loadResGetlengthcallback_);
        if ((r != 6) && ((r >= 6) || (r != 0))) {
            fn_803CFBE4(1, 0);
        }
    } else {
        RFLExit();
    }
}

/* jj_803CD920 */

extern void *RFLiGetLoader(void);
extern s32 RFLiAlloc32(s32 size);
extern void RFLiFree(s32 p);
extern s32 fn_803CF870(s32 a, s32 b, s32 c, void *d, s32 e);
extern void loadResRead1stcallback_(void);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void loadResGetlengthcallback_(void) {
    s32 temp_r3;
    s32 temp_r3_2;
    char *temp_r31;

    temp_r31 = (char *)RFLiGetLoader();
    if (RFLGetAsyncStatus() == 0) {
        temp_r3 = RFLiAlloc32(0x100);
        *(s32 *)(temp_r31 + 0xA0) = temp_r3;
        temp_r3_2 = fn_803CF870(1, temp_r3, 0x100, (void *)loadResRead1stcallback_, 0);
        if (temp_r3_2 != 6 && (temp_r3_2 >= 6 || temp_r3_2 != 0)) {
            RFLiFree(*(s32 *)(temp_r31 + 0xA0));
            *(s32 *)(temp_r31 + 0xA0) = 0;
            fn_803CFBE4(1, 0);
        }
    } else {
        fn_803CFBE4(1, (void *)errclosecallback_);
    }
}

/* jj_803CD484 */
extern void *RFLiGetLoader(void);

// TODO: clean up. This zeroes the RFLLoader state through raw byte offsets from RFLiGetLoader()
// (u16/u32 stores at +0x00..). Replace with a real RFLLoader struct and named fields once the
// layout is known, then re-check that the codegen still matches.
void RFLiInitLoader(void)
{
    char *p = (char *)RFLiGetLoader();

    *(unsigned short *)(p + 0x00) = 0;
    *(unsigned short *)(p + 0x02) = 0;
    *(unsigned int *)(p + 0x04) = 0;
    *(unsigned short *)(p + 0x08) = 0;
    *(unsigned short *)(p + 0x0A) = 0;
    *(unsigned int *)(p + 0x0C) = 0;
    *(unsigned short *)(p + 0x10) = 0;
    *(unsigned short *)(p + 0x12) = 0;
    *(unsigned int *)(p + 0x14) = 0;
    *(unsigned short *)(p + 0x18) = 0;
    *(unsigned short *)(p + 0x1A) = 0;
    *(unsigned int *)(p + 0x1C) = 0;
    *(unsigned short *)(p + 0x20) = 0;
    *(unsigned short *)(p + 0x22) = 0;
    *(unsigned int *)(p + 0x24) = 0;
    *(unsigned short *)(p + 0x28) = 0;
    *(unsigned short *)(p + 0x2A) = 0;
    *(unsigned int *)(p + 0x2C) = 0;
    *(unsigned short *)(p + 0x30) = 0;
    *(unsigned short *)(p + 0x32) = 0;
    *(unsigned int *)(p + 0x34) = 0;
    *(unsigned short *)(p + 0x38) = 0;
    *(unsigned short *)(p + 0x3A) = 0;
    *(unsigned int *)(p + 0x3C) = 0;
    *(unsigned short *)(p + 0x40) = 0;
    *(unsigned short *)(p + 0x42) = 0;
    *(unsigned int *)(p + 0x44) = 0;
    *(unsigned short *)(p + 0x48) = 0;
    *(unsigned short *)(p + 0x4A) = 0;
    *(unsigned int *)(p + 0x4C) = 0;
    *(unsigned short *)(p + 0x50) = 0;
    *(unsigned short *)(p + 0x52) = 0;
    *(unsigned int *)(p + 0x54) = 0;
    *(unsigned short *)(p + 0x58) = 0;
    *(unsigned short *)(p + 0x5A) = 0;
    *(unsigned int *)(p + 0x5C) = 0;
    *(unsigned short *)(p + 0x60) = 0;
    *(unsigned short *)(p + 0x62) = 0;
    *(unsigned int *)(p + 0x64) = 0;
    *(unsigned short *)(p + 0x68) = 0;
    *(unsigned short *)(p + 0x6A) = 0;
    *(unsigned int *)(p + 0x6C) = 0;
    *(unsigned short *)(p + 0x70) = 0;
    *(unsigned short *)(p + 0x72) = 0;
    *(unsigned int *)(p + 0x74) = 0;
    *(unsigned short *)(p + 0x78) = 0;
    *(unsigned short *)(p + 0x7A) = 0;
    *(unsigned int *)(p + 0x7C) = 0;
    *(unsigned short *)(p + 0x80) = 0;
    *(unsigned short *)(p + 0x82) = 0;
    *(unsigned int *)(p + 0x84) = 0;
    *(unsigned short *)(p + 0x88) = 0;
    *(unsigned short *)(p + 0x8A) = 0;
    *(unsigned int *)(p + 0x8C) = 0;
    *(unsigned int *)(p + 0x90) = 0;
    *(unsigned int *)(p + 0x94) = 0;
    *(unsigned int *)(p + 0x98) = 0;
    *(unsigned short *)(p + 0x9C) = 0;
    *(unsigned int *)(p + 0xA0) = 0;
    *(unsigned int *)(p + 0xA4) = 0;
}

