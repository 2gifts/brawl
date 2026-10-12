// Pad: the gamecube controller driver on top of SIBios. a channel goes through a state machine: probe the
// device type, read its origin, then switch the hardware polling on. wavebird receivers get an extra id fix
// step. PADRead just converts the latest polled words into a PADStatus.
#include <revolution/OS.h>
#include <revolution/PAD.h>
#include <string.h>

// built as c++ against the openrvl headers, keep c linkage
extern "C" {

#pragma auto_inline off

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef long long OSTime;
static inline int abs(int x) { return x < 0 ? -x : x; }

#define PAD_CHAN0_BIT 0x80000000
#define PAD_MAX_CONTROLLERS 4

#define PAD_ERR_NONE 0
#define PAD_ERR_NO_CONTROLLER (-1)
#define PAD_ERR_NOT_READY (-2)
#define PAD_ERR_TRANSFER (-3)

#define PAD_MOTOR_STOP 0
#define PAD_MOTOR_STOP_HARD 2

#define PAD_TRIGGER_R 0x0020
#define PAD_TRIGGER_L 0x0040
#define PAD_BUTTON_A 0x0100
#define PAD_BUTTON_B 0x0200
#define PAD_BUTTON_X 0x0400
#define PAD_BUTTON_Y 0x0800
#define PAD_BUTTON_START 0x1000

#define PAD_ALL 0x1F7F

#define SI_ERROR_UNDER_RUN 0x0001
#define SI_ERROR_OVER_RUN 0x0002
#define SI_ERROR_COLLISION 0x0004
#define SI_ERROR_NO_RESPONSE 0x0008
#define SI_ERROR_ALL (SI_ERROR_UNDER_RUN | SI_ERROR_OVER_RUN | SI_ERROR_COLLISION | SI_ERROR_NO_RESPONSE)

#define SI_TYPE_MASK 0x18000000
#define SI_TYPE_GC 0x08000000
#define SI_GC_STANDARD 0x01000000
#define SI_GC_NOMOTOR 0x20000000
#define SI_GC_WIRELESS 0x80000000
#define SI_WIRELESS_RECEIVED 0x40000000
#define SI_WIRELESS_IR 0x04000000
#define SI_WIRELESS_STATE 0x02000000
#define SI_WIRELESS_FIX_ID 0x00100000
#define SI_WIRELESS_LITE 0x00040000
#define SI_WIRELESS_CONT 0x00080000
#define SI_GC_CONTROLLER 0x09000000

typedef struct PADStatus {
    u16 button;
    s8 stickX;
    s8 stickY;
    s8 substickX;
    s8 substickY;
    u8 triggerLeft;
    u8 triggerRight;
    u8 analogA;
    u8 analogB;
    s8 err;
} PADStatus;

typedef void (*SIPadCallback)(s32 chan, u32 status, OSContext *context);
typedef void (*SIPadTypeCallback)(s32 chan, u32 type);

extern void OSSetWirelessID(s32 chan, u16 id);
extern BOOL SITransfer(s32 chan, void *output, u32 outputBytes, void *input, u32 inputBytes,
                       SIPadCallback callback, OSTime fire);
extern u32 SIGetType(s32 chan);
extern u32 SIGetTypeAsync(s32 chan, SIPadTypeCallback callback);
extern u32 SIGetStatus(s32 chan);
extern BOOL SIGetResponse(s32 chan, void *data);
extern void SISetCommand(s32 chan, u32 command);
extern void SITransferCommands(void);
extern u32 SIEnablePolling(u32 poll);
extern u32 SIDisablePolling(u32 poll);
extern BOOL SIIsChanBusy(s32 chan);
extern BOOL SIBusy(void);
extern BOOL SIUnregisterPollingHandler(void (*handler)(s16, OSContext *));
extern void SIRefreshSamplingRate(void);
extern u32 __PADFixBits;
extern void fn_801EA088(void);

// low memory: the wireless pad id the fix step hands out, and a few flags

u16 OS_WIRELESS_PAD_FIX_MODE : 0x800030E0;

static void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
static void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
static void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2]);
static BOOL OnReset(BOOL final, u32 event);
static void SamplingHandler(s16 interrupt, OSContext *context);
static void PADTypeAndStatusCallback(s32 chan, u32 type);
static void PADOriginCallback(s32 chan, u32 error, OSContext *context);
static void PADProbeCallback(s32 chan, u32 error, OSContext *context);
static void PADReceiveCheckCallback(s32 chan, u32 type);
static void PADOriginUpdateCallback(s32 chan, u32 error, OSContext *context);

