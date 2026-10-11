// exi bios: channel locking, immediate / dma transfers, device attach, the three interrupt handlers.
// shape follows open_rvl's EXIBios and the diag reference; field names are mine where the sibling
// headers had none. functions the game never calls (EXIProbe, EXIProbeReset, EXIClearInterrupts
// as an exported symbol) get dead stripped from the dol, they are drafted anyway so inlining matches.
#include "revolution.h"
#include "revolution/exi.h"
#include <cstring>

#define EXI_CHAN_0 0
#define EXI_CHAN_1 1
#define EXI_CHAN_2 2

#define EXI_DEV_EXT 0
#define EXI_DEV_NET 2

#define EXI_READ 0
#define EXI_WRITE 1

#define EXI_STATE_DMA_ACCESS (1 << 0)
#define EXI_STATE_IMM_ACCESS (1 << 1)
#define EXI_STATE_BUSY (EXI_STATE_DMA_ACCESS | EXI_STATE_IMM_ACCESS)
#define EXI_STATE_SELECTED (1 << 2)
#define EXI_STATE_ATTACHED (1 << 3)
#define EXI_STATE_LOCKED (1 << 4)

#define EXI_FREQ_1MHZ 0

#define EXI_ID_USB_ADAPTER 0x01010000
#define EXI_ID_IS_DOL_VIEWER 0x05070000

// low memory words the sdk keeps around
#define OS_EXI_LAST_INSERT(c) (((volatile s32 *)0x80000000)[0x30C0 / 4 + (c)])
#define OS_DVD_DEVICE_CODE (*(volatile u16 *)0x800030E6)

typedef struct EXIItem {
    u32 dev;
    EXICallback callback;
} EXIItem;

typedef struct EXIData {
    EXICallback exiCallback; // 0x00
    EXICallback tcCallback;  // 0x04
    EXICallback extCallback; // 0x08
    volatile s32 state;      // 0x0C
    s32 bytesRead;           // 0x10
    void *buffer;            // 0x14
    u32 dev;                 // 0x18
    u32 id;                  // 0x1C
    s32 lastInsert;          // 0x20
    s32 numItems;            // 0x24
    EXIItem items[3];        // 0x28
} EXIData;

extern u32 __OSGetDIConfig(void);
extern BOOL __OSInIPL;

const char *__EXIVersion = "<< RVL_SDK - EXI \trelease build: Jun 28 2007 18:29:36 (0x4199_60831) >>";

static EXIData Ecb[3];
static u32 IDSerialPort1;

static BOOL __EXIProbe(s32 chan);

static void SetExiInterruptMask(s32 chan, EXIData *exi) {
    EXIData *exi2 = &Ecb[EXI_CHAN_2];

    switch (chan) {
    case EXI_CHAN_0:
        if ((exi->exiCallback == NULL && exi2->exiCallback == NULL) || (exi->state & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_2_EXI);
        } else {
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_2_EXI);
        }
        break;
    case EXI_CHAN_1:
        if (exi->exiCallback == NULL || (exi->state & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_1_EXI);
        } else {
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_1_EXI);
        }
        break;
    case EXI_CHAN_2:
        if (__OSGetInterruptHandler(__OS_INTERRUPT_PI_DEBUG) == NULL || (exi->state & EXI_STATE_LOCKED)) {
            __OSMaskInterrupts(OS_INTERRUPTMASK_PI_DEBUG);
        } else {
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_PI_DEBUG);
        }
    }
}

static void CompleteTransfer(s32 chan) {
    EXIData *exi = &Ecb[chan];
    s32 len;

    if (exi->state & EXI_STATE_BUSY) {
        if ((exi->state & EXI_STATE_IMM_ACCESS) && (len = exi->bytesRead) != 0) {
            u8 *buf;
            u32 data;
            int i;

            buf = (u8 *)exi->buffer;
            data = EXI_CHAN_PARAMS[chan].data;
            for (i = 0; i < len; i++) {
                *buf++ = data >> (3 - i) * 8;
            }
        }

        exi->state &= ~EXI_STATE_BUSY;
    }
}

