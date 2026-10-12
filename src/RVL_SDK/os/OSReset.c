// OSReset: the shutdown function list, shutting devices down and the different ways out of a game
// (restart, back to the system menu, hot reset). the wii flavour of this file, so most exits go through
// the state flags + a shutdown pass before handing over to the menu/boot code in OSExec.
#include "revolution/os.h"
#include "revolution/os/OSReset.h"
#include "revolution/os/OSStateFlags.h"
#include "revolution/os/OSRtc.h"
#include "revolution/os/OSExec.h"
#include "revolution/os/OSStateTM.h"
#include "revolution/os/OSPlayRecord.h"
#include "revolution/dvd.h"
#include "revolution/pad.h"
#include "revolution/sc.h"
#include <cstring>

typedef struct {
    OSShutdownFunctionInfo *head;
    OSShutdownFunctionInfo *tail;
} ShutdownQueue;

static ShutdownQueue ShutdownFunctionQueue;

// not named yet, they live in OSExec / OSPlayRecord / the reboot code
extern void fn_801D9C80(void);
extern void fn_801E25C0(void);
extern void fn_801E2750(void);
extern void __OSShutdownToSBY(void);
extern void __VISetRGBModeImm(void);
extern void __OSReboot(u32, u32);
extern u32 lbl_805A0634;
extern u32 lbl_805A06A4;
extern u32 lbl_805A06E0;
extern u32 lbl_805A06E4;

extern struct {
    u32 isValid;
    u32 code;
    u32 rest[6];
} lbl_804DD120;

#define OS_ACTIVE_THREAD_HEAD (*(OSThread **)0x800000DC)

void OSRegisterShutdownFunction(OSShutdownFunctionInfo *info) {
    OSShutdownFunctionInfo *it;
    OSShutdownFunctionInfo *prev;
    OSShutdownFunctionInfo *tail;

    for (it = ShutdownFunctionQueue.head; it != NULL && it->priority <= info->priority; it = it->next) {
        ;
    }

    if (it == NULL) {
        tail = ShutdownFunctionQueue.tail;
        if (tail == NULL) {
            ShutdownFunctionQueue.head = info;
        } else {
            tail->next = info;
        }

        info->prev = tail;
        info->next = NULL;
        ShutdownFunctionQueue.tail = info;
    } else {
        info->next = it;

        prev = it->prev;
        it->prev = info;
        info->prev = prev;

        if (prev == NULL) {
            ShutdownFunctionQueue.head = info;
        } else {
            prev->next = info;
        }
    }
}

BOOL __OSCallShutdownFunctions(BOOL pass, u32 event) {
    OSShutdownFunctionInfo *iter;
    BOOL failure;
    u32 prio;

    prio = 0;
    failure = FALSE;

    for (iter = ShutdownFunctionQueue.head; iter != NULL; iter = iter->next) {
        if (failure && prio != iter->priority) {
            break;
        }

        failure |= !iter->func(pass, event);
        prio = iter->priority;
    }

    failure |= !__OSSyncSram();

    return !failure;
}

static void KillThreads(void) {
    OSThread *iter;
    OSThread *next;

    for (iter = OS_ACTIVE_THREAD_HEAD; iter != NULL; iter = next) {
        next = iter->linkActive.next;

        switch (iter->state) {
        case OS_THREAD_STATE_WAITING:
        case OS_THREAD_STATE_READY:
            OSCancelThread(iter);
            break;
        }
    }
}

void __OSShutdownDevices(u32 event) {
    BOOL padIntr;
    BOOL osIntr;
    BOOL keepEnable;

    switch (event) {
    case 0:
    case 5:
    case 6:
        keepEnable = FALSE;
        break;
    default:
        keepEnable = TRUE;
        break;
    }

    __OSStopAudioSystem();

    if (!keepEnable) {
        padIntr = __PADDisableRecalibration(TRUE);
    }

    while (!__OSCallShutdownFunctions(FALSE, event)) {
        ;
    }

    while (!__OSSyncSram()) {
        ;
    }

    osIntr = OSDisableInterrupts();
    __OSCallShutdownFunctions(TRUE, event);
    LCDisable();

    if (!keepEnable) {
        __PADDisableRecalibration(padIntr);
    }

    KillThreads();
}

u8 __OSGetDiscState(u8 state) {
    u32 flags;
    u8 out;

    if (__DVDGetCoverStatus() != 2) {
        out = 3;
    } else if (state == 1) {
        if (!__OSGetRTCFlags(&flags) || flags == 0) {
            goto disc_in_1;
        }

    disc_in_2:
        out = 2;
    } else {
        goto disc_in_2;

    disc_in_1:
        out = 1;
    }

    return out;
}

