// esp: thin client for the es (title/ticket management) iop device. every call fills a 32 byte aligned
// scratch block on the stack with the arguments, points a few io vectors at it and hands that to ioctlv.
#include <revolution/ESP.h>

// built as c++ against the openrvl headers, keep c linkage
extern "C" {

#define ESP_ERR_INVALID (-0x3F9)

#define ES_BUF_SIZE 0x100
#define ES_VECTOR_COUNT 6
#define ES_ALIGN 32
#define ES_ALIGN_MASK (ES_ALIGN - 1)
#define ESP_BUF(name) u8 name[ES_BUF_SIZE] __attribute__((aligned(32)))
#define ESP_VECTORS(buf) ((IPCIOVector *)((buf) + ES_BUF_SIZE - ES_VECTOR_COUNT * sizeof(IPCIOVector)))

#define ES_TICKET_VIEW_SIZE 0xD8
#define ES_LP_ENTRY_SIZE 8
#define ES_DATA_DIR_SIZE 0x1E

typedef u64 ESTitleId;
typedef u64 ESTicketId;

enum {
    ESP_CMD_LAUNCH_TITLE = 0x08,
    ESP_CMD_LIST_TITLE_CONTENTS_COUNT = 0x10,
    ESP_CMD_LIST_TITLE_CONTENTS = 0x11,
    ESP_CMD_GET_TICKET_VIEW_COUNT = 0x12,
    ESP_CMD_GET_TICKET_VIEWS = 0x13,
    ESP_CMD_GET_TMD_VIEW_SIZE = 0x14,
    ESP_CMD_GET_TMD_VIEW = 0x15,
    ESP_CMD_GET_CONSUMPTION = 0x16,
    ESP_CMD_SET_UID = 0x17,
    ESP_CMD_GET_DATA_DIR = 0x1D,
    ESP_CMD_DI_GET_TICKET_VIEW = 0x1B,
    ESP_CMD_GET_TITLE_ID = 0x20,
    ESP_CMD_ENCRYPT = 0x2C,
    ESP_CMD_DECRYPT = 0x2D,
    ESP_CMD_DI_GET_TMD_SIZE = 0x39,
    ESP_CMD_DI_GET_TMD = 0x3A,
};

s32 IOS_IoctlvReboot(s32 fd, s32 cmd, s32 inCount, s32 outCount, IPCIOVector *vectors);

static s32 __esFd = -1;

s32 ESP_InitLib(void) {
    s32 rv = 0;

    if (__esFd < 0) {
        __esFd = IOS_Open("/dev/es", IPC_OPEN_NONE);
        if (__esFd < 0) {
            rv = __esFd;
        }
    }

    return rv;
}

s32 ESP_CloseLib(void) {
    s32 rv = 0;

    if (__esFd >= 0) {
        rv = IOS_Close(__esFd);
        if (rv == 0) {
            __esFd = -1;
        }
    }

    return rv;
}

s32 ESP_LaunchTitle(ESTitleId titleId, void *ticketView) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    ESTitleId *p1 = (ESTitleId *)buf;

    if (__esFd < 0) {
        rv = ESP_ERR_INVALID;
        goto out;
    }
    if ((u32)ticketView & ES_ALIGN_MASK) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = titleId;

    v[0].base = p1;
    v[0].length = sizeof(ESTitleId);
    v[1].base = ticketView;
    v[1].length = ES_TICKET_VIEW_SIZE;

    rv = IOS_IoctlvReboot(__esFd, ESP_CMD_LAUNCH_TITLE, 2, 0, v);
    // the iop is rebooting into the new title, the handle is gone
    __esFd = -1;

out:
    return rv;
}

// HYPOTHESIS: name taken from the es command table (list title contents count / list), content ids are
// plain u32s
s32 ESP_ListTitleContents(ESTitleId titleId, u32 *cids, u32 *count) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    ESTitleId *p1 = (ESTitleId *)buf;
    u32 *p2 = (u32 *)(buf + ES_ALIGN);

    if (__esFd < 0 || count == (void *)NULL) {
        rv = ESP_ERR_INVALID;
        goto out;
    }
    if ((u32)cids & ES_ALIGN_MASK) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = titleId;
    if (cids == (void *)NULL) {
        v[0].base = p1;
        v[0].length = sizeof(ESTitleId);
        v[1].base = p2;
        v[1].length = sizeof(u32);
        rv = IOS_Ioctlv(__esFd, ESP_CMD_LIST_TITLE_CONTENTS_COUNT, 1, 1, v);
        if (rv == 0) {
            *count = *p2;
        }
    } else {
        if (*count == 0) {
            rv = ESP_ERR_INVALID;
            goto out;
        }
        *p2 = *count;
        v[0].base = p1;
        v[0].length = sizeof(ESTitleId);
        v[1].base = p2;
        v[1].length = sizeof(u32);
        v[2].base = cids;
        v[2].length = sizeof(u32) * (*count);
        rv = IOS_Ioctlv(__esFd, ESP_CMD_LIST_TITLE_CONTENTS, 2, 1, v);
    }

