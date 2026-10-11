// nwc24 is one big blob in the dol (config, mail box, download list, time, etc all in one split),
// so everything lives in this one file for now. section banners mark where the original
// translation units seem to start. function order here follows my draft, not the binary.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>

// MATCH-ONLY: the original is a bunch of separate source files, here they share one TU so mwcc would
// happily inline calls that crossed file boundaries. auto_inline is off for the whole file and the
// helpers that really were inlined inside one original file are marked inline by hand. a few global
// functions were both emitted and inlined into a sibling, those get an inline helper + thin wrapper.
#pragma auto_inline off

/* ---------------------------------------------------------------------------------------------
 * std api (little libc replacements the mail code uses instead of msl)
 * ------------------------------------------------------------------------------------------- */

char *Mail_strcpy(char *dst, const char *src) {
    char *d = dst;

    while (*src != 0) {
        *d++ = *src++;
    }

    *d = 0;
    return dst;
}

int Mail_strlen(const char *str) {
    int n = 0;

    while (*str != 0) {
        n++;
        str++;
    }

    return n;
}

int STD_strnlen(const char *str, size_t maxlen) {
    size_t i;

    for (i = 0; i < maxlen; i++, str++) {
        if (*str == 0) {
            break;
        }
    }

    return i;
}

void *Mail_memcpy(void *dst, const void *src, size_t n) {
    size_t i;

    for (i = 0; i < n; i++) {
        ((u8 *)dst)[i] = ((const u8 *)src)[i];
    }

    return dst;
}

void *Mail_memset(void *dst, int ch, size_t n) {
    size_t i;

    for (i = 0; i < n; i++) {
        ((u8 *)dst)[i] = ch;
    }

    return dst;
}

char *Mail_strcat(char *dst, const char *src) {
    int n = 0;
    const char *p = dst;
    char *d;

    while (*p != 0) {
        p++;
        n++;
    }

    d = dst + n;

    while (*src != 0) {
        *d++ = *src++;
    }

    *d = 0;
    return dst;
}

char *Mail_strncat(char *dst, const char *src, size_t maxlen) {
    int len = 0;
    const char *p = dst;
    char *d;
    size_t i;
    char c;

    while (*p != 0) {
        p++;
        len++;
    }

    d = dst + len;
    i = 0;

    while (i < maxlen && (c = *src) != 0) {
        *d++ = c;
        src++;
        i++;
    }

    dst[i + len] = 0;
    return dst;
}

/* ---------------------------------------------------------------------------------------------
 * utils
 * ------------------------------------------------------------------------------------------- */

void NWC24Data_Init(NWC24Data *data) {
    data->pData = NULL;
    data->size = 0;
}

void NWC24Data_SetDataP(NWC24Data *data, const void *ptr, u32 size) {
    data->pData = ptr;
    data->size = size;
}

void NWC24Date_Init(NWC24Date *date) {
    date->year = 2000;
    date->month = 1;
    date->day = 1;
    date->hour = 12;
    date->min = 0;
    date->sec = 0;
    date->BYTE_0x7 = 0;
}

// writes addr as 16 decimal digits (despite the name), zero padded, plus a terminator
void NWC24iConvIdToStr(u64 addr, char *out) {
    u64 a = addr;
    int i;
    const int n = sizeof(u64) * 2;

    for (i = n - 1; i >= 0; i--) {
        out[i] = '0' + a % 10;
        a /= 10;
    }

    out[n] = '\0';
}

/* ---------------------------------------------------------------------------------------------
 * ipc helpers (/dev/net/kd/* resource managers)
 * ------------------------------------------------------------------------------------------- */

static BOOL NWC24iIsRequestPending = FALSE;

static s32 CallbackAsyncIpc(s32 result, void *arg);

NWC24Err NWC24iOpenResourceManager(const char *user, const char *name, s32 *fdOut, u32 mode) {
#pragma unused(user)
    NWC24Err result;

    if (fdOut == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    result = IOS_Open(name, mode);
    *fdOut = result;

    if (result < 0) {
        if (result == -6) {
            return NWC24_ERR_INPROGRESS;
        }

        if (result == -8) {
            return NWC24_ERR_BUSY;
        }

        return NWC24_ERR_INTERNAL_IPC;
    }

    return NWC24_OK;
}

NWC24Err NWC24iCloseResourceManager(const char *user, s32 fd) {
#pragma unused(user)
    return IOS_Close(fd) < 0 ? NWC24_ERR_INTERNAL_IPC : NWC24_OK;
}

NWC24Err NWC24iIoctlResourceManager(const char *user, s32 fd, s32 type, void *in, s32 inSize, void *out, s32 outSize) {
#pragma unused(user)
    return IOS_Ioctl(fd, type, in, inSize, out, outSize) < 0 ? NWC24_ERR_INTERNAL_IPC : NWC24_OK;
}

NWC24Err NWC24iIoctlResourceManagerAsync(const char *user, s32 fd, s32 type, void *in, s32 inSize, void *out,
                                         s32 outSize, void *callbackArg) {
#pragma unused(user)
    if (IOS_IoctlAsync(fd, type, in, inSize, out, outSize, CallbackAsyncIpc, callbackArg) < 0) {
        return NWC24_ERR_INTERNAL_IPC;
    }

    NWC24iIsRequestPending = TRUE;
    return NWC24_OK;
}

BOOL NWC24iIsAsyncRequestPending(void) {
    return NWC24iIsRequestPending;
}

static s32 CallbackAsyncIpc(s32 result, void *arg) {
    if (arg != NULL) {
        *(s32 *)arg = result;
    }

    NWC24iIsRequestPending = FALSE;
    return 0;
}

/* ---------------------------------------------------------------------------------------------
 * system (shutdown hook + led)
 * ------------------------------------------------------------------------------------------- */

#define SHUTDOWN_RETRY_MAX 5
#define NWC24_IOCTL_SHUTDOWN 40

static s32 nwc24ShtFd = -1;
static s32 nwc24ShtRetryRest = 0;
static OSShutdownFunctionInfo ShutdownFuncInfo;

static inline NWC24Err EnableLedNotification(BOOL enable) {
    SCIdleModeInfo idleMode;
    u32 status;
    u32 led = 0;

    if (enable) {
        do {
            status = SCCheckStatus();

            if (status == SC_STATUS_FATAL) {
                return NWC24_ERR_FATAL;
            }
        } while (status == SC_STATUS_BUSY);

        SCGetIdleMode(&idleMode);
        led = idleMode.led;
    }

    __OSSetIdleLEDMode(led);
    return NWC24_OK;
}

NWC24Err NWC24EnableLedNotification(BOOL enable) {
    return EnableLedNotification(enable);
}

// newer than the openrvl source: scheduler ioctl 0x22, only used by the shutdown prep when we are the menu
extern NWC24Err NWC24iSetScriptMode(u32 mode);

NWC24Err NWC24iPrepareShutdown(void) {
    NWC24Err result = NWC24_OK;

    NWC24iRegister();

    ShutdownFuncInfo.func = NWC24Shutdown;
    ShutdownFuncInfo.priority = 110;
    OSRegisterShutdownFunction(&ShutdownFuncInfo);

    if (nwc24ShtFd < 0) {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &nwc24ShtFd, 1);
    }

    nwc24ShtRetryRest = SHUTDOWN_RETRY_MAX;
    EnableLedNotification(TRUE);

    if (OSGetAppType() == 0x40) {
        NWC24iSetScriptMode(1);
    }

    return result;
}

static u8 shtBuffer[32] __attribute__((aligned(32)));
static u8 shtResult[32] __attribute__((aligned(32)));

static inline NWC24Err RequestShutdown(u32 event, NWC24Err *resultOut) {
    *(u32 *)shtBuffer = event;
    return NWC24iIoctlResourceManagerAsync("NWC24iRequestShutdown", nwc24ShtFd, NWC24_IOCTL_SHUTDOWN, shtBuffer,
                                           sizeof(shtBuffer), shtResult, sizeof(shtResult), resultOut);
}

NWC24Err NWC24iRequestShutdown(u32 event, NWC24Err *resultOut) {
    return RequestShutdown(event, resultOut);
}

BOOL NWC24Shutdown(BOOL final, u32 event) {
    static BOOL shuttingdown = FALSE;
    static NWC24Err result = NWC24_OK;

    if (final) {
        return TRUE;
    }

    if (shuttingdown) {
        if (NWC24iIsAsyncRequestPending()) {
            return FALSE;
        }

        if (result >= 0) {
            return TRUE;
        }

        if (nwc24ShtRetryRest > 0) {
            shuttingdown = FALSE;
            nwc24ShtRetryRest--;
        } else {
            return TRUE;
        }
    } else if (RequestShutdown(event, &result) >= 0) {
        shuttingdown = TRUE;
    }

    return FALSE;
}

/* ---------------------------------------------------------------------------------------------
 * config (/shared2/wc24/nwc24msg.cfg)
 * ------------------------------------------------------------------------------------------- */

#define CONFIG_MAGIC 'WcCf'
#define CONFIG_VERSION 8
#define CONFIG_MAX_ID_COUNT 32
#define COMPANY_CODE_NINTENDO 1