// sdata
static const char *__PADVersion = "<< RVL_SDK - PAD \trelease build: May  8 2007 13:00:22 (0x4199_60831) >>";
static s32 ResettingChan = 32;
static u32 XPatchBits = 0xF0000000;
static u32 AnalogMode = 0x00000300;
static u32 Spec = 5;
static void (*MakeStatus)(s32, PADStatus *, u32 *) = SPEC2_MakeStatus;
static u32 CmdReadOrigin = 0x41000000;
static u32 CmdCalibrate = 0x42000000;

// data
static OSShutdownFunctionInfo ResetFunctionInfo = {OnReset, 127, NULL, NULL};

// sbss, in address order
static BOOL recalibrated;
static void (*SamplingCallback)(void);
u32 __PADSpec;
static u32 BarrelBits;
static u32 PendingBits;
static u32 CheckingBits;
static u32 WaitingBits;
static u32 RecalibrateBits;
static u32 ResettingBits;
static u32 EnabledBits;
static u32 Initialized;

// bss
static PADStatus Origin[PAD_MAX_CONTROLLERS];
static u32 Type[PAD_MAX_CONTROLLERS];
static u32 CmdProbeDevice[PAD_MAX_CONTROLLERS];
static PADStatus LastStatus[PAD_MAX_CONTROLLERS];

static void UpdateOrigin(s32 chan) {
    PADStatus *origin;
    u32 chanBit = PAD_CHAN0_BIT >> chan;

    origin = &Origin[chan];
    switch (AnalogMode & 0x00000700) {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x00000100:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->analogA &= ~15;
        origin->analogB &= ~15;
        break;
    case 0x00000200:
        origin->substickX &= ~15;
        origin->substickY &= ~15;
        origin->triggerLeft &= ~15;
        origin->triggerRight &= ~15;
        break;
    case 0x00000300:
        break;
    case 0x00000400:
        break;
    }

    origin->stickX -= 128;
    origin->stickY -= 128;
    origin->substickX -= 128;
    origin->substickY -= 128;

    if (XPatchBits & chanBit) {
        if (64 < origin->stickX && (SIGetType(chan) & 0xFFFF0000) == SI_GC_CONTROLLER) {
            origin->stickX = 0;
        }
    }
}

static inline void PADEnable(s32 chan) {
    u32 cmd;
    u32 chanBit;
    u32 data[2];

    chanBit = PAD_CHAN0_BIT >> chan;
    EnabledBits |= chanBit;
    SIGetResponse(chan, data);
    cmd = AnalogMode | 0x00400000;
    SISetCommand(chan, cmd);
    SIEnablePolling(EnabledBits);
}

static inline void PADDisable(s32 chan) {
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    chanBit = PAD_CHAN0_BIT >> chan;
    SIDisablePolling(chanBit);
    EnabledBits &= ~chanBit;
    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;
    PendingBits &= ~chanBit;
    BarrelBits &= ~chanBit;
    OSSetWirelessID(chan, 0);
    OSRestoreInterrupts(enabled);
}

// picks the next channel that still needs resetting and starts probing it
static inline void ResetNextChannel(void) {
    ResettingChan = __cntlzw(ResettingBits);
    if (ResettingChan != 32) {
        ResettingBits &= ~(PAD_CHAN0_BIT >> ResettingChan);
        memset(&Origin[ResettingChan], 0, sizeof(PADStatus));
        SIGetTypeAsync(ResettingChan, PADTypeAndStatusCallback);
    }
}

static void PADOriginCallback(s32 chan, u32 error, OSContext *context) {
    if (!(error & SI_ERROR_ALL)) {
        UpdateOrigin(ResettingChan);
        PADEnable(ResettingChan);
    }

    ResetNextChannel();
}