void OSShutdownSystem(void) {
    SCIdleModeInfo idleMode;
    OSIOSRev iosRev;
    OSStateFlags stateFlags;

    memset(&idleMode, 0, sizeof(SCIdleModeInfo));
    SCInit();
    while (SCCheckStatus() == SC_STATUS_BUSY) {
        ;
    }
    SCGetIdleMode(&idleMode);

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();
    __OSReadStateFlags(&stateFlags);

    stateFlags.lastDiscState = __OSGetDiscState(stateFlags.lastDiscState);
    if (idleMode.mode == 1) {
        stateFlags.lastShutdown = 5;
    } else {
        stateFlags.lastShutdown = 1;
    }

    __OSClearRTCFlags();
    __OSWriteStateFlags(&stateFlags);
    __OSGetIOSRev(&iosRev);

    if (idleMode.mode == 1) {
        lbl_805A06E4 = 1;
        OSDisableScheduler();
        __OSShutdownDevices(5);
        OSEnableScheduler();
        __OSLaunchMenu();
    } else {
        OSDisableScheduler();
        __OSShutdownDevices(2);
        __OSShutdownToSBY();
    }
}

static inline void __OSHotReset(void) {
    if (lbl_805A0634 != 0 || lbl_805A06A4 != 0) {
        fn_801E25C0();
    }

    fn_801E2750();

    OSPanic("OSReset.c", 973, "__OSHotReset(): Falied to reset system.\n");
}

void OSRestart(u32 resetCode) {
    u8 appType;

    appType = OSGetAppType();
    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();

    if (appType == 0x81) {
        OSDisableScheduler();
        __OSShutdownDevices(4);
        OSEnableScheduler();
        fn_801D9C80();
    } else if (appType == 0x80) {
        OSDisableScheduler();
        __OSShutdownDevices(4);
        OSEnableScheduler();
        __OSReboot(resetCode, lbl_805A06E0);
    }

    OSDisableScheduler();
    __OSShutdownDevices(1);
    __OSHotReset();
}

// HYPOTHESIS: shared tail of the return-to-menu variants, the arg lands in the last byte of the state flags
static void ReturnToMenuCommon(u8 menuMode) {
    OSStateFlags stateFlags;

    __OSStopPlayRecord();
    __OSUnRegisterStateEvent();
    __DVDPrepareReset();

    __OSReadStateFlags(&stateFlags);
    stateFlags.lastDiscState = __OSGetDiscState(stateFlags.lastDiscState);
    stateFlags.lastShutdown = 3;
    stateFlags.menuMode = menuMode;
    __OSClearRTCFlags();
    __OSWriteStateFlags(&stateFlags);

    OSDisableScheduler();
    __OSShutdownDevices(5);
    OSEnableScheduler();

    __OSLaunchMenu();
    OSDisableScheduler();
    __VISetRGBModeImm();
    __OSHotReset();
}

void OSReturnToMenu(void) {
    ReturnToMenuCommon(0);
    OSPanic("OSReset.c", 834, "OSReturnToMenu(): Falied to boot system menu.\n");
}

void OSReturnToDataManager(void) {
    ReturnToMenuCommon(1);
    OSPanic("OSReset.c", 856, "OSReturnToDataManager(): Falied to boot system menu.\n");
}

void OSReturnToSetting(u32 setting) {
    const char *url;

    switch (setting) {
    case OS_SETTING_CALENDAR:
        url = OS_SYSLAUNCH_URL_CALENDAR;
        break;
    case OS_SETTING_DISPLAY:
        url = OS_SYSLAUNCH_URL_DISPLAY;
        break;
    case OS_SETTING_SOUND:
        url = OS_SYSLAUNCH_URL_SOUND;
        break;
    case OS_SETTING_PARENTAL:
        url = OS_SYSLAUNCH_URL_PARENTAL;
        break;
    case OS_SETTING_INTERNET:
        url = OS_SYSLAUNCH_URL_INTERNET;
        break;
    case OS_SETTING_WC24:
        url = OS_SYSLAUNCH_URL_WC24;
        break;
    case OS_SETTING_UPDATE:
        url = OS_SYSLAUNCH_URL_UPDATE;
        break;
    default:
        OSReport("OSReturnToSetting(): You can't specify %d.  \n", setting);
        OSPanic("OSReset.c", 910, "");
    }

    __OSReturnToMenul(1, url, 0);
}

void __OSReturnToMenuForError(void) {
    OSStateFlags stateFlags;

    __OSReadStateFlags(&stateFlags);
    stateFlags.lastDiscState = 2;
    stateFlags.lastShutdown = 3;
    __OSClearRTCFlags();
    __OSWriteStateFlags(&stateFlags);

    __OSLaunchMenu();
    OSDisableScheduler();
    __VISetRGBModeImm();
    __OSHotReset();

    OSPanic("OSReset.c", 949, "__OSReturnToMenu(): Falied to boot system menu.\n");
}

void __OSHotResetForError(void) {
    __OSHotReset();
}

u32 OSGetResetCode(void) {
    if (lbl_804DD120.isValid != 0) {
        return lbl_804DD120.code | 0x80000000;
    }

    return *(volatile u32 *)0xCC003024 >> 3;
}

void OSResetSystem(int a, u32 b, BOOL c) {
#pragma unused(a, b, c)
    OSPanic("OSReset.c", 1124, "OSResetSystem() is obsoleted. It doesn't work any longer.\n");
}

void OSSetBootDol(void) {
    OSPanic("OSReset.c", 1137, "OSSetBootDol() is obsoleted. It doesn't work any longer.\n");
}
