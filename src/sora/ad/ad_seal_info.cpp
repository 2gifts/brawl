#include <types.h>
#include <ad/ad_seal_info.h>
#include <ty/ty_seal_listmng.h>
#include <memory.h>
#include <new>

const u8 lbl_80409798[48] = {
    0, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
    14, 14, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25,
    25, 25, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 0, 0, 0, 0
};

adSealInfoResult::~adSealInfoResult() {
    if (unk0) {
        delete unk0;
    }
}

void adSealInfoResult::getSealInfoPrepare() {
    void* memory = operator new(sizeof(tySealListDataManager), Heaps::MenuInstance);
    tySealListDataManager* manager = NULL;
    if (memory) {
        manager = new (memory) tySealListDataManager(true);
    }
    unk0 = manager;
}

bool adSealInfoResult::isLoadFinish() {
    return unk0->isLoadFinish();
}

u32 adSealInfoResult::getFighterSealInfoNum(s32 index) {
    u32 count = 0;
    u32 fighter;
    if (index < 0 || index > 43) {
        fighter = 35;
    } else {
        fighter = lbl_80409798[index];
    }
    for (u32 seal = 0; seal < 40; ++seal) {
        if (unk4[fighter][seal].unk0 == -1) {
            break;
        }
        ++count;
    }
    return count;
}

void* adSealInfoResult::getFighterSealInfo(s32 index) {
    u32 fighter;
    if (index < 0 || index > 43) {
        fighter = 35;
    } else {
        fighter = lbl_80409798[index];
    }
    return unk4[fighter];
}
