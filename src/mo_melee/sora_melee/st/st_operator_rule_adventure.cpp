#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <st/operator/st_operator_appearance_helper.h>
#include <st/operator/st_operator_bgm.h>
#include <st/operator/st_operator_rule_adventure.h>
#include <st/stage.h>
#include <types.h>

stOperatorRuleAdventure::stOperatorRuleAdventure()
    : unkCC(0.0f), unkD4(false), unkD5(false), unkD6(false), unkD7(false),
      unkD8(false), unkDC(150), unkE0(0), unkE4(120), unkE8(0), unkEC(0),
      unkF0(240), unkF4(-1), unkF8(NULL) {
    m_state = 0;
    unk2C_b1 = false;
}

bool stOperatorRuleAdventure::isEnd() {
    bool end = false;
    if (m_state == 12) {
        end = true;
    }
    gmAdvData* data = g_GameGlobal->m_advData;
    if (!end && data->m_saveRequest == 2) {
        if (g_Stage && !g_Stage->isNextStepBgmEqualNowStepBgmFromFlag()) {
            static_cast<stOperatorBgm*>(m_operatorBgm)->requestStop();
        }
        end = true;
    }
    if (end) {
        gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_None, false);
        gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Fighter, false);
        gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Enemy, false);
        gfTaskScheduler::getInstance()->setPauseCategory(gfTask::Category_Weapon, false);
    }
    return end;
}

void stOperatorRuleAdventure::initEndProduction(int kind) {
    unkE0 = 0;
    unkE8 = 0;
    switch (kind) {
    case 0:
        unkD5 = false;
        unkD6 = false;
        break;
    case 1:
        unkD5 = true;
        unkE4 = 0;
        unkDC = 60;
        unkD6 = false;
        break;
    case 2:
        unkD5 = true;
        unkE4 = 120;
        unkDC = 150;
        unkD6 = true;
        unkEC = 0;
        unkF0 = 240;
        break;
    case 3:
        unkD5 = true;
        unkE4 = 60;
        unkDC = 120;
        unkD6 = false;
        break;
    }
    if (unkF8) {
        static_cast<stOperatorAppearanceHelper*>(unkF8)->endOperator();
    }
}

stOperatorRuleAdventure::~stOperatorRuleAdventure() { }

void stOperatorRuleAdventure::setOpAppearanceHelper(void* helper) {
    unkF8 = helper;
}