// HYPOTHESIS: 0x800031C0 is a low-mem cache of the wc24 user id (8 bytes, flushed with DCStoreRange)
#define WC24_USERID_CACHE ((NWC24UserId *)0x800031C0)

static NWC24iConfig *config = NULL;
static BOOL ConfigModified = FALSE;

static const char *MBoxDir = "/shared2/wc24/mbox";
static const char *ConfigFile = "/shared2/wc24/nwc24msg.cfg";
static const char *CfgBakFile = "/shared2/wc24/nwc24msg.cbk";
// HYPOTHESIS: default group id, the sdk reads it from sdata instead of using a literal
static u16 DefaultGroupId = COMPANY_CODE_NINTENDO;

static u32 GetConfigCheckSum(void);
static NWC24Err CheckConfig(void);
static inline NWC24Err GenerateUserId(NWC24UserId *idOut);
static inline NWC24Err GenerateNewUserId_i(NWC24UserId *idOut);

// HYPOTHESIS: checks the crc on a user id (returns 0 when good). body not drafted.
extern NWC24Err NWC24iCheckUserIdCrc(NWC24UserId id);

NWC24Err NWC24GetMyUserId(NWC24UserId *idOut) {
    NWC24Err result = NWC24_OK;
    NWC24Err scdErr;

    if (NWC24IsMsgLibOpened() || NWC24IsMsgLibOpenedByTool()) {
        *idOut = config->userId;
    } else {
        // not open, so try the cached copy first
        *idOut = *WC24_USERID_CACHE;
        if (*idOut == 0) {
            result = NWC24_ERR_NULL;
        } else {
            result = NWC24iCheckUserIdCrc(*idOut);
        }

        if (result == NWC24_OK) {
            return result;
        }

        scdErr = NWC24SuspendScheduler();
        if (scdErr < 0) {
            return scdErr;
        }

        result = GenerateNewUserId_i(idOut);
        if (result == NWC24_ERR_ID_GENERATED || result == NWC24_ERR_ID_REGISTERED) {
            result = NWC24_OK;
        }

        scdErr = NWC24ResumeScheduler();
        if (scdErr < 0) {
            result = result != NWC24_OK ? result : scdErr;
        }
    }

    return result;
}

NWC24Err NWC24GenerateNewUserId(NWC24UserId *idOut) {
    return GenerateNewUserId_i(idOut);
}

NWC24Err NWC24iConfigOpen(void) {
    NWC24Err result;

    config = (NWC24iConfig *)NWC24WorkP->config;
    ConfigModified = FALSE;

    result = NWC24iConfigReload();
    if (result != NWC24_ERR_FILE_NOEXISTS) {
        if (result == NWC24_OK) {
            *WC24_USERID_CACHE = config->userId;
            DCStoreRange(WC24_USERID_CACHE, 0x20);
        }
    }

    return result;
}

NWC24Err NWC24iConfigReload(void) {
    NWC24Err result;
    NWC24Err close;
    NWC24File file;

    if (NWC24WorkP == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = NWC24FOpen(&file, ConfigFile, NWC24_OPEN_NAND_R);

    if (result == NWC24_OK) {
        result = NWC24FRead(config, sizeof(NWC24iConfig), &file);
        close = NWC24FClose(&file);
        result = result != NWC24_OK ? result : close;
    }

    if (result == NWC24_OK) {
        result = CheckConfig();

        if (result == NWC24_OK) {
            ConfigModified = FALSE;
            return result;
        }
    }

    // main copy was bad, fall back to the backup
    result = NWC24FOpen(&file, CfgBakFile, NWC24_OPEN_NAND_R);

    if (result == NWC24_OK) {
        result = NWC24FRead(config, sizeof(NWC24iConfig), &file);
        close = NWC24FClose(&file);
        result = result != NWC24_OK ? result : close;
    }

    if (result == NWC24_OK) {
        result = CheckConfig();

        if (result == NWC24_OK) {
            ConfigModified = TRUE;
        }
    }

    return result;
}

NWC24Err NWC24iConfigFlush(void) {
    NWC24Err result;
    NWC24Err close;
    NWC24File file;

    if (NWC24WorkP == NULL) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (!ConfigModified) {
        return NWC24_OK;
    }

    config->checksum = GetConfigCheckSum();

    result = NWC24FOpen(&file, ConfigFile, NWC24_OPEN_NAND_W);

    if (result == NWC24_OK) {
        result = NWC24FWrite(config, sizeof(NWC24iConfig), &file);
        close = NWC24FClose(&file);
        result = result != NWC24_OK ? result : close;

        ConfigModified = FALSE;
    }

    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, CfgBakFile, NWC24_OPEN_NAND_W);

    if (result == NWC24_OK) {
        result = NWC24FWrite(config, sizeof(NWC24iConfig), &file);
        close = NWC24FClose(&file);
        result = result != NWC24_OK ? result : close;

        ConfigModified = FALSE;
    }

    return result;
}

const char *NWC24GetAccountDomain(void) {
    return config->acctDomain;
}

const char *NWC24GetMBoxDir(void) {
    OSIOSRev rev;
    __OSGetIOSRev(&rev);

    // HYPOTHESIS: old IOS (< 0.7.6 or so) can't do /shared2, so the sdk just gives up
    if (rev.major == 0 && ((rev.minor == 7 && rev.micro == 0) || rev.minor < 7)) {
        OSPanic("NWC24Config.c", 695, "stopped.");
    }

    return MBoxDir;
}

u32 NWC24GetAppId(void) {
    u32 appId = *(u32 *)OSGetAppGamename();

    if (appId == 0 && OSGetAppType() == 0) {
        appId = *(u32 *)DVDGetCurrentDiskID()->gameName;
    }

    return appId;
}

u16 NWC24GetGroupId(void) {
    NANDStatus stat;
    u16 groupId;
    char *dir;

    groupId = DefaultGroupId;

    switch (OSGetAppType()) {
    case 0x40:
        groupId = COMPANY_CODE_NINTENDO;
        break;
    case 0x80:
        groupId = *(u16 *)DVDGetCurrentDiskID()->company;
        break;
    case 0x81:
        dir = NWC24WorkP->pathWork;
        if (NANDGetHomeDir(dir) == NAND_RESULT_OK) {
            if (NANDGetStatus(dir, &stat) == NAND_RESULT_OK) {
                groupId = stat.groupId;
            }
        }
        break;
    }

    return groupId;
}

static inline u32 GetConfigCheckSum(void) {
    u32 i;
    u32 csum;
    u32 *ptr;

    csum = 0;
    ptr = (u32 *)config;

    // last word is the checksum itself, so skip it
    for (i = 0; i < sizeof(NWC24iConfig) / sizeof(u32) - 1; i++) {
        csum += *ptr++;
    }

    return csum;
}

static NWC24Err CheckConfig(void) {
    if (config->magic != CONFIG_MAGIC) {
        return NWC24_ERR_BROKEN;
    }

    if (config->checksum != GetConfigCheckSum()) {
        return NWC24_ERR_BROKEN;
    }

    if (config->createCount >= CONFIG_MAX_ID_COUNT) {
        return NWC24_ERR_BROKEN;
    }

    if (config->version != CONFIG_VERSION) {
        return NWC24_ERR_VER_MISMATCH;
    }

    return NWC24_OK;
}

