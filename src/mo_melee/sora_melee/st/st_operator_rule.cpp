#include <if/if_mngr.h>
#include <st/operator/st_operator_rule.h>
#include <st/st_event_rule.h>
#include <string.h>
#include <types.h>

struct stOperatorRuleCharacterKindEntry {
    u8 m_gmKind;
    u8 m_muKind;
    u8 unk2;
    u8 m_figureId;
};

extern "C" {
// HYPOTHESIS: per-fighter-kind table (sora_melee .data 0x4EDEC), 0x33 entries.
extern stOperatorRuleCharacterKindEntry lbl_27_data_4EDEC[];
// HYPOTHESIS: the reset value of the replay-save state (sora_melee .data 0x4EDD8).
extern u32 lbl_27_data_4EDD8;
}

stOperatorRule::~stOperatorRule() { }

void stOperatorRule::processBegin() { }

bool stOperatorRule::isEnd() {
    return false;
}

bool stOperatorRule::isSuddenDeath() {
    return false;
}

void stOperatorRule::calcScore() { }

void stOperatorRule::notifyEventKnockout(u32) { }

bool stOperatorRule::notifyEventDead(int entryId, u32 playerIndex, int) {
    return true;
}

void stOperatorRule::notifyEventSetupCorps() { }

void stOperatorRule::nortifyNetworkOtherNodeExit() { }

u32 stOperatorRule::exchangeGmCharacterKindToMuCharKind(gmCharacterKind kind) {
    bool valid = false;
    if (kind >= 0 && kind < 0x33) {
        valid = true;
    }
    if (valid) {
        return lbl_27_data_4EDEC[kind].m_muKind;
    }
    return 0x23;
}

u32 stOperatorRule::exchangeGmCharacterKind2FigureId(gmCharacterKind kind) {
    bool valid = false;
    if (kind >= 0 && kind < 0x33) {
        valid = true;
    }
    if (valid) {
        return lbl_27_data_4EDEC[kind].m_figureId;
    }
    return 0;
}

bool stOperatorRule::canSaveReplay() {
    return !unk75;
}

void stOperatorRule::startSaveReplay() {
    unkB4 = lbl_27_data_4EDD8;
    unkB8 = 0;
    unkBC = false;
}

void stOperatorRule::notifyEventBeat(u32 playerIndex1, u32 playerIndex2) {
    if (m_condition->m_meleeInitData.m_0x3_2 && g_IfMngr) {
        if (m_condition->m_meleeInitData.m_isTeams == true &&
            (u8)m_condition->m_playersInitData[playerIndex1].m_teamNo ==
                (u8)m_condition->m_playersInitData[playerIndex2].m_teamNo) {
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerIndex1].m_playerId]->setScoreVary(-1);
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerIndex2].m_playerId]->setScoreVary(-1);
            return;
        } else {
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerIndex1].m_playerId]->setScoreVary(1);
        }
        g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerIndex2].m_playerId]->setScoreVary(-1);
    }
}

void stOperatorRule::startOperator() {
    if (unk54) {
        unk54->notifyEventStartGame();
    }
}

void stOperatorRule::notifyEventStartStage() {
    unk78 = 0;
    memset(unk7C, 0, sizeof(unk7C));
    memset(unk8C, 0, sizeof(unk8C));
    checkDispCorps();
    if (unk54) {
        unk54->notifyEventStartStage();
    }
}

void stOperatorRule::notifyEventYoshiEgg() {
    if (unk54) {
        unk54->notifyEventYoshiEgg();
    }
}

void stOperatorRule::notifyEventPikminBloom() {
    if (unk54) {
        unk54->notifyEventPikminBloom();
    }
}
