#include "revolution/ipc.h"

// tiny bookkeeping for outstanding ipc requests (looks like a debug/profiling aid:
// nothing here ever reads the tables back except to find a free slot)
#define IPC_PROF_SLOTS 32

static s32 IpcNumUnIssuedReqs;
static s32 IpcNumPendingReqs;
static void *IpcReqPtrArray[IPC_PROF_SLOTS];
static s32 IpcFdArray[IPC_PROF_SLOTS];

void IPCiProfInit(void) {
    s32 i;

    IpcNumPendingReqs = 0;
    IpcNumUnIssuedReqs = 0;

    for (i = 0; i < IPC_PROF_SLOTS; i++) {
        IpcReqPtrArray[i] = NULL;
        IpcFdArray[i] = -1;
    }
}

static void AddReqInfo(void *req, s32 fd);

void IPCiProfQueueReq(void *req, s32 fd) {
    IpcNumPendingReqs++;
    IpcNumUnIssuedReqs++;
    AddReqInfo(req, fd);
}

// A separate helper in the SDK (inlined; its own counter is what fixes the register order).
static void AddReqInfo(void *req, s32 fd) {
    u32 i;

    for (i = 0; i < IPC_PROF_SLOTS; i++) {
        if (IpcReqPtrArray[i] == NULL && IpcFdArray[i] == -1) {
            IpcReqPtrArray[i] = req;
            IpcFdArray[i] = fd;
            return;
        }
    }
}

void IPCiProfAck(void) {
    IpcNumUnIssuedReqs--;
}

void IPCiProfReply(void *req, s32 fd) {
    s32 i;

    IpcNumPendingReqs--;

    for (i = 0; i < IPC_PROF_SLOTS; i++) {
        if (req == IpcReqPtrArray[i] && fd == IpcFdArray[i]) {
            IpcReqPtrArray[i] = NULL;
            IpcFdArray[i] = -1;
            return;
        }
    }
}