static inline NWC24Err GenerateUserId(NWC24UserId *idOut) {
    NWC24Err result;
    u32 sp08 = 0;

    if (idOut == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    *idOut = 9999999999999999ULL;
    result = NWC24iRequestGenerateUserId(idOut, &sp08);

    // keep the low-mem copy in sync with whatever the scheduler handed back
    *WC24_USERID_CACHE = *idOut;
    DCStoreRange(WC24_USERID_CACHE, 0x20);
    return result;
}

static inline NWC24Err GenerateNewUserId_i(NWC24UserId *idOut) {
    NWC24Err open;
    NWC24Err close;

    open = NWC24BlockOpenMsgLib(TRUE);
    if (open < 0) {
        return open;
    }

    open = GenerateUserId(idOut);
    close = NWC24BlockOpenMsgLib(FALSE);

    return open >= 0 ? close : open;
}

/* ---------------------------------------------------------------------------------------------
 * time (/dev/net/kd/time, rtc counter bookkeeping)
 * ------------------------------------------------------------------------------------------- */

#define NWC24_IOCTL_SET_RTC_COUNTER 23
#define NWC24_IOCTL_GET_TIME_DIFFERENCE 24

typedef struct TimeCommonBuffer {
    u32 rtc;
    u32 flags;
} TimeCommonBuffer;

#pragma pack(push, 1)
typedef struct TimeCommonResult {
    s32 result;
    s64 diff;
} TimeCommonResult;
#pragma pack(pop)

#define TIME_IOCTL_IN ((TimeCommonBuffer *)nwc24TimeCommonBuffer)
#define TIME_IOCTL_OUT ((TimeCommonResult *)nwc24TimeCommonResult)

static BOOL nwc24TimeInitialized = FALSE;
static u32 nwc24TimeRtc = 0;
static s64 nwc24TimeDifference = 0;

static u8 nwc24TimeCommonResult[32] __attribute__((aligned(32)));
static u8 nwc24TimeCommonBuffer[32] __attribute__((aligned(32)));
static OSMutex nwc24TimeCommandMutex __attribute__((aligned(32)));

static NWC24Err GetRTC(u32 *rtc);
static NWC24Err TimeCheckCallingStatus(const char *user);
static void TimeInitMutex(void);
static void TimeLockRight(void);
static void TimeUnlockRight(void);

// HYPOTHESIS: a 128 byte scratch buffer that nothing in this unit touches. it is only here
// because the .bss layout wants it (some string/time helper in the original got dropped).
static u8 nwc24TimeUnusedBuf[128];
#pragma push
#pragma force_active on
static void *nwc24TimeUnusedRef = nwc24TimeUnusedBuf;
#pragma pop

NWC24Err NWC24iGetUniversalTime(s64 *timeOut) {
    static s64 whenCached = 0;

    NWC24Err result;

    if (timeOut == NULL) {
        nwc24TimeDifference = 0;
        return NWC24_ERR_INVALID_VALUE;
    }

    if (whenCached == 0 || whenCached + OS_TIMER_CLOCK <= __OSGetSystemTime() || nwc24TimeRtc == 0) {
        result = NWC24iGetTimeDifference(&nwc24TimeDifference);
        if (result < 0) {
            return result;
        }

        result = GetRTC(&nwc24TimeRtc);
        if (result != NWC24_OK) {
            return result;
        }

        whenCached = __OSGetSystemTime();
    }

    *timeOut = nwc24TimeDifference + nwc24TimeRtc;
    return NWC24_OK;
}

NWC24Err NWC24iGetTimeDifference(s64 *diffOut) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = TimeCheckCallingStatus(__FUNCTION__);
    if (result < 0) {
        return result;
    }

    TimeLockRight();
    {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/time", &fd, 0);

        if (result >= 0) {
            result = NWC24_IOCTL_DEVICE(fd, NWC24_IOCTL_GET_TIME_DIFFERENCE, NULL, 0, &nwc24TimeCommonResult,
                                        sizeof(nwc24TimeCommonResult));

            if (result >= 0) {
                result = TIME_IOCTL_OUT->result;

                if (result == 0 && diffOut != NULL) {
                    *diffOut = TIME_IOCTL_OUT->diff;
                }
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    TimeUnlockRight();

    return result;
}

NWC24Err NWC24iSetRtcCounter(u32 rtc, u32 flags) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = TimeCheckCallingStatus(__FUNCTION__);
    if (result < 0) {
        return result;
    }

    TimeLockRight();
    {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/time", &fd, 0);

        if (result >= 0) {
            TIME_IOCTL_IN->rtc = rtc;
            TIME_IOCTL_IN->flags = flags;

            result = NWC24_IOCTL_DEVICE(fd, NWC24_IOCTL_SET_RTC_COUNTER, &nwc24TimeCommonBuffer,
                                        sizeof(nwc24TimeCommonBuffer), &nwc24TimeCommonResult,
                                        sizeof(nwc24TimeCommonResult));

            if (result >= 0) {
                result = TIME_IOCTL_OUT->result;
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    TimeUnlockRight();

    return result;
}

NWC24Err NWC24iSynchronizeRtcCounter(BOOL forceSave) {
    u32 rtc;
    NWC24Err result;

    result = GetRTC(&rtc);
    if (result != NWC24_OK) {
        return result;
    }

    return NWC24iSetRtcCounter(rtc, forceSave ? 1 : 0);
}

static inline NWC24Err GetRTC(u32 *rtc) {
    u32 status;

    do {
        status = SCCheckStatus();

        if (status == SC_STATUS_FATAL) {
            return NWC24_ERR_FATAL;
        }
    } while (status != SC_STATUS_OK);

    *rtc = OSTicksToSeconds(OSGetTime()) - SCGetCounterBias();
    return NWC24_OK;
}

static inline NWC24Err TimeCheckCallingStatus(const char *user) {
#pragma unused(user)
    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    return NWC24_OK;
}

static inline void TimeInitMutex(void) {
    BOOL enabled = OSDisableInterrupts();

    if (!nwc24TimeInitialized) {
        OSInitMutex(&nwc24TimeCommandMutex);

        memset(&nwc24TimeCommonBuffer, 0, sizeof(nwc24TimeCommonBuffer));
        memset(&nwc24TimeCommonResult, 0, sizeof(nwc24TimeCommonResult));

        nwc24TimeInitialized = TRUE;
    }

    OSRestoreInterrupts(enabled);
}

static inline void TimeLockRight(void) {
    if (!nwc24TimeInitialized) {
        TimeInitMutex();
    }

    OSLockMutex(&nwc24TimeCommandMutex);
}

static inline void TimeUnlockRight(void) {
    OSUnlockMutex(&nwc24TimeCommandMutex);
}

/* ---------------------------------------------------------------------------------------------
 * manage (open/close the lib, version registration, mail-arrived flags)
 * ------------------------------------------------------------------------------------------- */

#define MANAGE_ERROR_CODE_BASE 109000

enum {
    NWC24_LIB_CLOSED,
    NWC24_LIB_OPENED,
    NWC24_LIB_OPENED_BY_TOOL,
    NWC24_LIB_BLOCKED
};

enum {
    NWC24_FAIL_SFL = (1 << 0),
    NWC24_FAIL_DL_TASK = (1 << 1),
    NWC24_FAIL_FATAL = (1 << 2)
};

const char *__NWC24Version = "<< RVL_SDK - NWC24 \trelease build: Jun 28 2007 18:29:32 (0x4199_60831) >>";

NWC24iWork *NWC24WorkP = NULL;

static int Opened = NWC24_LIB_CLOSED;
static u32 YouGotMail = 0;
static u32 GlobalErrorCode = 0;
static BOOL Registered = FALSE;

static inline void Register_i(void);
static inline BOOL IsOpened_i(void);
static inline BOOL IsBlocking_i(void);
static inline void SetErrorCode_i(u32 code);
static NWC24Err NWC24OpenLibInternal(NWC24iWork *work, int state);

// the small manage helpers get inlined into NWC24OpenLibInternal, but other files call the real ones
static inline void Register_i(void) {
    if (Registered) {
        return;
    }

    OSRegisterVersion(__NWC24Version);
    Registered = TRUE;
}

void NWC24iRegister(void) {
    Register_i();
}

NWC24Err NWC24OpenLib(void *work) {
    if (Opened == NWC24_LIB_OPENED_BY_TOOL) {
        return NWC24_ERR_BUSY;
    }

    return NWC24OpenLibInternal((NWC24iWork *)work, NWC24_LIB_OPENED);
}

static NWC24Err NWC24OpenLibInternal(NWC24iWork *work, int state) {
    NWC24Err result;
    NWC24Err failErr;
    u32 failFlag;

    SetErrorCode_i(NWC24_OK);

    if (!VFIsAvailable()) {
        return NWC24_ERR_FATAL;
    }

    if (IsOpened_i()) {
        return NWC24_ERR_LIB_OPENED;
    }

    if (IsBlocking_i()) {
        return NWC24_ERR_BUSY;
    }

    if (work == NULL) {
        return NWC24_ERR_NULL;
    }

    if ((u32)work % 32 != 0) {
        return NWC24_ERR_ALIGNMENT;
    }

    result = NWC24iTrySuspendForOpenLib();
    if (result == NWC24_OK) {
        Register_i();

        YouGotMail &= ~(1 << NWC24_MSGTYPE_RVL_MENU_SHARED);
        NWC24WorkP = work;

        NWC24InitBase64Table(NWC24WorkP->base64Work);

        failFlag = 0;
        failErr = NWC24_OK;

        result = NWC24iConfigOpen();
        if (result != NWC24_OK) {
            failErr = result;
            failFlag |= NWC24_FAIL_FATAL;
        }

        result = NWC24iOpenMBox();
        if (result != NWC24_OK) {
            failErr = result;
            failFlag |= NWC24_FAIL_FATAL;
        }

        result = NWC24iOpenFriendList();
        if (result != NWC24_OK) {
            failErr = result;
            failFlag |= NWC24_FAIL_FATAL;
        }

        result = NWC24iOpenSecretFriendList();
        if (result != NWC24_OK) {
            failErr = result;

            if (result == NWC24_ERR_FILE_NOEXISTS) {
                failFlag |= NWC24_FAIL_FATAL;
            } else {
                failFlag |= NWC24_FAIL_SFL;
            }
        }

        result = NWC24iOpenDlTaskList();
        if (result < 0) {
            failErr = result;

            if (result == NWC24_ERR_FILE_NOEXISTS) {
                failFlag |= NWC24_FAIL_FATAL;
            } else {
                failFlag |= NWC24_FAIL_DL_TASK;
            }
        }

        if (failFlag == (NWC24_FAIL_SFL | NWC24_FAIL_DL_TASK)) {
            failErr = NWC24_ERR_OLD_SYSTEM;
        }

        if (failFlag != 0) {
            NWC24WorkP = NULL;
            NWC24iResumeForCloseLib();
            result = failErr;
        } else {
            Opened = state;
            return NWC24_OK;
        }
    }

    switch (result) {
    case NWC24_ERR_OLD_SYSTEM:
    case NWC24_ERR_FILE_BROKEN:
    case NWC24_ERR_INTERNAL_VF:
    case NWC24_ERR_INTERNAL_IPC:
    case NWC24_ERR_NAND_CORRUPT:
    case NWC24_ERR_INPROGRESS:
    case NWC24_ERR_BUSY:
    case NWC24_ERR_MUTEX:
    case NWC24_ERR_FILE_OTHER:
    case NWC24_ERR_FILE_NOEXISTS:
    case NWC24_ERR_FILE_WRITE:
    case NWC24_ERR_FILE_READ:
    case NWC24_ERR_FILE_CLOSE:
    case NWC24_ERR_FILE_OPEN:
    case NWC24_ERR_BROKEN:
    case NWC24_ERR_FATAL:
        SetErrorCode_i(result - MANAGE_ERROR_CODE_BASE);
        break;
    }

    return result;
}

NWC24Err NWC24CloseLib(void) {
    s32 result;

    if (Opened != NWC24_LIB_OPENED) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = NWC24iConfigFlush();
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24iCloseDlTaskList();
    if (result < 0) {
        return result;
    }

    result = NWC24iResumeForCloseLib();
    if (result != NWC24_OK) {
        return result;
    }

    NWC24WorkP = NULL;
    Opened = NWC24_LIB_CLOSED;
    return result;
}

static inline BOOL IsOpened_i(void) {
    return Opened == NWC24_LIB_OPENED;
}

BOOL NWC24IsMsgLibOpened(void) {
    return IsOpened_i();
}

BOOL NWC24IsMsgLibOpenedByTool(void) {
    return Opened == NWC24_LIB_OPENED_BY_TOOL;
}

static inline BOOL IsBlocking_i(void) {
    return Opened == NWC24_LIB_BLOCKED;
}

BOOL NWC24IsMsgLibOpenBlocking(void) {
    return IsBlocking_i();
}

NWC24Err NWC24BlockOpenMsgLib(BOOL block) {
    s32 result;
    BOOL enabled;

    result = NWC24_OK;
    enabled = OSDisableInterrupts();

    if (block) {
        if (Opened == NWC24_LIB_CLOSED) {
            Opened = NWC24_LIB_BLOCKED;
        } else if (Opened == NWC24_LIB_OPENED) {
            result = NWC24_ERR_LIB_OPENED;
        } else {
            result = NWC24_ERR_BUSY;
        }
    } else {
        if (Opened == NWC24_LIB_BLOCKED) {
            Opened = NWC24_LIB_CLOSED;
        } else {
            result = NWC24_ERR_LIB_NOT_OPENED;
        }
    }

    OSRestoreInterrupts(enabled);
    return result;
}

NWC24Err NWC24iSetNewMsgArrived(u32 flags) {
    YouGotMail |= flags;
    return NWC24_OK;
}

s32 NWC24GetErrorCode(void) {
    return GlobalErrorCode;
}

static inline void SetErrorCode_i(u32 code) {
    GlobalErrorCode = code;
}

void NWC24iSetErrorCode(u32 code) {
    SetErrorCode_i(code);
}

/* ---------------------------------------------------------------------------------------------
 * friend lists
 * ------------------------------------------------------------------------------------------- */

#define FRIEND_LIST_MAGIC 'WcFl'
#define FRIEND_LIST_VERSION 2

static const char *FLFilePath = "/shared2/wc24/nwc24fl.bin";

static NWC24Err GetCachedFLHeader(NWC24iFLHeader **header);

NWC24Err NWC24iOpenFriendList(void) {
    NWC24iFLHeader *header = (NWC24iFLHeader *)NWC24WorkP->flHeader;
    Mail_memset(header, 0, sizeof(NWC24iFLHeader));
    return GetCachedFLHeader(&header);
}

static NWC24Err GetCachedFLHeader(NWC24iFLHeader **header) {
    NWC24File file;
    NWC24Err result;
    NWC24Err read;
    NWC24Err close;

    *header = (NWC24iFLHeader *)NWC24WorkP->flHeader;

    if ((*header)->magic != FRIEND_LIST_MAGIC) {
        result = NWC24FOpen(&file, FLFilePath, NWC24_OPEN_NAND_R);
        if (result != NWC24_OK) {
            return result;
        }

        NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
        read = NWC24FRead(*header, sizeof(NWC24iFLHeader), &file);
        close = NWC24FClose(&file);

        if (read != NWC24_OK) {
            result = read;
        } else {
            result = close;
        }

        if (result != NWC24_OK) {
            return result;
        }

        if ((*header)->magic != FRIEND_LIST_MAGIC) {
            return NWC24_ERR_BROKEN;
        }

        if ((*header)->version != FRIEND_LIST_VERSION) {
            return NWC24_ERR_VER_MISMATCH;
        }
    }

    return NWC24_OK;
}

#define SECRET_FRIEND_LIST_MAGIC 'WcFs'
#define SECRET_FRIEND_LIST_VERSION 2

static const char *SFLFilePath = "/shared2/wc24/nwc24fls.bin";

static NWC24Err GetCachedSecretFLHeader(NWC24iSecretFLHeader **header);

NWC24Err NWC24iOpenSecretFriendList(void) {
    NWC24iSecretFLHeader *header = (NWC24iSecretFLHeader *)NWC24WorkP->secretFlHeader;
    Mail_memset(header, 0, sizeof(NWC24iSecretFLHeader));
    return GetCachedSecretFLHeader(&header);
}

static NWC24Err GetCachedSecretFLHeader(NWC24iSecretFLHeader **header) {
    NWC24File file;
    NWC24Err result;
    NWC24Err read;
    NWC24Err close;

    *header = (NWC24iSecretFLHeader *)NWC24WorkP->secretFlHeader;

    if ((*header)->magic != SECRET_FRIEND_LIST_MAGIC) {
        result = NWC24FOpen(&file, SFLFilePath, NWC24_OPEN_NAND_R);
        if (result != NWC24_OK) {
            return result;
        }

        NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
        read = NWC24FRead(*header, sizeof(NWC24iSecretFLHeader), &file);
        close = NWC24FClose(&file);

        if (read != NWC24_OK) {
            result = read;
        } else {
            result = close;
        }

        if (result != NWC24_OK) {
            return result;
        }

        if ((*header)->magic != SECRET_FRIEND_LIST_MAGIC) {
            return NWC24_ERR_BROKEN;
        }

        if ((*header)->version != SECRET_FRIEND_LIST_VERSION) {
            return NWC24_ERR_VER_MISMATCH;
        }
    }

    return NWC24_OK;
}

/* ---------------------------------------------------------------------------------------------
 * file api (nand + vf wrappers, buffered read/write)
 * ------------------------------------------------------------------------------------------- */

#define WC24_DRIVE "@24"
#define NAND_RETRY_COUNT 3
#define NWC24_WR_BUFFER_SIZE 2048

// MSB gets set after a buffered read so the next read needs a fresh seek first
#define INVALIDATE_ALIGN 0x80000000

#define SLEEP_MSEC(x) OSSleepTicks(OSMillisecondsToTicks((s64)(x)))

extern s32 NANDSeek(NANDFileInfo *info, s32 offset, s32 whence);

static u32 RdBufferMutex = 0;
static u32 WrBufferMutex = 0;
static u32 InstanceIdGen = 0;

static inline NWC24Err CloseVf(NWC24File *file);
static NWC24Err BufferedWrite(const void *src, s32 size, NWC24File *file);
static NWC24Err BufferedWriteFlush(NWC24File *file);
static NWC24Err BufferedRead(void *dst, s32 size, NWC24File *file);
static NWC24Err AlignedSeek(NWC24File *file, s32 offset, NWC24SeekMode whence);
static NWC24Err ConvertError(s32 nanderr, NWC24Err wc24err);
static NWC24Err ConvertVfError(s32 vferr, NWC24Err wc24err);

NWC24Err NWC24FOpen(NWC24File *file, const char *path, u32 mode) {
    InstanceIdGen++;
    InstanceIdGen |= 0x80000000;

    file->id = InstanceIdGen;
    file->align = 0;
    file->mode = mode;

    if (mode == NWC24_OPEN_NAND_WBUFF || mode == NWC24_OPEN_NAND_RBUFF || mode == NWC24_OPEN_VF_WBUFF ||
        mode == NWC24_OPEN_VF_RBUFF) {
        if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
            return NWC24_ERR_LIB_NOT_OPENED;
        }
    }

    if (mode == NWC24_OPEN_NAND_WBUFF || mode == NWC24_OPEN_VF_WBUFF) {
        if (WrBufferMutex != 0) {
            return NWC24_ERR_MUTEX;
        }

        WrBufferMutex = file->id;
    }

    if (mode & NWC24_OPEN_VF) {
        return NWC24iFOpenVF(file, path, mode);
    }

    return NWC24iFOpenNand(file, path, mode);
}

NWC24Err NWC24iFOpenNand(NWC24File *file, const char *path, u32 mode) {
    s32 result;
    NANDAccessType access;
    u32 i;

    switch (mode) {
    case NWC24_OPEN_NAND_W:
    case NWC24_OPEN_NAND_WBUFF:
        result = NANDPrivateCreate(path, NAND_PERM_RWALL, 0);
        if (result != NAND_RESULT_OK && result != NAND_RESULT_EXISTS) {
            return NWC24_ERR_FILE_OTHER;
        }
        access = NAND_ACCESS_WRITE;
        break;
    case NWC24_OPEN_NAND_R:
    case NWC24_OPEN_NAND_RBUFF:
        access = NAND_ACCESS_READ;
        break;
    case NWC24_OPEN_NAND_RW:
        access = NAND_ACCESS_RW;
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDPrivateOpen(path, &file->nandf, access);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result != NAND_RESULT_OK) {
        if (mode == NWC24_OPEN_NAND_WBUFF) {
            WrBufferMutex = 0;
        }

        if (result == NAND_RESULT_NOEXISTS) {
            return NWC24_ERR_FILE_NOEXISTS;
        }

        return NWC24_ERR_FILE_OPEN;
    }

    return NWC24_OK;
}

NWC24Err NWC24iFOpenVF(NWC24File *file, const char *path, u32 mode) {
    s32 result;
    const char *access;

    switch (mode) {
    case NWC24_OPEN_VF_W:
    case NWC24_OPEN_VF_WBUFF:
        access = "w";
        break;
    case NWC24_OPEN_VF_R:
    case NWC24_OPEN_VF_RBUFF:
        access = "r";
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    file->vff = VFOpenFile(path, access, 0);

    if (file->vff == NULL) {
        result = VFGetLastError();

        if (mode == NWC24_OPEN_VF_WBUFF) {
            WrBufferMutex = 0;
        }

        if (*access == 'r' && result == VF_ERROR_0002) {
            return NWC24_ERR_FILE_NOEXISTS;
        }

        return ConvertVfError(result, NWC24_ERR_FILE_OPEN);
    }

    return NWC24_OK;
}

NWC24Err NWC24FClose(NWC24File *file) {
    NWC24Err close = NWC24_OK;
    NWC24Err read = NWC24_OK;

    if (file->mode == NWC24_OPEN_NAND_WBUFF || file->mode == NWC24_OPEN_VF_WBUFF) {
        read = BufferedWriteFlush(file);
        WrBufferMutex = 0;
    }

    if (file->mode & NWC24_OPEN_VF) {
        close = CloseVf(file);
    } else {
        close = NWC24iFCloseNand(file);
    }

    return read != NWC24_OK ? read : close;
}

NWC24Err NWC24iFCloseNand(NWC24File *file) {
    s32 result;
    NWC24Err err;
    u32 i;

    err = NWC24_OK;

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDClose(&file->nandf);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result == NAND_RESULT_CORRUPT) {
        err = NWC24_ERR_NAND_CORRUPT;
    } else if (result != NAND_RESULT_OK) {
        err = NWC24_ERR_FILE_CLOSE;
    }

    return err;
}

// the body is also inlined into NWC24FClose (same original file), so it lives in an inline helper
static inline NWC24Err CloseVf(NWC24File *file) {
    s32 result;

    result = VFCloseFile(file->vff);
    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_FILE_CLOSE);
    }

    return NWC24_OK;
}

NWC24Err NWC24iFCloseVF(NWC24File *file) {
    return CloseVf(file);
}

NWC24Err NWC24FSeek(NWC24File *file, s32 offset, NWC24SeekMode whence) {
    s32 result;
    u32 i;

    if (file->mode == NWC24_OPEN_NAND_RBUFF || file->mode == NWC24_OPEN_VF_RBUFF) {
        return AlignedSeek(file, offset, whence);
    }

    if (file->mode & NWC24_OPEN_VF) {
        result = VFSeekFile(file->vff, offset, whence);
        if (result != VF_OK) {
            return ConvertVfError(result, NWC24_ERR_FILE_OTHER);
        }

        return NWC24_OK;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDSeek(&file->nandf, offset, whence);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result < 0) {
        return NWC24_ERR_FILE_OTHER;
    }

    return NWC24_OK;
}

NWC24Err NWC24FRead(void *dst, s32 size, NWC24File *file) {
    s32 result;
    u32 i;

    switch (file->mode) {
    case NWC24_OPEN_NAND_W:
    case NWC24_OPEN_NAND_WBUFF:
    case NWC24_OPEN_VF_W:
        return NWC24_ERR_PROTECTED;
    case NWC24_OPEN_NAND_RBUFF:
    case NWC24_OPEN_VF_RBUFF:
        return BufferedRead(dst, size, file);
    }

    if (file->mode & NWC24_OPEN_VF) {
        result = VFReadFile(file->vff, dst, size, NULL);
        if (result != VF_OK) {
            return ConvertVfError(result, NWC24_ERR_FILE_READ);
        }

        return NWC24_OK;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDRead(&file->nandf, dst, size);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result == NAND_RESULT_CORRUPT) {
        return NWC24_ERR_NAND_CORRUPT;
    }

    if (result < 0) {
        return ConvertError(result, NWC24_ERR_FILE_READ);
    }

    return NWC24_OK;
}

NWC24Err NWC24FWrite(const void *src, s32 size, NWC24File *file) {
    s32 result;
    u32 i;

    switch (file->mode) {
    case NWC24_OPEN_NAND_WBUFF:
    case NWC24_OPEN_VF_WBUFF:
        return BufferedWrite(src, size, file);
    // HYPOTHESIS: NWC24_OPEN_VF_RBUFF is missing from this list on purpose-ish (looks like an sdk oversight)
    case NWC24_OPEN_NAND_R:
    case NWC24_OPEN_NAND_RBUFF:
    case NWC24_OPEN_VF_R:
        return NWC24_ERR_PROTECTED;
    }

    if (file->mode & NWC24_OPEN_VF) {
        result = VFWriteFile(file->vff, (void *)src, size);
        if (result != VF_OK) {
            return ConvertVfError(result, NWC24_ERR_FILE_WRITE);
        }

        return NWC24_OK;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDWrite(&file->nandf, src, size);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result == NAND_RESULT_CORRUPT) {
        return NWC24_ERR_NAND_CORRUPT;
    }

    if (result < 0) {
        return ConvertError(result, NWC24_ERR_FILE_WRITE);
    }

    if (result != size) {
        return NWC24_ERR_FILE_WRITE;
    }

    return NWC24_OK;
}

NWC24Err NWC24FGetLength(NWC24File *file, u32 *lengthOut) {
    s32 error;

    if (file->mode & NWC24_OPEN_VF) {
        error = VFGetFileSizeByFd(file->vff);
        if (error < 0) {
            return ConvertVfError(error, NWC24_ERR_FILE_OTHER);
        }

        *lengthOut = error;
        return NWC24_OK;
    }

    error = NANDGetLength(&file->nandf, lengthOut);
    if (error != NAND_RESULT_OK) {
        return NWC24_ERR_FILE_OTHER;
    }

    return NWC24_OK;
}

NWC24Err NWC24FDeleteVF(const char *path) {
    s32 result;

    result = VFDeleteFile(path);
    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_FILE_OTHER);
    }

    return NWC24_OK;
}

