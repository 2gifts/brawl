#pragma once

// Supersedes include/Brawl/Include/sc/sc_melee.h (shadows it via -I include): names the loaders after the stage loader,
// the uploader operator and the stage event work (from the melee scenes of sora_scene). Keep the rest in sync.

#include <StaticAssert.h>
#include <gm/gm_lib.h>
#include <gf/gf_scene.h>
#include <st/operator/st_operator_rule_targetbreak.h>
#include <st/operator/st_operator_rule_melee.h>
#include <st/operator/st_operator_drop_item.h>
#include <st/operator/st_operator_info.h>

#include <types.h>

class scMelee : public gfScene {
public:
    void* m_loaderPlayers[MAX_PLAYERS];
    void* m_loaderStage;
    void* m_loaderInfo;                 // 0x28 (stLoaderManager::m_loaderInfo)
    void* m_loaderStageSe;              // 0x2C
    void* m_loaderPokemonSe;            // 0x30
    void* m_loaderStageStarFox;         // 0x34 (stLoaderManager::m_loaderStageAlt)
    void* m_loaderSandbag;              // 0x38
    void* m_loaderStageAdvCommon;       // 0x3C
    void* m_operatorBgm;
    void* m_operatorAppearanceFighter;
    void* m_operatorReadyGo;
    void* m_operatorReadyGoSuddenDeath;
    stOperatorDropItemMelee* m_operatorDropItem;
    stOperatorRule* m_operatorRuleGameMode;
    stOperatorRuleMelee* m_operatorRuleMelee;
    void* m_operatorNoContest;
    void* m_operatorNetwork;
    void* m_operatorController;
    stOperatorInfo* m_operatorInfo;
    void* m_operatorFighterRebirths[MAX_PLAYERS];
    void* m_operatorFighterChange;
    void* m_operatorDefeatBoss;
    void* m_operatorEndProduction;
    void* m_operatorAppearanceHelper;
    void* m_operatorUploader;           // 0x98
    void* m_operatorDebugIntrusions[2];
    void* m_operatorDebugFighterReset;
    void* m_operatorDebugStagePause;
    void* m_eventRule;
    // HYPOTHESIS: the stage event work that the melee setups make (sora_melee 0x27CBE0 / 0x27F0A8); it gets the rebirth,
    // info and rule operators.
    void* m_stageEvent;                 // 0xB0
    char _180[29];
    u8 m_unkD1;                         // 0xD1 HYPOTHESIS: cleared by the staff roll scene
    char _210[2];
    int m_endNum;
    // HYPOTHESIS: flags of the scene; scButton tests b6 and b4 in process and its button task clears b5 when it ends.
    bool unkD8_b7 : 1;
    bool unkD8_b6 : 1;
    bool unkD8_b5 : 1;
    bool unkD8_b4 : 1;
    bool unkD8_b3 : 1;
    bool unkD8_b2 : 1;
    bool unkD8_b1 : 1;
    bool unkD8_b0 : 1;
    char _217[11];
    GameRule m_gameRule;
    char _232[21];
    // HYPOTHESIS: the name entry whose button settings are used (GameGlobal::getButtonConfig); scButton's constructor
    // clears it and muButtonTask reads it.
    u8 m_buttonConfigNo; // 0xFD
    char _254[2];

    scMelee(const char* name);
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual ~scMelee();
    virtual int getEndNum();
};
static_assert(sizeof(scMelee) == 256, "Class is wrong size!");
