#pragma once

#include <StaticAssert.h>
#include <types.h>

class adSealInfoResult;

// HYPOTHESIS: per-step flag record; the step id comes first, the rest is unknown.
struct adKeepStepFlag {
    u32 m_stepId;
    u8 unk4[0x104];
};
static_assert(sizeof(adKeepStepFlag) == 0x108, "Class is the wrong size!");

// HYPOTHESIS: rare item record (got and lost lists); fields unknown.
struct adKeepRareItem {
    s32 unk0;
    s32 unk4;
    u8 unk8;
    u8 unk9;
};
static_assert(sizeof(adKeepRareItem) == 0xC, "Class is the wrong size!");

// HYPOTHESIS: view of the gmAdvData fields this unit reads and writes.
struct adKeepAdvData {
    char unk0[0x5FD];
    u8 unk5FD;
    char unk5FE[0xE];
    u32 unk60C;
    char unk610[0x40];
    u32 m_globalFlags[0x20];
    s8 unk6D0[0x4240];
    u32 unk4910;
    bool unk4914[2];
    u8 unk4916;
    u8 unk4917;
    u32 unk4918;
    u32 unk491C;
    u16 unk4920;
};

class adKeepManager {
public:
    u32* m_stageFlags;                 // 0x00
    u32* m_globalFlags;                // 0x04
    adSealInfoResult* m_sealInfoResult; // 0x08
    adKeepStepFlag* m_stepFlags;       // 0x0C
    u32 m_stepFlagNum;                 // 0x10
    adKeepRareItem* m_gotRareItems;    // 0x14
    adKeepRareItem* m_lostRareItems;   // 0x18
    u32 m_gotRareItemNum;              // 0x1C
    u32 m_lostRareItemNum;             // 0x20
    u32 unk24_b31 : 1;                 // 0x24

    adKeepManager();
    static adKeepManager* getInstance();
    ~adKeepManager();
    adKeepStepFlag* getStepFlag(u32 stepId);
    void resetGlobalFlag();
    // HYPOTHESIS: parameter and return types from here on come from register use and callers only.
    void setStageFlag(u32 id, bool on);
    bool getStageFlag(u32 id);
    void setGlobalFlag(u32 id, bool on);
    bool getGlobalFlag(u32 id);
    bool getKeepFlag(u32 id, u32 kind);
    void useInitilizeTiming();
    void createSealInfoResult();
    void releaseSealInfoResult();
    void useStageSelectTiming();
    void useWorldMapTiming();
    void useStepInTiming();
    void useStepOutTiming();
    void useSealInfoPrepareStart();
    void useGameOverInTiming();
    void useGameOverOutTimingContinue();
    void useGameOverOutTiming();
    void useStageClearInTiming();
    void useStageClearOutTiming();
    void useSavePointTiming();
    void useQuitAdventureTiming();
    void useReturnMapTiming();
    void useGameEndingTiming();
    bool isLoadFinishSeal();
    void getSealInfoFromGameGlobal();
    void getFighterSealInfo(u32 index, u32* num, u32* info);
    u32 getAdGotRareItemNum();
    adKeepRareItem* getAdGotRareItem(u32 index);
    u32 getAdLostRareItemNum();
    adKeepRareItem* getAdLostRareItem(u32 index);
    void setResultOfTotaling(bool addCoin, bool applyItems);
    void setNanaDeadFlag(u32 index, bool dead);
    bool isNanaDeadFlag(u32 index);
};
static_assert(sizeof(adKeepManager) == 0x28, "Class is the wrong size!");
