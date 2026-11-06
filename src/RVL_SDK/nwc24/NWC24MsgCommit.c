// WiiConnect24 mail composition and transactional publication.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

#pragma auto_inline off

NWC24Err NWC24CommitMsg(NWC24MsgObj *pMsg);
NWC24Err NWC24CommitMsgInternal(NWC24iMsgObj *msg, NWC24MsgBoxId id);
NWC24Err fn_8039E9CC(const NWC24iMsgObj *msg);
NWC24Err fn_8039EA6C(NWC24iMsgObj *msg, NWC24MsgBoxId id);
NWC24Err fn_8039EDE8(const NWC24iAddr *addr, u32 flags, char *dst, int maxlen, int *written);
NWC24Err fn_8039EEF8(NWC24iMsgObj *msg);
NWC24Err fn_8039EFE0(NWC24iMsgObj *msg);
NWC24Err fn_8039F0F4(NWC24iMsgObj *msg);
NWC24Err fn_8039F1F4(NWC24iMsgObj *msg);
NWC24Err fn_8039F35C(NWC24iMsgObj *msg);
NWC24Err fn_8039F494(NWC24iMsgObj *msg);
NWC24Err fn_8039F5B0(NWC24iMsgObj *msg);
NWC24Err fn_8039F6B0(NWC24iMsgObj *msg);
NWC24Err fn_8039F7B0(NWC24iMsgObj *msg, int idx);
NWC24Err fn_8039F920(NWC24iMsgObj *msg);
NWC24Err fn_8039FA54(NWC24iMsgObj *msg);
NWC24Err fn_8039FB14(const u8 *data, int size, u32 *written);
NWC24Err fn_8039FC78(const char *data, int size, u32 *written);

static inline u32 Base64PartSize_i(u32 n) {
    return (n * 4 + 2) / 3 + (n / 57) * 2 + 4;
}

static NWC24File *OutFile;

static BOOL SendToSelfIsReceive = TRUE;

extern void fn_8038EC6C(OSCalendarTime *ct);
NWC24Err fn_803A1344(NWC24Date *date, s32 minutes);

static char BoundaryStr[40];