BOOL EXIImm(s32 chan, void *buf, s32 len, u32 type, EXICallback callback) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();

    if ((exi->state & EXI_STATE_BUSY) || !(exi->state & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->tcCallback = callback;
    if (callback != NULL) {
        u32 mask = OS_INTERRUPTMASK_EXI_0_TC;
        EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(mask >> (chan * 3));
    }

    exi->state |= EXI_STATE_IMM_ACCESS;

    if (type != EXI_READ) {
        int i;
        u32 word = 0;
        for (i = 0; i < len; i++) {
            word |= ((u8 *)buf)[i] << (3 - i) * 8;
        }

        EXI_CHAN_PARAMS[chan].data = word;
    }

    exi->buffer = buf;
    exi->bytesRead = (type != EXI_WRITE) ? len : 0;
    EXI_CHAN_PARAMS[chan].cr = EXI_CR_TSTART | type << 2 | (len - 1) << 4;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIImmEx(s32 chan, void *buf, s32 len, u32 type) {
    while (len != 0) {
        s32 immLen = (len < 4) ? len : 4;

        if (!EXIImm(chan, buf, immLen, type, NULL)) {
            return FALSE;
        }

        if (!EXISync(chan)) {
            return FALSE;
        }

        buf = (u8 *)buf + immLen;
        len -= immLen;
    }

    return TRUE;
}

BOOL EXIDma(s32 chan, void *buf, s32 len, u32 type, EXICallback callback) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled = OSDisableInterrupts();

    if ((exi->state & EXI_STATE_BUSY) || !(exi->state & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->tcCallback = callback;
    if (callback != NULL) {
        u32 mask = OS_INTERRUPTMASK_EXI_0_TC;
        EXIClearInterrupts(chan, FALSE, TRUE, FALSE);
        __OSUnmaskInterrupts(mask >> (chan * 3));
    }

    exi->state |= EXI_STATE_DMA_ACCESS;

    EXI_CHAN_PARAMS[chan].mar = (void *)((u32)buf & ~0x1F);
    EXI_CHAN_PARAMS[chan].length = len;
    EXI_CHAN_PARAMS[chan].cr = type << 2 | EXI_CR_TSTART | EXI_CR_DMA;

    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXISync(s32 chan) {
    EXIData *exi = &Ecb[chan];
    BOOL ret = FALSE;

    while (exi->state & EXI_STATE_SELECTED) {
        if (!(EXI_CHAN_PARAMS[chan].cr & EXI_CR_TSTART)) {
            BOOL enabled = OSDisableInterrupts();

            if (exi->state & EXI_STATE_SELECTED) {
                CompleteTransfer(chan);

                if (__OSGetDIConfig() != 0xFF || (OSGetConsoleType() & 0xF0000000) == 0x20000000 ||
                    exi->bytesRead != 4 || (EXI_CHAN_PARAMS[chan].cpr & EXI_CPR_CLK) ||
                    (EXI_CHAN_PARAMS[chan].data != EXI_ID_USB_ADAPTER &&
                     EXI_CHAN_PARAMS[chan].data != EXI_ID_IS_DOL_VIEWER && EXI_CHAN_PARAMS[chan].data != 0x04220001) ||
                    OS_DVD_DEVICE_CODE == 0x8200) {
                    ret = TRUE;
                }
            }

            OSRestoreInterrupts(enabled);
            break;
        }
    }

    return ret;
}

u32 EXIClearInterrupts(s32 chan, BOOL exi, BOOL tc, BOOL ext) {
    u32 cpr = EXI_CHAN_PARAMS[chan].cpr & 0xFFF & ~(EXI_CPR_EXIINT | EXI_CPR_TCINT | EXI_CPR_EXTINT);

    if (exi) {
        cpr |= EXI_CPR_EXIINT;
    }

    if (tc) {
        cpr |= EXI_CPR_TCINT;
    }

    if (ext) {
        cpr |= EXI_CPR_EXTINT;
    }

    EXI_CHAN_PARAMS[chan].cpr = cpr;
    return cpr;
}

EXICallback EXISetExiCallback(s32 chan, EXICallback callback) {
    EXIData *exi;
    EXICallback old;
    BOOL enabled;

    exi = &Ecb[chan];
    enabled = OSDisableInterrupts();

    old = exi->exiCallback;
    exi->exiCallback = callback;

    if (chan != EXI_CHAN_2) {
        SetExiInterruptMask(chan, exi);
    } else {
        SetExiInterruptMask(EXI_CHAN_0, &Ecb[EXI_CHAN_0]);
    }

    OSRestoreInterrupts(enabled);
    return old;
}

void EXIProbeReset(void) {
    OS_EXI_LAST_INSERT(0) = OS_EXI_LAST_INSERT(1) = 0;
    Ecb[EXI_CHAN_0].lastInsert = Ecb[EXI_CHAN_1].lastInsert = 0;
    __EXIProbe(EXI_CHAN_0);
    __EXIProbe(EXI_CHAN_1);
}

static BOOL __EXIProbe(s32 chan) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled;
    BOOL ret;
    u32 cpr;

    if (chan == EXI_CHAN_2) {
        return TRUE;
    }

    ret = TRUE;
    enabled = OSDisableInterrupts();
    cpr = EXI_CHAN_PARAMS[chan].cpr;

    if (!(exi->state & EXI_STATE_ATTACHED)) {
        if (cpr & EXI_CPR_EXTINT) {
            EXIClearInterrupts(chan, FALSE, FALSE, TRUE);
            OS_EXI_LAST_INSERT(chan) = exi->lastInsert = 0;
        }

        if (cpr & EXI_CPR_EXT) {
            s32 time = (s32)(OSTicksToMilliseconds(OSGetTime()) / 100) + 1;
            if (OS_EXI_LAST_INSERT(chan) == 0) {
                OS_EXI_LAST_INSERT(chan) = time;
            }

            if (time - OS_EXI_LAST_INSERT(chan) < 3) {
                ret = FALSE;
            }
        } else {
            OS_EXI_LAST_INSERT(chan) = exi->lastInsert = 0;
            ret = FALSE;
        }
    } else if (!(cpr & EXI_CPR_EXT) || (cpr & EXI_CPR_EXTINT)) {
        OS_EXI_LAST_INSERT(chan) = exi->lastInsert = 0;
        ret = FALSE;
    }

    OSRestoreInterrupts(enabled);
    return ret;
}

BOOL EXIProbe(s32 chan) {
    EXIData *exi = &Ecb[chan];

    BOOL ret = __EXIProbe(chan);
    if (ret && exi->lastInsert == 0) {
        u32 id;
        return EXIGetID(chan, EXI_DEV_EXT, &id);
    }

    return FALSE;
}

static BOOL __EXIAttach(s32 chan, EXICallback callback) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled;
    u32 mask;

    enabled = OSDisableInterrupts();
    if ((exi->state & EXI_STATE_ATTACHED) || !__EXIProbe(chan)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    EXIClearInterrupts(chan, TRUE, FALSE, FALSE);
    exi->extCallback = callback;
    mask = OS_INTERRUPTMASK_EXI_0_EXT;
    __OSUnmaskInterrupts(mask >> (chan * 3));
    exi->state |= EXI_STATE_ATTACHED;
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIAttach(s32 chan, EXICallback callback) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled;
    BOOL ret;

    EXIProbe(chan);
    enabled = OSDisableInterrupts();

    if (exi->lastInsert == 0) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    ret = __EXIAttach(chan, callback);
    OSRestoreInterrupts(enabled);
    return ret;
}

BOOL EXIDetach(s32 chan) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled;
    u32 mask;

    enabled = OSDisableInterrupts();
    if (!(exi->state & EXI_STATE_ATTACHED)) {
        OSRestoreInterrupts(enabled);
        return TRUE;
    }

    if ((exi->state & EXI_STATE_LOCKED) && exi->dev == EXI_DEV_EXT) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state &= ~EXI_STATE_ATTACHED;
    mask = OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_0_EXT;
    __OSMaskInterrupts(mask >> (chan * 3));
    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXISelect(s32 chan, u32 dev, u32 freq) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled;
    u32 cpr;

    enabled = OSDisableInterrupts();

    if ((exi->state & EXI_STATE_SELECTED) ||
        (chan != EXI_CHAN_2 &&
         ((dev == EXI_DEV_EXT && !(exi->state & EXI_STATE_ATTACHED) && !__EXIProbe(chan)) ||
          !(exi->state & EXI_STATE_LOCKED) || exi->dev != dev))) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state |= EXI_STATE_SELECTED;

    cpr = EXI_CHAN_PARAMS[chan].cpr;
    cpr &= (EXI_CPR_EXIINTMASK | EXI_CPR_TCINTMASK | EXI_CPR_EXTINTMASK);
    cpr |= (1 << dev) << 7 | freq << 4;
    EXI_CHAN_PARAMS[chan].cpr = cpr;

    if (exi->state & EXI_STATE_ATTACHED) {
        switch (chan) {
        case EXI_CHAN_0:
            __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXT);
            break;
        case EXI_CHAN_1:
            __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_1_EXT);
            break;
        }
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

BOOL EXIDeselect(s32 chan) {
    EXIData *exi = &Ecb[chan];
    u32 cpr;
    BOOL enabled;

    enabled = OSDisableInterrupts();

    if (!(exi->state & EXI_STATE_SELECTED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state &= ~EXI_STATE_SELECTED;
    cpr = EXI_CHAN_PARAMS[chan].cpr;
    EXI_CHAN_PARAMS[chan].cpr = cpr & (EXI_CPR_EXIINTMASK | EXI_CPR_TCINTMASK | EXI_CPR_EXTINTMASK);

    if (exi->state & EXI_STATE_ATTACHED) {
        switch (chan) {
        case EXI_CHAN_0:
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXT);
            break;
        case EXI_CHAN_1:
            __OSUnmaskInterrupts(OS_INTERRUPTMASK_EXI_1_EXT);
            break;
        }
    }

    OSRestoreInterrupts(enabled);

    if (chan != EXI_CHAN_2 && (cpr & EXI_CPR_CS0B)) {
        return __EXIProbe(chan) != FALSE;
    }

    return TRUE;
}

static void EXIIntrruptHandler(__OSInterrupt intr, OSContext *ctx) {
    EXIData *exi;
    s32 chan;
    EXICallback callback;
    OSContext temp;

    chan = (intr - __OS_INTERRUPT_EXI_0_EXI) / 3;
    exi = &Ecb[chan];

    EXIClearInterrupts(chan, TRUE, FALSE, FALSE);

    callback = exi->exiCallback;
    if (callback != NULL) {
        OSClearContext(&temp);
        OSSetCurrentContext(&temp);
        callback(chan, ctx);
        OSClearContext(&temp);
        OSSetCurrentContext(ctx);
    }
}

static void TCIntrruptHandler(__OSInterrupt intr, OSContext *ctx) {
    EXIData *exi;
    s32 chan;
    EXICallback callback;
    OSContext temp;

    chan = (intr - __OS_INTERRUPT_EXI_0_TC) / 3;
    exi = &Ecb[chan];

    __OSMaskInterrupts(0x80000000 >> intr);
    EXIClearInterrupts(chan, FALSE, TRUE, FALSE);

    callback = exi->tcCallback;
    if (callback != NULL) {
        exi->tcCallback = NULL;
        CompleteTransfer(chan);
        OSClearContext(&temp);
        OSSetCurrentContext(&temp);
        callback(chan, ctx);
        OSClearContext(&temp);
        OSSetCurrentContext(ctx);
    }
}

static void EXTIntrruptHandler(__OSInterrupt intr, OSContext *ctx) {
    s32 chan;
    EXIData *exi;
    EXICallback callback;
    OSContext temp;
    u32 mask;

    chan = (intr - __OS_INTERRUPT_EXI_0_EXT) / 3;
    mask = OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_0_EXT;
    __OSMaskInterrupts(mask >> (chan * 3));

    exi = &Ecb[chan];
    callback = exi->extCallback;
    exi->state &= ~EXI_STATE_ATTACHED;

    if (callback != NULL) {
        OSClearContext(&temp);
        OSSetCurrentContext(&temp);
        exi->extCallback = NULL;
        callback(chan, ctx);
        OSClearContext(&temp);
        OSSetCurrentContext(ctx);
    }
}

void EXIInit(void) {
    u32 id;

    do {
        do {
            ;
        } while ((EXI_CHAN_PARAMS[EXI_CHAN_0].cr & EXI_CR_TSTART) == 1);
    } while ((EXI_CHAN_PARAMS[EXI_CHAN_1].cr & EXI_CR_TSTART) == 1 ||
             (EXI_CHAN_PARAMS[EXI_CHAN_2].cr & EXI_CR_TSTART) == 1);

    __OSMaskInterrupts(OS_INTERRUPTMASK_EXI_0_EXI | OS_INTERRUPTMASK_EXI_0_TC | OS_INTERRUPTMASK_EXI_0_EXT |
                       OS_INTERRUPTMASK_EXI_1_EXI | OS_INTERRUPTMASK_EXI_1_TC | OS_INTERRUPTMASK_EXI_1_EXT |
                       OS_INTERRUPTMASK_EXI_2_EXI | OS_INTERRUPTMASK_EXI_2_TC);

    EXI_CHAN_PARAMS[EXI_CHAN_0].cpr = 0;
    EXI_CHAN_PARAMS[EXI_CHAN_1].cpr = 0;
    EXI_CHAN_PARAMS[EXI_CHAN_2].cpr = 0;

    EXI_CHAN_PARAMS[EXI_CHAN_0].cpr = EXI_CPR_ROMDIS;

    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_0_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_0_TC, TCIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_0_EXT, EXTIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_1_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_1_TC, TCIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_1_EXT, EXTIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_2_EXI, EXIIntrruptHandler);
    __OSSetInterruptHandler(__OS_INTERRUPT_EXI_2_TC, TCIntrruptHandler);

    EXIGetID(EXI_CHAN_0, EXI_DEV_NET, &IDSerialPort1);

    if (__OSInIPL) {
        EXIProbeReset();
    } else {
        if ((EXIGetID(EXI_CHAN_0, EXI_DEV_EXT, &id) && id == 0x07010000)) {
            __OSEnableBarnacle(EXI_CHAN_1, EXI_DEV_EXT);
        } else if (EXIGetID(EXI_CHAN_1, EXI_DEV_EXT, &id) && id == 0x07010000) {
            __OSEnableBarnacle(EXI_CHAN_0, EXI_DEV_NET);
        }
    }

    OSRegisterVersion(__EXIVersion);
}

BOOL EXILock(s32 chan, u32 dev, EXICallback callback) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled;
    int i;

    enabled = OSDisableInterrupts();

    if (exi->state & EXI_STATE_LOCKED) {
        if (callback != NULL) {
            for (i = 0; i < exi->numItems; i++) {
                if (exi->items[i].dev == dev) {
                    OSRestoreInterrupts(enabled);
                    return FALSE;
                }
            }

            exi->items[exi->numItems].callback = callback;
            exi->items[exi->numItems].dev = dev;
            exi->numItems++;
        }

        OSRestoreInterrupts(enabled);
        return FALSE;
    } else {
        exi->state |= EXI_STATE_LOCKED;
        exi->dev = dev;
        SetExiInterruptMask(chan, exi);
        OSRestoreInterrupts(enabled);
        return TRUE;
    }
}

BOOL EXIUnlock(s32 chan) {
    EXIData *exi = &Ecb[chan];
    BOOL enabled;
    EXICallback callback;

    enabled = OSDisableInterrupts();

    if (!(exi->state & EXI_STATE_LOCKED)) {
        OSRestoreInterrupts(enabled);
        return FALSE;
    }

    exi->state &= ~EXI_STATE_LOCKED;
    SetExiInterruptMask(chan, exi);

    if (exi->numItems > 0) {
        callback = exi->items[0].callback;

        if (--exi->numItems > 0) {
            memmove(&exi->items[0], &exi->items[1], exi->numItems * sizeof(EXIItem));
        }

        callback(chan, NULL);
    }

    OSRestoreInterrupts(enabled);
    return TRUE;
}

// does nothing useful as far as i can tell, the queued id probe is what it is
static void UnlockedHandler(s32 chan, OSContext *ctx) {
#pragma unused(ctx)
    u32 id;
    EXIGetID(chan, EXI_DEV_EXT, &id);
}

s32 EXIGetID(s32 chan, u32 dev, u32 *out) {
    EXIData *exi = &Ecb[chan];
    u32 imm;
    s32 ret;
    s32 time;
    BOOL enabled;

    if (chan == EXI_CHAN_0 && dev == EXI_DEV_NET && IDSerialPort1 != 0) {
        *out = IDSerialPort1;
        return 1;
    }

    if (chan < EXI_CHAN_2 && dev == EXI_DEV_EXT) {
        if (!__EXIProbe(chan)) {
            return 0;
        }

        if (exi->lastInsert == OS_EXI_LAST_INSERT(chan)) {
            *out = exi->id;
            return exi->lastInsert;
        }

        if (!__EXIAttach(chan, NULL)) {
            return 0;
        }

        time = OS_EXI_LAST_INSERT(chan);
    }

    enabled = OSDisableInterrupts();
    ret = !EXILock(chan, dev, (chan < EXI_CHAN_2 && dev == EXI_DEV_EXT) ? UnlockedHandler : NULL);
    if (ret == 0) {
        ret = !EXISelect(chan, dev, EXI_FREQ_1MHZ);
        if (ret == 0) {
            imm = 0x00000000;
            ret |= !EXIImm(chan, &imm, sizeof(u16), EXI_WRITE, NULL);
            ret |= !EXISync(chan);
            ret |= !EXIImm(chan, out, sizeof(u32), EXI_READ, NULL);
            ret |= !EXISync(chan);
            ret |= !EXIDeselect(chan);
        }
        EXIUnlock(chan);
    }

    OSRestoreInterrupts(enabled);

    if (chan < EXI_CHAN_2 && dev == EXI_DEV_EXT) {
        EXIDetach(chan);

        enabled = OSDisableInterrupts();
        ret |= time != OS_EXI_LAST_INSERT(chan);
        if (ret == 0) {
            exi->id = *out;
            exi->lastInsert = time;
        }

        OSRestoreInterrupts(enabled);
        return ret != 0 ? 0 : exi->lastInsert;
    }

    return ret == 0;
}
