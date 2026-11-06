// WiiConnect24 standard APIs, configuration, library state and message objects.
// The scheduler, calendar, friends, IDs, time, IPC, downloads and shutdown
// and mailbox, MIME, commit and read code have separate SDK source files.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: the original is a bunch of separate source files, here they share one TU so mwcc would
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

static inline int StrLen(const char *str);

char *Mail_strncat(char *dst, const char *src, size_t maxlen) {
    int len = StrLen(dst);
    int i;
    for (i = 0; i < maxlen && src[i] != 0; i++) {
        dst[len + i] = src[i];
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
        str++;
        n++;
    }

    return n;
}

static inline void StrCat(char *dst, const char *src) {
    const char *p = dst;
    int n = 0;
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

static int fn_80397878(char *buf, u32 value, int base, char hexOffset, int isSigned, int width, char padCh,
                       char flag) {
    int written;
    int count;
    int neg;
    char digit;

    if (isSigned && (value & 0x80000000)) {
        value = -value;
        neg = 1;
    } else {
        neg = 0;
    }

    written = count = 0;

    while (value != 0) {
        digit = value % base;
        value /= base;

        PutCharFront(buf, digit + (char)(digit > 9 ? hexOffset : '0'));
        count++;
        written++;
    }

    if (*buf == 0) {
        PutCharFront(buf, '0');
        count++;
        written++;
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
    int isSigned;
    int base;
    int width;
    char flag;
    char padCh;
    char ch;
    int hexOffset;
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
                    width = ch + width * 10 - '0';
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
            case 'c': {
                char value = va_arg(args, int);
                PutCharBack(buf++, value);
                total++;
                break;
            }
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

// Checks the decoded user ID against the SDK CRC polynomial.
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
    NWC24Err scheduler;

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
    scheduler = fn_8039A53C((s32 *)&sp08, mode);
    if (scheduler < 0) {
        GlobalErrorCode = sp08;
        result = scheduler;
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
    s8 d0, d1, d2, d3, d4, d5;
    s32 n;
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

/* ---------------------------------------------------------------------------------------------
 * message object helpers + message list
 * ------------------------------------------------------------------------------------------- */

// how big is a base64 part: 4 chars per 3 bytes, a CRLF every 76 chars and the closing line

NWC24Err NWC24SetMsgAttached(NWC24MsgObj *pMsg, const char *data, u32 size, NWC24MIMEType type) {
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;
    u32 total = 0;
    u32 i;

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    if (data == NULL || size == 0) {
        return NWC24_ERR_NULL;
    }

    if (msg->numAttached >= NWC24i_MSG_ATTACHMENT_MAX) {
        return NWC24_ERR_FULL;
    }

    if (NWC24GetMIMETypeStr(type) == NULL) {
        return NWC24_ERR_INVALID_VALUE;
    }

    // messages to the public can only carry the plain types
    if (msg->flags & NWC24_MSGOBJ_FOR_PUBLIC) {
        if (type != NWC24_TXT_PLAIN && type != NWC24_TXT_HTML && type != NWC24_IMG_JPEG && type != NWC24_APP_OCTET_STREAM) {
            return NWC24_ERR_NOT_SUPPORTED;
        }
    }

    if (type == NWC24_APP_WII_MINIDATA && size > 0x80) {
        return NWC24_ERR_INVALID_VALUE;
    }

    for (i = 0; i < msg->numAttached; i++) {
        total += msg->attachedSize[i];
    }

    total += size;
    if (total >= 0x245B0) {
        return NWC24_ERR_OVERFLOW;
    }

    NWC24Data_SetDataP(&msg->attached[msg->numAttached], data, size);
    msg->attachedSize[msg->numAttached] = size;
    msg->attachedType[msg->numAttached] = type;
    msg->numAttached++;
    msg->flags |= NWC24_MSGOBJ_MULTIPART;
    return NWC24_OK;
}

NWC24Err fn_8039B160(const NWC24iMsgObj *msg, u32 *count) {
    NWC24Err result = NWC24_OK;
    u32 n = msg->numAttached;

    *count = n;
    if (n > NWC24i_MSG_ATTACHMENT_MAX) {
        result = NWC24_ERR_BROKEN;
    }

    return result;
}

NWC24Err fn_8039B17C(const NWC24iMsgObj *msg, u32 idx, u32 *size) {
    if (idx >= NWC24i_MSG_ATTACHMENT_MAX || idx >= msg->numAttached) {
        return NWC24_ERR_INVALID_VALUE;
    }

    *size = msg->attachedSize[idx];
    return NWC24_OK;
}

NWC24Err fn_8039B1B0(u32 *src, u32 *dst) {
    *dst = *src;
    return NWC24_OK;
}
