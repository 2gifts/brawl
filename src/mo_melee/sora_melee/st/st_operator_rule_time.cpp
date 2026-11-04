#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <if/if_center.h>
#include <if/if_mngr.h>
#include <st/operator/st_operator_bgm.h>
#include <st/operator/st_operator_rule_time.h>
#include <st/stage.h>
#include <types.h>

// IfCenter::updateTimeFast (the map name); src/sora/if/if_center.cpp calls it by this stand-in name.
extern "C" void fn_800DDCF8(IfCenter* center, s32 time);

stOperatorRuleTime::stOperatorRuleTime() {
    m_state = 0;
    unkCC = 0.0f;
}

stOperatorRuleTime::~stOperatorRuleTime() { }

bool stOperatorRuleTime::isEnd() {
    return m_state == 5;
}

void stOperatorRuleTime::processBegin() {
    if (m_isPause == true) {
        return;
    }
    if (m_isExit == true) {
        return;
    }
    unkCC -= g_GameGlobal->getGameFrameDelta();
    if (unkCC < 0.0f) {
        unkCC = 0.0f;
    }
    if (g_Stage) {
        g_Stage->setFrameRuleTime(unkCC);
    }
    switch (m_state) {
    case 3:
        break;
    case 1:
        fn_800DDCF8(static_cast<IfCenter*>(g_IfMngr->m_ifCenter), static_cast<u32>(unkCC));
        if (unkCC == 0.0f) {
            static_cast<IfCenter*>(g_IfMngr->m_ifCenter)->setCenterState(4);
            if (m_operatorBgm) {
                static_cast<stOperatorBgm*>(m_operatorBgm)->requestStop();
            }
            m_state = 2;
            calcScore();
            g_ftManager->gameSet();
            m_isGameSet = true;
            unk2C_b1 = false;
        }
        break;
    case 2:
        if (static_cast<IfCenter*>(g_IfMngr->m_ifCenter)->m_state == 0) {
            m_state = 5;
            unk2C_b1 = true;
        }
        break;
    }
}

void stOperatorRuleTime::startOperator() {
    m_state = 1;
    u32 timeLimit = g_GameGlobal->m_modeMelee->m_meleeInitData.m_timeLimitFrames;
    if (timeLimit != 0) {
        unkCC = timeLimit;
    } else {
        m_state = 3;
    }
}

bool stOperatorRuleTime::isSuddenDeath() {
    gmResultInfo* result = g_GameGlobal->m_resultInfo;
    if (result == NULL) {
        return false;
    } else {
        return result->m_numWinners > 1;
    }
}
