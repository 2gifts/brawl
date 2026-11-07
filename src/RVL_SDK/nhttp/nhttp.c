#include <types.h>
#include <stdio.h>
#include <string.h>

int printf(const char*, ...);
#include <revolution/OS/OSMessage.h>
#include <revolution/OS/OSMutex.h>
#include <revolution/NHTTP/d_nhttp.h>

// The original SDK built these as separate objects, so calls between them stay
// out of line. Inlining a helper into a caller changes code that already matched.
#pragma auto_inline off

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_805365E0[0x40] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_80536620[0xca0] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A1148[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1150[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A1154[0x4] __attribute__((aligned(4))) = { 0 };

// Function bodies below are taken from https://github.com/jj-link/brawl.

void NHTTPi_kickCommThread(OSMessageQueue* queue) {
    OSSendMessage(queue, 0, 0);
}

void NHTTPi_InitRequestInfo(int *p) {
    *p = 0;
}

void* NHTTPi_memclr(void*, int);
void* NHTTPi_memcpy(void*, const void*, int);
extern int SOSend(int, void *, unsigned int, int);

int NHTTPi_SocSend_sub(int a, char *buf, unsigned int len, int d) {
    char tmp[32] __attribute__((aligned(32)));
    unsigned int head = (unsigned int)buf & 31 ? 32 - ((unsigned int)buf & 31) : 0;
    int total = 0;
    unsigned int n;
    int r;
    NHTTPi_memclr(tmp, 32);
    if (head != 0) {
        if (head > len) head = len;
        NHTTPi_memcpy(tmp, buf, head);
        r = SOSend(a, tmp, head, d);
        if (r > 0) {
            total = r;
            if ((unsigned int)r < head) return r;
            buf += r;
            len -= r;
        } else return r;
    }
    if ((int)len > 0 && (n = len & ~31) != 0) {
        r = SOSend(a, buf, n, d);
        if (r > 0) {
            total += r;
            if ((unsigned int)r < n) return total;
            buf += r;
            len -= r;
        } else {
            if (total > 0) return total;
            return r;
        }
    }
    if ((int)len > 0 && (n = len & 31) != 0) {
        NHTTPi_memclr(tmp, 32);
        NHTTPi_memcpy(tmp, buf, n);
        r = SOSend(a, tmp, n, d);
        if (r > 0) {
            total += r;
        } else {
            if (total > 0) return total;
            return r;
        }
    }
    return total;
}

int NHTTPi_encodeUrlChar(unsigned char *out, s8 in)
{
    s32 hi, lo;
    if (in == 0x20) {
        *out = 0x2B;
        return 1;
    }
    if ((in >= 0x30 && in <= 0x39) || (in >= 0x41 && in <= 0x5A) || (in >= 0x61 && in <= 0x7A)) {
        *out = (unsigned char)in;
        return 1;
    }
    hi = ((u32)in >> 4) & 0xF;
    *out = '%';
    lo = in & 0xF;
    out[1] = (hi < 0x0A) ? (hi + 0x30) : (hi + 0x37);
    out[2] = (lo < 0x0A) ? (lo + 0x30) : (lo + 0x37);
    return 3;
}

s32 NHTTPi_strToInt(u8 *arg0, s32 arg1) {
    s32 val;
    s32 seen;
    s32 prev;
    s32 i;
    u8 *p;
    s8 c;

    if (arg1 > 0xA) {
        return -1;
    }
    val = 0;
    seen = 0;
    p = arg0;
    for (i = arg1; i > 0; i--, p++) {
        c = (s8)*p;
        if (seen != 0) {
            if (c == 0x20 || c == 0) {
                break;
            }
        }
        if (seen == 0 && c == 0x20) {
            continue;
        }
        if (c < 0x30 || c > 0x39) {
            return -1;
        }
        prev = val;
        seen = 1;
        val = (c + val * 0xA) - 0x30;
        if (prev > val) {
            return -1;
        }
    }
    return val;
}

int NHTTPi_strtonum(unsigned char *arg0, int arg1) {
    int value;
    int count = 0;
    value = 0;
    for (; arg1; arg1--) {
        if ((char)*arg0 != ' ' && ((char)*arg0 >= '0') & ((char)*arg0 <= '9')) {
            count++;
            value = (char)*arg0 + value * 10 - '0';
            if (count > 9) return -1;
        }
        arg0++;
    }
    if (count != 0) return value;
    return -1;
}

NHTTPRequestHandle* fn_8039651C(const char* url, NHTTPReqMethod method, char* buffer, size_t bufferSize, NHTTPReqCallback callback, void* userContext, int a, int b);

NHTTPRequestHandle* NHTTPCreateRequest(const char* url, NHTTPReqMethod method, char* buffer, size_t bufferSize, NHTTPReqCallback callback, void* userContext) {
    return fn_8039651C(url, method, buffer, bufferSize, callback, userContext, 0, 0);
}

int NHTTPi_ControlConnectionList(int, int, int);

void* NHTTPi_Request2Connection(int a, int b) {
    return (void*)NHTTPi_ControlConnectionList(a, b, 1);
}

int NHTTPi_ControlConnectionList(int a, int b, int c);

void* NHTTPi_Response2Connection(int a, int b) {
    return (void*)NHTTPi_ControlConnectionList(a, b, 2);
}

int NHTTPi_ControlConnectionList(int a, int b, int c);

void* NHTTPi_GetConnection(int a, int b) {
    return (void*)NHTTPi_ControlConnectionList(a, b, 0);
}

// HYPOTHESIS: begin/end block inside the NHTTP system object. Offsets are the ones the stores use.
typedef struct NHTTPBgnEndInfo {
    u8 pad[0x7c4];
    void* alloc;     // HYPOTHESIS: allocator callback
    void* freeFn;    // HYPOTHESIS: free callback
    void* unk_7cc;
    s32 unk_7d0;
    s32 sslError;
    s32 error;
    s32 shutdown;
    s32 unk_7e0;
    s32 unk_7e4;
} NHTTPBgnEndInfo;

// HYPOTHESIS: OSMutex followed by an initialized flag. OSMutex is 0x18 bytes.
typedef struct NHTTPReqListLock {
    OSMutex mutex;
    s32 initialized;
} NHTTPReqListLock;

// HYPOTHESIS: message queue at the start, OSThread embedded at 0x30.
typedef struct NHTTPCommThread {
    OSMessageQueue queue;
    u8 pad[0x10];
    OSThread thread;
} NHTTPCommThread;

// HYPOTHESIS: flag at 0x348 of the thread-info block.
typedef struct NHTTPThreadInfo {
    u8 pad[0x348];
    s32 createMessageQueue;
} NHTTPThreadInfo;

// HYPOTHESIS: two list pointers, head then tail.
typedef struct NHTTPListInfo {
    void* head;
    void* tail;
} NHTTPListInfo;

// HYPOTHESIS: virtual content length at +0x2c.
typedef struct NHTTPVirtLen {
    u8 pad[0x2c];
    s32 virtualContentLength;
} NHTTPVirtLen;

// HYPOTHESIS: intrusive circular list node. The list pointer addresses the tail.
typedef struct NHTTPLink {
    struct NHTTPLink* next;
    struct NHTTPLink* prev;
    void* data;
} NHTTPLink;

// HYPOTHESIS: request state at +4; header list lives at +0x30.
typedef struct NHTTPReqHeader {
    u8 pad[4];
    s32 state;
} NHTTPReqHeader;

// HYPOTHESIS: connection list node; link to the next node is at +0x20.
typedef struct NHTTPConnNode {
    u8 pad[0x20];
    struct NHTTPConnNode* next;
} NHTTPConnNode;

void* NHTTPi_GetSystemInfoP(void);
void NHTTPi_CleanupAsync(void* sys, void* callback);
void* NHTTPi_CommThreadProcMain(void* arg);
int addHdrList(void* list, const char* name, const char* value);
struct NHTTPLink* NHTTPi_getHdrFromList(struct NHTTPLink** list);

void* NHTTPi_memcpy(void* dst, const void* src, int n) {
    return memcpy(dst, src, n);
}

unsigned long NHTTPi_strlen(const char* s) {
    return strlen(s);
}

int NHTTPi_strcmp(const char* a, const char* b) {
    return strcmp(a, b);
}

void* NHTTPi_memclr(void* dst, int n) {
    return memset(dst, 0, n);
}

NHTTPLink* NHTTPi_getReqFromReqQueue(NHTTPLink** list) {
    return NHTTPi_getHdrFromList(list);
}

void NHTTPi_exitLockReqList(void) {
}

void NHTTPi_lockReqList(OSMutex* mutex) {
    OSLockMutex(mutex);
}

void NHTTPi_unlockReqList(OSMutex* mutex) {
    OSUnlockMutex(mutex);
}

void* NHTTPi_GetBgnEndInfoP(void* sys) {
    return sys;
}

void NHTTPi_SetError(NHTTPBgnEndInfo* info, s32 err) {
    info->error = err;
}

void NHTTPi_SetSSLError(NHTTPBgnEndInfo* info, s32 err) {
    info->sslError = err;
}

s32 NHTTPi_GetSSLError(NHTTPBgnEndInfo* info) {
    return info->sslError;
}

s32 NHTTPi_IsCreateCommThreadMessageQueue(NHTTPThreadInfo* info) {
    return info->createMessageQueue;
}

void* NHTTPi_GetListInfoP(void* sys) {
    return (u8*)sys + 0x800;
}

void* NHTTPi_GetReqInfoP(void* sys) {
    return (u8*)sys + 0x808;
}

void* NHTTPi_GetThreadInfoP(void* sys) {
    return (u8*)sys + 0x840;
}

void* NHTTPi_GetMutexInfoP(void* sys) {
    return (u8*)sys + 0x80c;
}

void NHTTPi_InitMutexInfo(NHTTPReqListLock* info) {
    info->initialized = 0;
}

void NHTTPi_InitThreadInfo(NHTTPThreadInfo* info) {
    info->createMessageQueue = 0;
}

void NHTTPi_IsCreateCommThreadMessageQueueOn(NHTTPThreadInfo* info) {
    info->createMessageQueue = 1;
}

void NHTTPi_InitListInfo(NHTTPListInfo* list) {
    list->head = 0;
    list->tail = 0;
}

void NHTTPi_SetVirtualContentLength(NHTTPVirtLen* obj, s32 len) {
    if (obj == 0) {
        return;
    }
    obj->virtualContentLength = len;
}

s32 NHTTPi_GetVirtualContentLength(NHTTPVirtLen* obj) {
    if (obj != 0) {
        return obj->virtualContentLength;
    }
    return 0;
}

void NHTTPi_InitBgnEndInfo(NHTTPBgnEndInfo* info) {
    info->sslError = 0;
    info->error = 0;
    info->alloc = 0;
    info->freeFn = 0;
    info->unk_7cc = 0;
    info->unk_7d0 = -1;
    info->shutdown = 0;
    info->unk_7e0 = 0;
    info->unk_7e4 = 0;
}

int NHTTPi_GetConnectionListLength(void) {
    NHTTPConnNode* node = *(NHTTPConnNode**)lbl_805A1150;
    int count = 0;
    while (node != 0) {
        node = node->next;
        count++;
    }
    return count;
}

int NHTTPi_CommitConnectionList(int a, int b) {
    // HYPOTHESIS: -1 when the list update returns 0, else 0.
    int v = NHTTPi_ControlConnectionList(a, b, 3);
    return ~(v | -v) >> 31;
}

int NHTTPi_OmitConnectionList(int a, int b) {
    // HYPOTHESIS: -1 when the list update returns 0, else 0.
    int v = NHTTPi_ControlConnectionList(a, b, 4);
    return ~(v | -v) >> 31;
}

void* NHTTPi_Connection2Request(int a, int b) {
    u8* link = (u8*)NHTTPi_ControlConnectionList(a, b, 0);
    if (link != 0) {
        return *(void**)(link + 0x10);
    }
    return 0;
}

void* NHTTPi_Connection2Response(int a, int b) {
    u8* link = (u8*)NHTTPi_ControlConnectionList(a, b, 0);
    if (link != 0) {
        return *(void**)(link + 0x14);
    }
    return 0;
}

void fn_803962D4(void* callback) {
    NHTTPi_CleanupAsync(NHTTPi_GetSystemInfoP(), callback);
}

s32 fn_80396308(void) {
    return NHTTPi_GetSSLError(NHTTPi_GetSystemInfoP());
}

void NHTTPi_initLockReqList(NHTTPReqListLock* info) {
    if (info->initialized == 0) {
        OSInitMutex(&info->mutex);
        info->initialized = 1;
    }
}

void NHTTPi_destroyCommThread(NHTTPCommThread* thread, NHTTPBgnEndInfo* info) {
    info->shutdown = 1;
    OSSendMessage(&thread->queue, 0, 0);
    OSJoinThread(&thread->thread, 0);
}

void NHTTPi_idleCommThread(OSMessageQueue* queue) {
    OSMessage msg;
    OSReceiveMessage(queue, &msg, OS_MSG_BLOCKING);
}

void* NHTTPi_CommThreadProc(void* arg) {
    NHTTPi_CommThreadProcMain(arg);
    return 0;
}

int NHTTP_AddHeaderField(NHTTPReqHeader* req, const char* name, const char* value) {
    if (req->state != 0) {
        return 0;
    }
    return addHdrList((u8*)req + 0x30, name, value);
}

void NHTTPi_free(void* ptr) {
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP();
    void (*freeFn)(void*) = info->freeFn;
    if (freeFn != 0) {
        freeFn(ptr);
    }
}

void* NHTTPi_alloc(u32 size, s32 align) {
    NHTTPBgnEndInfo* info = (NHTTPBgnEndInfo*)NHTTPi_GetSystemInfoP();
    void* (*alloc)(u32, s32) = info->alloc;
    if (alloc != 0) {
        return alloc(size, align);
    }
    return 0;
}

NHTTPLink* NHTTPi_getHdrFromList(NHTTPLink** list) {
    NHTTPLink* node = *list;
    if (node != 0) {
        if (node != node->next) {
            node->next->prev = node->prev;
            node->prev->next = node->next;
            *list = node->prev;
        } else {
            *list = 0;
        }
    }
    return node;
}

void* NHTTPi_GetRequest(int a, int b) {
    u8* link = (u8*)NHTTPi_ControlConnectionList(a, b, 0);
    if (link != 0) {
        return *(void**)(link + 0x10);
    }
    return (void*)b;
}

void* NHTTPi_GetResponse(int a, int b) {
    u8* link = (u8*)NHTTPi_ControlConnectionList(a, b, 0);
    if (link != 0) {
        return *(void**)(link + 0x14);
    }
    return (void*)b;
}

void* NHTTPi_GetSystemInfoP(void) {
    if (*(s32*)lbl_805A1154 == 0) {
        u8* sys = lbl_80536620;
        *(u8**)lbl_805A1154 = sys;
        NHTTPi_InitBgnEndInfo((NHTTPBgnEndInfo*)sys);
        NHTTPi_InitListInfo((NHTTPListInfo*)(sys + 0x800));
        NHTTPi_InitRequestInfo((int*)(sys + 0x808));
        NHTTPi_InitMutexInfo((NHTTPReqListLock*)(sys + 0x80c));
        NHTTPi_InitThreadInfo((NHTTPThreadInfo*)(sys + 0x840));
    }
    return *(void**)lbl_805A1154;
}

void NHTTPi_InitConnectionList(void) {
    NHTTPConnNode* node = *(NHTTPConnNode**)lbl_805A1150;
    if (node != 0) {
        int count = 0;
        while (node != 0) {
            node = node->next;
            count++;
        }
        if (count != 0) {
            printf("*warning: %d connections rests! Please free connections.\n", count);
        }
    }
    *(s32*)lbl_805A1150 = 0;
}

int NHTTP_CancelRequestAsync(void* sys, int conn);

int fn_803967D0(int conn) {
    return NHTTP_CancelRequestAsync(NHTTPi_GetSystemInfoP(), conn) ? 0 : -1;
}

int NHTTPGetConnectionError(int id) {
    u8* conn = (u8*)NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    if (conn != 0) {
        return *(s32*)(conn + 4);
    }
    return -1;
}

int NHTTPGetConnectionStatus(int id) {
    u8* conn = (u8*)NHTTPi_GetConnection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    if (conn == 0) {
        return -1;
    }
    return *(s32*)conn;
}

int NHTTP_GetProgress(void* sys, int a, int b);

int NHTTPGetProgress(int a, int b) {
    return NHTTP_GetProgress(NHTTPi_GetSystemInfoP(), a, b) ? 0 : -1;
}

int NHTTPGetResultCode(NHTTPResponseHandle id) {
    u8* resp = (u8*)NHTTPi_GetResponse((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)id);
    if (resp == 0) {
        return -1;
    }
    if (*(s32*)(resp + 0x14) != 0) {
        return *(s32*)(resp + 0x18);
    }
    return -1;
}

int NHTTPGetBodyBuffer(void* conn, void* out, int* local);

int NHTTPGetBodyAll(NHTTPResponseHandle id, char** out) {
    void* conn = NHTTPi_Response2Connection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)id);
    int local;
    if (conn != 0) {
        local = 0;
        return NHTTPGetBodyBuffer(conn, out, &local);
    }
    return -1;
}

int NHTTPStartConnection(void* conn);

int NHTTPSendRequestAsync(NHTTPRequestHandle req) {
    u8* conn = (u8*)NHTTPi_Request2Connection((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), (int)req);
    if (conn != 0 && NHTTPStartConnection(conn) == 0) {
        return *(s32*)(conn + 0x18);
    }
    return -1;
}

void NHTTPi_freeReqQueue(NHTTPLink** list, int arg, void* data);

void NHTTPi_allFreeReqQueue(NHTTPLink** list, int arg) {
    while (*list != 0) {
        NHTTPi_freeReqQueue(list, arg, (*list)->data);
    }
}

int fn_80396A18(int id, s32 option) {
    u8* req = (u8*)NHTTPi_GetRequest((int)NHTTPi_GetMutexInfoP(NHTTPi_GetSystemInfoP()), id);
    if (req == 0) {
        return -1;
    }
    *(s32*)(req + 0xcc) = option;
    return 0;
}
