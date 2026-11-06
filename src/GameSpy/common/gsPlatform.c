#include <types.h>
#include <revolution/OS/OSInterrupt.h>
#include <revolution/OS/OSTime.h>
#include <string.h>
#include <ctype.h>
#include <GameSpy/common/gsPlatform.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
gsPlatformData lbl_80534638 = { 0 };
unsigned char lbl_805347B0[0x18] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_805347C8[0x40] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1038[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A103C[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1040[0x4] __attribute__((aligned(4))) = { 0 };
int lbl_805A1044 = 0;

// Function bodies below are derived from https://github.com/jj-link/brawl.

extern int SOInetAtoN(const char* name, u32* addr);

u32 inet_addr(const char* name) {
    u32 local;
    int result = SOInetAtoN(name, &local);
    if (result == 0) {
        return -1;
    }
    return local;
}

s32 gsiInterlockedDecrement(s32 *arg0)
{
    BOOL enabled = OSDisableInterrupts();
    s32 old = *arg0;
    s32 result = old - 1;
    *arg0 = result;
    OSRestoreInterrupts(enabled);
    return result;
}

int socket(int domain, int type, int protocol) {
    int result = SOSocket(domain, type, 0);

    if (result >= 0) {
        return result;
    }
    lbl_805A1044 = result;
    return -1;
}

int closesocket(int s) {
    int result = SOClose(s);

    if (result >= 0) {
        return result;
    }
    lbl_805A1044 = result;
    return -1;
}

int shutdown(int s, int how) {
    int result = SOShutdown(s, how);

    if (result >= 0) {
        return result;
    }
    lbl_805A1044 = result;
    return -1;
}

int listen(int s, int backlog) {
    int result = SOListen(s, backlog);

    if (result >= 0) {
        return result;
    }
    lbl_805A1044 = result;
    return -1;
}

int recv(int s, char* buf, int len, int flags) {
    int result = SORecv(s, buf, len, 4);

    if (result >= 0) {
        return result;
    }
    lbl_805A1044 = result;
    return -1;
}

int send(int s, const char* buf, int len, int flags) {
    int result = SOSend(s, buf, len, flags);

    if (result >= 0) {
        return result;
    }
    lbl_805A1044 = result;
    return -1;
}

int sendto(int s, const char* buf, int len, int flags, const void* to, int tolen) {
    u8 addr[8];
    int result;

    memcpy(addr, to, 8);
    addr[0] = tolen;
    result = SOSendTo(s, buf, len, flags, addr);
    if (result >= 0) {
        return result;
    }
    lbl_805A1044 = result;
    return -1;
}

int connect(int s, const void* to, int tolen) {
    u8 addr[8];
    int result;

    memcpy(addr, to, 8);
    addr[0] = tolen;
    result = SOConnect(s, addr);
    if (result >= 0) {
        return result;
    }
    lbl_805A1044 = result;
    return -1;
}

int GetReceiveBufferSize(int sock) {
    int size;
    int len = 4;
    int result = SOGetSockOpt(sock, 0xFFFF, 0x1002, &size, &len);

    int status;

    if (result >= 0) {
        status = result;
    } else {
        lbl_805A1044 = result;
        status = -1;
    }
    if (status == -1) {
        return -1;
    }
    return size;
}

int GetSendBufferSize(int sock) {
    int size;
    int len = 4;
    int result = SOGetSockOpt(sock, 0xFFFF, 0x1001, &size, &len);

    int status;

    if (result >= 0) {
        status = result;
    } else {
        lbl_805A1044 = result;
        status = -1;
    }
    if (status == -1) {
        return -1;
    }
    return size;
}

int GOAGetLastError(int s) {
    return lbl_805A1044;
}

int CanReceiveOnSocket(int sock) {
    int readFlag = 0;

    if (fn_8035C660(sock, &readFlag, NULL, NULL) == 1) {
        return readFlag;
    }
    return 0;
}

int CanSendOnSocket(int sock) {
    int writeFlag = 0;

    if (fn_8035C660(sock, NULL, &writeFlag, NULL) == 1) {
        return writeFlag;
    }
    return 0;
}

u32 gsiInterlockedIncrement(u32* value) {
    BOOL enabled = OSDisableInterrupts();
    u32 result = *value + 1;

    *value = result;
    OSRestoreInterrupts(enabled);
    return result;
}

void gsiTimeInSec(s64* t) {
    s64 seconds = OS_TICKS_TO_SEC(OSGetTime());

    if (t != NULL) {
        *t = seconds;
    }
}

static inline void gsiFreeThreadMemory(void* ptr) {
    if (ptr != NULL) {
        lbl_80534638.freeCB(ptr);
    }
}


// HYPOTHESIS: the state of a hostname lookup thread.
typedef struct GSIResolveHostnameInfo {
    char* mHostname;      // 0x0
    u32 mIP;              // 0x4
    int mFinished;        // 0x8
    GSIThread mThread;    // 0xC
} GSIResolveHostnameInfo;

void gsiCancelResolvingHostname(GSIResolveHostnameInfo* info) {
    GSIThread copy = info->mThread;

    OSCancelThread(&copy.mThread);
    if (copy.mStack != NULL) {
        gsiFreeThreadMemory(copy.mStack);
        copy.mStack = NULL;
    }
    gsiFreeThreadMemory(info->mHostname);
    info->mHostname = NULL;
    gsiFreeThreadMemory(info);
}

// HYPOTHESIS: the Wii socket fcntl, variadic like the SO library's.
int SOFcntl(int s, int cmd, ...);
u32 SONtoHl(u32 netlong);

int SetSockBlocking(int s, int blocking) {
    int flags = SOFcntl(s, 3, 0);

    if (blocking != 0) {
        flags &= ~4;
    } else {
        flags |= 4;
    }
    if (SOFcntl(s, 4, flags) == 0) {
        return 1;
    }
    return 0;
}

int IsPrivateIP(u32* addr) {
    u32 ip = SONtoHl(*addr);
    int first = ip >> 24;
    int second = (ip >> 16) & 0xFF;

    if (first == 10) {
        return 1;
    }
    if (first == 172 && second >= 16 && second <= 31) {
        return 1;
    }
    if (first == 192 && second == 168) {
        return 1;
    }
    return 0;
}

int Util_RandInt(int low, int high) {
    u32 range = high - low;
    u32 seed;
    u32 hi;
    u32 lo;
    u32 a;
    u32 b;

    if (range == 0) {
        return low;
    }
    seed = lbl_8059F598;
    hi = seed >> 16;
    lo = seed & 0xFFFF;
    a = hi * 16807;
    b = lo * 16807;
    seed = b + ((a & 0x7FFF) << 16);
    if (seed > 0x7FFFFFFF) {
        seed = (seed & 0x7FFFFFFF) + 1;
    }
    seed += a >> 15;
    if (seed > 0x7FFFFFFF) {
        seed = (seed & 0x7FFFFFFF) + 1;
    }
    lbl_8059F598 = seed;
    return low + seed % range;
}

int fn_8035D560(GSIThreadID* thread) {
    GSIThread copy;
    int result;

    if (!thread->mIsRunning) {
        return 0;
    }
    result = thread->mResult;
    copy = thread->mThread;
    if (!OSIsThreadTerminated(&copy.mThread)) {
        OSCancelThread(&copy.mThread);
    }
    if (copy.mStack != NULL) {
        gsiFreeThreadMemory(copy.mStack);
        copy.mStack = NULL;
    }
    gsiFreeThreadMemory(thread->mUserData);
    gsiFreeThreadMemory(thread);
    return result;
}

u32 current_time(void) {
    return OS_TICKS_TO_MSEC(OSGetTime());
}

char* goastrdup(const char* str) {
    char* copy;

    if (str == NULL) {
        return NULL;
    }
    copy = (char*)lbl_80534638.mallocCB(strlen(str) + 1);
    if (copy != NULL) {
        strcpy(copy, str);
    }
    return copy;
}

char* _strlwr(char* str) {
    char* start = str;

    while (*str != 0) {
        *str = tolower(*str);
        str++;
    }
    return start;
}

void B64InitEncodeStream(B64StreamData* stream, int data, int length, int type) {
    stream->unk0 = data;
    stream->unk4 = length;
    stream->unk8 = type;
}

void msleep(u32 msec) {
    OSSleepTicks(OS_MSEC_TO_TICKS((s64)msec));
}

void Util_RandSeed(int seed) {
    int value = 1;

    if (seed != 0) {
        value = seed & 0x7FFFFFFF;
    }
    lbl_8059F598 = value;
}

void gsiMemoryCallbacksSet(gsMallocCB p_malloc, gsFreeCB p_free, gsReallocCB p_realloc, gsMemalignCB p_memalign) {
    lbl_80534638.mallocCB = p_malloc;
    lbl_80534638.freeCB = p_free;
    lbl_80534638.reallocCB = p_realloc;
    lbl_80534638.memalignCB = p_memalign;
}

void* gsimalloc(u32 size) {
    return lbl_80534638.mallocCB(size);
}

void* gsirealloc(void* ptr, u32 size) {
    return lbl_80534638.reallocCB(ptr, size);
}

void gsifree(void* ptr) {
    if (ptr != NULL) {
        lbl_80534638.freeCB(ptr);
    }
}