NWC24Err NWC24MountVF(const char *drive, const char *filename) {
    s32 result;

    result = VFMountDriveNANDFlashEx(drive, filename);
    if (result == VF_ERROR_B001) {
        return NWC24_ERR_FILE_NOEXISTS;
    }

    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_INTERNAL_VF);
    }

    result = VFSetSyncMode(drive, 1);
    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_INTERNAL_VF);
    }

    return NWC24_OK;
}

NWC24Err NWC24UnmountVF(const char *drive) {
    s32 result;

    result = VFUnmountDrive(drive);
    if (result != VF_OK) {
        return ConvertVfError(result, NWC24_ERR_INTERNAL_VF);
    }

    return NWC24_OK;
}

NWC24Err NWC24CheckSizeVF(const char *drive, u32 *sizeOut) {
    s32 size;

    size = VFGetDriveFreeSize(drive);
    *sizeOut = size;

    if (size < 0) {
        return ConvertVfError(VFGetLastError(), NWC24_ERR_INTERNAL_VF);
    }

    return NWC24_OK;
}

static NWC24Err BufferedWrite(const void *src, s32 size, NWC24File *file) {
    u32 now;
    u32 left;
    u32 pos;
    u8 *buf;
    const u8 *bsrc;
    s32 result;
    NWC24Err err;
    u32 i;

    left = size;
    pos = file->align;
    buf = NWC24WorkP->writeBuffer;
    bsrc = (u8 *)src;
    err = NWC24_OK;

    while (left != 0) {
        now = NWC24_WR_BUFFER_SIZE - pos;

        if (left < now) {
            now = left;
        }

        memcpy(buf + pos, bsrc, now);

        pos += now;
        left -= now;
        bsrc += now;

        if (pos < NWC24_WR_BUFFER_SIZE) {
            continue;
        }

        pos = 0;

        if (file->mode & NWC24_OPEN_VF) {
            result = VFWriteFile(file->vff, buf, NWC24_WR_BUFFER_SIZE);
            if (result == VF_OK) {
                continue;
            }

            err = ConvertVfError(result, NWC24_ERR_FILE_WRITE);
            break;
        }

        for (i = 0; i < NAND_RETRY_COUNT; i++) {
            result = NANDWrite(&file->nandf, buf, NWC24_WR_BUFFER_SIZE);
            if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
                break;
            }

            SLEEP_MSEC(1000);
        }

        if (result == NAND_RESULT_CORRUPT) {
            return NWC24_ERR_NAND_CORRUPT;
        }

        if (result != NWC24_WR_BUFFER_SIZE) {
            err = NWC24_ERR_FILE_WRITE;
            break;
        }
    }

    file->align = pos;
    return err;
}

