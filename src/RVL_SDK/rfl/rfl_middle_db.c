#include <types.h>

u32 RFLGetMiddleDBBufferSize(u16 num) {
    return num << 6;
}

extern u8 lbl_8041AEC0[];
extern void RFLiConvertRaw2Info(void* raw, void* info);

void* fn_803D5B54(u32 index) {
    if (index >= 6) {
        return 0;
    }
    return lbl_8041AEC0 + index * 0x4A;
}

void RFLiGetDefaultData(void* info, int index) {
    RFLiConvertRaw2Info(lbl_8041AEC0 + index * 0x4A, info);
}
