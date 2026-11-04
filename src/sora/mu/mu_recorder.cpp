#include <if/if_snap_save.h>
#include <mu/mu_recorder.h>
#include <string.h>

extern const char lbl_8059E148[1];

void muRecorder::saveAdv(u32 slot) {
    unk10C = slot;
    unk100 = 2;
}

void muRecorder::loadAdv(u32 slot) {
    unk10C = slot;
    unk100 = 3;
}

muCollection::FileHandle::FileHandle(const char* name) {
    unk0[0] = 0;
    strcat(unk0, lbl_8059E148);
    unk0[0] = 0;
    strcat(unk0, name);
}

muCollection::FileInfo::FileInfo() : unk0(0), unk1(0), unk8(0), unkC(0), unk10(0) {
}
