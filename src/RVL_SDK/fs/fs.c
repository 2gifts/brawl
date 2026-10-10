#include "revolution/os.h"
#include "revolution/ipc.h"
#include "revolution/fs.h"
#include "private/iostypes.h"
#include "private/iosrestypes.h"
#include "private/iosresclt.h"
#include <cstring>

// ISFS: thin client for the IOS "/dev/fs" resource (nand filesystem).
// only the subset of functions that actually ended up in brawl's dol is here, the rest of the
// library (format, rename, version control, ...) was stripped by the linker.

// HYPOTHESIS: ioctl numbers below. 2/3/4/5/6/7/0xB/0xC/0xD are pinned by the asm,
// the names are my best guess at which is which.
#define ISFS_IOCTL_STATS       2
#define ISFS_IOCTL_DCREATE     3
#define ISFS_IOCTL_DREAD       4
#define ISFS_IOCTL_SETATTR     5
#define ISFS_IOCTL_GETATTR     6
#define ISFS_IOCTL_DEL         7
#define ISFS_IOCTL_FCREATE     9
#define ISFS_IOCTL_FSTATS      0xB
#define ISFS_IOCTL_GETUSAGE    0xC
#define ISFS_IOCTL_SHUTDOWN    0xD

#define ISFS_PATH_MAXLEN       64
#define ISFS_INODE_NAMELEN     12
#define ISFS_MAX_OUTST_CALLS   16
#define ISFS_INITLIB_BUFSIZE   0x100

#define ROUNDUP(sz) (((u32)(sz) + 31) & ~(u32)31)

#define IOSAlloc(size) iosAllocAligned(hId, (size), 32)

#define IOSFree(addr)          \
    if (addr) {                \
        iosFree(hId, addr);    \
    }

s32 IOS_OpenAsync(const char *, u32, IOSIpcCb, void *);
extern u32 strnlen(const u8 *str, u32 n);

static IOSFd __fsFd = -1;
static u32 __fsInitialized = 0;
static char *__devfs = 0;
static s32 _asynCnt = 0;
static IOSHeapId hId;

typedef struct isfs_GetAttr {
    IOSUid *ownerId;
    IOSGid *groupId;
    u32 *attr;
    u32 *ownerAcc;
    u32 *groupAcc;
    u32 *othersAcc;
} isfs_GetAttr;

typedef struct isfs_GetUsage {
    u32 *nblocks;
    u32 *ninodes;
} isfs_GetUsage;

// one of these per outstanding call, ioBuf is what actually gets handed to IOS
typedef struct __isfsCtxt {
    u8 ioBuf[ROUNDUP(ISFS_INITLIB_BUFSIZE)] __attribute__((aligned(32)));
    ISFSCallback cb;
    void *ctxt;
    u32 func;
    union {
        ISFSStats *stats;
        ISFSFileStats *fstats;
        u32 *num;
        isfs_GetAttr ga;
        isfs_GetUsage gu;
    } args;
} __isfsCtxt;

#define ISFS_FNC_GetStats     1
#define ISFS_FNC_ReadDir      2
#define ISFS_FNC_GetAttr      3
#define ISFS_FNC_GetUsage     4
#define ISFS_FNC_GetFileStats 5

