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