out:
    return rv;
}

s32 ESP_GetTicketViews(ESTitleId titleId, void *views, u32 *count) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    ESTitleId *p1 = (ESTitleId *)buf;
    u32 *p2 = (u32 *)(buf + ES_ALIGN);

    if (__esFd < 0 || count == (void *)NULL) {
        rv = ESP_ERR_INVALID;
        goto out;
    }
    if ((u32)views & ES_ALIGN_MASK) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = titleId;
    if (views == (void *)NULL) {
        v[0].base = p1;
        v[0].length = sizeof(ESTitleId);
        v[1].base = p2;
        v[1].length = sizeof(u32);
        rv = IOS_Ioctlv(__esFd, ESP_CMD_GET_TICKET_VIEW_COUNT, 1, 1, v);
        if (rv == 0) {
            *count = *p2;
        }
    } else {
        if (*count == 0) {
            rv = ESP_ERR_INVALID;
            goto out;
        }
        *p2 = *count;
        v[0].base = p1;
        v[0].length = sizeof(ESTitleId);
        v[1].base = p2;
        v[1].length = sizeof(u32);
        v[2].base = views;
        v[2].length = ES_TICKET_VIEW_SIZE * (*count);
        rv = IOS_Ioctlv(__esFd, ESP_CMD_GET_TICKET_VIEWS, 2, 1, v);
    }

out:
    return rv;
}

s32 ESP_DiGetTicketView(const void *ticket, void *ticketView) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);

    if (__esFd < 0 || ticketView == (void *)NULL) {
        rv = ESP_ERR_INVALID;
        goto out;
    }
    if (((u32)ticket & ES_ALIGN_MASK) || ((u32)ticketView & ES_ALIGN_MASK)) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    v[0].base = (void *)ticket;
    if (ticket == (void *)NULL) {
        v[0].length = 0;
    } else {
        v[0].length = 0x2A4;
    }
    v[1].base = ticketView;
    v[1].length = ES_TICKET_VIEW_SIZE;
    rv = IOS_Ioctlv(__esFd, ESP_CMD_DI_GET_TICKET_VIEW, 1, 1, v);

out:
    return rv;
}

s32 ESP_DiGetTmd(void *tmd, u32 *tmdSize) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    u32 *p1 = (u32 *)buf;

    if (__esFd < 0 || tmdSize == (void *)NULL) {
        rv = ESP_ERR_INVALID;
        goto out;
    }
    if ((u32)tmd & ES_ALIGN_MASK) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    if (tmd == (void *)NULL) {
        v[0].base = p1;
        v[0].length = sizeof(u32);
        rv = IOS_Ioctlv(__esFd, ESP_CMD_DI_GET_TMD_SIZE, 0, 1, v);
        if (rv == 0) {
            *tmdSize = *p1;
        }
    } else {
        if (*tmdSize == 0) {
            rv = ESP_ERR_INVALID;
            goto out;
        }
        *p1 = *tmdSize;
        v[0].base = p1;
        v[0].length = sizeof(u32);
        v[1].base = tmd;
        v[1].length = *tmdSize;
        rv = IOS_Ioctlv(__esFd, ESP_CMD_DI_GET_TMD, 1, 1, v);
    }

out:
    return rv;
}

s32 ESP_GetTmdView(ESTitleId titleId, void *tmdView, u32 *size) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    ESTitleId *p1 = (ESTitleId *)buf;
    u32 *p2 = (u32 *)(buf + ES_ALIGN);

    if (__esFd < 0 || size == (void *)NULL) {
        rv = ESP_ERR_INVALID;
        goto out;
    }
    if ((u32)tmdView & ES_ALIGN_MASK) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = titleId;
    if (tmdView == (void *)NULL) {
        v[0].base = p1;
        v[0].length = sizeof(ESTitleId);
        v[1].base = p2;
        v[1].length = sizeof(u32);
        rv = IOS_Ioctlv(__esFd, ESP_CMD_GET_TMD_VIEW_SIZE, 1, 1, v);
        if (rv == 0) {
            *size = *p2;
        }
    } else {
        if (*size == 0) {
            rv = ESP_ERR_INVALID;
            goto out;
        }
        *p2 = *size;
        v[0].base = p1;
        v[0].length = sizeof(ESTitleId);
        v[1].base = p2;
        v[1].length = sizeof(u32);
        v[2].base = tmdView;
        v[2].length = *size;
        rv = IOS_Ioctlv(__esFd, ESP_CMD_GET_TMD_VIEW, 2, 1, v);
    }