ISFSError ISFS_OpenLib(void) {
    ISFSError rc = ISFS_ERROR_OK;
    static void *lo = 0, *hi = 0;
    __isfsCtxt *ctxtMem = 0;

    if (!__fsInitialized) {
        lo = IPCGetBufferLo();
        hi = IPCGetBufferHi();
    }

    __devfs = (char *)ROUNDUP(lo);

    if (!__fsInitialized && ((u32)__devfs + ROUNDUP(ISFS_PATH_MAXLEN)) > (u32)hi) {
        OSReport("APP ERROR: Not enough IPC arena\n");
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    strcpy(__devfs, "/dev/fs");

    __fsFd = IOS_Open(__devfs, 0);
    if (__fsFd < 0) {
        rc = __fsFd;
        goto out;
    }

    // the path buffer isn't needed anymore, so the ctxt pool starts in the same spot
    ctxtMem = (__isfsCtxt *)((u32)__devfs);

    if (!__fsInitialized &&
        ((u32)ctxtMem + (ISFS_MAX_OUTST_CALLS + 1) * ROUNDUP(sizeof(__isfsCtxt))) > (u32)hi) {
        OSReport("APP ERROR: Not enough IPC arena\n");
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    if (!__fsInitialized) {
        IPCSetBufferLo((void *)((u32)ctxtMem + (ISFS_MAX_OUTST_CALLS + 1) * ROUNDUP(sizeof(__isfsCtxt))));
        __fsInitialized = 1;
    }

    hId = iosCreateHeap(ctxtMem, (ISFS_MAX_OUTST_CALLS + 1) * ROUNDUP(sizeof(__isfsCtxt)));
    if (hId < 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

out:
    return rc;
}

static IOSError _FSGetStatsCb(IOSError ret, void *ctxt);
static IOSError _FSReadDirCb(IOSError ret, void *ctxt);
static IOSError _FSGetAttrCb(IOSError ret, void *ctxt);
static IOSError _FSGetUsageCb(IOSError ret, void *ctxt);
static IOSError _FSGetFileStatsCb(IOSError ret, void *ctxt);

// shared completion callback for every async call, dispatches on ctxt->func to copy results out
static IOSError _isfsFuncCb(IOSError ret, void *ctxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *_ctxt = (__isfsCtxt *)ctxt;

    rc = ret;

    if (rc >= 0) {
        switch (_ctxt->func) {
        case ISFS_FNC_GetStats:
            _FSGetStatsCb(ret, ctxt);
            break;
        case ISFS_FNC_ReadDir:
            _FSReadDirCb(ret, ctxt);
            break;
        case ISFS_FNC_GetAttr:
            _FSGetAttrCb(ret, ctxt);
            break;
        case ISFS_FNC_GetUsage:
            _FSGetUsageCb(ret, ctxt);
            break;
        case ISFS_FNC_GetFileStats:
            _FSGetFileStatsCb(ret, ctxt);
            break;
        default:
            break;
        }
    }

    _asynCnt = 0;

    if (_ctxt->cb)
        _ctxt->cb(rc, _ctxt->ctxt);

    IOSFree(ctxt);
    return rc;
}

ISFSError ISFS_GetStats(ISFSStats *stats) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *blCtxt = 0;

    if (__fsFd < 0 || !stats) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    blCtxt->args.stats = stats;

    rc = IOS_Ioctl(__fsFd, ISFS_IOCTL_STATS, NULL, 0, blCtxt->ioBuf, sizeof(*stats));
    if (rc != IOS_ERROR_OK) {
        goto out;
    }

    memcpy(stats, blCtxt->ioBuf, sizeof(*stats));

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

static IOSError _FSGetStatsCb(IOSError ret, void *ctxt) {
    __isfsCtxt *_ctxt = (__isfsCtxt *)ctxt;
    ISFSError rc = ISFS_ERROR_OK;

    if (ret == IOS_ERROR_OK) {
        memcpy(_ctxt->args.stats, _ctxt->ioBuf, sizeof(*_ctxt->args.stats));
    }
    return rc;
}

ISFSError ISFS_CreateDir(const u8 *dname, u32 dirAttr, u32 ownerAcc, u32 groupAcc, u32 othersAcc) {
    ISFSError rc = ISFS_ERROR_OK;
    ISFSPathAttrArgs *pathAttrArgs;
    __isfsCtxt *blCtxt = 0;
    u32 len;

    if ((dname == NULL) || (__fsFd < 0) ||
        ((len = strnlen(dname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    pathAttrArgs = (ISFSPathAttrArgs *)blCtxt->ioBuf;
    memcpy(pathAttrArgs->path, dname, len + 1);
    pathAttrArgs->attr = (u8)dirAttr;
    pathAttrArgs->ownerAccess = (u8)ownerAcc;
    pathAttrArgs->groupAccess = (u8)groupAcc;
    pathAttrArgs->othersAccess = (u8)othersAcc;

    rc = IOS_Ioctl(__fsFd, ISFS_IOCTL_DCREATE, pathAttrArgs, sizeof(*pathAttrArgs), NULL, 0);

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

ISFSError ISFS_CreateDirAsync(const u8 *dname, u32 dirAttr, u32 ownerAcc, u32 groupAcc,
                              u32 othersAcc, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    u32 len;
    ISFSPathAttrArgs *pathAttrArgs;
    __isfsCtxt *ctxt;

    if ((dname == NULL) || (__fsFd < 0) ||
        ((len = strnlen(dname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    pathAttrArgs = (ISFSPathAttrArgs *)ctxt->ioBuf;
    memcpy(pathAttrArgs->path, (void *)dname, len + 1);
    pathAttrArgs->attr = (u8)dirAttr;
    pathAttrArgs->ownerAccess = (u8)ownerAcc;
    pathAttrArgs->groupAccess = (u8)groupAcc;
    pathAttrArgs->othersAccess = (u8)othersAcc;

    rc = IOS_IoctlAsync(__fsFd, ISFS_IOCTL_DCREATE, pathAttrArgs, sizeof(*pathAttrArgs), NULL, 0,
                        _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_ReadDir(const u8 *dname, u8 *nameList, u32 *num) {
    ISFSError rc = ISFS_ERROR_OK;
    IOSIoVector *v = 0;
    u32 len, numInputs, numOutputs, *numPtr;
    char *dnPtr;
    __isfsCtxt *blCtxt = 0;

    if ((dname == NULL) || (num == NULL) || (__fsFd < 0) || ((u32)nameList & (u32)(32 - 1)) ||
        ((len = strnlen(dname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    // iovec array lives at the start of ioBuf, path and count get placed after it
    v = (IOSIoVector *)blCtxt->ioBuf;
    dnPtr = (char *)ROUNDUP((u8 *)&v[4]);
    memcpy(dnPtr, dname, len + 1);
    v[0].base = (u8 *)dnPtr;
    v[0].length = ISFS_PATH_MAXLEN;
    numPtr = (u32 *)ROUNDUP((u32)dnPtr + ISFS_PATH_MAXLEN);
    v[1].base = (u8 *)numPtr;
    v[1].length = sizeof(numPtr);

    if (nameList) {
        numInputs = 2;
        numOutputs = 2;
        *numPtr = *num;
        v[2].base = nameList;
        v[2].length = *num * (ISFS_INODE_NAMELEN + 1);
        v[3].base = (u8 *)numPtr;
        v[3].length = sizeof(numPtr);
    } else {
        numInputs = 1;
        numOutputs = 1;
    }

    rc = IOS_Ioctlv(__fsFd, ISFS_IOCTL_DREAD, numInputs, numOutputs, v);
    if (rc != ISFS_ERROR_OK) {
        goto out;
    }
    *num = *numPtr;

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

static IOSError _FSReadDirCb(IOSError ret, void *ctxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *_ctxt = (__isfsCtxt *)ctxt;

    if (ret == IOS_ERROR_OK) {
        u8 *ptr;
        IOSIoVector *v = (IOSIoVector *)_ctxt->ioBuf;
        ptr = (u8 *)ROUNDUP((u8 *)&v[4]);
        ptr = (u8 *)ROUNDUP(ptr + ISFS_PATH_MAXLEN);
        *_ctxt->args.num = *(u32 *)ptr;
    }
    return rc;
}

ISFSError ISFS_ReadDirAsync(const u8 *dname, u8 *nameList, u32 *num, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    u32 len;
    u32 numOutputs;
    u32 numInputs;
    u32 *numPtr;
    IOSIoVector *v;
    __isfsCtxt *ctxt;
    char *dnPtr;

    if ((dname == NULL) || (num == NULL) || (__fsFd < 0) || ((u32)nameList & (u32)(32 - 1)) ||
        ((len = strnlen(dname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = ISFS_FNC_ReadDir;
    ctxt->args.num = num;

    v = (IOSIoVector *)ctxt->ioBuf;
    dnPtr = (char *)ROUNDUP((u8 *)&v[4]);
    memcpy(dnPtr, dname, len + 1);
    v[0].base = (u8 *)dnPtr;
    v[0].length = ISFS_PATH_MAXLEN;
    numPtr = (u32 *)ROUNDUP(((u32)dnPtr) + ISFS_PATH_MAXLEN);
    v[1].base = (u8 *)numPtr;
    v[1].length = sizeof(numPtr);

    if (nameList) {
        numInputs = 2;
        numOutputs = 2;
        *numPtr = *num;
        v[2].base = nameList;
        v[2].length = *num * (ISFS_INODE_NAMELEN + 1);
        v[3].base = (u8 *)numPtr;
        v[3].length = sizeof(numPtr);
    } else {
        numInputs = 1;
        numOutputs = 1;
    }

    rc = IOS_IoctlvAsync(__fsFd, ISFS_IOCTL_DREAD, numInputs, numOutputs, v, _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_SetAttr(const u8 *name, IOSUid ownerId, IOSGid groupId, u32 attr, u32 ownerAcc,
                       u32 groupAcc, u32 othersAcc) {
    ISFSError rc = ISFS_ERROR_OK;
    ISFSPathAttrArgs *pathAttrArgs;
    u32 len;
    __isfsCtxt *blCtxt = 0;

    if ((name == NULL) || (__fsFd < 0) ||
        ((len = strnlen(name, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    pathAttrArgs = (ISFSPathAttrArgs *)blCtxt->ioBuf;
    memcpy(pathAttrArgs->path, name, len + 1);
    pathAttrArgs->ownerId = ownerId;
    pathAttrArgs->groupId = groupId;
    pathAttrArgs->attr = (u8)attr;
    pathAttrArgs->ownerAccess = (u8)ownerAcc;
    pathAttrArgs->groupAccess = (u8)groupAcc;
    pathAttrArgs->othersAccess = (u8)othersAcc;

    rc = IOS_Ioctl(__fsFd, ISFS_IOCTL_SETATTR, (u8 *)pathAttrArgs, sizeof(*pathAttrArgs), NULL, 0);

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

ISFSError ISFS_SetAttrAsync(const u8 *name, IOSUid ownerId, IOSGid groupId, u32 attr, u32 ownerAcc,
                            u32 groupAcc, u32 othersAcc, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *ctxt;
    ISFSPathAttrArgs *pathAttrArgs;
    u32 len;

    if ((name == NULL) || (__fsFd < 0) ||
        ((len = strnlen(name, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    pathAttrArgs = (ISFSPathAttrArgs *)ctxt->ioBuf;
    memcpy(pathAttrArgs->path, name, len + 1);
    pathAttrArgs->ownerId = ownerId;
    pathAttrArgs->groupId = groupId;
    pathAttrArgs->attr = (u8)attr;
    pathAttrArgs->ownerAccess = (u8)ownerAcc;
    pathAttrArgs->groupAccess = (u8)groupAcc;
    pathAttrArgs->othersAccess = (u8)othersAcc;

    rc = IOS_IoctlAsync(__fsFd, ISFS_IOCTL_SETATTR, (u8 *)pathAttrArgs, sizeof(ISFSPathAttrArgs),
                        NULL, 0, _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_GetAttr(const u8 *name, IOSUid *ownerId, IOSGid *groupId, u32 *attr, u32 *ownerAcc,
                       u32 *groupAcc, u32 *othersAcc) {
    ISFSError rc = ISFS_ERROR_OK;
    ISFSPathAttrArgs *pathAttrArgs;
    u8 *ptr;
    u32 len;
    __isfsCtxt *blCtxt = 0;

    if ((name == NULL) || (__fsFd < 0) ||
        ((len = strnlen(name, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN) || (ownerId == NULL) ||
        (groupId == NULL) || (attr == NULL) || (ownerAcc == NULL) || (groupAcc == NULL) ||
        (othersAcc == NULL)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    // name goes in the front of ioBuf, the answer comes back in an aligned slot after it
    ptr = (u8 *)blCtxt->ioBuf;
    memcpy(ptr, name, len + 1);
    pathAttrArgs = (ISFSPathAttrArgs *)ROUNDUP(ptr + ISFS_PATH_MAXLEN);

    rc = IOS_Ioctl(__fsFd, ISFS_IOCTL_GETATTR, (u8 *)ptr, ISFS_PATH_MAXLEN, (u8 *)pathAttrArgs,
                   sizeof(*pathAttrArgs));
    if (rc != IOS_ERROR_OK) {
        goto out;
    }

    *ownerId = pathAttrArgs->ownerId;
    *groupId = pathAttrArgs->groupId;
    *attr = pathAttrArgs->attr;
    *ownerAcc = pathAttrArgs->ownerAccess;
    *groupAcc = pathAttrArgs->groupAccess;
    *othersAcc = pathAttrArgs->othersAccess;

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

static IOSError _FSGetAttrCb(IOSError ret, void *ctxt) {
    ISFSError rc = ret;

    if (ret == IOS_ERROR_OK) {
        __isfsCtxt *_ctxt = (__isfsCtxt *)ctxt;
        ISFSPathAttrArgs *pathAttrArgs;

        pathAttrArgs = (ISFSPathAttrArgs *)ROUNDUP(_ctxt->ioBuf + ISFS_PATH_MAXLEN);

        *_ctxt->args.ga.ownerId = pathAttrArgs->ownerId;
        *_ctxt->args.ga.groupId = pathAttrArgs->groupId;
        *_ctxt->args.ga.attr = pathAttrArgs->attr;
        *_ctxt->args.ga.ownerAcc = pathAttrArgs->ownerAccess;
        *_ctxt->args.ga.groupAcc = pathAttrArgs->groupAccess;
        *_ctxt->args.ga.othersAcc = pathAttrArgs->othersAccess;
    }
    return rc;
}

ISFSError ISFS_GetAttrAsync(const u8 *name, IOSUid *ownerId, IOSGid *groupId, u32 *attr,
                            u32 *ownerAcc, u32 *groupAcc, u32 *othersAcc, ISFSCallback cb,
                            void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *ctxt;
    ISFSPathAttrArgs *pathAttrArgs;
    u8 *ptr;
    u32 len;

    if ((name == NULL) || (__fsFd < 0) ||
        ((len = strnlen(name, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN) || (ownerId == NULL) ||
        (groupId == NULL) || (attr == NULL) || (ownerAcc == NULL) || (groupAcc == NULL) ||
        (othersAcc == NULL)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->args.ga.ownerId = ownerId;
    ctxt->args.ga.groupId = groupId;
    ctxt->args.ga.attr = attr;
    ctxt->args.ga.ownerAcc = ownerAcc;
    ctxt->args.ga.groupAcc = groupAcc;
    ctxt->args.ga.othersAcc = othersAcc;
    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = ISFS_FNC_GetAttr;

    ptr = (u8 *)ctxt->ioBuf;
    memcpy(ptr, name, len + 1);
    pathAttrArgs = (ISFSPathAttrArgs *)ROUNDUP(ptr + ISFS_PATH_MAXLEN);

    rc = IOS_IoctlAsync(__fsFd, ISFS_IOCTL_GETATTR, (u8 *)ptr, ISFS_PATH_MAXLEN, (u8 *)pathAttrArgs,
                        sizeof(*pathAttrArgs), _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_Delete(const u8 *name) {
    ISFSError rc = ISFS_ERROR_OK;
    u32 len;
    __isfsCtxt *blCtxt = 0;

    if ((name == NULL) || (__fsFd < 0) ||
        ((len = strnlen(name, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    memcpy(blCtxt->ioBuf, name, len + 1);

    rc = IOS_Ioctl(__fsFd, ISFS_IOCTL_DEL, (u8 *)blCtxt->ioBuf, ISFS_PATH_MAXLEN, NULL, 0);

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

ISFSError ISFS_DeleteAsync(const u8 *name, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *ctxt;
    u32 len;

    if ((name == NULL) || (__fsFd < 0) ||
        ((len = strnlen(name, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    memcpy((void *)ctxt->ioBuf, name, len + 1);
    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    rc = IOS_IoctlAsync(__fsFd, ISFS_IOCTL_DEL, (u8 *)ctxt->ioBuf, ISFS_PATH_MAXLEN, NULL, 0,
                        _isfsFuncCb, ctxt);

out:
    return rc;
}

// rename is ioctl 8 judging by the numbering (not confirmed, only the async one is linked)
ISFSError ISFS_RenameAsync(const u8 *oldName, const u8 *newName, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *ctxt;
    ISFSPathsArgs *pathsArgs;
    u32 oldLen;
    u32 newLen;

    if ((oldName == NULL) || (newName == NULL) || (__fsFd < 0) ||
        ((oldLen = strnlen(oldName, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN) ||
        ((newLen = strnlen(newName, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    pathsArgs = (ISFSPathsArgs *)ctxt->ioBuf;
    memcpy(pathsArgs->path1, oldName, oldLen + 1);
    memcpy(pathsArgs->path2, newName, newLen + 1);

    rc = IOS_IoctlAsync(__fsFd, 8, (u8 *)pathsArgs, sizeof(*pathsArgs), NULL, 0, _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_GetUsage(const u8 *dname, u32 *nblocks, u32 *ninodes) {
    ISFSError rc = ISFS_ERROR_OK;
    IOSIoVector *v = 0;
    u32 len, *blkPtr, *inodePtr;
    char *dnPtr;
    __isfsCtxt *blCtxt = 0;

    if ((dname == NULL) || (__fsFd < 0) || (nblocks == NULL) || (ninodes == NULL) ||
        ((len = strnlen(dname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    v = (IOSIoVector *)blCtxt->ioBuf;
    dnPtr = (char *)ROUNDUP((u8 *)&v[3]);
    memcpy(dnPtr, dname, len + 1);
    v[0].base = (u8 *)dnPtr;
    v[0].length = ISFS_PATH_MAXLEN;

    blkPtr = (u32 *)ROUNDUP(((u32)dnPtr) + ISFS_PATH_MAXLEN);
    inodePtr = (u32 *)ROUNDUP(((u32)blkPtr) + 4);
    v[1].base = (u8 *)blkPtr;
    v[1].length = 4;
    v[2].base = (u8 *)inodePtr;
    v[2].length = 4;

    rc = IOS_Ioctlv(__fsFd, ISFS_IOCTL_GETUSAGE, 1, 2, v);
    if (rc != ISFS_ERROR_OK) {
        goto out;
    }

    *nblocks = *blkPtr;
    *ninodes = *inodePtr;

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

static IOSError _FSGetUsageCb(IOSError ret, void *ctxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *_ctxt = (__isfsCtxt *)ctxt;

    if (ret == IOS_ERROR_OK) {
        u8 *ptr;
        IOSIoVector *v = (IOSIoVector *)_ctxt->ioBuf;

        ptr = (u8 *)ROUNDUP((u8 *)&v[4]);
        ptr = (u8 *)ROUNDUP(ptr + ISFS_PATH_MAXLEN);
        *_ctxt->args.gu.nblocks = *(u32 *)ptr;
        ptr = (u8 *)ROUNDUP(ptr + 4);
        *_ctxt->args.gu.ninodes = *(u32 *)ptr;
    }
    return rc;
}

ISFSError ISFS_CreateFile(const u8 *fname, u32 fileAttr, u32 ownerAcc, u32 groupAcc, u32 othersAcc) {
    ISFSError rc = ISFS_ERROR_OK;
    ISFSPathAttrArgs *pathAttrArgs;
    u32 len;
    __isfsCtxt *blCtxt = 0;

    if ((fname == NULL) || (__fsFd < 0) ||
        ((len = strnlen(fname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    pathAttrArgs = (ISFSPathAttrArgs *)blCtxt->ioBuf;
    memcpy(pathAttrArgs->path, fname, len + 1);
    pathAttrArgs->attr = (u8)fileAttr;
    pathAttrArgs->ownerAccess = (u8)ownerAcc;
    pathAttrArgs->groupAccess = (u8)groupAcc;
    pathAttrArgs->othersAccess = (u8)othersAcc;

    rc = IOS_Ioctl(__fsFd, ISFS_IOCTL_FCREATE, (u8 *)pathAttrArgs, sizeof(*pathAttrArgs), NULL, 0);

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

ISFSError ISFS_CreateFileAsync(const u8 *fname, u32 fileAttr, u32 ownerAcc, u32 groupAcc,
                               u32 othersAcc, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    ISFSPathAttrArgs *pathAttrArgs;
    __isfsCtxt *ctxt;
    u32 len;

    if ((fname == NULL) || (__fsFd < 0) ||
        ((len = strnlen(fname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    pathAttrArgs = (ISFSPathAttrArgs *)ctxt->ioBuf;
    memcpy(pathAttrArgs->path, fname, len + 1);
    pathAttrArgs->attr = (u8)fileAttr;
    pathAttrArgs->ownerAccess = (u8)ownerAcc;
    pathAttrArgs->groupAccess = (u8)groupAcc;
    pathAttrArgs->othersAccess = (u8)othersAcc;

    rc = IOS_IoctlAsync(__fsFd, ISFS_IOCTL_FCREATE, (u8 *)pathAttrArgs, sizeof(*pathAttrArgs), NULL,
                        0, _isfsFuncCb, ctxt);

out:
    return rc;
}

IOSFd ISFS_Open(const u8 *fname, u32 access) {
    ISFSError rc = ISFS_ERROR_OK;
    u32 len;
    __isfsCtxt *blCtxt = 0;

    if ((fname == NULL) || ((len = strnlen(fname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    memcpy(blCtxt->ioBuf, fname, len + 1);
    rc = IOS_Open((const char *)blCtxt->ioBuf, access);

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

IOSFd ISFS_OpenAsync(const u8 *fname, u32 access, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    u32 len;
    __isfsCtxt *ctxt;

    if ((fname == NULL) || ((len = strnlen(fname, ISFS_PATH_MAXLEN)) == ISFS_PATH_MAXLEN)) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    memcpy(ctxt->ioBuf, fname, len + 1);
    rc = IOS_OpenAsync((const char *)ctxt->ioBuf, access, _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_GetFileStats(IOSFd fd, ISFSFileStats *stats) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *blCtxt = 0;

    if ((stats == NULL) || ((u32)stats & (u32)(32 - 1))) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    blCtxt = IOSAlloc(sizeof(*blCtxt));
    if (blCtxt == 0) {
        rc = IOS_ERROR_FAIL_ALLOC;
        goto out;
    }

    rc = IOS_Ioctl(fd, ISFS_IOCTL_FSTATS, NULL, 0, blCtxt->ioBuf, sizeof(*stats));
    if (rc != IOS_ERROR_OK) {
        goto out;
    }

    memcpy(stats, blCtxt->ioBuf, sizeof(*stats));

out:
    if (blCtxt)
        IOSFree(blCtxt);
    return rc;
}

static IOSError _FSGetFileStatsCb(IOSError ret, void *ctxt) {
    __isfsCtxt *_ctxt = (__isfsCtxt *)ctxt;
    ISFSError rc = ISFS_ERROR_OK;

    if (ret == IOS_ERROR_OK) {
        memcpy(_ctxt->args.fstats, _ctxt->ioBuf, sizeof(*_ctxt->args.fstats));
    }
    return rc;
}

ISFSError ISFS_GetFileStatsAsync(IOSFd fd, ISFSFileStats *stats, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *ctxt;

    if ((stats == NULL) || ((u32)stats & (u32)(32 - 1))) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = ISFS_FNC_GetFileStats;
    ctxt->args.fstats = stats;

    rc = IOS_IoctlAsync(fd, ISFS_IOCTL_FSTATS, NULL, 0, ctxt->ioBuf, sizeof(*stats), _isfsFuncCb,
                        ctxt);

out:
    return rc;
}

ISFSError ISFS_Seek(IOSFd fd, s32 offset, u32 whence) {
    ISFSError rc = ISFS_ERROR_OK;
    rc = IOS_Seek(fd, offset, whence);
    return rc;
}

ISFSError ISFS_SeekAsync(IOSFd fd, s32 offset, u32 whence, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc;
    __isfsCtxt *ctxt;

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    rc = IOS_SeekAsync(fd, offset, whence, _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_Read(IOSFd fd, u8 *buf, u32 size) {
    ISFSError rc = ISFS_ERROR_OK;

    if ((buf == NULL) || ((u32)buf & (u32)(32 - 1))) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    rc = IOS_Read(fd, buf, size);

out:
    return rc;
}

ISFSError ISFS_ReadAsync(IOSFd fd, u8 *buf, u32 size, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *ctxt;

    if ((buf == NULL) || ((u32)buf & (u32)(32 - 1))) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    rc = IOS_ReadAsync(fd, buf, size, _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_Write(IOSFd fd, const u8 *buf, u32 size) {
    ISFSError rc = ISFS_ERROR_OK;

    if ((buf == NULL) || ((u32)buf & (u32)(32 - 1))) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    rc = IOS_Write(fd, (u8 *)buf, size);

out:
    return rc;
}

ISFSError ISFS_WriteAsync(IOSFd fd, const u8 *buf, u32 size, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc = ISFS_ERROR_OK;
    __isfsCtxt *ctxt;

    if ((buf == NULL) || ((u32)buf & (u32)(32 - 1))) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    rc = IOS_WriteAsync(fd, (void *)buf, size, _isfsFuncCb, ctxt);

out:
    return rc;
}

ISFSError ISFS_Close(IOSFd fd) {
    ISFSError rc = ISFS_ERROR_OK;
    rc = IOS_Close(fd);
    return rc;
}

ISFSError ISFS_CloseAsync(IOSFd fd, ISFSCallback cb, void *fsCtxt) {
    ISFSError rc;
    __isfsCtxt *ctxt;

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (ctxt == 0) {
        rc = ISFS_ERROR_BUSY;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    rc = IOS_CloseAsync(fd, _isfsFuncCb, ctxt);

out:
    return rc;
}

// note the alloc happens before the fd check here (the asm does it in that order)
ISFSError ISFS_ShutdownAsync(ISFSCallback cb, void *fsCtxt) {
    __isfsCtxt *ctxt;
    ISFSError rc = ISFS_ERROR_OK;

    ctxt = IOSAlloc(sizeof(*ctxt));
    if (__fsFd < 0) {
        rc = ISFS_ERROR_INVALID;
        goto out;
    }

    ctxt->cb = cb;
    ctxt->ctxt = fsCtxt;
    ctxt->func = 0;

    rc = IOS_IoctlAsync(__fsFd, ISFS_IOCTL_SHUTDOWN, NULL, 0, NULL, 0, _isfsFuncCb, ctxt);

out:
    return rc;
}
