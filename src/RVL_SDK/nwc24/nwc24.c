// nwc24 is one big blob in the dol (config, mail box, download list, time, etc all in one split),
// so everything lives in this one file for now. section banners mark where the original
// translation units seem to start. function order here follows my draft, not the binary.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

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
    int len;
    const char *p;
    char *d;
    size_t i;
    char c;

    len = 0;
    p = dst;
    while (*p != 0) {
        p++;
        len++;
    }

    d = dst + len;
    i = 0;

    while (i < maxlen && (c = *src) != 0) {
        *d++ = c;
        i++;
        src++;
    }

    dst[len + i] = 0;
    return dst;
}

// the little printf the mail code carries around. it only knows %% %c %s %d %u %o %x %X, a width
// (or *) and a '-' / '0' flag, it builds the output string in place by appending/prepending single chars
// HYPOTHESIS: the strlen / "put a char on the end" / "shift everything up and put a char at the front"
// bits were inlined helpers in the original, the unrolled shift loop is mwcc's doing

#pragma auto_inline on

static inline int StrLen(const char *str) {
    int n = 0;

    while (*str != 0) {
        n++;
        str++;
    }

    return n;
}

static inline void StrCat(char *dst, const char *src) {
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
}

// appends one char to the end of a string
static inline void PutCharBack(char *str, char ch) {
    int len = StrLen(str);
    str[len++] = ch;
    str[len] = 0;
}

// puts one char in front of a string, moving the rest (and the terminator) up by one
static inline void PutCharFront(char *str, char ch) {
    int len = StrLen(str);
    int i;

    for (i = len; i >= 0; i--) {
        str[i + 1] = str[i];
    }

    str[0] = ch;
}

static int fn_80397878(char *buf, u32 value, int base, int hexOffset, int isSigned, int width, char padCh,
                       char flag) {
    int count = 0;
    int written = 0;
    int neg;
    char digit;
    char c;

    if (isSigned && (value & 0x80000000)) {
        value = -value;
        neg = 1;
    } else {
        neg = 0;
    }

    while (value != 0) {
        digit = value % base;
        value /= base;

        PutCharFront(buf, digit > 9 ? digit + hexOffset : digit + '0');
        written++;
        count++;
    }

    if (*buf == 0) {
        PutCharFront(buf, '0');
        written++;
        count++;
    }

    if (padCh != '0') {
        padCh = ' ';
    }

    count += neg;

    while (count < width) {
        if (flag == 'L') {
            PutCharBack(buf, ' ');
            written++;
        } else {
            PutCharFront(buf, padCh);
            written++;
        }
        count++;
    }

    if (neg) {
        PutCharFront(buf, '-');
    }

    return written;
}

int Mail_vsprintf(char *buf, char *fmt, va_list args) {
    int i;
    int total;
    int isNumber;
    int hexOffset;
    int base;
    char padCh;
    int width;
    int isSigned;
    char ch;
    char flag;
    char lenMod;

    *buf = 0;
    total = 0;

    while ((ch = *fmt++) != 0) {
        while (*buf != 0) {
            buf++;
        }

        if (ch == '%') {
            ch = *fmt++;
            if (ch == '%') {
                PutCharBack(buf++, ch);
                total++;
                continue;
            }
            flag = ch;
            if (ch == '-') {
                ch = *fmt++;
            }
            padCh = ch;

            isSigned = 0;
            base = 10;
            hexOffset = '0';
            isNumber = 0;

            if (ch == '*') {
                width = va_arg(args, int);
                ch = *fmt++;
            } else {
                width = 0;
                while (ch >= '0' && ch <= '9') {
                    width = width * 10 + (ch - '0');
                    ch = *fmt++;
                }
            }

            lenMod = ch & 0xDF;
            if (lenMod == 'L') {
                ch = *fmt++;
            }

            switch (ch) {
            case 'd':
                isNumber = 1;
                isSigned = 1;
                break;
            case 'o':
                isNumber = 1;
                base = 8;
                break;
            case 'u':
                isNumber = 1;
                break;
            case 'x':
                isNumber = 1;
                base = 16;
                hexOffset = 'a' - 10;
                break;
            case 'X':
                isNumber = 1;
                base = 16;
                hexOffset = 'A' - 10;
                break;
            case 'c':
                PutCharBack(buf++, va_arg(args, int));
                total++;
                break;
            case 's': {
                char *str = va_arg(args, char *);
                int len;
                char left;

                if (str != NULL) {
                    len = StrLen(str);
                    StrCat(buf, str);
                } else {
                    len = 0;
                }

                total += len;
                left = flag;
                for (i = len; i < width; i++) {
                    total++;
                    if (left == '-') {
                        PutCharBack(buf, ' ');
                    } else {
                        PutCharFront(buf, ' ');
                    }
                }
                break;
            }
            }

            if (isNumber) {
                u32 value;

                if (lenMod == 'L') {
                    value = va_arg(args, u32);
                } else if (isSigned) {
                    value = va_arg(args, s32);
                } else {
                    value = va_arg(args, u32);
                }

                total += fn_80397878(buf, value, base, hexOffset, isSigned, width, padCh, flag);
            }
        } else {
            PutCharBack(buf++, ch);
            total++;
        }
    }

    return total;
}

int Mail_sprintf(char *buf, char *fmt, ...) {
    va_list args;
    int n;

    va_start(args, fmt);
    n = Mail_vsprintf(buf, fmt, args);
    va_end(args);
    return n;
}

#pragma auto_inline off

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

/* ---------------------------------------------------------------------------------------------
 * system status checks (works out whether the scheduler is allowed to run, and what to tell the user)
 * ------------------------------------------------------------------------------------------- */

// plain sc items that live in the sc unit, none of them have names yet
extern u32 fn_80210064(void);
extern u32 fn_802100A0(void);
extern u32 fn_802100DC(void);
extern BOOL fn_8020FC90(void *dst);
extern s32 NCDiGetEnabledConfigList(u32 *a, u32 *b, u32 *c);


static NWC24Err fn_8039A69C(s32 code, u32 mode, u32 *delay);
static NWC24Err fn_8039A53C(s32 *outCode, u32 mode);