static NWC24Err BufferedWriteFlush(NWC24File *file) {
    s32 result;
    u32 pos;
    u8 *buf;
    NWC24Err err;
    u32 i;

    pos = file->align;
    buf = NWC24WorkP->writeBuffer;
    err = NWC24_OK;

    if (pos == 0) {
        return err;
    }

    while (pos % 32 != 0) {
        buf[pos++] = 0x00;
    }

    if (file->mode & NWC24_OPEN_VF) {
        result = VFWriteFile(file->vff, buf, pos);
        if (result != VF_OK) {
            err = ConvertVfError(result, NWC24_ERR_FILE_WRITE);
        }

        return err;
    }

    for (i = 0; i < NAND_RETRY_COUNT; i++) {
        result = NANDWrite(&file->nandf, buf, pos);
        if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
            break;
        }

        SLEEP_MSEC(1000);
    }

    if (result == NAND_RESULT_CORRUPT) {
        return NWC24_ERR_NAND_CORRUPT;
    }

    if (result != pos) {
        err = NWC24_ERR_FILE_WRITE;
    }

    return err;
}

static NWC24Err BufferedRead(void *dst, s32 size, NWC24File *file) {
    u32 total;
    u32 now;
    u32 left;
    s32 result;
    u32 pos;
    u8 *buf;
    u8 *bdst;
    NWC24Err err;
    u32 i;
    u32 bytesread;

    bdst = (u8 *)dst;
    total = size;
    buf = NWC24WorkP->readBuffer;
    err = NWC24_OK;

    if (RdBufferMutex != 0) {
        return NWC24_ERR_MUTEX;
    }

    RdBufferMutex = file->id;

    if (file->align & INVALIDATE_ALIGN) {
        return NWC24_ERR_INVALID_OPERATION;
    }

    pos = file->align;

    while (total > 0) {
        now = NWC24i_IO_BUFFER_SIZE;
        left = now - pos;

        if (total < left) {
            left = total;
            now = (total + pos + 31) & ~31;
        }

        if (file->mode & NWC24_OPEN_VF) {
            result = VFReadFile(file->vff, buf, now, &bytesread);
            if (result != VF_OK) {
                err = ConvertVfError(result, NWC24_ERR_FILE_READ);
                break;
            }

            result = bytesread;
        } else {
            for (i = 0; i < NAND_RETRY_COUNT; i++) {
                result = NANDRead(&file->nandf, buf, now);
                if (!(result == NAND_RESULT_BUSY || result == NAND_RESULT_ALLOC_FAILED)) {
                    break;
                }

                SLEEP_MSEC(1000);
            }

            if (result == NAND_RESULT_CORRUPT) {
                return NWC24_ERR_NAND_CORRUPT;
            }
        }

        if (result != now) {
            err = NWC24_ERR_FILE_READ;
            break;
        } else {
            memcpy(bdst, buf + pos, left);
            bdst += left;
            total -= left;
            pos = 0;
        }
    }

    file->align = (file->align + size) % 32;
    file->align |= INVALIDATE_ALIGN;
    RdBufferMutex = 0;
    return err;
}

