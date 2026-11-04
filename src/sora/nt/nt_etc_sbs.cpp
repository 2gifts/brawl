#include <nt/etc/nt_etc_so.h>
#include <nt/nt_etc_sbs.h>
#include <revolution/DWC/dwc_nastime.h>
#include <revolution/OS/OSError.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/OS/OSTime.h>
#include <string.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049ECF8[0x10] __attribute__((aligned(8))) = { 0 };
OSMutex lbl_8049ED08 __attribute__((aligned(8)));

// HYPOTHESIS: leading .data block; nothing in these functions reads it, but it is the string-pool base.
char lbl_80461390[0x6C] = "SeBQcoBRMEYxbDhcvtAQ0001079d0000520f10000000069107d7smashbrosxwii";

extern s32 lbl_805A03EC;
extern s32 lbl_805A03F0;
extern s32 lbl_805A03F4;

extern "C" int SBS_Init(const char* gameName, int gameId, const char* secretKey, DWCAccUserData* userData, int param);
extern "C" void SBS_ShutdownAsync();
extern "C" int SBS_GetWatchingDataAsync(void* buf, int size, DWCDate* date, DWCTime* time);
extern "C" int SBS_GetWinCountAsync(void* out);
extern "C" void SBS_CancelRequest();
extern "C" int DWC_GetLastError(int* errorCode);
extern "C" s32 DWC_GetLastErrorEx(s32* errorCode, s32* errorType);
extern "C" void DWC_ClearError();

// HYPOTHESIS: scoped mutex guard, inlined at every lock
class nteSBSLock {
public:
    nteSBSLock(OSMutex* mutex) : m_mutex(mutex), m_isLocked(1) {
        if (m_isLocked) {
            OSLockMutex(m_mutex);
        }
    }

    ~nteSBSLock() {
        if (m_isLocked) {
            OSUnlockMutex(m_mutex);
        }
    }

private:
    OSMutex* m_mutex;
    u8 m_isLocked : 8;
};

nteSBS::nteSBS(u8 flag) : gfTask("nteSBS", Category_Network, 14, 13, true) {
    unk40 = 0;
    unk44 = 0;
    _fInitialized = false;
    unk49 = false;
    unk4C = nullptr;
    unk50 = nullptr;
    unk54 = 0;
    unk58 = 0;
    unk5C = flag;
    memset(unk60, 0, sizeof(unk60));
    unk2C_b1 = false;
}

bool nteSBS::startupSBS(DWCAccUserData* userData, const char* gameName, const char* secretKey, int param) {
    if (_fInitialized)
        return true;
    {
        nteSBSLock lock(&lbl_8049ED08);
        if (SBS_Init(gameName, 0x67A, secretKey, userData, param) != 0)
            return false;
        unk49 = true;
    }
    OSReport("\n");
    OSReport("startupSBS\n");
    OSReport("\n");
    _fInitialized = true;
    return true;
}

bool nteSBS::finishSBS() {
    OSReport("\n");
    OSReport("finishSBS Start\n");
    if (!_fInitialized) {
        OSReport("_fInitialized == false\n");
        return true;
    }
    if (isNowInProgress()) {
        OSReport("isNowInProgress == true\n");
        return false;
    }
    OSReport("finishSBS Call SBS_ShutdownAsync()\n");
    {
        nteSBSLock lock(&lbl_8049ED08);
        unk49 = false;
        SBS_ShutdownAsync();
    }
    OSReport("finishSBS End\n");
    OSReport("\n");
    if (isNowInProgress())
        return false;
    unk44 = 3;
    unk40 = 1;
    return true;
}

bool nteSBS::download(void* buf, int size, DWCDate* date, DWCTime* time) {
    if (!_fInitialized)
        return true;
    if (isNowInProgress())
        return false;
    {
        nteSBSLock lock(&lbl_8049ED08);
        if (SBS_GetWatchingDataAsync(buf, size, date, time) != 0) {
            s32 type;
            s32 err;
            s32 has = 0;
            if (unk5C != 0) {
                err = -lbl_805A03F0;
                type = lbl_805A03F4;
                has = lbl_805A03EC;
            }
            if (has == 0)
                has = DWC_GetLastErrorEx(&err, &type);
            if (has != 0) {
                if (unk50) {
                    if (err < 0)
                        err = -err;
                    unk50->showError(err, type);
                }
                if (unk5C == 0)
                    DWC_ClearError();
            }
            return false;
        }
    }
    if (isNowInProgress())
        return false;
    unk44 = 1;
    unk40 = 1;
    return true;
}

bool nteSBS::downloadWinCount(void* out) {
    if (!_fInitialized)
        return true;
    if (isNowInProgress())
        return false;
    {
        nteSBSLock lock(&lbl_8049ED08);
        if (SBS_GetWinCountAsync(out) != 0) {
            s32 type;
            s32 err;
            s32 has = 0;
            if (unk5C != 0) {
                err = -lbl_805A03F0;
                type = lbl_805A03F4;
                has = lbl_805A03EC;
            }
            if (has == 0)
                has = DWC_GetLastErrorEx(&err, &type);
            if (has != 0) {
                if (unk50) {
                    if (err < 0)
                        err = -err;
                    unk50->showError(err, type);
                }
                if (unk5C == 0)
                    DWC_ClearError();
            }
            return false;
        }
    }
    if (isNowInProgress())
        return false;
    unk44 = 4;
    unk40 = 1;
    return true;
}

void nteSBS::cancel() {
    OSReport("nteSBS::cancel\n");
    if (_fInitialized == true && unk49 == true && isNowInProgress()) {
        nteSBSLock lock(&lbl_8049ED08);
        OSReport("nteSBS::CancelRequest\n");
        SBS_CancelRequest();
    } else {
        OSReport("nteSBS::CancelRequest Skip!!!\n");
    }
}

int nteSBS::getErrorCode(u32 kind) {
    int result = 0;
    switch (kind) {
    case 2:
        DWC_GetLastError(&result);
        if (result < 0)
            result *= -1;
        break;
    case 4:
        result = 0x27D8;
        break;
    case 5:
        result = 0x27D9;
        break;
    case 6:
        result = 0x27DA;
        break;
    case 7:
        result = 0x27DE;
        break;
    case 8:
        result = 0x27DF;
        break;
    case 9:
        result = 0x27E0;
        break;
    case 10:
        result = 0x27E1;
        break;
    default:
        break;
    }
    return result;
}

s64 nteSBS::cnvTime(DWCDate* date, DWCTime* time) {
    OSCalendarTime cal;
    cal.sec = time->sec;
    cal.min = time->min;
    cal.hour = time->hour;
    cal.mday = date->mday;
    cal.month = date->month;
    cal.year = date->year;
    cal.wday = date->wday;
    cal.yday = date->yday;
    cal.msec = 0;
    cal.usec = 0;
    if (cal.year < 2000 || cal.month < 0 || cal.month > 11 || cal.mday < 1 || cal.mday > 31)
        return 0;
    return OSCalendarTimeToTicks(&cal);
}
