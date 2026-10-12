// usb: thin client for the iop usb stack (/dev/usb). open/close devices by vid+pid, control/bulk/interrupt
// transfers and a few queries, all through io vectors carved out of a private ipc heap. async calls park their
// scratch buffers in a command block and free them from the completion callback.
#include <revolution/FS.h>
#include <revolution/OS.h>
#include <revolution/USB.h>
#include <stdio.h>
#include <string.h>

// built as c++ against the openrvl headers, keep c linkage
extern "C" {

#define USB_HEAP_SIZE 0x4000
#define USB_MAX_PATH 64
#define IPC_ALIGN 32
#define ROUNDUP_IPC(sz) (((u32)(sz) + IPC_ALIGN - 1) & ~(u32)(IPC_ALIGN - 1))

#define USB_ERR_INVALID (-4)
#define USB_ERR_ALLOC (-22)

typedef enum {
    USB_IOCTLV_CTRLMSG = 0,
    USB_IOCTLV_BLKMSG = 1,
    USB_IOCTLV_INTRMSG = 2,
} USBIoctl;

// nclean values the completion callback accepts: how many scratch buffers hang off the block
typedef enum {
    USB_NCLEAN_CLOSEDEVICE = 0,
    USB_NCLEAN_SMALLMSG = 2,
    USB_NCLEAN_BULKMSG = 3,
    USB_NCLEAN_INTRMSG = 4,
    USB_NCLEAN_CTRLMSG = 7,
    USB_NCLEAN_MAX = 8
} USBNClean;

typedef void (*USBIsoCallback)(IPCResult result, void* xfer, void* arg);

typedef struct USBMsg {
    void* buffer; // at 0x0
    u16 length;   // at 0x4
} USBMsg;

typedef struct USBCommandBlock {
    USBCallback callback;    // at 0x0
    USBIsoCallback isoCallback; // at 0x4, used when callback is not set
    void* callbackArg;       // at 0x8
    void* xfer;              // at 0xC
    void* spare;             // at 0x10
    void* clean[USB_NCLEAN_MAX]; // at 0x14
    u32 nclean;              // at 0x34
    char UNK_0x38[0x40 - 0x38];
    union {
        char path[ROUNDUP_IPC(USB_MAX_PATH)];
        char des[ROUNDUP_IPC(18)];
        char reqBuf[ROUNDUP_IPC(sizeof(USBMsg))];
    }; // at 0x40
} USBCommandBlock;

typedef struct USBDevDescr {
    u8 raw[18];
} USBDevDescr;

static s32 hId = -1;

static void* lo = NULL;
static void* hi = NULL;

static u8 s_usb_log = FALSE;
static u8 s_usb_err = TRUE;

static void USB_LOG(const char* fmt, ...) {
    va_list list;

    if (s_usb_log) {
        OSReport("USB: ");
        va_start(list, fmt);
        vprintf(fmt, list);
        va_end(list);
    }
}

static void USB_ERR(const char* fmt, ...) {
    va_list list;

    if (s_usb_err) {
        OSReport("USB ERR: ");
        va_start(list, fmt);
        vprintf(fmt, list);
        va_end(list);
    }
}

static void* IOSAlloc(u32 size) {
    void* mem;

    mem = iosAllocAligned(hId, size, IPC_ALIGN);

    if (mem == NULL) {
        USB_ERR("iosAllocAligned(%d, %u) failed: %d\n", hId, size, mem);
    }

    return mem;
}

static void IOSFree(void* mem) {
    s32 result;

    if (mem != NULL) {
        result = iosFree(hId, mem);

        if (result < 0) {
            USB_ERR("iosFree(%d, 0x%x) failed: %d\n", hId, mem, result);
        }
    }
}

IPCResult IUSB_OpenLib(void) {
    IPCResult result;
    BOOL enabled;

    result = IPC_RESULT_OK;
    enabled = OSDisableInterrupts();

    if (hId != -1) {
        USB_LOG("Library is already initialized. Heap Id = %d\n", hId);
        goto end;
    }

    if (lo == NULL) {
        lo = IPCGetBufferLo();
        hi = IPCGetBufferHi();

        USB_LOG("iusb size: %d lo: %x hi: %x\n", sizeof(USBCommandBlock), lo, hi);

        if ((u8*)lo + ROUNDUP_IPC(USB_HEAP_SIZE) > hi) {
            USB_ERR("Not enough IPC arena\n");
            result = IPC_RESULT_ALLOC_FAILED;
            goto end;
        }

        IPCSetBufferLo((u8*)lo + ROUNDUP_IPC(USB_HEAP_SIZE));
    }

    hId = iosCreateHeap(lo, USB_HEAP_SIZE);
    if (hId < 0) {
        USB_ERR("Not enough heaps\n");
        result = IPC_RESULT_ALLOC_FAILED;
        goto end;
    }

end:
    OSRestoreInterrupts(enabled);
    return result;
}

IPCResult IUSB_CloseLib(void) {
    return IPC_RESULT_OK;
}

static s32 _intBlkCtrlCb(s32 result, void* arg) {
    u32 i;
    s32 rv = result;
    USBCommandBlock* block = (USBCommandBlock*)arg;

    USB_LOG("_intrBlkCtrlIsoCb returned: %d\n", rv);
    USB_LOG("_intrBlkCtrlIsoCb: nclean = %d\n", block->nclean);

    if (block->nclean != USB_NCLEAN_CTRLMSG && block->nclean != USB_NCLEAN_BULKMSG &&
        block->nclean != USB_NCLEAN_CLOSEDEVICE && block->nclean != USB_NCLEAN_INTRMSG &&
        block->nclean != USB_NCLEAN_SMALLMSG) {
        USB_ERR("__intrBlkCtrlIsoCb: got invalid nclean\n");
    } else {
        for (i = 0; i < block->nclean; ++i) {
            USB_LOG("Freeing clean[%d] = %x\n", i, block->clean[i]);
            IOSFree(block->clean[i]);
        }
        block->nclean = 0;
    }

    USB_LOG("cb = %x cbArg = %x\n", block->callback, block->callbackArg);
    if (block->callback != NULL) {
        block->callback((IPCResult)result, block->callbackArg);
    } else if (block->isoCallback != NULL) {
        USB_LOG("calling iso callback\n");
        block->isoCallback((IPCResult)result, block->xfer, block->callbackArg);
    }

    IOSFree(block);
    return rv;
}

IPCResult IUSB_OpenDeviceIds(const char* interface, u16 vid, u16 pid, IPCResult* resultOut) {
    s32 result = 0;
    USBCommandBlock* block = NULL;

    if (resultOut == NULL) {
        result = USB_ERR_INVALID;
        goto end;
    }

    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
    if (block == NULL) {
        USB_ERR("OpenDeviceIds: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto end;
    }
    memset(block, 0, sizeof(USBCommandBlock));

    snprintf(block->path, USB_MAX_PATH, "/dev/usb/%s/%x/%x", interface, vid, pid);

    USB_LOG("OpenDevice - %s\n", block->path);

    result = IOS_Open(block->path, IPC_OPEN_NONE);
    USB_LOG("OpenDevice returned: %d\n", result);

    *resultOut = (IPCResult)result;

end:
    IOSFree(block);
    return (IPCResult)result;
}

IPCResult IUSB_OpenDeviceIdsAsync(const char* interface, u16 vid, u16 pid, USBCallback callback, void* callbackArg) {
    s32 result = 0;
    USBCommandBlock* block;

    USB_LOG("OpenDevice\n");
    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
    if (block == NULL) {
        USB_ERR("OpenDeviceIdsAsync: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto end;
    }
    memset(block, 0, sizeof(USBCommandBlock));

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->nclean = 0;

    snprintf(block->path, USB_MAX_PATH, "/dev/usb/%s/%x/%x", interface, vid, pid);

    USB_LOG("OpenDevice - %s\n", block->path);

    result = IOS_OpenAsync(block->path, IPC_OPEN_NONE, _intBlkCtrlCb, block);
    USB_LOG("OpenDevice returned: %d\n", result);
    if (result < 0) {
        IOSFree(block);
    }

end:
    return (IPCResult)result;
}

IPCResult IUSB_CloseDevice(s32 fd) {
    s32 result;

    USB_LOG("CloseDevice\n");

    result = IOS_Close(fd);

    USB_LOG("CloseDevice returned: %d\n", result);

    return (IPCResult)result;
}

IPCResult IUSB_CloseDeviceAsync(s32 fd, USBCallback callback, void* callbackArg) {
    s32 result = 0;
    USBCommandBlock* block;

    USB_LOG("CloseDevice\n");
    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
    if (block == NULL) {
        USB_ERR("CloseDeviceAsync: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto end;
    }
    memset(block, 0, sizeof(USBCommandBlock));

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->nclean = USB_NCLEAN_CLOSEDEVICE;

    result = IOS_CloseAsync(fd, _intBlkCtrlCb, block);
    USB_LOG("CloseDevice returned: %d\n", result);
    if (result < 0) {
        IOSFree(block);
    }

end:
    return (IPCResult)result;
}

static s32 __openDevice(const char* path) {
    s32 result;
    USBCommandBlock* block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));

    if (block == NULL) {
        USB_ERR("openDevice: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto done;
    }
    strncpy(block->path, path, USB_MAX_PATH);
    result = IOS_Open(block->path, IPC_OPEN_NONE);
    IOSFree(block);

done:
    return result;
}

typedef struct USBDeviceInfo {
    u8 raw[0x40];
} USBDeviceInfo;

IPCResult IUSB_GetDeviceList(const char* path, USBDeviceInfo* deviceList, u8 maxDev, u8 deviceClass, u8* numDev) {
    s32 result = 0;
    IPCIOVector* vector = NULL;
    u8* smaxDev = NULL;
    u8* sclass = NULL;
    u8* snumDev = NULL;
    s32 fd;

    if ((u32)deviceList & 0x1f) {
        result = USB_ERR_INVALID;
        goto end;
    }

    if ((fd = __openDevice(path)) < 0) {
        result = fd;
        goto end;
    }

    vector = (IPCIOVector*)IOSAlloc(ROUNDUP_IPC(sizeof(IPCIOVector)) * 4);
    smaxDev = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    sclass = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    snumDev = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    if (smaxDev == NULL || sclass == NULL || snumDev == NULL || vector == NULL) {
        USB_ERR("getDeviceList: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto end;
    }

    *smaxDev = maxDev;
    *sclass = deviceClass;
    *snumDev = 0;
    vector[0].base = smaxDev;
    vector[0].length = sizeof(u8);
    vector[1].base = sclass;
    vector[1].length = sizeof(u8);
    vector[2].base = snumDev;
    vector[2].length = sizeof(u8);
    vector[3].base = deviceList;
    vector[3].length = sizeof(USBDeviceInfo) * maxDev;

    DCInvalidateRange(deviceList, sizeof(USBDeviceInfo) * maxDev);
    DCInvalidateRange(snumDev, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(sclass, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(smaxDev, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(vector, ROUNDUP_IPC(sizeof(IPCIOVector)) * 4);

    result = IOS_Ioctlv(fd, 0, 2, 2, vector);
    *numDev = *snumDev;
    IOS_Close(fd);

end:
    IOSFree(snumDev);
    IOSFree(smaxDev);
    IOSFree(sclass);
    IOSFree(vector);
    return (IPCResult)result;
}

static IPCResult __IntrBlkMsgInt(s32 fd, u32 endpoint, u32 length, void* buffer, u8 type, USBCallback callback,
                                 void* callbackArg, u8 async) {
    s32 result = 0;

    IPCIOVector* vector = (IPCIOVector*)IOSAlloc(ROUNDUP_IPC(sizeof(IPCIOVector)) * 3);
    u8* sep = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    u16* slength = (u16*)IOSAlloc(ROUNDUP_IPC(sizeof(u32)));

    if (vector == NULL || sep == NULL || slength == NULL) {
        USB_ERR("__IntrBlkMsgInt: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto clean;
    }

    *sep = (u8)endpoint;
    *slength = (u16)length;

    vector[0].base = sep;
    vector[0].length = sizeof(u8);
    vector[1].base = slength;
    vector[1].length = sizeof(u16);
    vector[2].base = buffer;
    vector[2].length = length;

    DCFlushRange(sep, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(slength, ROUNDUP_IPC(sizeof(u16)));
    DCFlushRange(vector, ROUNDUP_IPC(sizeof(IPCIOVector)) * 3);

    if (async == 0) {
        result = IOS_Ioctlv(fd, type, 2, 1, vector);
        USB_LOG("intr/blk ioctl returned: %d\n", result);
        goto clean;
    } else {
        USBCommandBlock* block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
        if (block == NULL) {
            USB_ERR("IntBlkMsgInt (async): Not enough memory\n");
            result = USB_ERR_ALLOC;
            goto clean;
        }
        memset(block, 0, sizeof(USBCommandBlock));
        block->callback = callback;
        block->callbackArg = callbackArg;
        USB_LOG("intrblkmsg: cb = 0x%x cbArg = 0x%x\n", block->callback, block->callbackArg);
        block->nclean = USB_NCLEAN_BULKMSG;
        block->clean[0] = sep;
        block->clean[1] = slength;
        block->clean[2] = vector;
        ((USBMsg*)block->reqBuf)->buffer = buffer;
        ((USBMsg*)block->reqBuf)->length = (u16)length;

        result = IOS_IoctlvAsync(fd, type, 2, 1, vector, _intBlkCtrlCb, block);
        if (result < 0) {
            IOSFree(block);
            goto clean;
        }
        goto end;
    }

clean:
    IOSFree(sep);
    IOSFree(slength);
    IOSFree(vector);

end:
    return (IPCResult)result;
}

IPCResult IUSB_ReadIntrMsgAsync(s32 fd, u32 endpoint, u32 length, void* buffer, USBCallback callback,
                                void* callbackArg) {
    DCInvalidateRange(buffer, length);
    return __IntrBlkMsgInt(fd, endpoint, length, buffer, USB_IOCTLV_INTRMSG, callback, callbackArg, 1);
}

IPCResult IUSB_ReadBlkMsgAsync(s32 fd, u32 endpoint, u32 length, void* buffer, USBCallback callback,
                               void* callbackArg) {
    DCInvalidateRange(buffer, length);
    return __IntrBlkMsgInt(fd, endpoint, length, buffer, USB_IOCTLV_BLKMSG, callback, callbackArg, 1);
}

IPCResult IUSB_WriteBlkMsgAsync(s32 fd, u32 endpoint, u32 length, const void* buffer, USBCallback callback,
                                void* callbackArg) {
    DCFlushRange((void*)buffer, length);
    return __IntrBlkMsgInt(fd, endpoint, length, (void*)buffer, USB_IOCTLV_BLKMSG, callback, callbackArg, 1);
}

static IPCResult __CtrlMsgInt(s32 fd, u8 requestType, u8 request, u16 value, u16 index, u16 length, void* buffer,
                              USBCallback callback, void* callbackArg, u8 async) {
    s32 result = 0;
    IPCIOVector* vector;
    u8* sreqType;
    u8* srequest;
    u8* sep;
    u16* svalue;
    u16* sindex;
    u16* slength;

    if ((buffer == NULL && length != 0) || ((u32)buffer & (IPC_ALIGN - 1))) {
        result = USB_ERR_INVALID;
        USB_ERR("ctrlmsg: bad data buffer\n");
        goto end;
    }

    vector = (IPCIOVector*)IOSAlloc(ROUNDUP_IPC(sizeof(IPCIOVector)) * 7);
    sreqType = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    srequest = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    sep = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    svalue = (u16*)IOSAlloc(ROUNDUP_IPC(sizeof(u16)));
    sindex = (u16*)IOSAlloc(ROUNDUP_IPC(sizeof(u16)));
    slength = (u16*)IOSAlloc(ROUNDUP_IPC(sizeof(u16)));
    if (sreqType == NULL || srequest == NULL || sep == NULL || svalue == NULL || sindex == NULL ||
        slength == NULL || vector == NULL) {
        USB_ERR("Ctrl Msg: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto clean;
    }

    *sreqType = requestType;
    *srequest = request;
    // the iop wants these big endian, same as on the wire
    __sthbrx(value, svalue, 0);
    __sthbrx(index, sindex, 0);
    __sthbrx(length, slength, 0);
    *sep = 0;

    vector[0].base = sreqType;
    vector[0].length = sizeof(u8);
    vector[1].base = srequest;
    vector[1].length = sizeof(u8);
    vector[2].base = svalue;
    vector[2].length = sizeof(u16);
    vector[3].base = sindex;
    vector[3].length = sizeof(u16);
    vector[4].base = slength;
    vector[4].length = sizeof(u16);
    vector[5].base = sep;
    vector[5].length = sizeof(u8);
    vector[6].base = buffer;
    vector[6].length = length;

    DCFlushRange(sreqType, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(srequest, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(sep, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(svalue, ROUNDUP_IPC(sizeof(u16)));
    DCFlushRange(sindex, ROUNDUP_IPC(sizeof(u16)));
    DCFlushRange(slength, ROUNDUP_IPC(sizeof(u16)));
    DCFlushRange(vector, ROUNDUP_IPC(sizeof(IPCIOVector)) * 7);

    if (async == 0) {
        result = IOS_Ioctlv(fd, USB_IOCTLV_CTRLMSG, 6, 1, vector);
        goto clean;
    } else {
        USBCommandBlock* block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
        if (block == NULL) {
            USB_ERR("CtrlMsgInt (async): Not enough memory\n");
            result = USB_ERR_ALLOC;
            goto clean;
        }
        memset(block, 0, sizeof(USBCommandBlock));
        block->callback = callback;
        block->callbackArg = callbackArg;
        USB_LOG("ctrlmsgint: cb = 0x%x cbArg = 0x%x\n", block->callback, block->callbackArg);
        block->nclean = USB_NCLEAN_CTRLMSG;
        block->clean[0] = sreqType;
        block->clean[1] = srequest;
        block->clean[2] = svalue;
        block->clean[3] = sindex;
        block->clean[4] = slength;
        block->clean[5] = sep;
        block->clean[6] = vector;
        ((USBMsg*)block->reqBuf)->buffer = buffer;
        ((USBMsg*)block->reqBuf)->length = length;

        result = IOS_IoctlvAsync(fd, USB_IOCTLV_CTRLMSG, 6, 1, vector, _intBlkCtrlCb, block);
        USB_LOG("Ctrl Msg async returned: %d\n", result);
        if (result < 0) {
            IOSFree(block);
            goto clean;
        }
        goto end;
    }

clean:
    IOSFree(sreqType);
    IOSFree(srequest);
    IOSFree(svalue);
    IOSFree(sindex);
    IOSFree(slength);
    IOSFree(sep);
    IOSFree(vector);

end:
    return (IPCResult)result;
}

IPCResult IUSB_WriteCtrlMsgAsync(s32 fd, u8 requestType, u8 request, u16 value, u16 index, u16 length, void* buffer,
                                 USBCallback callback, void* callbackArg) {
    DCFlushRange(buffer, length);
    return __CtrlMsgInt(fd, requestType, request, value, index, length, buffer, callback, callbackArg, 1);
}

// string descriptors come back as utf-16, squash them down to ascii in place
static s8 unicode2ascii(char* tbuf, int buflen) {
    char buf[128];
    s8 di, si;

    if (tbuf[1] != 3) {
        di = -1;
        goto out;
    }

    for (di = 0, si = 2; si < tbuf[0] && si < buflen; si += 2) {
        if (di >= (int)(sizeof(buf) - 1)) {
            break;
        }
        if (tbuf[si + 1]) {
            buf[di++] = '?';
        } else {
            buf[di++] = tbuf[si];
        }
    }
    buf[di] = 0;
    memcpy(tbuf, buf, (u32)di);

out:
    return di;
}

static void _GetStrCb(IPCResult result, void* arg) {
    s32 rv = result;
    USBCommandBlock* block = (USBCommandBlock*)arg;
    char* buf;
    u16 buflen;
    s8 len;

    USB_LOG("GetStrCb returned: %d\n", rv);

    if (rv <= 0) {
        goto out;
    }

    buf = (char*)((USBMsg*)block->reqBuf)->buffer;
    buflen = ((USBMsg*)block->reqBuf)->length;
    USB_LOG("GetStrCb: buf = 0x%x buflen = %u\n", buf, buflen);
    if ((len = unicode2ascii(buf, buflen)) < 0) {
        USB_ERR("Failed to convert buffer from unicode 2 ascii\n");
    } else {
        buf[len] = '\0';
    }

out:
    if (block->callback) {
        USB_LOG("calling cb 0x%x with arg 0x%x\n", block->callback, block->callbackArg);
        block->callback(result, block->callbackArg);
    }
    IOSFree(block);
}

IPCResult IUSB_GetAsciiStr(s32 fd, u8 ep, u16 index, u16 langId, char* buf, u16 buflen) {
#pragma unused(ep)
    s32 result = 0;
    s8 len;

    USB_LOG("GetStr\n");

    DCInvalidateRange(buf, (u32)buflen);

    result = __CtrlMsgInt(fd, 0x80, 6, (u16)((3 << 8) + index), langId, buflen, buf, NULL, NULL, 0);
    if (result <= 0) {
        USB_ERR("Failed __CtrlMsg: %d", result);
        goto out;
    }
    if ((len = unicode2ascii(buf, buflen)) < 0) {
        USB_ERR("Failed to convert unicode 2 ascii\n");
    } else {
        buf[len] = '\0';
    }

out:
    return (IPCResult)result;
}

IPCResult IUSB_GetAsciiStrAsync(s32 fd, u8 ep, u16 index, u16 langId, char* buf, u16 buflen, USBCallback callback,
                                void* callbackArg) {
#pragma unused(ep)
    s32 result = 0;
    USBCommandBlock* block;

    USB_LOG("GetStr - _GetStrCb\n");
    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
    if (block == NULL) {
        USB_ERR(" GetAsciiStrAsync: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto out;
    }
    memset(block, 0, sizeof(USBCommandBlock));

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->nclean = 0;
    ((USBMsg*)block->reqBuf)->buffer = buf;
    ((USBMsg*)block->reqBuf)->length = buflen;

    DCInvalidateRange(buf, buflen);
    result = __CtrlMsgInt(fd, 0x80, 6, (u16)((3 << 8) + index), langId, buflen, buf, (USBCallback)_GetStrCb, block, 1);
    if (result < 0) {
        USB_ERR("__CtrlMsgInt failed %d\n", result);
        IOSFree(block);
        goto out;
    }

out:
    return (IPCResult)result;
}

static void _GetDescrCb(IPCResult result, void* arg) {
    s32 rv = result;
    USBCommandBlock* block = (USBCommandBlock*)arg;

    USB_LOG("GetDescrCb returned: %d\n", rv);

    if (rv > 0) {
        *(USBDevDescr*)(block->spare) = *(USBDevDescr*)block->des;
    }

    if (block->callback) {
        block->callback(result, block->callbackArg);
    }

    IOSFree(block);
}

IPCResult IUSB_GetDevDescr(s32 fd, USBDevDescr* des) {
    s32 result = 0;
    USBCommandBlock* block = NULL;

    USB_LOG("GetDevDescr\n");
    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
    if (block == NULL) {
        USB_ERR("GetDevDescr: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto out;
    }
    memset(block, 0, sizeof(USBCommandBlock));

    DCInvalidateRange(block->des, (u32)sizeof(*des));

    result = __CtrlMsgInt(fd, 0x80, 6, 0x0100, 0, sizeof(*des), block->des, NULL, NULL, 0);
    if (result < 0) {
        USB_ERR("Failed __CtrlMsg: %d", result);
        goto out;
    }

    USB_LOG("GetDevDescr: %d\n", result);

    *des = *((USBDevDescr*)block->des);

out:
    IOSFree(block);
    return (IPCResult)result;
}

IPCResult IUSB_GetDevDescrAsync(s32 fd, USBDevDescr* des, USBCallback callback, void* callbackArg) {
    s32 result = 0;
    USBCommandBlock* block;

    USB_LOG("GetDevDescr - _GetDescrCb\n");
    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
    if (block == NULL) {
        USB_ERR("GetDevDescrAsync: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto out;
    }
    memset(block, 0, sizeof(USBCommandBlock));

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->nclean = 0;
    block->spare = (void*)des;

    DCInvalidateRange(block->des, (u32)sizeof(*des));
    result = __CtrlMsgInt(fd, 0x80, 6, 0x0100, 0, sizeof(*des), block->des, (USBCallback)_GetDescrCb, block, 1);
    if (result < 0) {
        IOSFree(block);
        goto out;
    }

out:
    return (IPCResult)result;
}

IPCResult IUSB_DeviceRemovalNotifyAsync(s32 fd, USBCallback callback, void* callbackArg) {
    s32 result;
    USB_LOG("DeviceRemovalNotifyAsync\n");
    result = IOS_IoctlAsync(fd, 0, NULL, 0, NULL, 0, (IPCAsyncCallback)callback, callbackArg);
    return (IPCResult)result;
}

typedef struct USBIsoTransfer {
    void* buf;      // at 0x0
    u16* packets;   // at 0x4
    u8 numPackets;  // at 0x8
} USBIsoTransfer;

static s32 __checkIsoArgs(USBIsoTransfer* xfer, u16* buflen) {
    s32 result = USB_ERR_INVALID;

    if (xfer == NULL || xfer->buf == NULL || xfer->numPackets == 0 || xfer->numPackets > 8 || xfer->packets == NULL) {
        goto out;
    }

    {
        u32 i;
        *buflen = 0;
        for (i = 0; i < xfer->numPackets; ++i) {
            if (xfer->packets[i] > 1023) {
                USB_ERR("packet %u too big: %u\n", i, xfer->packets[i]);
                goto out;
            }
            *buflen += xfer->packets[i];
        }
    }
    result = 0;

out:
    return result;
}

IPCResult IUSB_IsoMsgAsync(s32 fd, u8 ep, USBIsoTransfer* xfer, USBIsoCallback callback, void* callbackArg) {
    s32 result = 0;
    u8* sep;
    u8* snumPackets;
    u16* sbuflen;
    IPCIOVector* vector;
    USBCommandBlock* block;
    u16 buflen;

    if (ep == 0 || callback == NULL || (__checkIsoArgs(xfer, &buflen) < 0)) {
        USB_ERR("Invalid parameters for ISO transfer request\n");
        result = USB_ERR_INVALID;
        goto out;
    }

    vector = (IPCIOVector*)IOSAlloc(ROUNDUP_IPC(sizeof(IPCIOVector)) * 5);
    sep = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    sbuflen = (u16*)IOSAlloc(ROUNDUP_IPC(sizeof(u16)));
    snumPackets = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));

    if (vector == NULL || sep == NULL || sbuflen == NULL || block == NULL) {
        USB_ERR("IUSB_IsoMsgAsync: Not enough memory\n");
        result = USB_ERR_ALLOC;
        IOSFree(block);
        goto clean;
    }
    memset(block, 0, sizeof(USBCommandBlock));

    *sep = ep;
    *sbuflen = buflen;
    *snumPackets = xfer->numPackets;

    vector[0].base = sep;
    vector[0].length = sizeof(u8);
    vector[1].base = sbuflen;
    vector[1].length = sizeof(u16);
    vector[2].base = snumPackets;
    vector[2].length = sizeof(u8);
    vector[3].base = xfer->packets;
    vector[3].length = sizeof(u16) * xfer->numPackets;
    vector[4].base = xfer->buf;
    vector[4].length = buflen;

    DCFlushRange(sep, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(sbuflen, ROUNDUP_IPC(sizeof(u16)));
    DCFlushRange(snumPackets, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(vector, ROUNDUP_IPC(sizeof(IPCIOVector)) * 5);

    block->isoCallback = callback;
    block->xfer = xfer;
    block->callbackArg = callbackArg;
    block->nclean = USB_NCLEAN_INTRMSG;
    block->clean[0] = sep;
    block->clean[1] = sbuflen;
    block->clean[2] = snumPackets;
    block->clean[3] = vector;

    result = IOS_IoctlvAsync(fd, 0, 3, 2, vector, _intBlkCtrlCb, block);
    if (result < 0) {
        IOSFree(block);
        goto clean;
    }
    goto out;

clean:
    IOSFree(sep);
    IOSFree(sbuflen);
    IOSFree(snumPackets);
    IOSFree(vector);

out:
    return (IPCResult)result;
}

IPCResult IUSB_DeviceInsertionNotifyAsync(const char* path, u16 vid, u16 pid, USBCallback callback,
                                          void* callbackArg) {
    s32 result;
    s32 fd;
    IPCIOVector* vector;
    u16* svid;
    u16* spid;
    USBCommandBlock* block;

    if (path == NULL || vid == 0 || pid == 0) {
        result = USB_ERR_INVALID;
        goto done;
    }
    if ((fd = IOS_Open(path, IPC_OPEN_NONE)) < 0) {
        result = fd;
        USB_ERR("Open(%s) failed\n", path);
        goto done;
    }
    vector = (IPCIOVector*)IOSAlloc(ROUNDUP_IPC(sizeof(IPCIOVector)) * 2);
    spid = (u16*)IOSAlloc(ROUNDUP_IPC(sizeof(u16)));
    svid = (u16*)IOSAlloc(ROUNDUP_IPC(sizeof(u16)));
    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));
    if (vector == NULL || spid == NULL || svid == NULL || block == NULL) {
        USB_ERR("getDeviceList: Not enough memory\n");
        result = USB_ERR_ALLOC;
        goto clean;
    }
    memset(block, 0, sizeof(USBCommandBlock));
    memset(block, 0, sizeof(USBCommandBlock));

    *svid = vid;
    *spid = pid;
    vector[0].base = svid;
    vector[0].length = sizeof(u16);
    vector[1].base = spid;
    vector[1].length = sizeof(u16);

    DCFlushRange(svid, ROUNDUP_IPC(sizeof(u16)));
    DCFlushRange(spid, ROUNDUP_IPC(sizeof(u16)));
    DCFlushRange(vector, ROUNDUP_IPC(sizeof(IPCIOVector)) * 2);

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->nclean = USB_NCLEAN_BULKMSG;
    block->clean[0] = svid;
    block->clean[1] = spid;
    block->clean[2] = vector;

    result = IOS_IoctlvAsync(fd, 0, 2, 0, vector, _intBlkCtrlCb, block);
    if (result < 0) {
        goto clean;
    }
    IOS_Close(fd);
    goto done;

clean:
    IOSFree(svid);
    IOSFree(spid);
    IOSFree(vector);
    IOSFree(block);

done:
    return (IPCResult)result;
}

IPCResult IUSB_DeviceClassInsertionNotifyAsync(const char* path, u8 devClass, USBCallback callback,
                                               void* callbackArg) {
    s32 result;
    s32 fd;
    IPCIOVector* vector;
    u8* sdevClass;
    USBCommandBlock* block;

    if (path == NULL || devClass == 0) {
        OSReport("Invalid path or devClass in insertion notification call\n");
        result = USB_ERR_INVALID;
        goto done;
    }
    if ((fd = IOS_Open(path, IPC_OPEN_NONE)) < 0) {
        result = fd;
        OSReport("Open(%s) failed\n", path);
        goto done;
    }
    vector = (IPCIOVector*)IOSAlloc(ROUNDUP_IPC(sizeof(IPCIOVector)) * 2);
    sdevClass = (u8*)IOSAlloc(ROUNDUP_IPC(sizeof(u8)));
    block = (USBCommandBlock*)IOSAlloc(ROUNDUP_IPC(sizeof(USBCommandBlock)));

    *sdevClass = devClass;
    vector[0].base = sdevClass;
    vector[0].length = sizeof(u8);

    DCFlushRange(sdevClass, ROUNDUP_IPC(sizeof(u8)));
    DCFlushRange(vector, ROUNDUP_IPC(sizeof(IPCIOVector)) * 2);

    block->callback = callback;
    block->callbackArg = callbackArg;
    block->nclean = 2;
    block->clean[0] = sdevClass;
    block->clean[1] = vector;

    result = IOS_IoctlvAsync(fd, 0, 1, 0, vector, _intBlkCtrlCb, block);
    IOS_Close(fd);

done:
    return (IPCResult)result;
}

}
