#include <st/operator/st_operator_appearance_fighter_null.h>
#include <types.h>
#include <st/operator/st_operator_ready_go.h>
#include <gm/gm_global.h>

extern const float lbl_27_rodata_3EE0[2];

stOperatorAppearanceFighterNull::stOperatorAppearanceFighterNull() { }

stOperatorAppearanceFighterNull::~stOperatorAppearanceFighterNull() { }

void stOperatorAppearanceFighterNull::startAppearance() {
    // HYPOTHESIS: unk90 points to the ready-go operator.
    stOperatorReadyGo* readyGo = static_cast<stOperatorReadyGo*>(unk90);
    if (readyGo) {
        readyGo->startOperator();
    }
    u16 stageKind = g_GameGlobal->m_modeMelee->m_meleeInitData.m_stageKind;
    if (stageKind == 5 || stageKind == 8 || stageKind == 14) {
        g_GameGlobal->m_stageData->m_motionRatio = lbl_27_rodata_3EE0[0];
    }
}

void stOperatorAppearanceFighterNull::processBegin() { }

bool stOperatorAppearanceFighterNull::isEnd() {
    return true;
}