static inline NWC24Err AlignedSeek(NWC24File *file, s32 offset, NWC24SeekMode whence) {
    s32 result;
    s32 alignofs;

    file->align = offset % 32;
    alignofs = offset - file->align;

    if (file->mode & NWC24_OPEN_VF) {
        result = VFSeekFile(file->vff, alignofs, whence);
        if (result != VF_OK) {
            return ConvertVfError(result, NWC24_ERR_FILE_OTHER);
        }

        return NWC24_OK;
    }

    result = NANDSeek(&file->nandf, alignofs, whence);
    if (result < 0) {
        return NWC24_ERR_FILE_OTHER;
    }

    return NWC24_OK;
}

static inline NWC24Err ConvertError(s32 nanderr, NWC24Err wc24err) {
    switch (nanderr) {
    case NAND_RESULT_ECC_CRIT:
    case NAND_RESULT_AUTHENTICATION:
        return NWC24_ERR_FILE_BROKEN;
    case NAND_RESULT_CORRUPT:
        return NWC24_ERR_NAND_CORRUPT;
    default:
        return wc24err;
    }
}

static inline NWC24Err ConvertVfError(s32 vferr, NWC24Err wc24err) {
    if (vferr == VF_ERROR_0005) {
        return ConvertError(VFGetLastDeviceError(WC24_DRIVE), wc24err);
    }

    return wc24err;
}

/* ---------------------------------------------------------------------------------------------
 * scheduler (/dev/net/kd/request)
 * ------------------------------------------------------------------------------------------- */

#define NWC24_IOCTL_SUSPEND_SCHEDULER 1
#define NWC24_IOCTL_TRY_SUSPEND_SCHEDULER 2
#define NWC24_IOCTL_RESUME_SCHEDULER 3
#define NWC24_IOCTL_GENERATE_USER_ID 15

typedef struct ScdCommonBuffer {
    u32 word0;
} ScdCommonBuffer;

#pragma pack(push, 1)
typedef struct ScdCommonResult {
    s32 result;
    union {
        u64 userid;
        s32 exResult;
    };
    u32 word8;
} ScdCommonResult;
#pragma pack(pop)

#define SCD_IOCTL_IN ((ScdCommonBuffer *)nwc24ScdCommonBuffer)
#define SCD_IOCTL_OUT ((ScdCommonResult *)nwc24ScdCommonResult)

static u32 nwc24ScdInitialized = 0;
static s32 nwc24ScdSuspendCnt = 0;
static s32 nwc24ScdOpenCnt = 0;

static OSMutex nwc24ScdCommandMutex;
static OSMutex nwc24ScdCounterMutex;

static u8 nwc24ScdCommonBuffer[32] __attribute__((aligned(32)));
static u8 nwc24ScdCommonResult[32] __attribute__((aligned(32)));
static u8 nwc24ScdStatBuf[256] __attribute__((aligned(32)));

static NWC24Err ExecSuspendScheduler(void);
static NWC24Err ExecTrySuspendScheduler(u32 arg0);
static NWC24Err ExecResumeScheduler(void);
static NWC24Err ExecNoParamCommand(const char *user, s32 type, NWC24Err *exResultOut);

static void InitScdMutex(void);

static void ScdLockRight(void);
static BOOL ScdTryLockRight(void);
static void ScdUnlockRight(void);

static void LockCounters(void);
static BOOL TryLockCounters(void);
static void UnlockCounters(void);

static NWC24Err ScdCheckCallingStatus(const char *user, BOOL block);

s32 NWC24SuspendScheduler(void) {
    s32 count;

    LockCounters();
    {
        count = ExecSuspendScheduler();
        if (count >= 0) {
            nwc24ScdSuspendCnt++;
            count -= nwc24ScdOpenCnt;
        }
    }
    UnlockCounters();

    return count;
}

s32 NWC24ResumeScheduler(void) {
    s32 count;

    LockCounters();
    {
        if (nwc24ScdOpenCnt > 0 && nwc24ScdSuspendCnt == 0) {
            count = 0;
        } else {
            count = ExecResumeScheduler();

            if (nwc24ScdSuspendCnt > 0) {
                nwc24ScdSuspendCnt--;
                count -= nwc24ScdOpenCnt;
            }
        }
    }
    UnlockCounters();

    return count;
}

