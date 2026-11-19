#include "revolution/wpad.h"
#include <mem.h>

// TODO: clean up, find what keeps these helpers from being inlined instead of the pragma
#pragma dont_inline on

extern const char* _gamecode;
extern u8 _gametype;
extern u16 lbl_804F9EE0[];

BOOL __CanPushCmdQueue(WPADCmdQueue* queue, s32 num);

s32 fn_802252E8(s32 chan, void* buf, u16 len, u32 addr, WPADCallback callback);
s32 WPADReadMemoryAsync(s32 chan, void* buf, u16 len, u32 addr, WPADCallback callback);
void fn_802249F8(s32 chan, s32 result);
void fn_80224B68(s32 chan, s32 result);
void fn_80224CA8(s32 chan, s32 result);
void fn_80224CE0(s32 chan, s32 result);

void ResultCallback(s32 chan, s32 result) {
    WPADCallback callback;

    _wmb[chan].used = FALSE;
    callback = _wmb[chan].callback;
    _wmb[chan].callback = NULL;
    if (callback) {
        callback(chan, result);
    }
}

void fn_802249F8(s32 chan, s32 result) {
    u16 count;
    WPADCallback callback;
    BOOL enabled;

    if (_wmb[chan].len == 0) {
        _wmb[chan].used = FALSE;
        callback = _wmb[chan].callback;
        _wmb[chan].callback = NULL;
        if (callback) {
            callback(chan, result);
        }
    } else {
        count = _wmb[chan].len > 16 ? 16 : _wmb[chan].len;
        if (result == WPAD_ERR_NONE) {
            _wmb[chan].len -= count;
            _wmb[chan].addr += 16;
            _wmb[chan].p_buf += 16;
            enabled = OSDisableInterrupts();
            fn_802252E8(chan, _wmb[chan].p_buf, count, _wmb[chan].addr, fn_802249F8);
            OSRestoreInterrupts(enabled);
        } else if (result == WPAD_ERR_BUSY) {
            enabled = OSDisableInterrupts();
            fn_802252E8(chan, _wmb[chan].p_buf, count, _wmb[chan].addr, fn_802249F8);
            OSRestoreInterrupts(enabled);
        } else {
            _wmb[chan].used = FALSE;
            callback = _wmb[chan].callback;
            _wmb[chan].callback = NULL;
            if (callback) {
                callback(chan, result);
            }
        }
    }
}

void fn_80224B68(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (p_wpd->gameInfoErr[1] == 0) {
        p_wpd->gameInfoErr[1] = (result == 0) ? 0 : -3;
    }
    if (result == 0) {
        if ((p_wpd->gameInfoErr[0] == 0) || (p_wpd->gameInfoErr[1] == 0)) {
            BOOL enabled;
            u16 count = (u16)((_wmb[chan].len > 16) ? 16 : _wmb[chan].len);
            _wmb[chan].len -= count;
            enabled = (0, OSDisableInterrupts());
            fn_802252E8(chan, _wmb[chan].p_buf, count, _wmb[chan].addr, fn_802249F8);
            OSRestoreInterrupts(enabled);
            return;
        }
        result = -3;
    }
    if (_wmb[chan].callback != NULL) {
        _wmb[chan].used = FALSE;
        _wmb[chan].callback(chan, result);
        _wmb[chan].callback = NULL;
    }
}

void fn_80224CA8(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (p_wpd->gameInfoErr[0] == 0) {
        p_wpd->gameInfoErr[0] = (result == 0) ? 0 : -3;
    }
}

void fn_80224CE0(s32 chan, s32 result) {
    WPADControlBlock* p_wpd = _wpdcb[chan];

    if (p_wpd->gameInfoErr[1] == 0) {
        p_wpd->gameInfoErr[1] = (result == 0) ? 0 : -3;
    }
}

s32 fn_80224D18(s32 chan, u8* buf, u16 len, u32 addr, WPADCallback callback) {
    BOOL enabled;
    WPADControlBlock* p_wpd = _wpdcb[chan];
    s32 result;
    s32 ready;

    enabled = OSDisableInterrupts();
    result = p_wpd->status;
    ready = p_wpd->setup;
    if (result != WPAD_ERR_NO_CONTROLLER) {
        if (!ready) {
            result = WPAD_ERR_BUSY;
        } else if (!_wmb[chan].used) {
            if (!__CanPushCmdQueue(&p_wpd->cmdq, 9)) {
                result = WPAD_ERR_BUSY;
                goto done;
            }
            {
                u8 i;
                _wmb[chan].used = TRUE;
                _wmb[chan].callback = callback;
                _wmb[chan].len = len;
                _wmb[chan].addr = addr + 0x9A;
                _wmb[chan].p_buf = buf;
                memcpy(p_wpd->gameInfo.gameId, _gamecode, 4);
                memcpy(p_wpd->gameInfo.gameTitle, lbl_804F9EE0, 0x22);
                p_wpd->gameInfo.calendarTimeTick = OSGetTime();
                p_wpd->gameInfo.gameType = _gametype;
                p_wpd->gameInfo.checkSum = 0;
                for (i = 0; i < 47; i++) {
                    p_wpd->gameInfo.checkSum += ((u8*)&p_wpd->gameInfo)[i];
                }
                p_wpd->gameInfo.checkSum += 0x55;
                p_wpd->gameInfoErr[0] = 0;
                p_wpd->gameInfoErr[1] = 0;
                fn_802252E8(chan, &p_wpd->gameInfo, 0x10, 0x2A, fn_80224CA8);
                fn_802252E8(chan, (u8*)&p_wpd->gameInfo + 0x10, 0x10, 0x3A, fn_80224CA8);
                fn_802252E8(chan, (u8*)&p_wpd->gameInfo + 0x20, 0x10, 0x4A, fn_80224CA8);
                fn_802252E8(chan, (u8*)&p_wpd->gameInfo + 0x30, 8, 0x5A, fn_80224CA8);
                fn_802252E8(chan, &p_wpd->gameInfo, 0x10, 0x62, fn_80224CE0);
                fn_802252E8(chan, (u8*)&p_wpd->gameInfo + 0x10, 0x10, 0x72, fn_80224CE0);
                fn_802252E8(chan, (u8*)&p_wpd->gameInfo + 0x20, 0x10, 0x82, fn_80224CE0);
                fn_802252E8(chan, (u8*)&p_wpd->gameInfo + 0x30, 8, 0x92, fn_80224B68);
                OSRestoreInterrupts(enabled);
                return 0;
            }
        } else {
            result = WPAD_ERR_BUSY;
        }
    }
done:
    OSRestoreInterrupts(enabled);
    if (callback) {
        callback(chan, result);
    }
    return result;
}

