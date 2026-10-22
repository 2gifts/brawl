#define ST_STAGE_OUT_EFFECT_INIT_FLAG
#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <ef/ef_screen.h>
#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <it/it_manager.h>
#include <sc/sc_melee.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/st_adventure2.h>
#include <st/operator/st_operator_defeat_boss.h>
#include <st/operator/st_operator_rule.h>
#include <st/st_trigger.h>
#include <st/stage.h>
#include <types.h>

extern "C" {
// HYPOTHESIS: stTriggerMng::getEnemyKindIdForEmGenerator (map name), the enemy kind of the index-th generator trigger.
u16 fn_27_23345C(stTriggerMng* mng, u32 index);
// HYPOTHESIS: efScreen::cancelFill (map name); the same stand-in as src/mo_stage/st_dxzebes/st_dxzebes.cpp.
void fn_8005E3C8(efScreen* screen, efScreenHandle* handle, float frames);
}

// HYPOTHESIS: the two-word tag records built after the gimmick-id tables (see include/ft/kirby/ft_kirby_unk8.h).
struct stOperatorDefeatBossUnk8 {
    u32 unk0;
    u32 unk4;
    stOperatorDefeatBossUnk8(u32 a, u32 b) : unk0(a), unk4(b) { }
};
static stOperatorDefeatBossUnk8 g_stOperatorDefeatBossUnk8A(0xFF, 0);
static stOperatorDefeatBossUnk8 g_stOperatorDefeatBossUnk8B(0xFF, 1);

// HYPOTHESIS: set while items are enabled in the match (see ft_manager.cpp).
extern int g_ftItemEnabled;

// HYPOTHESIS: a byte of one-bit flags in the adventure data (gmAdvData is only partly described).
struct AdvFlags {
    u8 unk0 : 1;
    u8 unk1 : 1;
    u8 unk2 : 1;
    u8 unk3 : 1;
    u8 unk4 : 1;
    u8 unk5 : 1;
    u8 unk6 : 1;
    u8 unk7 : 1;
};

// HYPOTHESIS: the running stage as a Subspace Emissary stage, or NULL.
static inline stAdventure2* getAdventureStage() {
    Stage* stage = g_Stage;
    if (stage != NULL && stage->m_stageKind == Stages::Adventure) {
        return static_cast<stAdventure2*>(stage);
    }
    return NULL;
}

void stOperatorDefeatBoss::processBegin() {
    if (getAdventureStage() != NULL) {
        void (stOperatorDefeatBoss::*phases[])() = {
            &stOperatorDefeatBoss::phaseDefeatbossIdle,
            &stOperatorDefeatBoss::phaseInit,
            &stOperatorDefeatBoss::phaseFreezeInit,
            &stOperatorDefeatBoss::phaseFreezeUpdate,
            &stOperatorDefeatBoss::phaseFreezeEnd,
            &stOperatorDefeatBoss::phaseSlowInit,
            &stOperatorDefeatBoss::phaseSlowUpdate,
            &stOperatorDefeatBoss::phaseSlowEnd,
            &stOperatorDefeatBoss::phaseLostInit,
            &stOperatorDefeatBoss::phaseLostUpdate,
            &stOperatorDefeatBoss::phaseLostEnd,
            &stOperatorDefeatBoss::phaseFadeInit,
            &stOperatorDefeatBoss::phaseFadeUpdate,
            &stOperatorDefeatBoss::phaseFadeEnd,
            &stOperatorDefeatBoss::phaseDone,
        };
        (this->*phases[m_phase])();
    }
}

void stOperatorDefeatBoss::startOperator() { }

void stOperatorDefeatBoss::phaseDefeatbossIdle() {
    Stage* stage = g_Stage;
    if (stage == NULL) {
        return;
    }
    if (!stage->unk199) {
        return;
    }
    if ((*(u32*)&g_gfApplication->unkE8[4] >> 27) != 0) {
        return;
    }
    reinterpret_cast<AdvFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_advData) + 0x4924)->unk5 = true;
    m_phase = Phase_Init;
}

void stOperatorDefeatBoss::phaseInit() {
    bool found;
    u32 i = 0;
    u32 num = *reinterpret_cast<u32*>(reinterpret_cast<u8*>(g_stTriggerMng) + 0x50);
    for (; i < num; i++) {
        if ((EnemyKind)fn_27_23345C(g_stTriggerMng, i) == Enemy_Boss_Taboo) {
            found = true;
            goto done;
        }
    }
    found = false;
done:
    unk50 = found;
    unk51 = reinterpret_cast<AdvFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_advData) + 0x4924)->unk1;
    if (unk50) {
        m_param = getAdventureStage()->getDestroyBossParamCommon(1);
    } else if (unk51) {
        m_param = getAdventureStage()->getDestroyBossParamCommon(2);
    } else {
        m_param = getAdventureStage()->getDestroyBossParamCommon(0);
    }
    m_frame = 0;
    Stage* stage = g_Stage;
    if (stage != NULL) {
        if (!stage->unk19A) {
            m_phase = Phase_FreezeInit;
        } else {
            m_phase = Phase_LostInit;
        }
        stage->setStageOutEffectInit(1);
    }
}