NWC24Err NWC24iRequestGenerateUserId(NWC24UserId *idOut, u32 *arg1) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = ScdCheckCallingStatus(__FUNCTION__, TRUE);
    if (result < 0) {
        return result;
    }

    ScdLockRight();
    {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &fd, 0);

        if (result >= 0) {
            result = NWC24_IOCTL_DEVICE(fd, NWC24_IOCTL_GENERATE_USER_ID, NULL, 0, nwc24ScdCommonResult,
                                        sizeof(nwc24ScdCommonResult));

            if (result >= 0) {
                result = SCD_IOCTL_OUT->result;

                if (result == NWC24_OK || result == NWC24_ERR_ID_GENERATED || result == NWC24_ERR_ID_REGISTERED) {
                    if (idOut != NULL) {
                        *idOut = SCD_IOCTL_OUT->userid;
                    }

                    if (arg1 != NULL) {
                        *arg1 = SCD_IOCTL_OUT->word8;
                    }
                }
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    ScdUnlockRight();

    return result;
}

NWC24Err NWC24iGetSchedulerStat(void *stat, u32 size) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    if (size > 0x100) {
        return NWC24_ERR_OVERFLOW;
    }

    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    ScdLockRight();
    {
        result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &fd, 0);

        if (result >= 0) {
            result = NWC24_IOCTL_DEVICE(fd, 0x1E, NULL, 0, stat, size);

            if (result >= 0) {
                result = *(s32 *)stat;
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }

        // HYPOTHESIS: word 3 of the stat block carries the "new mail" flags
        if (size > 0x10) {
            NWC24iSetNewMsgArrived(((u32 *)stat)[3]);
        }
    }
    ScdUnlockRight();

    return result;
}

// like ScdCheckCallingStatus but also refuses while the lib open is blocked
static inline NWC24Err ScdCheckCallingStatusStrict(void) {
    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    if (NWC24IsMsgLibOpened() || NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_OPENED;
    }

    if (NWC24IsMsgLibOpenBlocking()) {
        return NWC24_ERR_BUSY;
    }

    return NWC24_OK;
}

NWC24Err NWC24iSetScriptMode(u32 mode) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = ScdCheckCallingStatusStrict();
    if (result < 0) {
        return result;
    }

    ScdLockRight();
    {
        memset(nwc24ScdCommonBuffer, 0, sizeof(nwc24ScdCommonBuffer));

        result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &fd, 0);

        if (result >= 0) {
            SCD_IOCTL_IN->word0 = mode;

            result = NWC24_IOCTL_DEVICE(fd, 0x22, nwc24ScdCommonBuffer, sizeof(nwc24ScdCommonBuffer),
                                        nwc24ScdCommonResult, sizeof(nwc24ScdCommonResult));

            if (result >= 0) {
                result = SCD_IOCTL_OUT->result;
            }

            close = NWC24_CLOSE_DEVICE(fd);
            if (result >= 0) {
                result = close;
            }
        }
    }
    ScdUnlockRight();

    return result;
}

NWC24Err NWC24iTrySuspendForOpenLib(void) {
    NWC24Err result;

    if (!TryLockCounters()) {
        return NWC24_ERR_MUTEX;
    }

    result = ExecTrySuspendScheduler(0);
    if (result >= 0) {
        nwc24ScdOpenCnt++;
        result = NWC24_OK;
    }

    UnlockCounters();
    return result;
}

NWC24Err NWC24iResumeForCloseLib(void) {
    NWC24Err result;

    LockCounters();
    {
        result = ExecResumeScheduler();
        if (result >= 0) {
            nwc24ScdOpenCnt--;
            result = NWC24_OK;
        }
    }
    UnlockCounters();

    return result;
}

static NWC24Err ExecSuspendScheduler(void) {
    return ExecNoParamCommand(NULL, NWC24_IOCTL_SUSPEND_SCHEDULER, NULL);
}

static NWC24Err ExecTrySuspendScheduler(u32 arg0) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    result = ScdCheckCallingStatus(__FUNCTION__, TRUE);
    if (result < 0) {
        return result;
    }

    if (!ScdTryLockRight()) {
        return NWC24_ERR_MUTEX;
    }

    result = NWC24_OPEN_DEVICE("/dev/net/kd/request", &fd, 0);

    if (result >= 0) {
        SCD_IOCTL_IN->word0 = arg0;

        result = NWC24_IOCTL_DEVICE(fd, NWC24_IOCTL_TRY_SUSPEND_SCHEDULER, nwc24ScdCommonBuffer,
                                    sizeof(nwc24ScdCommonBuffer), nwc24ScdCommonResult, sizeof(nwc24ScdCommonResult));

        if (result >= 0) {
            result = SCD_IOCTL_OUT->result;
        }

        close = NWC24_CLOSE_DEVICE(fd);
        if (close < 0) {
            result = close;
        }
    }

    ScdUnlockRight();
    return result;
}

static NWC24Err ExecResumeScheduler(void) {
    return ExecNoParamCommand(NULL, NWC24_IOCTL_RESUME_SCHEDULER, NULL);
}

static NWC24Err ExecNoParamCommand(const char *user, s32 type, NWC24Err *exResultOut) {
    s32 fd;
    NWC24Err result;
    NWC24Err close;

    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    ScdLockRight();
    {
        result = NWC24iOpenResourceManager(user, "/dev/net/kd/request", &fd, 0);

        if (result >= 0) {
            result = NWC24iIoctlResourceManager(user, fd, type, NULL, 0, nwc24ScdCommonResult,
                                                sizeof(nwc24ScdCommonResult));

            if (result >= 0) {
                result = SCD_IOCTL_OUT->result;

                if (result == NWC24_ERR_FAILED || result == NWC24_ERR_CONFIG_NETWORK) {
                    if (exResultOut != NULL) {
                        *exResultOut = SCD_IOCTL_OUT->exResult;
                    }
                }
            }

            close = NWC24iCloseResourceManager(user, fd);
            if (close < 0) {
                result = close;
            }
        }
    }
    ScdUnlockRight();

    return result;
}

// HYPOTHESIS: plain "no parameter" scheduler commands 6..9. only the socket library (so.c) calls
// them, around its startup/cleanup, so they are probably "tell kd the network is going up/down".
NWC24Err NWC24iExecCmd6(NWC24Err *out) {
    return ExecNoParamCommand(NULL, 6, out);
}

NWC24Err NWC24iExecCmd7(NWC24Err *out) {
    return ExecNoParamCommand(NULL, 7, out);
}

NWC24Err NWC24iExecCmd8(void) {
    return ExecNoParamCommand(NULL, 8, NULL);
}

NWC24Err NWC24iExecCmd9(void) {
    return ExecNoParamCommand(NULL, 9, NULL);
}

static inline void InitScdMutex(void) {
    BOOL enabled = OSDisableInterrupts();

    if (!(nwc24ScdInitialized & 1)) {
        OSInitMutex(&nwc24ScdCommandMutex);
        OSInitMutex(&nwc24ScdCounterMutex);

        memset(nwc24ScdCommonBuffer, 0, sizeof(nwc24ScdCommonBuffer));
        memset(nwc24ScdCommonResult, 0, sizeof(nwc24ScdCommonResult));

        nwc24ScdInitialized |= 1;
    }

    OSRestoreInterrupts(enabled);
}

static inline void ScdLockRight(void) {
    if (!(nwc24ScdInitialized & 1)) {
        InitScdMutex();
    }

    OSLockMutex(&nwc24ScdCommandMutex);
}

static inline BOOL ScdTryLockRight(void) {
    if (!(nwc24ScdInitialized & 1)) {
        InitScdMutex();
    }

    return OSTryLockMutex(&nwc24ScdCommandMutex);
}

static inline void ScdUnlockRight(void) {
    OSUnlockMutex(&nwc24ScdCommandMutex);
}

static inline void LockCounters(void) {
    if (!(nwc24ScdInitialized & 1)) {
        InitScdMutex();
    }

    OSLockMutex(&nwc24ScdCounterMutex);
}

static inline BOOL TryLockCounters(void) {
    if (!(nwc24ScdInitialized & 1)) {
        InitScdMutex();
    }

    return OSTryLockMutex(&nwc24ScdCounterMutex);
}

static inline void UnlockCounters(void) {
    OSUnlockMutex(&nwc24ScdCounterMutex);
}

static inline NWC24Err ScdCheckCallingStatus(const char *user, BOOL block) {
#pragma unused(user)
#pragma unused(block)
    if (!OSGetCurrentThread()) {
        return NWC24_ERR_FATAL;
    }

    if (NWC24IsMsgLibOpened() || NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_OPENED;
    }

    return NWC24_OK;
}

/* ---------------------------------------------------------------------------------------------
 * mime / charset / encoding lookup tables
 * ------------------------------------------------------------------------------------------- */

const char *MIMETypeStr[] = {"application/octet-stream", "application/x-wii-minidata", "application/x-wii-msgboard",
                             "image/jpeg",               "image/x-wii-picture",        "multipart/alternative",
                             "multipart/mixed",          "multipart/related",          "text/html",
                             "text/plain"};
NWC24MIMEType MIMETypeConv[] = {NWC24_APP_OCTET_STREAM, NWC24_APP_WII_MINIDATA, NWC24_APP_WII_MSGBOARD,
                                NWC24_IMG_JPEG,         NWC24_IMG_WII_PICTURE,  NWC24_MUL_ALTERNATIVE,
                                NWC24_MUL_MIXED,        NWC24_MUL_RELATED,      NWC24_TXT_HTML,
                                NWC24_TXT_PLAIN};
const char *MIMETypeSuffix[] = {"dat", "wii", "wii", "jpg", "wii", "???", "???", "???", "htm", "txt"};

