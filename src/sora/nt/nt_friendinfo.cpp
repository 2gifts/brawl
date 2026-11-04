#include <gf/gf_sys_recorder2.h>
#include <nt/nt_offline.h>
#include <revolution/NET/crc.h>
#include <string.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049EC90[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049ECA0[0x18] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A044C[0x4] __attribute__((aligned(4))) = { 0 };


extern "C" int DWC_CheckUserData(const DWCAccUserData* userdata);

ntFriendInfoSave::ntFriendInfoSave()
    : gfTask("ntFriendInfoSave", Category_None, 14, 6, true), unk40(0), unk44(false) {
    unk2C_b1 = false;
}

ntFriendInfoSave::~ntFriendInfoSave() {
    if (unk40) {
        gfSysRecorder2::dropInstance();
        unk40 = 0;
    }
}

void ntFriendInfoSave::processDefault() {
    if (unk40 && !unk40->m_busy) {
        gfSysRecorder2::dropInstance();
        unk40 = 0;
        if (unk44) exit();
        --s_savingCount;
    }
}

void ntFriendInfo::setSaveData(const void* data, u32 size) {
    unk8 = true;
    if (size >= 0x2804) {
        memcpy(unk0, data, 0x2804);
        if (unk0->unk0_0 == 0x2804) {
            u16 crc = unk0->unk2;
            unk0->unk2 = 0;
            if (crc == NETCalcCRC16((const char*)unk0, 0x2804)) unk8 = false;
        }
    }
    if (unk8) memset(unk0, 0, 0x2804);
    unk9 = true;
}

bool ntFriendInfo::isValid(const DWCAccUserData* data) {
    return DWC_CheckUserData(data) == 1;
}

