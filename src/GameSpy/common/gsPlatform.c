#include <revolution/OS/OSThread.h>
#include <types.h>
#include <revolution/OS/OSInterrupt.h>
#include <revolution/OS/OSTime.h>
#include <string.h>
#include <ctype.h>
#include <GameSpy/common/gsPlatform.h>
#include <GameSpy/common/gsCore.h>
#include <revolution/OS/OSMutex.h>

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

// TODO: clean up, give variables real names
void fn_8035D248(GSICriticalSection* theCrit) {
    OSInitMutex((OSMutex*)theCrit);
}

void fn_8035D24C(GSICriticalSection* theCrit) {
    OSLockMutex((OSMutex*)theCrit);
}

void fn_8035D250(GSICriticalSection* theCrit) {
    OSUnlockMutex((OSMutex*)theCrit);
}

void fn_8035D254(GSICriticalSection* theCrit) {}

void fn_8035D770(void) {
}

// TODO: clean up, give variables real names
s32 fn_8035DA30(s32 arg0, s32 arg1) {
    u32 value, product;
    u16 high;
    u32 range;
    range = arg1 - arg0;
    if (range != 0) {
        value = lbl_8059F598;
        high = value >> 16;
        value &= 0xffff;
        product = high * 0x41A7;
        value *= 0x41A7;
        value += (product << 16) & 0x7FFF0000;
        if (value > 0x7FFFFFFF) {
            value &= 0x7FFFFFFF;
            value++;
        }
        value += product >> 15;
        if (value > 0x7FFFFFFF) {
            value &= 0x7FFFFFFF;
            value++;
        }
        lbl_8059F598 = value;
        product = value % range;
        return product + arg0;
    }
    return arg0;
}

// TODO: clean up, give variables real names
/* jj_8035DAA4 */
extern u8 lbl_805A4980[4];
extern u8 lbl_805A4984[4];
extern u8 lbl_805A4988[8];
static inline void readChunk(u8 *src, u8 *buf, s32 len) {
    s32 i;
    for (i = 0; i < ((len < 3) ? len : 3); i++) {
        buf[i] = src[i];
    }
    for (; i < 3; i++) {
        buf[i] = 0;
    }
}
void fn_8035DAA4(u8 *arg0, u8 *arg1, s32 arg2, s32 arg3) {
    u8 *table;
    u8 *start;
    u8 *lim;
    s32 len;
    u8 c;
    u8 buf[3];
    start = arg1;
    len = arg2;
    switch (arg3) {
    case 1: table = lbl_805A4984; break;
    case 2: table = lbl_805A4988; break;
    default: table = lbl_805A4980; break;
    }
    while (len > 0) {
        readChunk(arg0, buf, len);
        arg1[0] = buf[0] >> 2;
        arg1[1] = ((buf[1] >> 4) | ((buf[0] & 3) << 4)) & 0x3F;
        arg1[2] = ((buf[2] >> 6) | ((buf[1] & 0xF) << 2)) & 0x3F;
        arg1[3] = buf[2] & 0x3F;
        arg0 += 3;
        len -= 3;
        arg1 += 4;
    }
    lim = arg1;
    if (arg2 % 3 == 1) lim = arg1 - 2;
    else if (arg2 % 3 == 2) lim = arg1 - 1;
    *arg1 = 0;
    while (arg1 > start) {
        arg1--;
        if (arg1 >= lim) *arg1 = table[2];
        else {
            c = *arg1;
            if ((s8)c <= 0x19) *arg1 = c + 0x41;
            else if ((s8)c <= 0x33) *arg1 = c + 0x47;
            else if ((s8)c <= 0x3D) *arg1 = c - 4;
            else if ((s8)c == 0x3E) *arg1 = table[0];
            else if ((s8)c == 0x3F) *arg1 = table[1];
        }
    }
}

// TODO: clean up, give variables real names
/* jj_8035D720 */
void SocketStartUp(void) {}

/* jj_8035D724 */
void SocketShutDown(void) {}

/* jj_8035CD7C */
extern int SOSetSockOpt(int a, int b, int c, const void *data, int length);
extern int lbl_805A1044;

int SetReceiveBufferSize(int s, int value) {
    int tmp;
    int result;

    tmp = value;
    result = SOSetSockOpt(s, 0xFFFF, 0x1002, &tmp, 4);
    result = (result >= 0) ? result : (lbl_805A1044 = result, -1);
    return result != -1;
}

/* jj_8035CDD8 */
extern int SOSetSockOpt(int a, int b, int c, const void *data, int length);
extern int lbl_805A1044;

int SetSendBufferSize(int s, int value) {
    int tmp;
    int result;
    tmp = value;
    result = SOSetSockOpt(s, 0xFFFF, 0x1001, &tmp, 4);
    result = (result >= 0) ? result : (lbl_805A1044 = result, -1);
    return result != -1;
}

/* jj_8035D364 */
typedef struct Allocator {
    void *(*alloc)(unsigned int);
    void (*free)(void *);
    unsigned char rest[0x170];
} Allocator;
typedef struct Context {
    char *name;
    int result;
    int active;
    int unknown;
    OSThread thread;
    char *stack;
    int tail;
} Context;
#define lbl_80534638 (*(Allocator*)&lbl_80534638)
extern void *fn_8035D2A8(void *);
int fn_8035D364(const char *name, Context **out)
{
    Context *p;
    char *copy;
    p = lbl_80534638.alloc(0x330);
    if (p == 0)
        return -1;
    if (name == 0) {
        copy = 0;
    } else {
        copy = lbl_80534638.alloc(strlen(name) + 1);
        if (copy != 0)
            strcpy(copy, name);
    }
    p->name = copy;
    if (copy == 0) {
        if (p != 0)
            lbl_80534638.free(p);
        return -1;
    }
    p->active = 0;
    p->stack = lbl_80534638.alloc(0x1000);
    OSCreateThread(&p->thread, fn_8035D2A8, p, p->stack + 0x1000, 0x1000, 0x10, 1);
    OSResumeThread(&p->thread);
    *out = p;
    return 0;
}
#undef lbl_80534638