static void PADOriginUpdateCallback(s32 chan, u32 error, OSContext *context) {
    if (!(EnabledBits & (PAD_CHAN0_BIT >> chan))) {
        return;
    }

    if (!(error & SI_ERROR_ALL)) {
        UpdateOrigin(chan);
    }

    if (error & SI_ERROR_NO_RESPONSE) {
        PADDisable(chan);
    }
}

static void PADProbeCallback(s32 chan, u32 error, OSContext *context) {
    if (!(error & SI_ERROR_ALL)) {
        PADEnable(ResettingChan);
        WaitingBits |= PAD_CHAN0_BIT >> ResettingChan;
    }

    ResetNextChannel();
}

static void PADTypeAndStatusCallback(s32 chan, u32 type) {
    u32 chanBit;
    u32 recalibrate;
    BOOL rc = TRUE;
    u32 error;

    chan = ResettingChan;
    chanBit = PAD_CHAN0_BIT >> chan;
    error = type & 0xFF;
    recalibrate = RecalibrateBits & chanBit;
    RecalibrateBits &= ~chanBit;

    if (error & SI_ERROR_ALL) {
        ResetNextChannel();
        return;
    }

    type &= ~0xFF;
    Type[ResettingChan] = type;

    if ((type & SI_TYPE_MASK) != SI_TYPE_GC || !(type & SI_GC_STANDARD)) {
        ResetNextChannel();
        return;
    }

    if (Spec < 2) {
        PADEnable(ResettingChan);
        ResetNextChannel();
        return;
    }

    if (!(type & SI_GC_WIRELESS) || (type & SI_WIRELESS_IR)) {
        if (recalibrate) {
            rc = SITransfer(ResettingChan, &CmdCalibrate, 3, &Origin[ResettingChan], 10, PADOriginCallback, 0);
        } else {
            rc = SITransfer(ResettingChan, &CmdReadOrigin, 1, &Origin[ResettingChan], 10, PADOriginCallback, 0);
        }
    } else if ((type & SI_WIRELESS_FIX_ID) && !(type & SI_WIRELESS_CONT) && !(type & SI_WIRELESS_LITE)) {
        if (type & SI_WIRELESS_RECEIVED) {
            rc = SITransfer(chan, &CmdReadOrigin, 1, &Origin[chan], 10, PADOriginCallback, 0);
        } else {
            rc = SITransfer(chan, &CmdProbeDevice[chan], 3, &Origin[chan], 8, PADProbeCallback, 0);
        }
    }

    if (!rc) {
        PendingBits |= chanBit;
        ResetNextChannel();
    }
}

static void PADReceiveCheckCallback(s32 chan, u32 type) {
    u32 error;
    u32 chanBit;

    chanBit = PAD_CHAN0_BIT >> chan;
    if (!(EnabledBits & chanBit)) {
        return;
    }

    error = type & 0xFF;
    type &= ~0xFF;

    WaitingBits &= ~chanBit;
    CheckingBits &= ~chanBit;

    if (!(error & SI_ERROR_ALL) && (type & SI_GC_WIRELESS) && (type & SI_WIRELESS_FIX_ID) &&
        (type & SI_WIRELESS_RECEIVED) && !(type & SI_WIRELESS_IR) && !(type & SI_WIRELESS_CONT) &&
        !(type & SI_WIRELESS_LITE)) {
        SITransfer(chan, &CmdReadOrigin, 1, &Origin[chan], 10, PADOriginUpdateCallback, 0);
    } else {
        PADDisable(chan);
    }
}

