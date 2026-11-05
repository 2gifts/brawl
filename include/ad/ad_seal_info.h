#pragma once

#include <types.h>

class tySealListDataManager;

class adSealInfoResult {
public:
    tySealListDataManager* unk0;
    // HYPOTHESIS: forty sixteen-byte records per fighter, with fifty allocated banks.
    struct {
        s32 unk0;
        u32 unk4;
        float unk8;
        float unkC;
    } unk4[50][40];

    ~adSealInfoResult();
    void getSealInfoPrepare();
    bool isLoadFinish();
    u32 getFighterSealInfoNum(s32 index);
    void* getFighterSealInfo(s32 index);
};