s32 fn_802250C8(s32 chan, void* buf, u16 len, u32 addr, WPADCallback callback) {
    WPADControlBlock* p_wpd;
    s32 result;
    BOOL interrupts;
    s32 missing;
    s32 ready;

    p_wpd = _wpdcb[chan];
    interrupts = OSDisableInterrupts();
    result = p_wpd->status;
    ready = p_wpd->setup;

    if (result != WPAD_ERR_NO_CONTROLLER) {
        if (ready == 0) {
            result = WPAD_ERR_BUSY;
            goto finish;
        }

        if (_wmb[chan].used != 0)
            goto busy;

        missing = (p_wpd->gameInfoErr[0] == 0 || p_wpd->gameInfoErr[1] == 0);
        result = missing ? 0 : -6;
        if (result != 0)
            goto finish;

        if (memcmp(_gamecode, p_wpd->gameInfo.gameId, 4) == 0) {
            _wmb[chan].used = TRUE;
            _wmb[chan].callback = callback;
            OSRestoreInterrupts(interrupts);
            return WPADReadMemoryAsync(chan, buf, len, addr + 0x9A, ResultCallback);
        }

        result = WPAD_ERR_NOPERM;
        goto finish;

    busy:
        result = WPAD_ERR_BUSY;
    }

finish:
    OSRestoreInterrupts(interrupts);
    if (callback != 0)
        callback(chan, result);
    return result;
}

s32 WPADReadMemoryAsync(s32 chan, void* buf, u16 len, u32 addr, WPADCallback callback) {
    WPADControlBlock* p_wpd;
    s32 ready;
    s32 result;
    BOOL en;

    p_wpd = _wpdcb[chan];
    en = OSDisableInterrupts();
    result = p_wpd->status;
    ready = p_wpd->setup;
    OSRestoreInterrupts(en);
    if (result != WPAD_ERR_NO_CONTROLLER) {
        if (ready == 0) {
            result = WPAD_ERR_BUSY;
        } else {
            result = WPADiSendReadData(&p_wpd->cmdq, buf, len, addr, callback) ? WPAD_ERR_NONE : WPAD_ERR_BUSY;
        }
    }
    if (result != 0 && callback != 0) {
        callback(chan, result);
    }
    return result;
}

s32 fn_802252E8(s32 chan, void* buf, u16 len, u32 addr, WPADCallback callback) {
    WPADControlBlock* p_wpd;
    s32 ready;
    s32 result;
    BOOL en;

    p_wpd = _wpdcb[chan];
    en = OSDisableInterrupts();
    result = p_wpd->status;
    ready = p_wpd->setup;
    OSRestoreInterrupts(en);
    if (result != WPAD_ERR_NO_CONTROLLER) {
        if (ready == 0) {
            result = WPAD_ERR_BUSY;
        } else {
            result = WPADiSendWriteData(&p_wpd->cmdq, buf, len, addr, callback) ? WPAD_ERR_NONE : WPAD_ERR_BUSY;
        }
    }
    if (result != 0 && callback != 0) {
        callback(chan, result);
    }
    return result;
}

s32 fn_802253B4(s32 chan, void** out) {
    WPADControlBlock* p_wpd = _wpdcb[chan];
    BOOL interrupts = OSDisableInterrupts();
    s32 flag;
    s32 result;

    flag = 0;
    if (p_wpd->gameInfoErr[0] == 0 || p_wpd->gameInfoErr[1] == 0) {
        flag = 1;
    }
    result = flag ? 0 : -4;
    if (result == 0) {
        *out = (u8*)&p_wpd->gameInfo + 8;
    } else {
        *out = NULL;
    }
    OSRestoreInterrupts(interrupts);
    return result;
}

void fn_80225454(u16* title) {
    u16* dst = lbl_804F9EE0;
    int i;

    for (i = 0; i < 16; i++) {
        if ((*dst = *title) == 0)
            break;
        title++;
        dst++;
    }

    lbl_804F9EE0[16] = 0;
}
