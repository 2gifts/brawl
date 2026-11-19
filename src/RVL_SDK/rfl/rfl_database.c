#include <types.h>

extern void* RFLiGetDBManager(void);
extern void* RFLiGetHDBManager(void);
extern void* MEMAllocFromExpHeapEx(void* heap, u32 size, int align);
extern void RFLiClearDBBuffer(void);
extern void convertRaw2InfoCore__803D4E0C(void* raw, void* info);
extern int fn_803D4CB8(u32 id);
extern void* memset(void*, int, unsigned long);
extern void* memcpy(void*, const void*, unsigned long);

void RFLiInitHiddenDatabase(void) {
    u32* manager = RFLiGetHDBManager();
    if (manager != NULL) {
        manager[0x34 / 4] = 0;
        manager[0x38 / 4] = 0;
    }
}

void RFLiInitDatabase(void* heap) {
    u32* manager = RFLiGetDBManager();
    manager[0] = (u32)MEMAllocFromExpHeapEx(heap, 0x1F1E0, 0x20);
    manager[1] = 0;
    manager[2] = 0;
    manager[3] = 0;
    RFLiClearDBBuffer();
}

void RFLiConvertRaw2Info(u8* raw, u8* info) {
    convertRaw2InfoCore__803D4E0C(raw, info);
    memcpy(info + 0x2E, raw + 0x36, 0x14);
    *(u16*)(info + 0x42) = 0;
}

void RFLiConvertHRaw2Info(u8* raw, u8* info) {
    convertRaw2InfoCore__803D4E0C(raw, info);
    memset(info + 0x2E, 0, 0x14);
    *(u16*)(info + 0x42) = 0;
    *(u16*)(info + 0x44) &= ~0x7FC0;
}

void RFLiConvertRaw2HRaw(u8* raw, u8* hraw) {
    memset(hraw, 0, 0x40);
    memcpy(hraw, raw, 0x36);
    *(u16*)hraw &= ~0x3FE0;
}

BOOL RFLIsAvailableOfficialData(u32 index) {
    if (index >= 100) {
        return FALSE;
    }
    return fn_803D4CB8(index) != 0;
}

// TODO: clean up, give variables real names
/* jj_803D521C */
int RFLiIsValidID(void* arg0) {
    int var_r4;
    if (arg0 == NULL)
        return 0;
    var_r4 = 0;
    if (*(int*)arg0 != 0 || *(int*)((char*)arg0 + 4) != 0)
        var_r4 = 1;
    return var_r4;
}

/* jj_803D51A4 */

// TODO: clean up, give variables real names
u16 fn_803D51A4(void) {
    u16 count = 0;
    unsigned int i = 0;
    do {
        int result;
        if ((u16)i >= 100U) {
            result = 0;
        } else {
            result = fn_803D4CB8((u16)i) != 0;
        }
        if (result != 0) {
            count++;
        }
        i++;
    } while (i < 100U);
    return count;
}

/* jj_803D4BD8 */
typedef struct { char pad[0x1b40]; int state; } State;
typedef struct { char pad[12]; void (*callback)(void); } Context;
extern int RFLAvailable(void);
extern int RFLiIsWorking(void);
extern void bootloadDBopencallback_(void);
extern int fn_803CEEFC(int, int, void *);
extern int RFLiGetLastReason(void);
extern void RFLiClearDBBuffer(void);
extern State *RFLiGetManager(void);
extern int RFLGetAsyncStatus(void);
// TODO: clean up, give variables real names
int RFLiBootLoadDatabaseAsync(void (*callback)(void)) {
    Context *p;
    if (RFLAvailable() == 0) return 5;
    if (RFLiIsWorking() != 0) return 5;
    p = RFLiGetDBManager();
    if (p == 0) return 5;
    p->callback = callback;
    switch (fn_803CEEFC(0, 1, (void *)bootloadDBopencallback_)) {
    case 0:
    case 6:
        break;
    case 2:
        if (RFLiGetLastReason() == -12) {
            RFLiClearDBBuffer();
            RFLiGetManager()->state = 0;
            if (p->callback != 0) p->callback();
        }
        break;
    }
    return RFLGetAsyncStatus();
}

/* jj_803D5254 */
#define CRC_STEP()                              \
    if (crc & 0x8000) {                         \
        crc <<= 1;                              \
        crc ^= 0x1021;                          \
    } else {                                    \
        crc <<= 1;                              \
    }                                           \
    if (byte & 0x80) {                          \
        crc ^= 1;                               \
    }                                           \
    byte <<= 1;

u16 RFLiCalculateCRC(u8 *data, u32 len) {
    u32 i;
    u16 crc;
    u8 byte;
    u16 j;

    for (i = 0, crc = 0; i < len; i++) {
        byte = *data;
        for (j = 0; j < 6; j++) {
            CRC_STEP();
            CRC_STEP();
            CRC_STEP();
            CRC_STEP();
            j += 2;
        }
        data++;
    }
    return crc;
}

/* jj_803D4950 */
extern void OSCancelAlarm(void *alarm);
extern void OSSetAlarmUserData(void *alarm, void *arg);
extern void OSSetAlarm(void *alarm, s64 tick, void *handler);
extern void RFLiSetFileBroken(s32 arg0);
extern void RFLiStartWorking(void);
extern void fn_803CFBE4(s32 a, s32 b);
extern void bootloadCheckCRCCb_(s32 arg0);
extern void alarmCheckCb_(void);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void bootloadDBcallback_(void) {
    s32 *p;
    s32 *q;
    s32 v;

    p = RFLiGetDBManager();
    switch (RFLGetAsyncStatus()) {
    case 0:
        q = RFLiGetDBManager();
        if (RFLiIsWorking() == 0) {
            RFLiStartWorking();
        }
        v = q[0];
        q[24] = v;
        q[25] = 0x1F1E0;
        q[26] = v;
        q[27] = 0;
        *(s16 *)&q[28] = 0;
        q[42] = (s32)(void *)bootloadCheckCRCCb_;
        OSCancelAlarm(&q[30]);
        OSSetAlarmUserData(&q[30], &q[24]);
        OSSetAlarm(&q[30], (s64)(((*(u32 *)0x800000F8 >> 2) / 1000) * 0x13), (void *)alarmCheckCb_);
        return;
    case 2:
        switch (RFLiGetLastReason()) {
        case -4:
            RFLiSetFileBroken(3);
            break;
        default:
        case -5:
        case -15:
            *(s32 *)((char *)RFLiGetManager() + 0x1B40) = 3;
            RFLiSetFileBroken(1);
            break;
        }
        fn_803CFBE4(0, p[3]);
        return;
    default:
    case 1:
        RFLiSetFileBroken(1);
        fn_803CFBE4(0, p[3]);
        return;
    }
}