void stOperatorDefeatBoss::phaseFreezeInit() {
    scMelee* scene = dynamic_cast<scMelee*>(gfSceneManager::getInstance()->searchScene("scMelee"));
    stOperatorRule* rule = scene->m_operatorRuleGameMode;
    if (rule != NULL && rule->isEnd()) {
        return;
    }
    gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_None, true);
    gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Fighter, true);
    gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Enemy, true);
    gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Weapon, true);
    setPaused(false);
    Vec3f pos(0.0f, 0.0f, 0.0f);
    cmReqQuake(cmQuake::Amplitude_More_Large, &pos);
    Stage* stage = g_Stage;
    if (stage != NULL && !stage->unk19D) {
        if (unk50) {
            g_ecMgr->setEffect(ef_ptc_adv_cmn_slow_taboo);
        } else {
            g_ecMgr->setEffect(ef_ptc_adv_cmn_stop);
        }
    }
    m_frame = 0;
    m_phase = Phase_FreezeUpdate;
}

void stOperatorDefeatBoss::phaseFreezeUpdate() {
    if (m_frame < m_param.m_freezeFrame) {
        m_frame++;
    } else {
        m_phase = Phase_FreezeEnd;
    }
}

void stOperatorDefeatBoss::phaseFreezeEnd() {
    gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_None, false);
    gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Fighter, false);
    gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Enemy, false);
    gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Weapon, false);
    cmRemoveQuake(1);
    m_phase = Phase_SlowInit;
}

void stOperatorDefeatBoss::phaseSlowInit() {
    g_GameGlobal->setSlowRate(m_param.m_slowRate);
    g_ecMgr->setEffect(ef_ptc_adv_cmn_slow);
    m_frame = 0;
    m_phase = Phase_SlowUpdate;
}

void stOperatorDefeatBoss::phaseSlowUpdate() {
    if (m_frame < m_param.m_slowFrame) {
        m_frame++;
    } else {
        m_phase = Phase_SlowEnd;
    }
}

void stOperatorDefeatBoss::phaseSlowEnd() {
    if (!unk51) {
        g_GameGlobal->setSlowRate(1);
        m_phase = Phase_LostInit;
    } else {
        m_phase = Phase_FadeInit;
    }
}

void stOperatorDefeatBoss::phaseLostInit() {
    m_frame = 0;
    m_phase = Phase_LostUpdate;
}

void stOperatorDefeatBoss::phaseLostUpdate() {
    if (m_frame < m_param.m_lostFrame) {
        if (g_ftItemEnabled != 0) {
            itManager::getInstance()->getGenItemNum();
        }
        m_frame++;
    } else if (!g_Stage->unk19A) {
        if (getAdventureStage()->unk19B) {
            m_phase = Phase_LostEnd;
        }
    } else {
        m_phase = Phase_LostEnd;
    }
    if (g_Stage->unk19A == 0 && getAdventureStage()->unk19C) {
        m_phase = Phase_LostEnd;
    }
}

void stOperatorDefeatBoss::phaseLostEnd() {
    m_phase = Phase_FadeInit;
}

void stOperatorDefeatBoss::phaseFadeInit() {
    GXColor white = { 0xFF, 0xFF, 0xFF, 0xFF };
    GXColor black = { 0x00, 0x00, 0x00, 0xFF };
    GXColor color;
    if (unk51) {
        color = white;
        float frames = m_param.m_fadeFrame / m_param.m_slowRate;
        GXColor fill = color;
        int handle = g_efScreen->requestFill(frames, 8, 0, &fill);
        reinterpret_cast<u8&>(m_screenHandle) = (u32)handle >> 24;
    } else {
        color = black;
        float frames = m_param.m_fadeFrame;
        GXColor fill = color;
        int handle = g_efScreen->requestFill(frames, 7, 0, &fill);
        reinterpret_cast<u8&>(m_screenHandle) = (u32)handle >> 24;
    }
    m_frame = 0;
    m_phase = Phase_FadeUpdate;
}

void stOperatorDefeatBoss::phaseFadeUpdate() {
    if (m_frame < m_param.m_fadeFrame) {
        m_frame++;
    } else {
        m_phase = Phase_FadeEnd;
    }
}

void stOperatorDefeatBoss::phaseFadeEnd() {
    if (m_screenHandle.isValid()) {
        float frames = m_param.m_fadeFrame;
        efScreenHandle handle = m_screenHandle;
        fn_8005E3C8(g_efScreen, &handle, frames);
        reinterpret_cast<s8&>(m_screenHandle) = -1;
    }
    if (g_stTriggerMng != NULL) {
        // MATCH-ONLY: the trigger record is filled as plain bytes (stTriggerData declares bitfields).
        struct {
            u16 m_triggerId;
            u8 m_isValid;
            u8 m_keepKind;
        } data;
        data.m_triggerId = 1000;
        data.m_isValid = true;
        data.m_keepKind = stTriggerData::Keep_None;
        g_stTriggerMng->setTriggerFlag(reinterpret_cast<stTriggerData*>(&data));
    }
    m_phase = Phase_Done;
}

void stOperatorDefeatBoss::phaseDone() { }

stOperatorDefeatBoss::~stOperatorDefeatBoss() { }