const char *CharsetStr[] = {"euc-jp",      "iso-2022-jp", "iso-8859-1", "iso-8859-10", "iso-8859-15", "iso-8859-2",
                            "iso-8859-3",  "iso-8859-5",  "iso-8859-7", "iso-8859-9",  "shift_jis",   "us-ascii",
                            "utf-16be",    "utf-32be",    "utf-8",      "windows-1252"};
NWC24Charset CharsetConv[] = {NWC24_EUC_JP,      NWC24_ISO_2022_JP, NWC24_ISO_8859_1, NWC24_ISO_8859_10,
                              NWC24_ISO_8859_15, NWC24_ISO_8859_2,  NWC24_ISO_8859_3, NWC24_ISO_8859_5,
                              NWC24_ISO_8859_7,  NWC24_ISO_8859_9,  NWC24_SHIFT_JIS,  NWC24_US_ASCII,
                              NWC24_UTF_16BE,    NWC24_UTF_32BE,    NWC24_UTF_8,      NWC24_WINDOWS_1252};

const char *EncodingStr[] = {"7bit", "8bit", "base64", "quoted-printable"};
NWC24Encoding EncodingConv[] = {NWC24_ENC_7BIT, NWC24_ENC_8BIT, NWC24_ENC_BASE64, NWC24_ENC_QUOTED_PRINTABLE};

#define LENGTHOF(x) (sizeof(x) / sizeof((x)[0]))

const char *NWC24GetMIMETypeStr(NWC24MIMEType type) {
    int i;

    for (i = 0; i < LENGTHOF(MIMETypeConv); i++) {
        if (type == MIMETypeConv[i]) {
            return MIMETypeStr[i];
        }
    }

    return NULL;
}

const char *NWC24iGetMIMETypeSuffix(NWC24MIMEType type) {
    int i;

    for (i = 0; i < LENGTHOF(MIMETypeConv); i++) {
        if (type == MIMETypeConv[i]) {
            return MIMETypeSuffix[i];
        }
    }

    return NULL;
}

const char *NWC24GetCharsetStr(NWC24Charset set) {
    int i;

    for (i = 0; i < LENGTHOF(CharsetConv); i++) {
        if (set == CharsetConv[i]) {
            return CharsetStr[i];
        }
    }

    return NULL;
}

const char *NWC24GetEncodingStr(NWC24Encoding enc) {
    int i;

    for (i = 0; i < LENGTHOF(EncodingConv); i++) {
        if (enc == EncodingConv[i]) {
            return EncodingStr[i];
        }
    }

    return NULL;
}

/* ---------------------------------------------------------------------------------------------
 * message objects
 * ------------------------------------------------------------------------------------------- */

#define YEAR_MIN 2000
#define YEAR_MAX 2035
#define ALT_NAME_MAX 35
#define SMTP_LINE_MAX 1000

NWC24Err NWC24InitMsgObj(NWC24MsgObj *pMsg, NWC24MsgType type) {
    u32 i;
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    Mail_memset(msg, 0, sizeof(NWC24iMsgObj));

    msg->id = 0;
    msg->flags = NWC24_MSGOBJ_INITIALIZED;
    msg->appId = NWC24GetAppId();
    msg->command = 0;
    msg->tag = 0;
    msg->groupId = NWC24GetGroupId();

    switch (type) {
    case NWC24_MSGTYPE_RVL_MENU_SHARED:
        msg->flags |= NWC24_MSGOBJ_FOR_RECIPIENT | NWC24_MSGOBJ_FOR_APP | NWC24_MSGOBJ_FOR_MENU;
        break;
    case NWC24_MSGTYPE_RVL_APP:
        msg->flags |= NWC24_MSGOBJ_FOR_RECIPIENT | NWC24_MSGOBJ_FOR_APP;
        break;
    case NWC24_MSGTYPE_RVL_MENU:
        msg->flags |= NWC24_MSGOBJ_FOR_RECIPIENT | NWC24_MSGOBJ_FOR_MENU;
        break;
    case NWC24_MSGTYPE_RVL_APP_HIDDEN:
        msg->flags |= NWC24_MSGOBJ_FOR_RECIPIENT;
        break;
    case NWC24_MSGTYPE_PUBLIC:
        msg->flags |= NWC24_MSGOBJ_FOR_PUBLIC;
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    msg->createTime = 0;
    msg->flags |= 0x200000; // HYPOTHESIS: newer-sdk flag bit (1 << 21), not in the openrvl source or sibling enum
    msg->unk2C = 0;
    NWC24GetMyUserId(&msg->from.id);
    msg->numTo = 0;

    for (i = 0; i < NWC24i_MSG_RECIPIENT_MAX; i++) {
        if (type == NWC24_MSGTYPE_PUBLIC) {
            NWC24Data_Init(&msg->to[i].data);
        } else {
            msg->to[i].id = 0;
        }
    }

    NWC24Data_Init(&msg->subject);
    NWC24Data_Init(&msg->text);
    msg->charset = NWC24_US_ASCII;
    msg->encoding = NWC24_ENC_7BIT;
    msg->numAttached = 0;

    for (i = 0; i < NWC24i_MSG_ATTACHMENT_MAX; i++) {
        NWC24Data_Init(&msg->attached[i]);
        msg->attachedSize[i] = 0;
        msg->attachedType[i] = NWC24_TXT_PLAIN;
    }

    NWC24Data_Init(&msg->fromField);
    NWC24Data_Init(&msg->toField);
    NWC24Data_Init(&msg->contentType);
    NWC24Data_Init(&msg->txEncoding);
    NWC24Data_Init(&msg->user);
    NWC24Data_Init(&msg->face);
    NWC24Data_Init(&msg->alt);
    msg->msgBoard = 0;
    msg->dwcId.size = 0;
    msg->iconNew = 0x80000000;

    return NWC24_OK;
}

NWC24Err NWC24SetMsgToId(NWC24MsgObj *pMsg, NWC24UserId id) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (!(msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    if (msg->numTo >= NWC24i_MSG_RECIPIENT_MAX) {
        return NWC24_ERR_FULL;
    }

    msg->to[msg->numTo].id = id;
    msg->numTo++;

    return NWC24_OK;
}

NWC24Err NWC24SetMsgText(NWC24MsgObj *pMsg, const char *text, u32 len, NWC24Charset charset, NWC24Encoding encoding) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (NWC24GetCharsetStr(charset) == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (NWC24GetEncodingStr(encoding) == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (text == NULL) {
        NWC24Data_Init(&msg->text);
        return NWC24_OK;
    }

    // HYPOTHESIS: this sdk build checks 8bit-vs-public instead of requiring a terminator after len
    if (encoding == NWC24_ENC_8BIT && (msg->flags & NWC24_MSGOBJ_FOR_PUBLIC)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    // smtp lines max out at 1000 chars incl the CRLF
    if (encoding == NWC24_ENC_7BIT) {
        u32 lineLength;
        const char *it;
        const char *end;

        lineLength = 0;
        it = text;
        end = text + len;

        for (; it < end; it++) {
            if (it[0] == '\r' && it[1] == '\n') {
                lineLength = 0;
                it++;
            } else if (++lineLength > SMTP_LINE_MAX - 2) {
                return NWC24_ERR_FORMAT;
            }
        }
    }

    NWC24Data_SetDataP(&msg->text, text, len);
    msg->charset = charset;
    msg->encoding = encoding;
    return NWC24_OK;
}

// HYPOTHESIS: name for the subject setter (the field is called subject, setter shape is the same as
// SetMsgText). not in the sibling header, the asm only shows it filling msg->subject.
NWC24Err NWC24SetMsgSubject(NWC24MsgObj *pMsg, const char *text, u32 len) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (text == NULL || *text == 0) {
        return NWC24_ERR_NULL;
    }

    if (text[len] != 0) {
        return NWC24_ERR_STRING_END;
    }

    NWC24Data_SetDataP(&msg->subject, text, len);
    return NWC24_OK;
}

NWC24Err NWC24SetMsgTag(NWC24MsgObj *pMsg, u16 tag) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (!(msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    msg->tag = (msg->tag & 0xFFFF0000) | tag;
    return NWC24_OK;
}

NWC24Err NWC24SetMsgLedPattern(NWC24MsgObj *pMsg, u16 pattern) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (pattern == 0) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (!(msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT) || !(msg->flags & NWC24_MSGOBJ_FOR_MENU)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    if (msg->command != 0 && !(msg->command & 0x40000)) {
        return NWC24_ERR_NOT_SUPPORTED;
    }

    // only the system menu (app id HAEA) may use the big pattern values
    if (!NWC24IsMsgLibOpenedByTool() && NWC24GetAppId() != NWC24i_APP_ID_IPL && pattern >= 0x4000) {
        return NWC24_ERR_INVALID_VALUE;
    }

    msg->command = pattern | 0x40000;
    return NWC24_OK;
}

NWC24Err NWC24GetMsgSize(const NWC24MsgObj *pMsg, u32 *size) {
    const NWC24iMsgObj *msg = (const NWC24iMsgObj *)pMsg;

    if (!(msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    *size = msg->length;
    return NWC24_OK;
}