static inline NWC24Err WriteSMTP_DATA(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteMessageIdField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteSubjectField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteXWiiCmdField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteXWiiTagField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteXWiiDWCIdField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteXWiiIconNewField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteXWiiMBNoReplyField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteXWiiMBRegDateField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteXWiiMBDelayField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteUserField(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteMIMETopHeader(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteMIMEPartBoundary(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteTextBody(NWC24iMsgObj* pMsg);
static inline NWC24Err WriteAttachData(NWC24iMsgObj* pMsg, s32 idx);
static inline NWC24Err WriteMIMEPartBoundaryEnd(NWC24iMsgObj* pMsg);

NWC24Err NWC24CommitMsg(NWC24MsgObj *pMsg) {
    NWC24MsgBoxId id = NWC24_MSGBOX_SEND;
    NWC24iMsgObj *msg = (NWC24iMsgObj *)pMsg;
    NWC24UserId myId;

    if (!NWC24IsMsgLibOpened() && !NWC24IsMsgLibOpenedByTool()) {
        return NWC24_ERR_LIB_NOT_OPENED;
    }

    if (!(msg->flags & NWC24_MSGOBJ_INITIALIZED) || (msg->flags & NWC24_MSGOBJ_DELIVERING)) {
        return NWC24_ERR_PROTECTED;
    }

    // a message to yourself skips the send box and lands in the receive box directly
    if (SendToSelfIsReceive && (msg->flags & NWC24_MSGOBJ_FOR_RECIPIENT)) {
        NWC24GetMyUserId(&myId);
        if (msg->numTo == 1 && msg->to[0].id == myId) {
            id = NWC24_MSGBOX_RECV;
        }
    }

    return NWC24CommitMsgInternal(msg, id);
}

NWC24Err NWC24CommitMsgInternal(NWC24iMsgObj* pMsg, NWC24MsgBoxId id) {
    NWC24File file;
    s32 i = 0;
    u32 msgId;
    NWC24Data subject;
    NWC24Data text;
    NWC24Data attachment[NWC24i_MSG_ATTACHMENT_MAX];
    NWC24Err result;
    NWC24Err err;
    u32 arrivedFlags;

    NWC24Data_Init(&subject);
    NWC24Data_Init(&text);

    for (i = 0; i < NWC24i_MSG_ATTACHMENT_MAX; i++) {
        NWC24Data_Init(&attachment[i]);
    }

    NWC24iSetErrorCode(NWC24_OK);

    err = fn_8039E9CC(pMsg);
    if (err != NWC24_OK) {
        goto _exit;
    }

    err = fn_8039EA6C(pMsg, id);
    if (err != NWC24_OK) {
        goto _exit;
    }

    result = NWC24iMBoxOpenNewMsg(id, &file, &msgId);
    if (result != NWC24_OK) {
        err = result;
        goto _error_break;
    }

    pMsg->id = msgId;
    pMsg->length = 0;

    OutFile = &file;

#define HANDLE_ERROR(X)                                                                                                                              \
    if ((X) != NWC24_OK) {                                                                                                                           \
        NWC24FClose(&file);                                                                                                                          \
        err = result;                                                                                                                                \
        goto _error_break;                                                                                                                           \
    }

    if (id == NWC24_MSGBOX_SEND) {
        result = fn_8039EEF8(pMsg);
        HANDLE_ERROR(result);

        result = fn_8039EFE0(pMsg);
        HANDLE_ERROR(result);

        result = WriteSMTP_DATA(pMsg);
        HANDLE_ERROR(result);
    }

    result = fn_8039F35C(pMsg);
    HANDLE_ERROR(result);

    result = fn_8039F0F4(pMsg);
    HANDLE_ERROR(result);

    result = fn_8039F1F4(pMsg);
    HANDLE_ERROR(result);

    result = WriteMessageIdField(pMsg);
    HANDLE_ERROR(result);

    subject.offset = pMsg->length + NWC24i_KSTRLEN("Subject: ") - 1;
    result = WriteSubjectField(pMsg);
    if (result == NWC24_OK) {
        subject.size = pMsg->length - subject.offset - 2;
    } else if (result == NWC24_ERR_NULL) {
        result = NWC24_OK;
        subject.pData = 0;
    }

    HANDLE_ERROR(result);

    if (pMsg->flags & NWC24_MSGOBJ_FOR_RECIPIENT) {
        result = fn_8039F494(pMsg);
        HANDLE_ERROR(result);

        result = WriteXWiiCmdField(pMsg);
        HANDLE_ERROR(result);

        result = WriteXWiiTagField(pMsg);
        HANDLE_ERROR(result);

        result = WriteXWiiDWCIdField(pMsg);
        HANDLE_ERROR(result);

        result = fn_8039F6B0(pMsg);
        HANDLE_ERROR(result);

        result = fn_8039F5B0(pMsg);
        HANDLE_ERROR(result);

        result = WriteXWiiIconNewField(pMsg);
        HANDLE_ERROR(result);

        if (pMsg->msgBoard != 0) {
            result = WriteXWiiMBNoReplyField(pMsg);
            HANDLE_ERROR(result);

            result = WriteXWiiMBRegDateField(pMsg);
            HANDLE_ERROR(result);

            result = WriteXWiiMBDelayField(pMsg);
            HANDLE_ERROR(result);
        }
    }

    result = WriteUserField(pMsg);
    HANDLE_ERROR(result);

    Mail_memset(BoundaryStr, 0, sizeof(BoundaryStr));
    Mail_sprintf(BoundaryStr, "Boundary-NWC24-%08X%05X", pMsg->createTime, pMsg->id);

    result = WriteMIMETopHeader(pMsg);
    HANDLE_ERROR(result);

    if (pMsg->flags & NWC24_MSGOBJ_MULTIPART) {
        pMsg->unk10 = pMsg->length;

        result = WriteMIMEPartBoundary(pMsg);
        HANDLE_ERROR(result);
    }

    result = fn_8039F920(pMsg);
    HANDLE_ERROR(result);

    if (!(pMsg->flags & NWC24_MSGOBJ_MULTIPART)) {
        pMsg->unk10 = pMsg->length;
    }

    text.offset = pMsg->length;
    result = WriteTextBody(pMsg);
    HANDLE_ERROR(result);
    text.size = pMsg->length - text.offset;

    for (i = 0; i < (u32)pMsg->numAttached; i++) {
        result = WriteMIMEPartBoundary(pMsg);
        HANDLE_ERROR(result);

        result = fn_8039F7B0(pMsg, i);
        HANDLE_ERROR(result);

        attachment[i].offset = pMsg->length;
        result = WriteAttachData(pMsg, i);
        HANDLE_ERROR(result);
        attachment[i].size = pMsg->length - attachment[i].offset;

        if (i == pMsg->numAttached - 1) {
            result = WriteMIMEPartBoundaryEnd(pMsg);
            HANDLE_ERROR(result);
        }
    }

_error_break:
    if (err == NWC24_OK) {
        // Preserve the writer status when closing the message.
        NWC24iMBoxCloseMsg(&file);
    } else {
        NWC24iMBoxCancelMsg(&file, id, msgId);
        NWC24iMBoxFlushHeader(id);
        goto _exit;
    }

    pMsg->subject = subject;
    pMsg->text = text;
    for (i = 0; i < NWC24i_MSG_ATTACHMENT_MAX; i++) {
        pMsg->attached[i] = attachment[i];
    }
    pMsg->flags |= NWC24_MSGOBJ_DELIVERING;

    if (id == NWC24_MSGBOX_RECV) {
        pMsg->flags |= NWC24_MSGOBJ_TO_RECV;
    } else if (id == NWC24_MSGBOX_SEND) {
        pMsg->flags |= NWC24_MSGOBJ_TO_SEND;
    }

    result = NWC24iMBoxAddMsgObj(id, pMsg);

    if (err != NWC24_OK) {
        result = err;
    }

    err = result;

    if (id == NWC24_MSGBOX_RECV) {
        arrivedFlags = NWC24_MSG_ARRIVED;

        if (pMsg->flags & NWC24_MSGOBJ_FOR_MENU) {
            arrivedFlags |= NWC24_MSG_ARRIVED_FOR_MENU;
        }

        NWC24iSetNewMsgArrived(arrivedFlags);
    }

_exit:
    if (err == NWC24_ERR_FULL || (err <= NWC24_ERR_FILE_OPEN && err >= NWC24_ERR_FILE_OTHER) || err == NWC24_ERR_NAND_CORRUPT ||
        err == NWC24_ERR_FILE_EXISTS || err == NWC24_ERR_INTERNAL_VF || err == NWC24_ERR_FILE_BROKEN) {
        NWC24iSetErrorCode(err - NWC24i_MSG_ERROR_CODE_BASE);
    }

    return err;
}
#undef HANDLE_ERROR

static inline NWC24Err WriteSMTP_DATA(NWC24iMsgObj* pMsg) {
    NWC24Err result;
    char* pWork;

    result = NWC24_OK;

    pWork = NWC24WorkP->stringWork;
    Mail_strcpy(pWork, "DATA\r\n\0");

    result = NWC24FWrite(pWork, Mail_strlen(pWork), OutFile);
    if (result == NWC24_OK) {
        pMsg->length += NWC24i_KSTRLEN("DATA\r\n") - 1;
    }

    return result;
}

static inline NWC24Err WriteMessageIdField(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    u32 fieldLen;
    char buffer[30 + 1 + 1];
    const char* pDomain;
    NWC24UserId myId;

    pWork = NWC24WorkP->stringWork;

    pDomain = NWC24GetAccountDomain();
    NWC24GetMyUserId(&myId);

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(pWork, "Message-Id: <");

    Mail_memset(buffer, 0, sizeof(buffer));
    Mail_sprintf(buffer, "%05X%08X%08X%08X", pMsg->id, (u32)(myId >> 32), (u32)(myId >> 0), pMsg->createTime);
    Mail_strcat(pWork, buffer);

    Mail_strcat(pWork, pDomain);

    Mail_strcat(pWork, ">\r\n");

    fieldLen = Mail_strlen(pWork);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteSubjectField(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    u32 fieldLen;

    pWork = NWC24WorkP->stringWork;
    result = NWC24_OK;

    if (pMsg->subject.size == 0) {
        return NWC24_ERR_NULL;
    }

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(pWork, "Subject: ");
    Mail_strncat(pWork, (const char*)pMsg->subject.pData, 0x3FD);
    Mail_strncat(pWork, "\r\n\0", NWC24i_STRING_WORK_SIZE);

    fieldLen = STD_strnlen(pWork, NWC24i_STRING_WORK_SIZE);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteXWiiCmdField(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    u32 fieldLen;

    if (pMsg->command == 0) {
        return NWC24_OK;
    }

    result = NWC24_OK;
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(pWork, "X-Wii-Cmd: %08X\r\n", pMsg->command);

    fieldLen = Mail_strlen(pWork);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteXWiiTagField(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    u32 fieldLen;

    if (pMsg->tag == 0) {
        return NWC24_OK;
    }

    result = NWC24_OK;
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(pWork, "X-Wii-Tag: %08X\r\n", pMsg->tag);

    fieldLen = Mail_strlen(pWork);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteXWiiDWCIdField(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    u32 fieldLen;

    if (!(pMsg->flags & NWC24_MSGOBJ_DWC)) {
        return NWC24_OK;
    }

    result = NWC24_OK;
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(pWork, "X-Wii-DWCId: %08X\r\n", pMsg->dwcId.size);

    fieldLen = Mail_strlen(pWork);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteXWiiIconNewField(NWC24iMsgObj* pMsg) {
    char* pWork;
    u32 fieldLen;
    NWC24Err result;
    if (pMsg->iconNew == 0 || pMsg->iconNew == 0x80000000) {
        return NWC24_OK;
    }
    pWork = NWC24WorkP->stringWork;
    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(pWork, "X-Wii-IconNew: %08X\r\n", pMsg->iconNew);
    fieldLen = Mail_strlen(pWork);
    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }
    return result;
}

static inline NWC24Err WriteXWiiMBNoReplyField(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    BOOL noreply;

    noreply = NWC24i_MSGOBJ_GET_MB_NOREPLY(pMsg);
    if (!noreply) {
        return NWC24_OK;
    }

    result = NWC24_OK;
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(pWork, "X-Wii-MB-NoReply: -\r\n");

    result = NWC24FWrite(pWork, NWC24i_KSTRLEN("X-Wii-MB-NoReply: -\r\n") - 1, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += NWC24i_KSTRLEN("X-Wii-MB-NoReply: -\r\n") - 1;
    }

    return result;
}

static inline NWC24Err WriteXWiiMBRegDateField(NWC24iMsgObj* pMsg) {
    NWC24Err result = NWC24_OK;
    char* pWork;
    s32 fieldLen;
    u32 regDate;

    if (NWC24i_MSGOBJ_GET_MB_REGDATE(pMsg) == 0) {
        return result;
    }

    regDate = NWC24i_MSGOBJ_GET_MB_REGDATE(pMsg);
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(pWork, "X-Wii-MB-RegDate: %04X\r\n", regDate);

    fieldLen = STD_strnlen(pWork, NWC24i_STRING_WORK_SIZE);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteXWiiMBDelayField(NWC24iMsgObj* pMsg) {
    NWC24Err result = NWC24_OK;
    char* pWork;
    s32 fieldLen;
    u32 delay;

    if (NWC24i_MSGOBJ_GET_MB_DELAY(pMsg) == 0) {
        return result;
    }

    delay = NWC24i_MSGOBJ_GET_MB_DELAY(pMsg);
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(pWork, "X-Wii-MB-Delay: %02X\r\n", delay >> 16);

    fieldLen = STD_strnlen(pWork, NWC24i_STRING_WORK_SIZE);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteUserField(NWC24iMsgObj* pMsg) {
    NWC24Err result;

    result = NWC24_OK;

    if (pMsg->user.size == 0) {
        return NWC24_OK;
    }

    result = NWC24FWrite(pMsg->user.pData, pMsg->user.size, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += pMsg->user.size;
    }

    return result;
}

static inline NWC24Err WriteMIMETopHeader(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    u32 fieldLen;

    result = NWC24_OK;
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(pWork, "MIME-Version: 1.0\r\n");

    if (pMsg->flags & NWC24_MSGOBJ_MULTIPART) {
        Mail_strcat(pWork, "Content-Type: multipart/mixed;\r\n boundary=\"");
        Mail_strcat(pWork, BoundaryStr);
        Mail_strcat(pWork, "\"\r\n");
    }

    fieldLen = Mail_strlen(pWork);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteMIMEPartBoundary(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    u32 fieldLen;

    result = NWC24_OK;
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(pWork, "\r\n--%s", BoundaryStr);
    Mail_strcat(pWork, "\r\n");

    fieldLen = Mail_strlen(pWork);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

static inline NWC24Err WriteTextBody(NWC24iMsgObj* pMsg) {
    NWC24Err result;

    result = fn_8039FA54(pMsg);
    if (result == NWC24_ERR_NULL) {
        return NWC24_OK;
    }

    return result;
}

static inline NWC24Err WriteAttachData(NWC24iMsgObj* pMsg, s32 idx) {
    NWC24Err result;
    u32 written;

    result = fn_8039FB14((const u8*)pMsg->attached[idx].pData, pMsg->attached[idx].size, &written);

    if (result == NWC24_OK) {
        pMsg->length += written;
    }

    return result;
}

static inline NWC24Err WriteMIMEPartBoundaryEnd(NWC24iMsgObj* pMsg) {
    char* pWork;
    NWC24Err result;
    u32 fieldLen;

    result = NWC24_OK;
    pWork = NWC24WorkP->stringWork;

    Mail_memset(pWork, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(pWork, "\r\n--%s", BoundaryStr);
    Mail_strcat(pWork, "--\r\n");

    fieldLen = Mail_strlen(pWork);

    result = NWC24FWrite(pWork, fieldLen, OutFile);
    if (result == NWC24_OK) {
        pMsg->length += fieldLen;
    }

    return result;
}

// checks the header fields of a message
NWC24Err fn_8039E9CC(const NWC24iMsgObj *msg) {
    char *it;

    if (msg->numTo == 0) {
        return NWC24_ERR_NULL;
    }

    if (msg->numTo > NWC24i_MSG_RECIPIENT_MAX) {
        return NWC24_ERR_INVALID_VALUE;
    }

    // folded header lines have to start with a blank
    it = (char *)msg->subject.pData;
    if (it != NULL) {
        for (; *it != 0; it++) {
            if (*it == '\r' && it[1] != '\n') {
                return NWC24_ERR_FORMAT;
            }

            if (*it == '\n' && it[1] != ' ') {
                return NWC24_ERR_FORMAT;
            }
        }
    }

    if (msg->numAttached > NWC24i_MSG_ATTACHMENT_MAX) {
        return NWC24_ERR_INVALID_VALUE;
    }

    return NWC24_OK;
}

// works out how big the finished message gets and makes sure the box has room for it
NWC24Err fn_8039EA6C(NWC24iMsgObj *msg, NWC24MsgBoxId id) {
    u32 total = 0;
    u32 extra;
    int i;
    NWC24Err result;

    for (i = 0; i < msg->numAttached; i++) {
        total = total + (msg->attachedSize[i] * 4 + 2) / 3 + msg->attachedSize[i] / 57 * 2 + 4;
    }

    switch (msg->encoding) {
    case NWC24_ENC_7BIT:
    case NWC24_ENC_8BIT:
        extra = msg->text.size;
        break;
    case NWC24_ENC_BASE64:
        extra = Base64PartSize_i(msg->text.size);
        break;
    case NWC24_ENC_QUOTED_PRINTABLE:
        extra = msg->text.size * 4 / 3;
        break;
    default:
        extra = 0;
        break;
    }

    total += extra;
    if (total >= NWC24i_MBOX_MAX_SIZE) {
        return NWC24_ERR_OVERFLOW;
    }

    result = NWC24iMBoxCheck(id, total + 0x400);
    return result != NWC24_OK ? result : NWC24_OK;
}

// writes a mail address, either "w<id>@domain" or a plain string
NWC24Err fn_8039EDE8(const NWC24iAddr *addr, u32 flags, char *dst, int maxlen, int *written) {
    NWC24Err result = NWC24_OK;
    char *domain;
    char idStr[24];
    int n;

    if (flags & 1) {
        domain = (char *)NWC24GetAccountDomain();
        n = Mail_strlen(domain);
        if (n <= 0) {
            result = NWC24_ERR_CONFIG;
        } else if (n + 0x12 >= maxlen) {
            result = NWC24_ERR_NOMEM;
        } else {
            NWC24iConvIdToStr(addr->id, idStr);
            n = Mail_sprintf(dst, "%c%s%s", 'w', idStr, domain);
            if (n == 0) {
                result = NWC24_ERR_FATAL;
            } else {
                *written = n;
            }
        }
    } else if (flags & 2) {
        if (addr->data.size + 3 >= (u32)maxlen) {
            result = NWC24_ERR_NOMEM;
        } else {
            n = Mail_sprintf(dst, "%s", addr->data.pData);
            if (n == 0) {
                result = NWC24_ERR_FATAL;
            } else {
                *written = n;
            }
        }
    } else {
        result = NWC24_ERR_INVALID_VALUE;
    }

    return result;
}

// MAIL FROM: <sender>
NWC24Err fn_8039EEF8(NWC24iMsgObj *msg) {
    char *buf = NWC24WorkP->stringWork;
    char *p;
    int n;
    int len;
    int mode;
    NWC24Err result;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strncat(buf, "MAIL FROM: ", 0x3FE);
    p = buf + 11;

    mode = 1;
    if (msg->flags & NWC24_MSGOBJ_FLAG_20) {
        mode = 2;
    }

    result = fn_8039EDE8(&msg->from, mode, p, 0x3FE, &n);
    switch (result) {
    case NWC24_OK:
        break;
    default:
        return result;
    }
    p += n;
    p[0] = '\r';
    p[1] = '\n';
    len = n + 13;
    if (0x3FE - len <= 0) {
        return NWC24_ERR_NOMEM;
    }

    if (result == NWC24_OK) {
        result = NWC24FWrite(NWC24WorkP, len, OutFile);
        if (result == NWC24_OK) {
            msg->length += len;
        }
    }

    return result;
}

// RCPT TO: <rcpt> for everybody in the to list
NWC24Err fn_8039EFE0(NWC24iMsgObj *msg) {
    char *p;
    u32 i;
    int n;
    u32 total;
    int avail;
    NWC24Err result;

    total = 0;
    result = NWC24_OK;
    p = NWC24WorkP->stringWork;
    Mail_memset(p, 0, NWC24i_STRING_WORK_SIZE);
    avail = 0x3FE;
    for (i = 0; i < msg->numTo; i++) {
        Mail_strncat(p, "RCPT TO: ", avail);
        p += 9;
        result = fn_8039EDE8(&msg->to[i], msg->flags, p, avail, &n);
        if (result != NWC24_OK) {
            break;
        }
        p += n;
        *p++ = '\r';
        *p++ = '\n';
        avail -= n + 11;
        total += n + 11;
        if (avail <= 0) {
            result = NWC24_ERR_NOMEM;
            break;
        }
    }
    if (result == NWC24_OK) {
        result = NWC24FWrite(NWC24WorkP, total, OutFile);
        if (result == NWC24_OK) {
            msg->length += total;
        }
    }
    return result;
}

// From:
NWC24Err fn_8039F0F4(NWC24iMsgObj *msg) {
    char *buf = NWC24WorkP->stringWork;
    char *p;
    int n;
    int len;
    int mode;
    NWC24Err result;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strncat(buf, "From: ", 0x3FE);
    p = buf + 6;
    msg->fromField.offset = msg->length + 6;

    mode = 1;
    if (msg->flags & NWC24_MSGOBJ_FLAG_20) {
        mode = 2;
    }

    result = fn_8039EDE8(&msg->from, mode, p, 0x3FE, &n);
    switch (result) {
    case NWC24_OK:
        break;
    default:
        return result;
    }
    p += n;
    p[0] = '\r';
    p[1] = '\n';
    len = n + 8;
    if (0x3FE - len <= 0) {
        return NWC24_ERR_NOMEM;
    }

    if (result == NWC24_OK) {
        result = NWC24FWrite(NWC24WorkP, len, OutFile);
        if (result == NWC24_OK) {
            u32 end = msg->length + len;

            msg->length = end;
            msg->fromField.size = end - msg->fromField.offset - 2;
        }
    }

    return result;
}

// To: (comma separated, folded)
NWC24Err fn_8039F1F4(NWC24iMsgObj *msg) {
    NWC24iAddr *addr;
    int n;
    char *p;
    char *buf = NWC24WorkP->stringWork;
    int total;
    int i;
    int avail;
    NWC24Err result = NWC24_OK;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strncat(buf, "To: ", NWC24i_STRING_WORK_SIZE);
    msg->toField.offset = msg->length + 4;

    p = buf + 4;
    addr = msg->to;
    i = 0;
    total = 4;
    avail = 0x3FC;
    for (; i < msg->numTo; addr++, i++) {
        result = fn_8039EDE8(addr, msg->flags, p, avail, &n);
        if (result != NWC24_OK) {
            break;
        }

        avail -= n;
        p += n;
        total += n;
        if (avail <= 4) {
            result = NWC24_ERR_NOMEM;
            break;
        }

        if (i < msg->numTo - 1) {
            p[0] = ',';
            p[1] = '\r';
            p[2] = '\n';
            p[3] = ' ';
            total += 4;
            avail -= 4;
            p += 4;
        }
    }

    p[0] = '\r';
    p[1] = '\n';

    if (result == NWC24_OK) {
        result = NWC24FWrite(NWC24WorkP, total + 2, OutFile);
        if (result == NWC24_OK) {
            u32 end = msg->length + total + 2;

            msg->length = end;
            msg->toField.size = end - msg->toField.offset - 2;
        }
    }

    return result;
}

static char *MonthNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

// Date:
NWC24Err fn_8039F35C(NWC24iMsgObj *msg) {
    NWC24Date date;
    OSCalendarTime ct;
    char *buf;
    int len;
    NWC24Err result;

    NWC24Date_Init(&date);
    fn_8038EC6C(&ct);

    date.year = ct.year;
    date.month = ct.mon + 1;
    date.day = ct.mday;
    date.hour = ct.hour;
    date.min = ct.min;
    date.sec = ct.sec;

    if (date.month > 12) {
        return NWC24_ERR_FORMAT;
    }

    NWC24iDateToMinutes(&msg->createTime, &date);
    if (msg->flags & 0x1000000) {
        fn_803A1344(&date, msg->unk2C);
    }

    buf = NWC24WorkP->stringWork;
    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_sprintf(buf, "Date: %02d %s %d %02d:%02d:%02d -0000\r\n", date.day, MonthNames[date.month - 1], date.year, date.hour,
                 date.min, date.sec);

    len = Mail_strlen(buf);
    result = NWC24FWrite(buf, len, OutFile);
    if (result == NWC24_OK) {
        msg->length += len;
    }

    return result;
}

// X-Wii-AppId:
NWC24Err fn_8039F494(NWC24iMsgObj *msg) {
    char *buf = NWC24WorkP->stringWork;
    u32 flags = 0;
    char tmp[16];
    int len;
    NWC24Err result;

    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(buf, "X-Wii-AppId: ");

    if (!NWC24IsMsgLibOpenedByTool() && NWC24GetAppId() != NWC24i_APP_ID_IPL && msg->appId != FOURCC('H', 'A', 'C', 'A')) {
        msg->appId = NWC24GetAppId();
    }

    if (msg->flags & NWC24_MSGOBJ_FOR_APP) {
        flags |= 1;
    }

    if (msg->flags & NWC24_MSGOBJ_FOR_MENU) {
        flags |= 2;
    }

    Mail_memset(tmp, 0, sizeof(tmp));
    Mail_sprintf(tmp, "%d-%08X-%04X\r\n", flags, msg->appId, msg->groupId);
    Mail_strcat(buf, tmp);

    len = Mail_strlen(buf);
    result = NWC24FWrite(buf, len, OutFile);
    if (result == NWC24_OK) {
        msg->length += len;
    }

    return result;
}

// X-WiiFace: (base64, folded)
NWC24Err fn_8039F5B0(NWC24iMsgObj *msg) {
    const u8 *data;
    int pos;
    int remaining;
    int chunk;
    u32 n;
    NWC24Err result;
    char *buf;

    if (msg->face.size == 0) {
        return NWC24_OK;
    }

    buf = NWC24WorkP->stringWork;
    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(buf, "X-WiiFace:");

    data = (const u8 *)msg->face.pData;
    remaining = msg->face.size;
    pos = 10;
    while (remaining > 0) {
        buf[pos++] = ' ';
        chunk = remaining;
        if (chunk > 42) {
            chunk = 42;
        }

        NWC24Base64Encode(data, chunk, buf + pos, 0x4C, &n);
        data += chunk;
        remaining -= chunk;
        pos += n;
        buf[pos++] = '\r';
        buf[pos++] = '\n';
    }

    result = NWC24FWrite(buf, pos, OutFile);
    if (result == NWC24_OK) {
        msg->length += pos;
    }

    return result;
}

// X-Wii-AltName: (base64, folded)
NWC24Err fn_8039F6B0(NWC24iMsgObj *msg) {
    char *buf;
    const u8 *data;
    int pos;
    int remaining;
    int chunk;
    u32 n;
    NWC24Err result;

    if (msg->alt.size == 0) {
        return NWC24_OK;
    }

    buf = NWC24WorkP->stringWork;
    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    Mail_strcpy(buf, "X-Wii-AltName:");

    data = (const u8 *)msg->alt.pData;
    remaining = msg->alt.size;
    pos = 14;
    while (remaining > 0) {
        buf[pos++] = ' ';
        chunk = remaining;
        if (chunk > 42) {
            chunk = 42;
        }

        NWC24Base64Encode(data, chunk, buf + pos, 0x4C, &n);
        data += chunk;
        remaining -= chunk;
        pos += n;
        buf[pos++] = '\r';
        buf[pos++] = '\n';
    }

    result = NWC24FWrite(buf, pos, OutFile);
    if (result == NWC24_OK) {
        msg->length += pos;
    }

    return result;
}

static char *PartTypeFmt = "Content-Type: %s;\r\n name=%c%07d.%s\r\n";
static char *PartEncodingFmt = "Content-Transfer-Encoding: base64\r\n";
static char *PartDispositionFmt = "Content-Disposition: attachment;\r\n filename=%c%07d.%s\r\n\r\n";
static char *TextTypeFmt = "Content-Type: text/plain; charset=%s\r\n";
static char *TextEncodingFmt = "Content-Transfer-Encoding: %s\r\n\r\n";

// header of one attachment part
NWC24Err fn_8039F7B0(NWC24iMsgObj *msg, int idx) {
    char *buf;
    const char *mimeStr;
    const char *suffix;
    int len;
    int total;
    NWC24Err result;

    result = NWC24_OK;
    total = 0;
    Mail_memset(NWC24WorkP->stringWork, 0, NWC24i_STRING_WORK_SIZE);
    buf = NWC24WorkP->stringWork;
    mimeStr = NWC24GetMIMETypeStr(msg->attachedType[idx]);
    suffix = NWC24iGetMIMETypeSuffix(msg->attachedType[idx]);
    len = Mail_strlen(PartTypeFmt) + Mail_strlen(PartEncodingFmt) +
          Mail_strlen(PartDispositionFmt) + Mail_strlen(mimeStr) + 4;
    if (len >= NWC24i_STRING_WORK_SIZE) {
        return NWC24_ERR_NOMEM;
    }
    len = Mail_sprintf(buf, PartTypeFmt, mimeStr, (char)('a' + idx), msg->id, suffix);
    if (len <= 0) {
        return NWC24_ERR_FAILED;
    }
    buf += len;
    total += len;
    len = Mail_sprintf(buf, PartEncodingFmt);
    if (len <= 0) {
        return NWC24_ERR_FAILED;
    }
    buf += len;
    total += len;
    len = Mail_sprintf(buf, PartDispositionFmt, (char)('a' + idx), msg->id, suffix);
    if (len <= 0) {
        return NWC24_ERR_FAILED;
    }
    buf += len;
    total += len;
    result = NWC24FWrite(NWC24WorkP, total, OutFile);
    if (result == NWC24_OK) {
        msg->length += total;
    }
    return result;
}

// headers for the text part (charset + transfer encoding)
NWC24Err fn_8039F920(NWC24iMsgObj *msg) {
    char *buf;
    const char *charset;
    const char *encoding;
    u32 total;
    NWC24Err result;
    int len;

    total = 0;
    result = NWC24_OK;
    buf = NWC24WorkP->stringWork;
    Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
    charset = NWC24GetCharsetStr(msg->charset);
    if (charset != NULL) {
        Mail_sprintf(buf, TextTypeFmt, charset);
        msg->contentType.offset = msg->length + 0x22;
        len = Mail_strlen(charset);
        msg->contentType.size = len + 1;
        len = Mail_strlen(buf);
        total += len;
        buf += len;
    }
    encoding = NWC24GetEncodingStr(msg->encoding);
    if (encoding != NULL) {
        Mail_sprintf(buf, TextEncodingFmt, encoding);
        msg->txEncoding.offset = total + msg->length + 0x1B;
        len = Mail_strlen(encoding);
        msg->txEncoding.size = len + 1;
        len = Mail_strlen(buf);
        total += len;
    }
    if (total == 0) {
        return NWC24_OK;
    }
    result = NWC24FWrite(NWC24WorkP, total, OutFile);
    if (result == NWC24_OK) {
        msg->length += total;
    }
    return result;
}

// the body text
NWC24Err fn_8039FA54(NWC24iMsgObj *msg) {
    NWC24Err result;
    u32 written;

    if (msg->text.size == 0) {
        return NWC24_ERR_NULL;
    }

    switch (msg->encoding) {
    case NWC24_ENC_7BIT:
    case NWC24_ENC_8BIT:
        result = NWC24FWrite(msg->text.pData, msg->text.size, OutFile);
        written = msg->text.size;
        break;
    case NWC24_ENC_BASE64:
        result = fn_8039FB14((const u8 *)msg->text.pData, msg->text.size, &written);
        break;
    case NWC24_ENC_QUOTED_PRINTABLE:
        result = fn_8039FC78((const char *)msg->text.pData, msg->text.size, &written);
        break;
    default:
        result = NWC24_ERR_INVALID_VALUE;
        break;
    }

    if (result == NWC24_OK) {
        msg->dwcId.offset = msg->text.size;
        msg->length += written;
    }

    return result;
}

// base64 text, 57 bytes (76 chars) per line
NWC24Err fn_8039FB14(const u8 *data, int size, u32 *written) {
    const u8 *cursor = data;
    int remaining = size;
    char *buf = NWC24WorkP->stringWork;
    u32 total = 0;
    NWC24Err result = NWC24_OK;
    u32 n;

    while (remaining >= 57) {
        Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
        result = NWC24Base64Encode(cursor, 57, buf, 0x4C, &n);
        if (result != NWC24_OK) {
            break;
        }

        buf[n++] = '\r';
        buf[n++] = '\n';
        buf[n] = 0;
        result = NWC24FWrite(buf, n, OutFile);
        if (result != NWC24_OK) {
            break;
        }

        cursor += 57;
        remaining -= 57;
        total += n;
    }

    if (result == NWC24_OK) {
        Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
        result = NWC24Base64Encode(cursor, remaining, buf, 0x4C, &n);
        if (result == NWC24_OK) {
            buf[n++] = '\r';
            buf[n++] = '\n';
            buf[n++] = '\r';
            buf[n++] = '\n';
            buf[n] = 0;
            result = NWC24FWrite(buf, n, OutFile);
            if (result == NWC24_OK) {
                total += n;
            }
        }
    }

    *written = total;
    return result;
}

// quoted printable text, the soft line break has to go out between the chunks
NWC24Err fn_8039FC78(const char *data, int size, u32 *written) {
    char *buf = NWC24WorkP->stringWork;
    const char *cursor = data;
    int remaining = size;
    u32 total = 0;
    NWC24Err result = NWC24_OK;
    u32 inN;
    u32 outN;

    while (remaining > 0) {
        // BYTES: the native soft-break guard tests the byte count as a signed word.
        if ((s32)total > 0) {
            Mail_memcpy(buf, "=\r\n", 3);
            result = NWC24FWrite(buf, 3, OutFile);
            if (result != NWC24_OK) {
                break;
            }

            total += 3;
        }

        Mail_memset(buf, 0, NWC24i_STRING_WORK_SIZE);
        result = NWC24EncodeQuotedPrintable(buf, 0x400, &outN, (const u8 *)cursor, remaining, &inN);
        if (result != NWC24_OK && result != NWC24_ERR_OVERFLOW) {
            break;
        }

        result = NWC24FWrite(buf, outN, OutFile);
        if (result != NWC24_OK) {
            break;
        }

        cursor += inN;
        remaining -= inN;
        total += outN;
    }

    *written = total;
    return result;
}