out:
    return rv;
}

s32 ESP_GetDataDir(ESTitleId titleId, char *dataDir) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    ESTitleId *p1 = (ESTitleId *)buf;

    if (__esFd < 0 || dataDir == (void *)NULL) {
        rv = ESP_ERR_INVALID;
        goto out;
    }
    if ((u32)dataDir & ES_ALIGN_MASK) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = titleId;
    v[0].base = p1;
    v[0].length = sizeof(ESTitleId);
    v[1].base = dataDir;
    v[1].length = ES_DATA_DIR_SIZE;
    rv = IOS_Ioctlv(__esFd, ESP_CMD_GET_DATA_DIR, 1, 1, v);

out:
    return rv;
}

s32 ESP_GetTitleId(ESTitleId *titleId) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);

    if (__esFd < 0 || titleId == (void *)NULL) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    v[0].base = buf;
    v[0].length = sizeof(ESTitleId);
    rv = IOS_Ioctlv(__esFd, ESP_CMD_GET_TITLE_ID, 0, 1, v);
    if (rv == 0) {
        *titleId = *(ESTitleId *)buf;
    }

out:
    return rv;
}

s32 ESP_GetConsumption(ESTicketId ticketId, void *entries, u32 *nEntries) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    ESTicketId *p1 = (ESTicketId *)buf;
    u32 *p2 = (u32 *)(buf + ES_ALIGN);

    if (__esFd < 0) {
        rv = ESP_ERR_INVALID;
        goto out;
    }
    if ((u32)entries & ES_ALIGN_MASK) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = ticketId;
    v[0].base = p1;
    v[0].length = sizeof(ESTicketId);
    if (entries == (void *)NULL) {
        v[1].base = NULL;
        v[1].length = 0;
    } else {
        v[1].base = entries;
        v[1].length = ES_LP_ENTRY_SIZE * (*nEntries);
        *p2 = *nEntries;
    }
    v[2].base = p2;
    v[2].length = sizeof(u32);
    rv = IOS_Ioctlv(__esFd, ESP_CMD_GET_CONSUMPTION, 1, 2, v);

    *nEntries = *p2;

out:
    return rv;
}

// HYPOTHESIS: these three keep the es naming (ES_Encrypt/ES_Decrypt/ES_SetUid), signatures follow the
// command shapes (key handle + iv + input -> iv + output)
s32 ESP_Encrypt(u32 keyHandle, u8 *iv, u8 *input, u32 inputSize, u8 *output) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    u32 *p1 = (u32 *)buf;
    u32 *p2 = (u32 *)(buf + ES_ALIGN);

    if (__esFd < 0) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = keyHandle;
    *p2 = inputSize;
    v[0].base = p1;
    v[0].length = sizeof(u32);
    v[1].base = iv;
    v[1].length = 16;
    v[2].base = input;
    v[2].length = inputSize;
    v[3].base = iv;
    v[3].length = 16;
    v[4].base = output;
    v[4].length = inputSize;
    rv = IOS_Ioctlv(__esFd, ESP_CMD_ENCRYPT, 3, 2, v);

out:
    return rv;
}

s32 ESP_Decrypt(u32 keyHandle, u8 *iv, u8 *input, u32 inputSize, u8 *output) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    u32 *p1 = (u32 *)buf;
    u32 *p2 = (u32 *)(buf + ES_ALIGN);

    if (__esFd < 0) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = keyHandle;
    *p2 = inputSize;
    v[0].base = p1;
    v[0].length = sizeof(u32);
    v[1].base = iv;
    v[1].length = 16;
    v[2].base = input;
    v[2].length = inputSize;
    v[3].base = iv;
    v[3].length = 16;
    v[4].base = output;
    v[4].length = inputSize;
    rv = IOS_Ioctlv(__esFd, ESP_CMD_DECRYPT, 3, 2, v);

out:
    return rv;
}

s32 ESP_SetUid(ESTitleId titleId) {
    s32 rv = 0;
    ESP_BUF(buf);
    IPCIOVector *v = ESP_VECTORS(buf);
    ESTitleId *p1 = (ESTitleId *)buf;

    if (__esFd < 0) {
        rv = ESP_ERR_INVALID;
        goto out;
    }

    *p1 = titleId;
    v[0].base = p1;
    v[0].length = sizeof(ESTitleId);
    rv = IOS_Ioctlv(__esFd, ESP_CMD_SET_UID, 1, 0, v);

out:
    return rv;
}

}
