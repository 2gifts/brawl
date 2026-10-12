// scapi: typed getters/setters over the system config items (aspect ratio, language, sound mode,
// bluetooth bits, ...). every getter reads one item through the generic finders and clamps whatever came out.
#include <types.h>

#define SCAPI_BOOL int

typedef struct {
    u8 mode;
    u8 led;
} SCIdleMode;

enum {
    ITEM_COUNTER_BIAS = 0,
    ITEM_ASPECT_RATIO = 1,
    ITEM_DISPLAY_OFFSET_H = 5,
    ITEM_EULA = 7,
    ITEM_IDLE_MODE = 9,
    ITEM_LANGUAGE = 11,
    ITEM_PARENTAL_CONTROL = 13,
    ITEM_PROGRESSIVE_MODE = 14,
    ITEM_SCREEN_SAVER_MODE = 15,
    ITEM_SIMPLE_ADDRESS = 16,
    ITEM_SOUND_MODE = 17,
    ITEM_NET_CONTENT_RESTRICTIONS = 20,
    ITEM_NET_WC_FLAGS = 23,
    ITEM_BT_DEVICE_INFO = 28,
    ITEM_BT_CMPDEV_INFO = 29,
    ITEM_BT_DPD_SENSIBILITY = 30,
    ITEM_BT_SPEAKER_VOLUME = 31,
    ITEM_BT_MOTOR_MODE = 32,
    ITEM_BT_SENSOR_BAR_POSITION = 33,
};

extern SCAPI_BOOL SCFindByteArrayItem(void *dst, u32 len, int id);
extern SCAPI_BOOL SCReplaceByteArrayItem(const void *src, u32 len, int id);
extern SCAPI_BOOL SCFindU8Item(u8 *dst, int id);
extern SCAPI_BOOL SCFindS8Item(s8 *dst, int id);
extern SCAPI_BOOL SCFindU32Item(u32 *dst, int id);
extern SCAPI_BOOL SCReplaceU8Item(u8 data, int id);
extern SCAPI_BOOL SCFindBoolItem(u32 *dst, int id);
extern s8 SCGetProductArea(void);
extern int OSDisableInterrupts(void);
extern void OSRestoreInterrupts(int);
extern void *memset(void *, int, u32);

static u8 simpleAddrBuf[0x1008] __attribute__((aligned(32)));

u8 SCGetAspectRatio(void) {
    u8 item;

    if (!SCFindU8Item(&item, ITEM_ASPECT_RATIO)) {
        item = 0;
    } else if (item != 1) {
        item = 0;
    }

    return item;
}

s8 SCGetDisplayOffsetH(void) {
    s8 item;

    if (!SCFindS8Item(&item, ITEM_DISPLAY_OFFSET_H)) {
        item = 0;
    } else if (item < -32) {
        item = -32;
    } else if (item > 32) {
        item = 32;
    }

    return item & ~1;
}

void SCGetIdleMode(SCIdleMode *mode) {
    SCFindByteArrayItem(mode, sizeof(SCIdleMode), ITEM_IDLE_MODE);
}

u8 SCGetLanguage(void) {
    u8 item;

    if (!SCFindU8Item(&item, ITEM_LANGUAGE)) {
        if (SCGetProductArea() == 0) {
            item = 0;
        } else {
            item = 1;
        }
    } else if (item > 9) {
        item = 1;
    }

    return item;
}

// HYPOTHESIS: no name for this one yet, it hands back the 0x4a byte parental control block
SCAPI_BOOL fn_8020FC90(void *dst) {
    return SCFindByteArrayItem(dst, 0x4A, ITEM_PARENTAL_CONTROL);
}

u8 SCGetProgressiveMode(void) {
    u8 item;

    if (!SCFindU8Item(&item, ITEM_PROGRESSIVE_MODE)) {
        item = 0;
    } else if (item != 1) {
        item = 0;
    }

    return item;
}

u8 SCGetScreenSaverMode(void) {
    u8 item;

    if (!SCFindU8Item(&item, ITEM_SCREEN_SAVER_MODE)) {
        item = 1;
    } else if (item != 1) {
        item = 0;
    }

    return item;
}

u8 SCGetSoundMode(void) {
    u8 item;

    if (!SCFindU8Item(&item, ITEM_SOUND_MODE)) {
        item = 1;
    } else if (item != 0 && item != 1 && item != 2) {
        item = 1;
    }

    return item;
}