// HYPOTHESIS: reads the id creation stage out of the config
NWC24Err fn_80399BB8(s32 *stage) {
    *stage = config->createStage;
    return NWC24_OK;
}

// checks a string: not null and not empty, no longer than maxlen, at least minlen
NWC24Err fn_80399EE0(const char *str, u32 minLen, u32 maxLen) {
    u32 len;

    if (str == NULL) {
        return NWC24_ERR_NULL;
    }

    len = STD_strnlen(str, maxLen + 1);
    if (len == 0) {
        return NWC24_ERR_NULL;
    }

    if (len > maxLen) {
        return NWC24_ERR_FULL;
    }

    if (len < minLen) {
        return NWC24_ERR_FORMAT;
    }

    return NWC24_OK;
}

// strlcpy, hands back the number of chars copied
int fn_80399F5C(char *dst, const char *src, int size) {
    char *d;
    int i;

    if (dst == NULL || src == NULL) {
        return 0;
    }

    d = dst;
    i = 0;
    while (i + 1 < size) {
        *d = *src;
        if (*src == 0) {
            break;
        }

        src++;
        i++;
        d++;
    }

    if (i + 1 >= size && size != 0) {
        dst[i] = 0;
    }

    return i;
}

NWC24Err fn_8039A350(u32 mode) {
    NWC24Err result;
    u8 parental[0x4A];
    u32 sp18;
    u32 sp14;
    u32 sp10;
    s32 stage;
    BOOL forScheduler;
    u32 eula;
    u32 status;
    u32 isEnabled;
    u32 sp08;
    u32 restrictions;

    GlobalErrorCode = 0;

    if (Opened != NWC24_LIB_OPENED) {
        result = NWC24_ERR_LIB_NOT_OPENED;
        goto end;
    }

    result = -39;

    do {
        status = SCCheckStatus();
    } while (status == 1);

    if (status == 2) {
        GlobalErrorCode = -112 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    isEnabled = fn_802100DC() & 1;
    eula = fn_802100A0();
    restrictions = fn_80210064() & 2;
    fn_8020FC90(parental);

    if (isEnabled == 0) {
        GlobalErrorCode = -139 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    if (eula == 0) {
        GlobalErrorCode = -107 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    forScheduler = mode & 1;
    if (forScheduler && (parental[0] & 0x80) && restrictions != 0) {
        GlobalErrorCode = -107 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    result = -31;
    if (NCDiGetEnabledConfigList(&sp18, &sp14, &sp10) < 0) {
        GlobalErrorCode = -133 - MANAGE_ERROR_CODE_BASE;
        goto end;
    }

    if (sp18 == 0 && sp14 == 0 && sp10 == 0) {
        GlobalErrorCode = -50299;
        goto end;
    }

    if (forScheduler) {
        fn_80399BB8(&stage);
        if (stage != NWC24_IDCS_REGISTERED) {
            GlobalErrorCode = -144 - MANAGE_ERROR_CODE_BASE;
            goto end;
        }
    }

    sp08 = 0;
    result = fn_8039A53C((s32 *)&sp08, mode);
    if (result < 0) {
        GlobalErrorCode = sp08;
        goto end;
    }

    if ((result = NWC24iMBoxCheck(0, 0)) == NWC24_ERR_FULL) {
        GlobalErrorCode = -106 - MANAGE_ERROR_CODE_BASE;
    }

    if (result != NWC24_ERR_FULL) {
        result = NWC24_OK;
    }

end:
    return result;
}

// walks the scheduler's recent error list (newest first) until the delays add up to a minute
static NWC24Err fn_8039A53C(s32 *outCode, u32 mode) {
    u32 count;
    u32 n;
    u32 *stat = (u32 *)NWC24WorkP->WORK_0x400;
    u32 sum = 0;
    u32 delay;
    NWC24Err result;
    u32 *entry;
    u32 i;
    s32 idx;

    *outCode = 0;
    result = NWC24iGetSchedulerStat(stat, 0x100);

    if (result == NWC24_ERR_FATAL || result == NWC24_ERR_NOMEM || result == -42) {
        *outCode = result - 0x1AA2C - 1 + 1;
        return NWC24_ERR_FATAL;
    }

    if (result != NWC24_OK) {
        return NWC24_OK;
    }

    if (mode & 1) {
        if ((stat[0x44 / 4] & 3) == 3) {
            return NWC24_OK;
        }
    } else if (mode & 2) {
        if ((stat[0x48 / 4] & 0xF) == 0xF) {
            return NWC24_OK;
        }
    }

    count = stat[0x14 / 4];
    n = 32;
    if (count < 32) {
        n = count;
    }

    idx = (s32)(count + 31) % 32;
    for (i = 0; i < n; i++) {
        entry = &stat[idx];
        result = fn_8039A69C(entry[0x80 / 4], mode, &delay);
        if (result != NWC24_OK) {
            sum += delay;
            if (sum >= 60) {
                *outCode = entry[0x80 / 4];
                return result;
            }
        }

        idx--;
        if (idx < 0) {
            idx = 31;
        }
    }

    *outCode = 0;
    return NWC24_OK;
}

// HYPOTHESIS: turns one scheduler error code (a six digit number, digits mean something) into how long the
// user should be told to wait and whether it counts as a network or server problem
static NWC24Err fn_8039A69C(s32 code, u32 mode, u32 *delay) {
    s32 n;
    s8 d0, d1, d2, d3, d4, d5;
    s32 low;

    *delay = 0;

    n = -code;
    d0 = n % 10;
    n /= 10;
    d1 = n % 10;
    n /= 10;
    d2 = n % 10;
    n /= 10;
    d3 = n % 10;
    n /= 10;
    d4 = n % 10;
    n /= 10;
    d5 = n % 10;

    if (d5 == 0 && d4 == 5) {
        *delay = 60;
        return -31;
    }

    if (d3 == 7) {
        if (!(mode & 2)) {
            return NWC24_OK;
        }
    } else if (!(mode & 1)) {
        return NWC24_OK;
    }

    if (d5 == 1 && d4 == 1) {
        if (d2 == 1) {
            return NWC24_OK;
        }

        if (d2 == 4 && d1 == 0 && d0 == 7) {
            *delay = 60;
            return -31;
        }

        if (!(mode & 1)) {
            return NWC24_OK;
        }

        *delay = 30;
        return -32;
    } else if (d5 == 1 && d4 == 0) {
        if (!(mode & 1)) {
            return NWC24_OK;
        }

        low = d0 + d1 * 10;
        switch (d2) {
        case 0:
            if (low >= 70) {
                return NWC24_OK;
            }

            *delay = 60;
            switch (low) {
            case 0:
            case 1:
            case 2:
            case 3:
                return -1;
            case 30:
            case 31:
                return -31;
            case 32:
                return -32;
            }
            return NWC24_OK;
        case 2:
            switch (-low) {
            case -1:
                *delay = 60;
                return -1;
            case -11:
            case -45:
                return NWC24_OK;
            case -31:
            case -44:
            case -33:
                *delay = 60;
                return -31;
            case -32:
                *delay = 20;
                return -32;
            }
            return NWC24_OK;
        case 3:
            switch (low) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
                *delay = 30;
                return -31;
            case 6:
            case 7:
            case 8:
                *delay = 20;
                return -32;
            }
            return NWC24_OK;
        case 4:
            switch (low) {
            case 9:
            case 10:
            case 11:
            case 12:
                *delay = 60;
                return -32;
            default:
                *delay = 20;
                return -31;
            }
        }
    }

    return NWC24_OK;
}

/* ---------------------------------------------------------------------------------------------
 * mail box control files (wc24send.ctl / wc24recv.ctl) and the message files next to them
 * ------------------------------------------------------------------------------------------- */

#define MBC_MAGIC 0x57635466 // 'WcTf'
#define MBC_MAGIC_BROKEN 0x42524B4E // 'BRKN'
#define MBC_VERSION 4
#define MBC_BLOCK_SIZE 0x80
#define MBC_FIRST_ENTRY 0x80

// HYPOTHESIS: message ids count up to a million and wrap, so older/newer has to be decided with a window
#define MSG_ID_WRAP_HIGH 900000
#define MSG_ID_WRAP_LOW 100000

// how many people have the mbox filesystem mounted, and which box it is mounted for
static struct {
    s32 count;
    NWC24MsgBoxId box;
} MBoxMount;

extern NWC24Err fn_8039D0EC(NWC24iMBCEntry *entry, NWC24iMsgObj *msg);
extern NWC24Err fn_8039CF40(NWC24iMsgObj *msg, NWC24iMBCEntry *entry);

static NWC24Err fn_8039C984(NWC24MsgBoxId id, NWC24iMBCHeader **hdrOut);
static NWC24Err fn_8039CDB4(NWC24MsgBoxId id);

static inline NWC24Err MBoxCtrlPath_i(NWC24MsgBoxId id, char *dst, s32 maxlen) {
    char *dir = (char *)NWC24GetMBoxDir();

    if (maxlen < STD_strnlen(dir, NWC24i_MBOX_DIR_LENGTH) + 14) {
        return NWC24_ERR_NOMEM;
    }

    switch (id) {
    case NWC24_MSGBOX_SEND:
        Mail_sprintf(dst, "%s%s", dir, "/wc24send.ctl");
        break;
    case NWC24_MSGBOX_RECV:
        Mail_sprintf(dst, "%s%s", dir, "/wc24recv.ctl");
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

static inline NWC24Err MBoxMbxPath_i(NWC24MsgBoxId id, char *dst, s32 maxlen) {
    char *dir = (char *)NWC24GetMBoxDir();

    if (maxlen < STD_strnlen(dir, NWC24i_MBOX_DIR_LENGTH) + 14) {
        return NWC24_ERR_NOMEM;
    }

    switch (id) {
    case NWC24_MSGBOX_SEND:
        Mail_sprintf(dst, "%s%s", dir, "/wc24send.mbx");
        break;
    case NWC24_MSGBOX_RECV:
        Mail_sprintf(dst, "%s%s", dir, "/wc24recv.mbx");
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

// the files for single messages live on the vf drive, they are named after the message id
static inline NWC24Err MBoxMsgPath_i(NWC24MsgBoxId id, char *dst, u32 msgId) {
    switch (id) {
    case NWC24_MSGBOX_SEND:
        Mail_sprintf(dst, "@24:/mb/s%07d.msg", msgId);
        break;
    case NWC24_MSGBOX_RECV:
        Mail_sprintf(dst, "@24:/mb/r%07d.msg", msgId);
        break;
    default:
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

static inline NWC24Err MBoxUnmount_i(void) {
    u32 size;
    NWC24iMBCHeader *hdr;
    NWC24Err result;

    if (MBoxMount.count == 0) {
        return NWC24_OK;
    }

    if (--MBoxMount.count > 0) {
        return NWC24_OK;
    }

    result = fn_8039C984(MBoxMount.box, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24CheckSizeVF("@24", &size);
    if (result != NWC24_OK) {
        return result;
    }

    hdr->freeSpace = size;
    return NWC24UnmountVF("@24");
}

// MATCH-ONLY: same again for the copy that sits inside MBoxRemoveMsgFile_i
static inline NWC24Err MBoxUnmountRm_i(void) {
    NWC24iMBCHeader *hdr;
    u32 size;
    NWC24Err result;

    if (MBoxMount.count == 0) {
        return NWC24_OK;
    }

    if (--MBoxMount.count > 0) {
        return NWC24_OK;
    }

    result = fn_8039C984(MBoxMount.box, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24CheckSizeVF("@24", &size);
    if (result != NWC24_OK) {
        return result;
    }

    hdr->freeSpace = size;
    return NWC24UnmountVF("@24");
}

// MATCH-ONLY: CloseMsg has its own copy of the unmount code, the locals are laid out the other way round
static inline NWC24Err MBoxUnmountClose_i(void) {
    NWC24iMBCHeader *hdr;
    u32 size;
    NWC24Err result;

    if (MBoxMount.count == 0) {
        return NWC24_OK;
    }

    if (--MBoxMount.count > 0) {
        return NWC24_OK;
    }

    result = fn_8039C984(MBoxMount.box, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24CheckSizeVF("@24", &size);
    if (result != NWC24_OK) {
        return result;
    }

    hdr->freeSpace = size;
    return NWC24UnmountVF("@24");
}

// deletes the single message file and gives the mount back
static inline NWC24Err MBoxRemoveMsgFile_i(NWC24MsgBoxId id, u32 msgId) {
    NWC24Err result;
    NWC24Err del;
    NWC24Err unmount;
    char *path;

    result = fn_8039CDB4(id);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxMsgPath_i(id, path, msgId);
    if (result != NWC24_OK) {
        return result;
    }

    del = NWC24FDeleteVF(path);
    unmount = MBoxUnmountRm_i();
    if (del != NWC24_OK) {
        return del;
    }

    return unmount;
}

// writes the first block (the header) of a control file
static inline NWC24Err MBoxWriteHeader_i(NWC24iMBCHeader *hdr, NWC24File *file) {
    NWC24Err result = NWC24FSeek(file, 0, NWC24_SEEK_BEG);

    if (result != NWC24_OK) {
        return result;
    }

    return NWC24FWrite(hdr, MBC_BLOCK_SIZE, file);
}

// hands out the next message id of a box
static inline NWC24Err MBoxGetNextMsgId_i(NWC24iMBCHeader *hdr, u32 *idOut) {
    if (hdr->magic != MBC_MAGIC) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (hdr->nextMsgId > 1000000) {
        hdr->nextMsgId = 1;
    }

    *idOut = hdr->nextMsgId;
    hdr->nextMsgId++;
    return NWC24_OK;
}

static inline int CompareMsgId(u32 a, u32 b) {
    if (a == b) {
        return 0;
    }

    if (a > MSG_ID_WRAP_HIGH && b < MSG_ID_WRAP_LOW) {
        return -1;
    }

    if (a < MSG_ID_WRAP_LOW && b > MSG_ID_WRAP_HIGH) {
        return 1;
    }

    return a > b ? 1 : -1;
}

NWC24Err NWC24iMBoxGetCtrlFilePath(NWC24MsgBoxId id, char *dst, s32 maxlen) {
    return MBoxCtrlPath_i(id, dst, maxlen);
}

// how many messages are in a box
NWC24Err fn_8039B1C0(NWC24MsgBoxId id, u32 *numMsgs) {
    NWC24iMBCHeader *hdr;
    NWC24Err result;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        *numMsgs = 0;
        return result;
    }

    *numMsgs = hdr->numMsgs;
    return NWC24_OK;
}

// can the current app see this mail box entry?
static BOOL fn_8039B4C8(NWC24iMBCEntry *entry) {
    u32 flags = entry->flags;

    if (!(flags & 0x200000)) {
        return FALSE;
    }

    if (flags & 0xFE000000) {
        return FALSE;
    }

    if (entry->command & 0xFFF80000) {
        return FALSE;
    }

    if (flags & NWC24_MSGOBJ_FOR_PUBLIC) {
        if (entry->appId == 0) {
            return TRUE;
        }

        return (NWC24GetAppId() & ~0xFF) == (entry->appId & ~0xFF);
    }

    if (NWC24GetAppId() == NWC24i_APP_ID_IPL) {
        return TRUE;
    }

    if (entry->flags & NWC24_MSGOBJ_FOR_APP) {
        return TRUE;
    }

    return (NWC24GetAppId() & ~0xFF) == (entry->appId & ~0xFF);
}

NWC24Err fn_8039C16C(NWC24MsgBoxId id, u32 msgId, BOOL checkAccess);

NWC24Err fn_8039B5AC(NWC24MsgBoxId id, u32 msgId) {
    BOOL checkAccess = TRUE;

    if (NWC24IsMsgLibOpenedByTool()) {
        checkAccess = FALSE;
    }

    return fn_8039C16C(id, msgId, checkAccess);
}

NWC24Err fn_8039B244(NWC24iMsgObj *out, NWC24MsgBoxId id, u32 msgId) {
    NWC24iMBCHeader *hdr;
    NWC24File file;
    NWC24Err result;
    NWC24Err err;
    NWC24Err closeErr;
    NWC24iMBCEntry *entry;
    char *path;
    u32 count = 0;
    u32 off;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    entry = &NWC24WorkP->mbcEntry;
    memset(entry, 0, MBC_BLOCK_SIZE);

    if (msgId == 0) {
        return NWC24_ERR_HIDDEN;
    }

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_RW);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24_ERR_NOT_FOUND;
    for (off = MBC_FIRST_ENTRY; off < hdr->fileSize; off += MBC_BLOCK_SIZE) {
        err = NWC24FSeek(&file, off, NWC24_SEEK_BEG);
        if (err != NWC24_OK) {
            result = err;
            break;
        }

        err = NWC24FRead(entry, MBC_BLOCK_SIZE, &file);
        if (err != NWC24_OK) {
            result = err;
            break;
        }

        if (entry->id != 0) {
            count++;
            if (entry->id == msgId) {
                result = NWC24_ERR_HIDDEN;
                if (NWC24IsMsgLibOpenedByTool()) {
                    result = NWC24_OK;
                    break;
                }

                if (fn_8039B4C8(entry)) {
                    result = NWC24_OK;
                }

                break;
            }
        }
    }

    // the cached message count was wrong, fix it up while the file is open
    if (result == NWC24_ERR_NOT_FOUND && hdr->numMsgs != count) {
        hdr->numMsgs = count;
        MBoxWriteHeader_i(hdr, &file);
    }

    closeErr = NWC24FClose(&file);
    if (result != NWC24_OK) {
        if (msgId == 0) {
            result = NWC24_ERR_INVALID_VALUE;
        }

        return result;
    }

    if (closeErr != NWC24_OK) {
        return closeErr;
    }

    fn_8039D0EC(entry, out);
    return result;
}

NWC24Err fn_8039B9B0(NWC24MsgBoxId id, u32 msgId, NWC24File *file);

NWC24Err NWC24iMBoxOpenNewMsg(NWC24MsgBoxId id, NWC24File *file, u32 *pMsgID) {
    NWC24iMBCHeader *hdr;
    NWC24Err result;
    char *path;

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = MBoxGetNextMsgId_i(hdr, pMsgID);
    if (result != NWC24_OK) {
        return result;
    }

    result = fn_8039CDB4(id);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxMsgPath_i(id, path, *pMsgID);
    if (result != NWC24_OK) {
        return result;
    }

    return NWC24FOpen(file, path, NWC24_OPEN_VF_WBUFF);
}

NWC24Err fn_8039B9B0(NWC24MsgBoxId id, u32 msgId, NWC24File *file) {
    NWC24Err result;
    char *path;

    result = fn_8039CDB4(id);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxMsgPath_i(id, path, msgId);
    if (result != NWC24_OK) {
        return result;
    }

    return NWC24FOpen(file, path, NWC24_OPEN_VF_RBUFF);
}

NWC24Err NWC24iMBoxCloseMsg(NWC24File *file) {
    NWC24Err close;
    NWC24Err unmount;

    close = NWC24FClose(file);
    unmount = MBoxUnmountClose_i();
    if (close != NWC24_OK && unmount != NWC24_OK) {
        return unmount;
    }

    return close;
}

NWC24Err NWC24iMBoxCancelMsg(NWC24File *file, NWC24MsgBoxId id, u32 msgId) {
    NWC24Err result;
    NWC24Err err;
    char *path;
    NWC24Err close;
    NWC24Err del;

    close = NWC24FClose(file);

    err = fn_8039CDB4(id);
    if (err != NWC24_OK) {
        result = err;
        goto forceUnmount;
    }

    path = NWC24WorkP->pathWork;
    err = MBoxMsgPath_i(id, path, msgId);
    if (err != NWC24_OK) {
        result = err;
        goto forceUnmount;
    }

    del = NWC24FDeleteVF(path);
    err = MBoxUnmount_i();
    if (del != NWC24_OK) {
        err = del;
    }
    result = err;

forceUnmount:
    MBoxMount.count = 1;
    err = MBoxUnmount_i();

    if (close != NWC24_OK) {
        return close;
    }

    if (result != NWC24_OK) {
        return result;
    }

    return err != NWC24_OK ? err : NWC24_OK;
}

// finds (and loads, if needed) the control file header of a box
static NWC24Err fn_8039C984(NWC24MsgBoxId id, NWC24iMBCHeader **hdrOut) {
    NWC24Err result = NWC24_OK;
    NWC24File file;
    NWC24Err closeErr;
    NWC24Err err;
    NWC24iMBCHeader *hdr;
    char *path;

    if (id == NWC24_MSGBOX_SEND) {
        *hdrOut = &NWC24WorkP->sendCtrl;
    } else if (id == NWC24_MSGBOX_RECV) {
        *hdrOut = &NWC24WorkP->recvCtrl;
    } else {
        *hdrOut = NULL;
        return NWC24_ERR_INVALID_VALUE;
    }

    if ((*hdrOut)->magic != MBC_MAGIC) {
        path = NWC24WorkP->pathWork;
        result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
        if (result != NWC24_OK) {
            return result;
        }

        result = NWC24FOpen(&file, path, NWC24_OPEN_READ);
        if (result != NWC24_OK) {
            return result;
        }

        hdr = *hdrOut;
        err = NWC24FSeek(&file, 0, NWC24_SEEK_BEG);
        if (err != NWC24_OK) {
            result = err;
        } else {
            err = NWC24FRead(hdr, MBC_BLOCK_SIZE, &file);
            if (err != NWC24_OK) {
                result = err;
            } else {
                result = NWC24_OK;
                if (hdr->magic != MBC_MAGIC) {
                    result = NWC24_ERR_BROKEN;
                }
            }
        }

        closeErr = NWC24FClose(&file);
        if (result == NWC24_OK && closeErr != NWC24_OK) {
            result = closeErr;
        }
    }

    if ((*hdrOut)->version != MBC_VERSION) {
        return NWC24_ERR_VER_MISMATCH;
    }

    if ((*hdrOut)->freeChain & 0x1F) {
        result = NWC24_ERR_BROKEN;
    }

    return result;
}

// takes a free block off the free chain and puts the message in it
static NWC24Err fn_8039CB7C(NWC24iMBCHeader *hdr, NWC24iMsgObj *msg, NWC24File *file) {
    u32 block = hdr->freeChain;
    NWC24iMBCEntry *entry = &NWC24WorkP->mbcEntry;
    NWC24Err result;
    u32 next;

    if (block == 0) {
        return NWC24_ERR_FULL;
    }

    if (block >= hdr->fileSize || (block - MBC_FIRST_ENTRY) % MBC_BLOCK_SIZE != 0) {
        return NWC24_ERR_BROKEN;
    }

    result = NWC24FSeek(file, block, NWC24_SEEK_BEG);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FRead(entry, MBC_BLOCK_SIZE, file);
    if (result != NWC24_OK) {
        return result;
    }

    next = entry->appId;
    if (next >= hdr->fileSize || (next - MBC_FIRST_ENTRY) % MBC_BLOCK_SIZE != 0) {
        return NWC24_ERR_BROKEN;
    }

    hdr->freeChain = next;
    fn_8039CF40(msg, entry);

    result = NWC24FSeek(file, block, NWC24_SEEK_BEG);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FWrite(entry, MBC_BLOCK_SIZE, file);
    if (result != NWC24_OK) {
        return result;
    }

    if (hdr->numMsgs == 0) {
        hdr->oldestMsgId = msg->id;
    }

    hdr->totalMsgSize += msg->length;
    hdr->numMsgs++;
    return NWC24_OK;
}

// gives a block back to the free chain
static NWC24Err fn_8039CCC8(NWC24iMBCHeader *hdr, NWC24File *file, u32 block) {
    NWC24iMBCEntry *entry = &NWC24WorkP->mbcEntry;
    NWC24Err result;

    result = NWC24FSeek(file, block, NWC24_SEEK_BEG);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FRead(entry, MBC_BLOCK_SIZE, file);
    if (result != NWC24_OK) {
        return result;
    }

    hdr->numMsgs--;
    hdr->totalMsgSize -= entry->length;
    memset(entry, 0, MBC_BLOCK_SIZE);
    entry->appId = hdr->freeChain;
    hdr->freeChain = block;

    result = NWC24FSeek(file, block, NWC24_SEEK_BEG);
    if (result != NWC24_OK) {
        return result;
    }

    return NWC24FWrite(entry, MBC_BLOCK_SIZE, file);
}

// mounts the vf drive that holds the message files of one box
static NWC24Err fn_8039CDB4(NWC24MsgBoxId id) {
    NWC24Err result;
    char *path;

    if (MBoxMount.count != 0 && MBoxMount.box != id) {
        MBoxMount.count = 1;
        MBoxUnmount_i();
        return NWC24_ERR_FATAL;
    }

    if (++MBoxMount.count > 1) {
        return NWC24_OK;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxMbxPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24MountVF("@24", path);
    if (result != NWC24_OK) {
        return result;
    }

    MBoxMount.box = id;
    return NWC24_OK;
}

static NWC24Err fn_8039C60C(NWC24iMBCHeader *hdr, NWC24iMsgObj *msg, NWC24File *file, NWC24MsgBoxId id);

NWC24Err NWC24iMBoxAddMsgObj(NWC24MsgBoxId id, NWC24iMsgObj *msg) {
    NWC24iMBCHeader *hdr;
    NWC24File file;
    NWC24Err result;
    char *path;

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    if (hdr->numMsgs == hdr->capacity) {
        return NWC24_ERR_FULL;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_RW);
    if (result != NWC24_OK) {
        return result;
    }

    result = fn_8039CB7C(hdr, msg, &file);
    if (result == NWC24_OK) {
        result = fn_8039C60C(hdr, msg, &file, id);
        if (result != NWC24_OK) {
            goto close;
        }
    } else if (result == NWC24_ERR_BROKEN) {
        hdr->magic = MBC_MAGIC_BROKEN;
    } else {
        goto close;
    }

    result = MBoxWriteHeader_i(hdr, &file);

close: {
    NWC24Err closeErr = NWC24FClose(&file);

    return result != NWC24_OK ? result : closeErr;
}
}

NWC24Err NWC24iMBoxFlushHeader(NWC24MsgBoxId id) {
    NWC24iMBCHeader *hdr;
    NWC24File file;
    NWC24Err result;
    NWC24Err closeErr;
    char *path;

    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_RW);
    if (result != NWC24_OK) {
        return result;
    }

    result = MBoxWriteHeader_i(hdr, &file);

    closeErr = NWC24FClose(&file);
    return result != NWC24_OK ? result : closeErr;
}

// makes room for a message of the given size: the send box just refuses when full, the receive box throws
// the oldest message out until there is space
NWC24Err NWC24iMBoxCheck(NWC24MsgBoxId id, u32 size) {
    NWC24iMBCHeader *hdr;
    NWC24Err result;
    u32 limit;
    volatile u32 oldest;

    if (size >= NWC24i_MBOX_MAX_SIZE) {
        return NWC24_ERR_OVERFLOW;
    }

    limit = size + 0x4000;
    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    if (id == NWC24_MSGBOX_SEND) {
        if (hdr->numMsgs >= hdr->capacity) {
            return NWC24_ERR_FULL;
        }

        if (hdr->freeSpace <= limit) {
            return NWC24_ERR_FULL;
        }
    } else if (id == NWC24_MSGBOX_RECV) {
        while (hdr->numMsgs >= hdr->capacity || hdr->freeSpace <= limit) {
            oldest = hdr->oldestMsgId;
            result = fn_8039C16C(id, oldest, FALSE);
            if (result != NWC24_OK) {
                return result;
            }

            result = fn_8039C984(id, &hdr);
            if (result != NWC24_OK) {
                return result;
            }
        }
    } else {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

// deletes one message from a box (checking if the current app may, unless told not to)
NWC24Err fn_8039C16C(NWC24MsgBoxId id, u32 msgId, BOOL checkAccess) {
    NWC24Err err;
    char *path;
    u32 off;
    NWC24File file;
    u32 appId;
    u32 count = 0;
    u32 foundOff = 0;
    NWC24Err removeErr;
    NWC24Err writeErr;
    u32 oldest = 0;
    NWC24Err result;
    NWC24iMBCHeader *hdr;
    NWC24iMBCEntry *entry;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    entry = &NWC24WorkP->mbcEntry;
    result = fn_8039C984(id, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    path = NWC24WorkP->pathWork;
    result = MBoxCtrlPath_i(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&file, path, NWC24_OPEN_RW);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24_ERR_NOT_FOUND;
    for (off = MBC_FIRST_ENTRY; off < hdr->fileSize; off += MBC_BLOCK_SIZE) {
        writeErr = NWC24FSeek(&file, off, NWC24_SEEK_BEG);
        if (writeErr != NWC24_OK) {
            result = writeErr;
            break;
        }

        writeErr = NWC24FRead(entry, MBC_BLOCK_SIZE, &file);
        if (writeErr != NWC24_OK) {
            result = writeErr;
            break;
        }

        if (entry->id == 0) {
            continue;
        }

        count++;
        if (entry->id == msgId) {
            if (checkAccess) {
                if ((entry->flags & NWC24_MSGOBJ_FOR_PUBLIC) && entry->appId == 0) {
                    if (NWC24GetAppId() != NWC24i_APP_ID_IPL) {
                        result = NWC24_ERR_PROTECTED;
                        break;
                    }
                } else if ((NWC24GetAppId() & ~0xFF) != (entry->appId & ~0xFF)) {
                    if (!(entry->flags & NWC24_MSGOBJ_FOR_MENU) || NWC24GetAppId() != NWC24i_APP_ID_IPL) {
                        result = NWC24_ERR_PROTECTED;
                        break;
                    }
                }
            }

            foundOff = off;
            result = NWC24_OK;
        } else if (oldest == 0 || CompareMsgId(oldest, entry->id) > 0) {
            oldest = entry->id;
        }
    }

    if (result == NWC24_ERR_NOT_FOUND && hdr->numMsgs != count) {
        hdr->numMsgs = count;
        MBoxWriteHeader_i(hdr, &file);
    }

    if (result != NWC24_OK) {
        NWC24FClose(&file);
        if (msgId == 0) {
            result = NWC24_ERR_INVALID_VALUE;
        }

        return result;
    }

    hdr->oldestMsgId = oldest;
    removeErr = fn_8039CCC8(hdr, &file, foundOff);
    err = MBoxRemoveMsgFile_i(id, msgId);
    if (removeErr != NWC24_OK) {
        err = removeErr;
    }

    writeErr = MBoxWriteHeader_i(hdr, &file);

    if (err != NWC24_OK) {
        writeErr = err;
    }

    result = NWC24FClose(&file);
    return writeErr != NWC24_OK ? writeErr : result;
}

// looks for an older copy of the message that is about to be stored (same tag or same contents) and
// throws it out, also keeps the oldest message id of the box up to date
static NWC24Err fn_8039C60C(NWC24iMBCHeader *hdr, NWC24iMsgObj *msg, NWC24File *file, NWC24MsgBoxId id) {
    NWC24Err result = NWC24_OK;
    u32 off;
    u32 oldest = 0;
    u32 differs;
    u32 foundId = 0;
    u32 count = 0;
    u32 foundOff = 0;
    NWC24Err removeErr;
    NWC24iMBCEntry *entry;

    if (id == NWC24_MSGBOX_SEND) {
        return NWC24_OK;
    }

    entry = &NWC24WorkP->mbcEntry;
    for (off = MBC_FIRST_ENTRY; off < hdr->fileSize; off += MBC_BLOCK_SIZE) {
        if (count >= hdr->numMsgs) {
            break;
        }

        result = NWC24FSeek(file, off, NWC24_SEEK_BEG);
        if (result != NWC24_OK) {
            break;
        }

        result = NWC24FRead(entry, MBC_BLOCK_SIZE, file);
        if (result != NWC24_OK) {
            break;
        }

        if (entry->id == 0) {
            continue;
        }

        count++;
        differs = 1;
        if (entry->id == msg->id) {
            differs = 0;
        }

        if ((entry->flags & msg->flags) & 1) {
            if (entry->appId != msg->appId) {
                differs = 0;
            }

            if (entry->from.id != msg->from.id) {
                differs = 0;
            }
        }

        if (differs) {
            if ((msg->tag & 0xFFFF) != 0 && (entry->tag & 0xFFFF) == (msg->tag & 0xFFFF)) {
                foundId = entry->id;
                foundOff = off;
                continue;
            }

            if (msg->unk1C != 0 && entry->unk1C == msg->unk1C && entry->flags == msg->flags &&
                entry->length == msg->length && entry->unk10 == msg->unk10 && entry->createTime == msg->createTime) {
                foundId = entry->id;
                foundOff = off;
                continue;
            }
        }

        if (oldest == 0 || CompareMsgId(oldest, entry->id) > 0) {
            oldest = entry->id;
        }
    }

    if (result != NWC24_OK) {
        return result;
    }

    hdr->oldestMsgId = oldest;
    if (foundId != 0) {
        removeErr = fn_8039CCC8(hdr, file, foundOff);
        result = MBoxRemoveMsgFile_i(id, foundId);
        if (removeErr != NWC24_OK) {
            result = removeErr;
        }
    }

    return result;
}

// the message object keeps strings/blobs as {offset, size} pairs, in the control file they are packed into
// one word (size in the top 12 bits, offset in the low 20)
static inline u32 PackData_i(const NWC24Data *data) {
    return (data->size << 20) | (data->offset & 0xFFFFF);
}

static inline void UnpackData_i(NWC24Data *data, u32 packed) {
    data->offset = packed & 0xFFFFF;
    data->size = packed >> 20;
}

// message object -> control file entry
NWC24Err fn_8039CF40(NWC24iMsgObj *msg, NWC24iMBCEntry *entry) {
    entry->id = msg->id;
    entry->flags = msg->flags;
    entry->length = msg->length;
    entry->appId = msg->appId;
    entry->unk10 = msg->unk10;
    entry->tag = msg->tag;
    entry->command = msg->command;
    entry->unk1C = msg->unk1C;
    entry->from = msg->from;
    entry->createTime = msg->createTime;
    entry->unk2C = msg->unk2C;
    entry->numTo = msg->numTo;
    entry->numAttached = msg->numAttached;
    entry->groupId = msg->groupId;
    entry->fromField = PackData_i(&msg->fromField);
    entry->toField = PackData_i(&msg->toField);
    entry->subject = PackData_i(&msg->subject);
    entry->contentType = PackData_i(&msg->contentType);
    entry->txEncoding = PackData_i(&msg->txEncoding);
    entry->text = msg->text;
    entry->attached[0] = msg->attached[0];
    entry->attached[1] = msg->attached[1];
    entry->attachedSize[0] = msg->attachedSize[0];
    entry->attachedSize[1] = msg->attachedSize[1];
    entry->attachedType[0] = msg->attachedType[0];
    entry->attachedType[1] = msg->attachedType[1];
    entry->dwcId = msg->dwcId;
    entry->unk78 = msg->iconNew;
    return NWC24_OK;
}

// control file entry -> message object
NWC24Err fn_8039D0EC(NWC24iMBCEntry *entry, NWC24iMsgObj *msg) {
    int i;

    msg->id = entry->id;
    msg->flags = entry->flags;
    msg->length = entry->length;
    msg->appId = entry->appId;
    msg->unk10 = entry->unk10;
    msg->tag = entry->tag;
    msg->command = entry->command;
    msg->unk1C = entry->unk1C;
    msg->from = entry->from;
    msg->createTime = entry->createTime;
    msg->unk2C = entry->unk2C;
    msg->numTo = entry->numTo;
    msg->numAttached = entry->numAttached;
    msg->groupId = entry->groupId;
    UnpackData_i(&msg->fromField, entry->fromField);
    UnpackData_i(&msg->toField, entry->toField);
    UnpackData_i(&msg->subject, entry->subject);
    UnpackData_i(&msg->contentType, entry->contentType);
    UnpackData_i(&msg->txEncoding, entry->txEncoding);
    msg->text = entry->text;
    msg->attached[0] = entry->attached[0];
    msg->attached[1] = entry->attached[1];
    msg->attachedSize[0] = entry->attachedSize[0];
    msg->attachedSize[1] = entry->attachedSize[1];
    msg->attachedType[0] = entry->attachedType[0];
    msg->attachedType[1] = entry->attachedType[1];
    msg->dwcId = entry->dwcId;
    msg->iconNew = entry->unk78;

    for (i = 0; i < NWC24i_MSG_RECIPIENT_MAX; i++) {
        msg->to[i].id = 0;
    }

    return NWC24_OK;
}

/* ---------------------------------------------------------------------------------------------
 * base64 / quoted printable
 * ------------------------------------------------------------------------------------------- */

// the alphabet, with the pad char on the end
static char *Base64Chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

// makes a lookup table that maps ascii to the 6 bit value (-1 for everything that is not part of the alphabet)
void NWC24InitBase64Table(char *table) {
    int i;

    for (i = 0; i < 256; i++) {
        table[i] = -1;
    }

    for (i = 0; i < 26; i++) {
        table['A' + i] = i;
    }

    for (i = 0; i < 26; i++) {
        table['a' + i] = 26 + i;
    }

    for (i = 0; i < 10; i++) {
        table['0' + i] = 52 + i;
    }

    table['+'] = 62;
    table['/'] = 63;
}

static NWC24Err fn_8039D30C(const u8 *data, int dataSize, int *dataRead, char *text, int textSize, int *textWritten) {
    NWC24Err result = NWC24_OK;
    int in = 0;
    int out = 0;

    for (; in < dataSize; in += 3) {
        u32 value;
        BOOL has1;
        BOOL has2;

        if (out + 3 >= textSize) {
            result = NWC24_ERR_OVERFLOW;
            break;
        }

        value = data[in];
        has1 = FALSE;
        has2 = FALSE;
        value <<= 8;
        if (in + 1 < dataSize) {
            value |= data[in + 1];
            has1 = TRUE;
        }

        value <<= 8;
        if (in + 2 < dataSize) {
            value |= data[in + 2];
            has2 = TRUE;
        }

        text[out] = Base64Chars[(value >> 18) & 0x3F];
        text[out + 1] = Base64Chars[(value >> 12) & 0x3F];
        text[out + 2] = Base64Chars[has1 ? (value >> 6) & 0x3F : 64];
        text[out + 3] = Base64Chars[has2 ? value & 0x3F : 64];
        out += 4;
    }

    *textWritten = out;
    if (dataRead != NULL) {
        *dataRead = in < dataSize ? in : dataSize;
    }

    return result;
}

NWC24Err NWC24Base64Encode(const u8 *data, s32 dataSize, char *text, s32 textSize, u32 *textWritten) {
    return fn_8039D30C(data, dataSize, NULL, text, textSize, (int *)textWritten);
}

// goes the other way, skips everything that is not in the alphabet
NWC24Err fn_8039D45C(const u8 *data, int dataSize, u8 *out, u32 outSize, u32 *outWritten) {
    char *table = NWC24WorkP->base64Work;
    int acc = 0;
    int bits = 0;
    u32 n = 0;
    int i;

    for (i = 0; i < dataSize; i++) {
        s8 value = table[*data];

        if (value >= 0 && value < 64) {
            bits += 6;
            acc = (acc << 6) | value;
            if (bits >= 8) {
                if (n >= outSize) {
                    *outWritten = outSize;
                    return NWC24_ERR_OVERFLOW;
                }

                bits -= 8;
                n++;
                *out++ = acc >> bits;
            }
        }

        data++;
    }

    *outWritten = n;
    return NWC24_OK;
}

// HYPOTHESIS: this is the inlined memcpy the compiler unrolled, the loop counter is a signed int
static inline void CopyBytes_i(char *dst, const char *src, int n) {
    int i;

    for (i = 0; i < n; i++) {
        *dst++ = *src++;
    }
}

// a hex digit as ascii, the compiler turned the modulo 10 into a multiply so it must be written like that
static inline int HexDigit_i(int digit) {
    if (digit >= 10) {
        return 'A' + digit % 10;
    }

    return '0' + digit % 10;
}

static NWC24Err fn_8039D7C4(char *text, u32 textSize, u32 *textWritten, const char *data, u32 dataSize,
                            u32 *dataRead, BOOL noSoftBreaks) {
    NWC24Err result = NWC24_OK;
    int lineLen;
    u32 written;
    u32 consumedTotal;
    char buf[4];

    if (text == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (data == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    if (dataSize == 0) {
        return NWC24_ERR_OVERFLOW;
    }

    if (dataRead != NULL) {
        *dataRead = 0;
    }

    if (textWritten != NULL) {
        *textWritten = 0;
    }

    lineLen = 0;
    written = 0;
    consumedTotal = 0;
    while (consumedTotal < dataSize) {
        int consumed;
        int out;
        BOOL softBreak = FALSE;
        char ch = *data;

        if (ch >= 0x21 && ch <= 0x7E && ch != '=') {
            buf[0] = ch;
            consumed = 1;
            out = 1;
        } else if (consumedTotal + 1 < dataSize && ch == '\r' && data[1] == '\n') {
            buf[0] = ch;
            consumed = 2;
            out = 1;
            buf[1] = data[1];
        } else {
            buf[0] = '=';
            buf[1] = HexDigit_i((u8)*data >> 4);
            buf[2] = HexDigit_i(*data & 0xF);
            consumed = 1;
            out = 3;
        }

        if (!noSoftBreaks && lineLen + out >= 75) {
            softBreak = TRUE;
            out += 3;
        }

        if (written + out >= textSize) {
            result = NWC24_ERR_OVERFLOW;
            break;
        }

        written += out;
        if (softBreak) {
            *text = '=';
            lineLen = 0;
            out -= 3;
            text[1] = '\r';
            text[2] = '\n';
            text += 3;
        }

        lineLen += out;
        CopyBytes_i(text, buf, out);
        text += out;
        consumedTotal += consumed;
        data += consumed;
    }

    if (dataRead != NULL) {
        *dataRead = consumedTotal;
    }

    if (textWritten != NULL) {
        *textWritten = written;
    }

    return result;
}

NWC24Err NWC24EncodeQuotedPrintable(char *text, u32 textSize, u32 *textWritten, const u8 *data, u32 dataSize,
                                    u32 *dataRead) {
    return fn_8039D7C4(text, textSize, textWritten, (const char *)data, dataSize, dataRead, FALSE);
}