BOOL PADReset(u32 mask) {
    BOOL enabled;
    u32 diableBits;

    enabled = OSDisableInterrupts();

    mask |= PendingBits;
    PendingBits = 0;
    mask &= ~(WaitingBits | CheckingBits);
    ResettingBits |= mask;

    diableBits = ResettingBits & EnabledBits;
    EnabledBits &= ~mask;
    BarrelBits &= ~mask;

    if (Spec == 4) {
        RecalibrateBits |= mask;
    }

    SIDisablePolling(diableBits);
    if (ResettingChan == 32) {
        ResetNextChannel();
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL PADRecalibrate(u32 mask) {
    BOOL enabled;
    u32 disableBits;

    enabled = OSDisableInterrupts();

    mask |= PendingBits;
    PendingBits = 0;
    mask &= ~(WaitingBits | CheckingBits);
    ResettingBits |= mask;

    disableBits = ResettingBits & EnabledBits;
    EnabledBits &= ~mask;
    BarrelBits &= ~mask;

    if (!(OS_PAD_FLAGS & 0x40)) {
        RecalibrateBits |= mask;
    }

    SIDisablePolling(disableBits);
    if (ResettingChan == 32) {
        ResetNextChannel();
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL PADInit(void) {
    s32 chan;
    u32 spec;

    if (Initialized) {
        return TRUE;
    }

    OSRegisterVersion(__PADVersion);

    spec = __PADSpec;
    if (spec) {
        __PADSpec = 0;
        switch (spec) {
        case 0:
            MakeStatus = SPEC0_MakeStatus;
            break;
        case 1:
            MakeStatus = SPEC1_MakeStatus;
            break;
        case 2:
        case 3:
        case 4:
        case 5:
            MakeStatus = SPEC2_MakeStatus;
            break;
        }

        Spec = spec;
    }

    Initialized = TRUE;

    if (__PADFixBits != 0) {
        OSTime time = OSGetTime();

        OS_WIRELESS_PAD_FIX_MODE = (u16)((((time) & 0xffff) + ((time >> 16) & 0xffff) + ((time >> 32) & 0xffff) +
                                          ((time >> 48) & 0xffff)) &
                                         0x3fffu);
        RecalibrateBits = 0xF0000000;
    }

    for (chan = 0; chan < PAD_MAX_CONTROLLERS; ++chan) {
        CmdProbeDevice[chan] = (0x4D << 24) | (chan << 22) | ((OS_WIRELESS_PAD_FIX_MODE & 0x3fffu) << 8);
    }

    SIRefreshSamplingRate();
    OSRegisterShutdownFunction(&ResetFunctionInfo);

    return PADReset(0xF0000000);
}

u32 PADRead(PADStatus *status) {
    BOOL enabled;
    s32 chan;
    u32 data[2];
    u32 chanBit;
    u32 sr;
    u32 motor;

    enabled = OSDisableInterrupts();

    motor = 0;
    for (chan = 0; chan < PAD_MAX_CONTROLLERS; chan++, status++) {
        chanBit = PAD_CHAN0_BIT >> chan;

        if (PendingBits & chanBit) {
            PADReset(0);
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        if ((ResettingBits & chanBit) || ResettingChan == chan) {
            status->err = PAD_ERR_NOT_READY;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        if (!(EnabledBits & chanBit)) {
            status->err = (s8)PAD_ERR_NO_CONTROLLER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        if (SIIsChanBusy(chan)) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        sr = SIGetStatus(chan);
        if (sr & SI_ERROR_NO_RESPONSE) {
            SIGetResponse(chan, data);

            if (WaitingBits & chanBit) {
                status->err = PAD_ERR_NONE;
                memset(status, 0, offsetof(PADStatus, err));

                if (!(CheckingBits & chanBit)) {
                    CheckingBits |= chanBit;
                    SIGetTypeAsync(chan, PADReceiveCheckCallback);
                }
                continue;
            }

            PADDisable(chan);

            status->err = (s8)PAD_ERR_NO_CONTROLLER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        if (!(SIGetType(chan) & SI_GC_NOMOTOR)) {
            motor |= chanBit;
        }

        if (!SIGetResponse(chan, data)) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        if (data[0] & 0x80000000) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, offsetof(PADStatus, err));
            continue;
        }

        MakeStatus(chan, status, data);

        // the origin bit is set whenever the pad lost its origin, read it again
        if (status->button & 0x2000) {
            status->err = PAD_ERR_TRANSFER;
            memset(status, 0, offsetof(PADStatus, err));
            SITransfer(chan, &CmdReadOrigin, 1, &Origin[chan], 10, PADOriginUpdateCallback, 0);
        } else {
            status->err = PAD_ERR_NONE;
            status->button &= ~0x80;
        }
    }

    OSRestoreInterrupts(enabled);
    return motor;
}

void PADControlMotor(s32 chan, u32 command) {
    BOOL enabled;
    u32 chanBit;

    enabled = OSDisableInterrupts();
    chanBit = PAD_CHAN0_BIT >> chan;
    if ((EnabledBits & chanBit) && !(SIGetType(chan) & SI_GC_NOMOTOR)) {
        if (Spec < 2 && command == PAD_MOTOR_STOP_HARD) {
            command = PAD_MOTOR_STOP;
        }

        if (OS_PAD_FLAGS & 0x20) {
            command = PAD_MOTOR_STOP;
        }

        SISetCommand(chan, (0x00400000 | AnalogMode | (command & 0x00000003)));
        SITransferCommands();
    }

    OSRestoreInterrupts(enabled);
}

static void SPEC0_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0008) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0001) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_START : 0;

    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);

    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];

    status->analogA = 0;
    status->analogB = 0;

    if (170 <= status->triggerLeft) {
        status->button |= PAD_TRIGGER_L;
    }
    if (170 <= status->triggerRight) {
        status->button |= PAD_TRIGGER_R;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static void SPEC1_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    status->button = 0;
    status->button |= ((data[0] >> 16) & 0x0080) ? PAD_BUTTON_A : 0;
    status->button |= ((data[0] >> 16) & 0x0100) ? PAD_BUTTON_B : 0;
    status->button |= ((data[0] >> 16) & 0x0020) ? PAD_BUTTON_X : 0;
    status->button |= ((data[0] >> 16) & 0x0010) ? PAD_BUTTON_Y : 0;
    status->button |= ((data[0] >> 16) & 0x0200) ? PAD_BUTTON_START : 0;

    status->stickX = (s8)(data[1] >> 16);
    status->stickY = (s8)(data[1] >> 24);
    status->substickX = (s8)(data[1]);
    status->substickY = (s8)(data[1] >> 8);

    status->triggerLeft = (u8)(data[0] >> 8);
    status->triggerRight = (u8)data[0];

    status->analogA = 0;
    status->analogB = 0;

    if (170 <= status->triggerLeft) {
        status->button |= PAD_TRIGGER_L;
    }
    if (170 <= status->triggerRight) {
        status->button |= PAD_TRIGGER_R;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;
}

static inline s8 ClampS8(s8 var, s8 org) {
    if (0 < org) {
        s8 min = (s8)(-128 + org);
        if (var < min) {
            var = min;
        }
    } else if (org < 0) {
        s8 max = (s8)(127 + org);
        if (max < var) {
            var = max;
        }
    }
    return (s8)(var - org);
}

static inline u8 ClampU8(u8 var, u8 org) {
    if (var < org) {
        var = org;
    }
    return (u8)(var - org);
}

static void SPEC2_MakeStatus(s32 chan, PADStatus *status, u32 data[2]) {
    PADStatus *origin;

    status->button = (u16)((data[0] >> 16) & 0x3FFF);
    status->stickX = (s8)(data[0] >> 8);
    status->stickY = (s8)(data[0]);

    switch (AnalogMode & 0x00000700) {
    case 0x00000000:
    case 0x00000500:
    case 0x00000600:
    case 0x00000700:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(((data[1] >> 12) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 8) & 0x0f) << 4);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1]) & 0x0f) << 4);
        break;
    case 0x00000100:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(data[1] >> 16);
        status->triggerRight = (u8)(data[1] >> 8);
        status->analogA = (u8)(((data[1] >> 4) & 0x0f) << 4);
        status->analogB = (u8)(((data[1]) & 0x0f) << 4);
        break;
    case 0x00000200:
        status->substickX = (s8)(((data[1] >> 28) & 0x0f) << 4);
        status->substickY = (s8)(((data[1] >> 24) & 0x0f) << 4);
        status->triggerLeft = (u8)(((data[1] >> 20) & 0x0f) << 4);
        status->triggerRight = (u8)(((data[1] >> 16) & 0x0f) << 4);
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1]);
        break;
    case 0x00000300:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = (u8)(data[1] >> 8);
        status->triggerRight = (u8)(data[1]);
        status->analogA = 0;
        status->analogB = 0;
        break;
    case 0x00000400:
        status->substickX = (s8)(data[1] >> 24);
        status->substickY = (s8)(data[1] >> 16);
        status->triggerLeft = 0;
        status->triggerRight = 0;
        status->analogA = (u8)(data[1] >> 8);
        status->analogB = (u8)(data[1]);
        break;
    }

    status->stickX -= 128;
    status->stickY -= 128;
    status->substickX -= 128;
    status->substickY -= 128;

    if ((Type[chan] & 0xFFFF0000) == SI_GC_CONTROLLER && ((status->button & 0x80) ^ 0x80)) {
        BarrelBits |= PAD_CHAN0_BIT >> chan;
        status->stickX = 0;
        status->stickY = 0;
        status->substickX = 0;
        status->substickY = 0;
        return;
    }

    BarrelBits &= ~(PAD_CHAN0_BIT >> chan);

    origin = &Origin[chan];
    status->stickX = ClampS8(status->stickX, origin->stickX);
    status->stickY = ClampS8(status->stickY, origin->stickY);
    status->substickX = ClampS8(status->substickX, origin->substickX);
    status->substickY = ClampS8(status->substickY, origin->substickY);
    status->triggerLeft = ClampU8(status->triggerLeft, origin->triggerLeft);
    status->triggerRight = ClampU8(status->triggerRight, origin->triggerRight);

    // wake the system from idle whenever the pad actually moved
    {
        if (abs(status->stickX * status->stickX - LastStatus[chan].stickX * LastStatus[chan].stickX) >= 9 ||
            abs(status->stickY * status->stickY - LastStatus[chan].stickY * LastStatus[chan].stickY) >= 9 ||
            abs(status->substickX * status->substickX - LastStatus[chan].substickX * LastStatus[chan].substickX) >= 9 ||
            abs(status->substickY * status->substickY - LastStatus[chan].substickY * LastStatus[chan].substickY) >= 9 ||
            abs(status->triggerLeft * status->triggerLeft - LastStatus[chan].triggerLeft * LastStatus[chan].triggerLeft) >= 9 ||
            abs(status->triggerRight * status->triggerRight - LastStatus[chan].triggerRight * LastStatus[chan].triggerRight) >= 9 ||
            status->button != LastStatus[chan].button) {
            fn_801EA088();
        }

        memcpy(&LastStatus[chan], status, sizeof(PADStatus));
    }
}

