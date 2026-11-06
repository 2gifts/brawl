#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: only the explicitly inline SDK helpers are inlined in this unit.
#pragma auto_inline off

#define WC24_DRIVE "@24"

NWC24Err fn_8039B1C0(NWC24MsgBoxId id, u32 *numMsgs);
NWC24Err fn_8039B244(NWC24iMsgObj *out, NWC24MsgBoxId id, u32 msgId);
BOOL fn_8039B4C8(NWC24iMBCEntry *entry);
NWC24Err fn_8039B5AC(NWC24MsgBoxId id, u32 msgId);
NWC24Err NWC24iOpenMBox(void);
NWC24Err NWC24iMBoxGetCtrlFilePath(NWC24MsgBoxId id, char *dst, s32 maxlen);
NWC24Err NWC24iMBoxOpenNewMsg(NWC24MsgBoxId id, NWC24File *file, u32 *pMsgID);
NWC24Err fn_8039B9B0(NWC24MsgBoxId id, u32 msgId, NWC24File *file);
NWC24Err NWC24iMBoxCloseMsg(NWC24File *file);
NWC24Err NWC24iMBoxCancelMsg(NWC24File *file, NWC24MsgBoxId id, u32 msgId);
NWC24Err NWC24iMBoxAddMsgObj(NWC24MsgBoxId id, NWC24iMsgObj *msg);
NWC24Err NWC24iMBoxFlushHeader(NWC24MsgBoxId id);
NWC24Err NWC24iMBoxCheck(NWC24MsgBoxId id, u32 size);
NWC24Err fn_8039C16C(NWC24MsgBoxId id, u32 msgId, BOOL checkAccess);
static NWC24Err fn_8039C60C(NWC24iMBCHeader *hdr, const NWC24iMsgObj *msg, NWC24File *file, NWC24MsgBoxId id);
static NWC24Err fn_8039C984(NWC24MsgBoxId id, NWC24iMBCHeader **hdrOut);
static NWC24Err fn_8039CB7C(NWC24iMBCHeader *hdr, const NWC24iMsgObj *msg, NWC24File *file);
static NWC24Err fn_8039CCC8(NWC24iMBCHeader *hdr, NWC24File *file, u32 block);
static NWC24Err fn_8039CDB4(NWC24MsgBoxId id);
NWC24Err fn_8039CF40(const NWC24iMsgObj *msg, NWC24iMBCEntry *entry);
NWC24Err fn_8039D0EC(const NWC24iMBCEntry *entry, NWC24iMsgObj *msg);

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

// the message object keeps strings/blobs as {offset, size} pairs, in the control file they are packed into
// one word (size in the top 12 bits, offset in the low 20)
static inline u32 PackData_i(const NWC24Data *data) {
    return (data->offset & 0xFFFFF) | (data->size << 20);
}

static inline void UnpackData_i(NWC24Data *data, u32 packed) {
    data->offset = packed & 0xFFFFF;
    data->size = packed >> 20;
}

// HYPOTHESIS: shared SDK mailbox initialization was stripped from Brawl;
// its retained temporary-file paths identify this public SDK source.
NWC24Err NWC24CreateVF(const char *path, u32 size);
NWC24Err NWC24FDelete(const char *path);
static NWC24Err CreateCtrlFile(NWC24MsgBoxId id, BOOL forceCreate);

