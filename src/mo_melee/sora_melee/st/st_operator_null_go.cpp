#include <gm/gm_global.h>
#include <gm/gm_global_record.h>
#include <st/operator/st_operator_bgm.h>
#include <st/operator/st_operator_null_go.h>
#include <types.h>

// TODO: processBegin (the go step: BGM, the seven registered objects, ftManager::readyGo).

stOperatorNullGo::stOperatorNullGo(bool runGo) : stOperatorReadyGo(true) {
    m_runGo = runGo;
}

stOperatorNullGo::~stOperatorNullGo() { }

bool stOperatorNullGo::isEnd() {
    return m_state == 5;
}

// Without the go step the match starts right away (and the BGM with it when the adventure flag asks for it).
void stOperatorNullGo::startOperator() {
    g_GameGlobal->m_stageData->m_motionRatio = 1.0f;
    if (m_runGo) {
        m_state = 3;
        return;
    }
    m_state = 5;
    unk54_b7 = true;
    unk4C = 0.0f;
    if (g_GameGlobal->m_advData->m_flags.m_unk1 && unk74 != 0) {
        reinterpret_cast<stOperatorBgm*>(unk74)->requestPlay();
    }
}