static inline BOOL PADIdle(void) {
    return ResettingBits == 0 && ResettingChan == 32;
}

static BOOL OnReset(BOOL final, u32 event) {
    BOOL sync;

    if (SamplingCallback) {
        SamplingCallback = NULL;
        SIUnregisterPollingHandler(SamplingHandler);
    }

    if (!final) {
        sync = FALSE;
        if (PADIdle() && !SIBusy()) {
            sync = TRUE;
        }

        if (!recalibrated && sync) {
            recalibrated = PADRecalibrate(0xF0000000);
            return FALSE;
        }
        return sync;
    } else {
        recalibrated = FALSE;
    }

    return TRUE;
}

static void SamplingHandler(s16 interrupt, OSContext *context) {
    OSContext exceptionContext;

    if (SamplingCallback) {
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(&exceptionContext);
        SamplingCallback();
        OSClearContext(&exceptionContext);
        OSSetCurrentContext(context);
    }
}

BOOL __PADDisableRecalibration(BOOL disable) {
    BOOL enabled = OSDisableInterrupts();
    BOOL old = (OS_PAD_FLAGS & 0x40) ? TRUE : FALSE;

    OS_PAD_FLAGS &= ~0x40;

    if (disable) {
        OS_PAD_FLAGS |= 0x40;
    }

    OSRestoreInterrupts(enabled);
    return old;
}

}