static inline NWC24Err IsFileThere(const char *path) {
    NWC24File file;
    NWC24Err result = NWC24FOpen(&file, path, NWC24_OPEN_READ);
    if (result == NWC24_OK) { result = NWC24FClose(&file); }
    return result;
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

// can the current app see this mail box entry?
BOOL fn_8039B4C8(NWC24iMBCEntry *entry) {
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

NWC24Err fn_8039B5AC(NWC24MsgBoxId id, u32 msgId) {
    BOOL checkAccess = TRUE;

    if (NWC24IsMsgLibOpenedByTool()) {
        checkAccess = FALSE;
    }

    return fn_8039C16C(id, msgId, checkAccess);
}

NWC24Err NWC24iOpenMBox(void) {
    NWC24iMBCHeader *hdr;
    NWC24File receiveFile;
    NWC24File sendFile;
    NWC24Err result;
    char *path = NWC24WorkP->pathWork;

    Mail_memset(&NWC24WorkP->sendCtrl, 0, MBC_BLOCK_SIZE);
    result = fn_8039C984(NWC24_MSGBOX_SEND, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    Mail_memset(&NWC24WorkP->recvCtrl, 0, MBC_BLOCK_SIZE);
    result = fn_8039C984(NWC24_MSGBOX_RECV, &hdr);
    if (result != NWC24_OK) {
        return result;
    }

    result = MBoxMbxPath_i(NWC24_MSGBOX_RECV, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&receiveFile, path, NWC24_OPEN_READ);
    if (result == NWC24_OK) {
        result = NWC24FClose(&receiveFile);
    }

    if (result != NWC24_OK) {
        return result;
    }

    result = MBoxMbxPath_i(NWC24_MSGBOX_SEND, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }

    result = NWC24FOpen(&sendFile, path, NWC24_OPEN_READ);
    if (result == NWC24_OK) {
        result = NWC24FClose(&sendFile);
    }

    if (result != NWC24_OK) {
        return result;
    }

    MBoxMount.count = 0;
    MBoxMount.box = NWC24_MSGBOX_SEND;
    return NWC24_OK;
}

NWC24Err NWC24iMBoxGetCtrlFilePath(NWC24MsgBoxId id, char *dst, s32 maxlen) {
    return MBoxCtrlPath_i(id, dst, maxlen);
}

NWC24Err NWC24iInitMBox(void) {
    NWC24Err result;
    char* path = NWC24WorkP->pathWork;
    int renewSend, renewRecv;
    Mail_memset(&NWC24WorkP->sendCtrl, 0, sizeof(NWC24iMBCHeader));
    renewSend = CreateCtrlFile(NWC24_MSGBOX_SEND, FALSE);
    if (renewSend < 0) {
        return renewSend;
    }
    Mail_memset(&NWC24WorkP->recvCtrl, 0, sizeof(NWC24iMBCHeader));
    renewRecv = CreateCtrlFile(NWC24_MSGBOX_RECV, FALSE);
    if (renewRecv < 0) {
        return renewRecv;
    }
    result = MBoxMbxPath_i(NWC24_MSGBOX_RECV, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }
    result = IsFileThere(path);
    if (result == NWC24_ERR_FILE_NOEXISTS || renewRecv) {
        result = NWC24CreateVF(path, 0x700000);
    }
    if (result != NWC24_OK) {
        return result;
    }
    result = MBoxMbxPath_i(NWC24_MSGBOX_SEND, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }
    result = IsFileThere(path);
    if (result == NWC24_ERR_FILE_NOEXISTS || renewSend) {
        result = NWC24CreateVF(path, 0x200000);
    }
    if (result != NWC24_OK) {
        return result;
    }
    MBoxMount.count = 0;
    MBoxMount.box = NWC24_MSGBOX_SEND;
    NWC24FDelete("/shared2/wc24/mbox/recvtmp.msg");
    NWC24FDelete("/shared2/wc24/mbox/dlcnt.bin");
    return result;
}

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
    // MATCH-ONLY: preserve the existing stack-stored ID in the eviction loop.
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
    char *path;
    u32 count = 0;
    NWC24File file;
    u32 off;
    u32 foundOff = 0;
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
    result = fn_8039CCC8(hdr, &file, foundOff);
    writeErr = MBoxRemoveMsgFile_i(id, msgId);
    result = result != NWC24_OK ? result : writeErr;

    writeErr = MBoxWriteHeader_i(hdr, &file);

    result = result != NWC24_OK ? result : writeErr;
    writeErr = NWC24FClose(&file);
    result = result != NWC24_OK ? result : writeErr;
    return result;
}

// looks for an older copy of the message that is about to be stored (same tag or same contents) and
// throws it out, also keeps the oldest message id of the box up to date
static NWC24Err fn_8039C60C(NWC24iMBCHeader *hdr, const NWC24iMsgObj *msg, NWC24File *file, NWC24MsgBoxId id) {
    NWC24Err result = NWC24_OK;
    u32 off;
    u32 visitedCount = 0;
    u32 differs;
    u32 replacedId = 0;
    u32 replacedOffset = 0;
    u32 oldestId = 0;
    NWC24Err removeErr;
    NWC24iMBCEntry *entry;

    if (id == NWC24_MSGBOX_SEND) {
        return NWC24_OK;
    }

    entry = &NWC24WorkP->mbcEntry;
    for (off = MBC_FIRST_ENTRY; off < hdr->fileSize; off += MBC_BLOCK_SIZE) {
        if (visitedCount >= hdr->numMsgs) {
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

        visitedCount++;
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
                replacedId = entry->id;
                replacedOffset = off;
                continue;
            }

            if (msg->unk1C != 0 && entry->unk1C == msg->unk1C && entry->flags == msg->flags &&
                entry->length == msg->length && entry->unk10 == msg->unk10 && entry->createTime == msg->createTime) {
                replacedId = entry->id;
                replacedOffset = off;
                continue;
            }
        }

        if (oldestId == 0 || CompareMsgId(oldestId, entry->id) > 0) {
            oldestId = entry->id;
        }
    }

    if (result != NWC24_OK) {
        return result;
    }

    hdr->oldestMsgId = oldestId;
    if (replacedId != 0) {
        removeErr = fn_8039CCC8(hdr, file, replacedOffset);
        result = MBoxRemoveMsgFile_i(id, replacedId);
        if (removeErr != NWC24_OK) {
            result = removeErr;
        }
    }

    return result;
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
static NWC24Err fn_8039CB7C(NWC24iMBCHeader *hdr, const NWC24iMsgObj *msg, NWC24File *file) {
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

    hdr->numMsgs++;
    hdr->totalMsgSize += msg->length;
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

// message object -> control file entry
NWC24Err fn_8039CF40(const NWC24iMsgObj *msg, NWC24iMBCEntry *entry) {
    // Snapshot the five compact offset/length words before copying the record.
    u32 fromField = PackData_i(&msg->fromField);
    u32 toField = PackData_i(&msg->toField);
    u32 subject = PackData_i(&msg->subject);
    u32 contentType = PackData_i(&msg->contentType);
    u32 txEncoding = PackData_i(&msg->txEncoding);

    int i;

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
    entry->fromField = fromField;
    entry->toField = toField;
    entry->subject = subject;
    entry->contentType = contentType;
    entry->txEncoding = txEncoding;
    entry->text = msg->text;
    entry->dwcId = msg->dwcId;
    entry->unk78 = msg->iconNew;
    for (i = 0; i < NWC24i_MSG_ATTACHMENT_MAX; i++) {
        entry->attached[i] = msg->attached[i];
        entry->attachedSize[i] = msg->attachedSize[i];
        entry->attachedType[i] = msg->attachedType[i];
    }
    return NWC24_OK;
}

// control file entry -> message object
NWC24Err fn_8039D0EC(const NWC24iMBCEntry *entry, NWC24iMsgObj *msg) {
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
    msg->text = entry->text;
    msg->dwcId = entry->dwcId;
    msg->iconNew = entry->unk78;

    // Expand the compact references after copying the common message fields.
    UnpackData_i(&msg->fromField, entry->fromField);
    UnpackData_i(&msg->toField, entry->toField);
    UnpackData_i(&msg->subject, entry->subject);
    UnpackData_i(&msg->contentType, entry->contentType);
    UnpackData_i(&msg->txEncoding, entry->txEncoding);
    for (i = 0; i < NWC24i_MSG_ATTACHMENT_MAX; i++) {
        msg->attached[i] = entry->attached[i];
        msg->attachedSize[i] = entry->attachedSize[i];
        msg->attachedType[i] = entry->attachedType[i];
    }
    for (i = 0; i < NWC24i_MSG_RECIPIENT_MAX; i++) {
        msg->to[i].id = 0;
    }

    return NWC24_OK;
}

static NWC24Err CreateCtrlFile(NWC24MsgBoxId id, BOOL forceCreate) {
    NWC24Err result;
    NWC24Err close;
    NWC24File file;
    NWC24iMBCHeader* header;
    NWC24iMBCEntry* entry = &NWC24WorkP->mbcEntry;
    char* path = NWC24WorkP->pathWork;
    u32 offset;
    result = fn_8039C984(id, &header);
    if (result == NWC24_OK && !forceCreate) {
        return result;
    }
    result = NWC24iMBoxGetCtrlFilePath(id, path, NWC24i_PATH_WORK_SIZE);
    if (result != NWC24_OK) {
        return result;
    }
    result = NWC24FOpen(&file, path, NWC24_OPEN_NAND_W);
    if (result != NWC24_OK) {
        return result;
    }
    header = id == NWC24_MSGBOX_SEND ? &NWC24WorkP->sendCtrl : &NWC24WorkP->recvCtrl;
    Mail_memset(header, 0, sizeof(NWC24iMBCHeader));
    header->magic = MBC_MAGIC;
    header->version = MBC_VERSION;
    header->capacity = id == NWC24_MSGBOX_SEND ? 127 : 255;
    header->freeSpace = id == NWC24_MSGBOX_SEND ? 0x200000 : 0x700000;
    Mail_memset(header->lastUIDL, '0', sizeof(header->lastUIDL) - 1);
    header->fileSize = sizeof(NWC24iMBCHeader) + header->capacity * sizeof(NWC24iMBCEntry);
    header->nextMsgId = 1;
    header->freeChain = sizeof(NWC24iMBCHeader);
    result = NWC24FWrite(header, sizeof(NWC24iMBCHeader), &file);
    Mail_memset(entry, 0, sizeof(NWC24iMBCEntry));
    for (offset = sizeof(NWC24iMBCHeader); offset < header->fileSize; offset += sizeof(NWC24iMBCEntry)) {
        entry->appId = (offset + sizeof(NWC24iMBCEntry)) % header->fileSize;
        close = NWC24FWrite(entry, sizeof(NWC24iMBCEntry), &file);
        if (result == NWC24_OK) {
            result = close;
        }
    }
    close = NWC24FClose(&file);
    if (result != NWC24_OK) {
        return result;
    }
    return close != NWC24_OK ? close : 1;
}