u32 SCGetCounterBias(void) {
    u32 item;

    if (!SCFindU32Item(&item, ITEM_COUNTER_BIAS)) {
        // six years worth of seconds
        item = 2192 * 24 * 60 * 60;
    }

    return item;
}

SCAPI_BOOL SCGetBtDeviceInfoArray(void *info) {
    return SCFindByteArrayItem(info, 0x461, ITEM_BT_DEVICE_INFO);
}

SCAPI_BOOL SCSetBtDeviceInfoArray(const void *info) {
    return SCReplaceByteArrayItem(info, 0x461, ITEM_BT_DEVICE_INFO);
}

SCAPI_BOOL SCGetBtCmpDevInfoArray(void *array) {
    return SCFindByteArrayItem(array, 0x205, ITEM_BT_CMPDEV_INFO);
}

SCAPI_BOOL SCSetBtCmpDevInfoArray(const void *array) {
    return SCReplaceByteArrayItem(array, 0x205, ITEM_BT_CMPDEV_INFO);
}

u32 SCGetBtDpdSensibility(void) {
    u32 item;

    if (!SCFindU32Item(&item, ITEM_BT_DPD_SENSIBILITY)) {
        item = 2;
    } else if (item < 1) {
        item = 1;
    } else if (item > 5) {
        item = 5;
    }

    return item;
}

u8 SCGetWpadMotorMode(void) {
    u8 item;

    if (!SCFindU8Item(&item, ITEM_BT_MOTOR_MODE)) {
        item = 1;
    } else if (item != 1) {
        item = 0;
    }

    return item;
}

SCAPI_BOOL SCSetWpadMotorMode(u8 mode) {
    return SCReplaceU8Item(mode, ITEM_BT_MOTOR_MODE);
}

u8 SCGetWpadSensorBarPosition(void) {
    u8 item;

    if (!SCFindU8Item(&item, ITEM_BT_SENSOR_BAR_POSITION)) {
        item = 0;
    } else if (item != 1) {
        item = 0;
    }

    return item;
}

u8 SCGetWpadSpeakerVolume(void) {
    u8 item;

    if (!SCFindU8Item(&item, ITEM_BT_SPEAKER_VOLUME)) {
        item = 89;
    } else if (item > 127) {
        item = 127;
    }

    return item;
}

SCAPI_BOOL SCSetWpadSpeakerVolume(u8 volume) {
    return SCReplaceU8Item(volume, ITEM_BT_SPEAKER_VOLUME);
}

// HYPOTHESIS: unnamed, reads the simple address block into a shared buffer, validates the id in its first
// word and hands that id back (or -1)
u32 fn_8020FF8C(void) {
    int valid;
    u32 word;
    int enabled;
    u32 current;

    if (SCFindByteArrayItem(simpleAddrBuf, 0x1008, ITEM_SIMPLE_ADDRESS) && (word = *(u32 *)simpleAddrBuf) != 0xFFFFFFFF &&
        (word & 0xFF000000) != 0 && (word & 0xFF000000) != 0xFF000000 && (word & 0x00FF0000) != 0x00FF0000) {
        enabled = OSDisableInterrupts();
        current = *(u32 *)simpleAddrBuf;
        if ((current & 0x00FF0000) == 0) {
            memset(simpleAddrBuf, 0, 0x1008);
            *(u32 *)simpleAddrBuf = current;
        }
        OSRestoreInterrupts(enabled);
        valid = 1;
    } else {
        valid = 0;
    }

    if (valid) {
        return *(u32 *)simpleAddrBuf;
    }
    return -1;
}

// HYPOTHESIS: names missing for these three, they are plain u32 items with a default of zero
u32 fn_80210064(void) {
    u32 item;

    if (!SCFindU32Item(&item, ITEM_NET_CONTENT_RESTRICTIONS)) {
        item = 0;
    }

    return item;
}

u32 fn_802100A0(void) {
    u32 item;

    if (!SCFindBoolItem(&item, ITEM_EULA)) {
        item = 0;
    }

    return item;
}

u32 fn_802100DC(void) {
    u32 item;

    if (!SCFindU32Item(&item, ITEM_NET_WC_FLAGS)) {
        item = 0;
    }

    return item;
}
