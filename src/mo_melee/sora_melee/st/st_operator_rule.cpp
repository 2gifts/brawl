#include <if/if_mngr.h>
#include <st/operator/st_operator_rule.h>
#include <st/st_event_rule.h>
#include <string.h>
#include <types.h>
#include <ft/ft_manager.h>

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

void stOperatorRule::updateRealtimeHandi(gmResultInfo* result, float deltaFrame) {
    if (m_condition->m_meleeInitData.m_0x5_5) {
        if (0.0f < deltaFrame) {
            unk78 = 0;
            int highest;
            int lowest;
            int count = 0;
            for (int player = 0; player < 4; player++) {
                if (g_ftManager->isValidEntryId(g_ftManager->getEntryId(player))) {
                    gmPlayerResultInfo* playerResult = &result->m_playersResultInfo[m_condition->m_playersInitData[player].m_playerNo];
                    int score;
                    if (m_condition->m_meleeInitData.m_isStamina) {
                        score = 0;
                    } else if (m_condition->m_meleeInitData.m_gameRule == 2) {
                        score = playerResult->m_coins;
                    } else if (m_condition->m_meleeInitData.m_gameRule == 1) {
                        score = playerResult->m_startStockCount;
                    } else {
                        score = playerResult->m_rankCount;
                    }
                    if (!unk78) {
                        highest = score;
                        lowest = score;
                    } else {
                        highest = highest > score ? highest : score;
                        lowest = lowest < score ? lowest : score;
                    }
                    unk7C[player] = score;
                    unk78 |= 1 << player;
                    count++;
                }
            }
            int difference = highest - lowest;
            float scale = 0.5f * difference;
            float strength;
            if (m_condition->m_meleeInitData.m_gameRule == 0) {
                strength = difference < 5 ? 0.5f : 1.0f;
            } else if (m_condition->m_meleeInitData.m_gameRule == 1) {
                strength = difference < 2 ? 0.5f : 1.0f;
            } else {
                strength = 1.0f;
            }
            strength *= count / 4.0f;
            float step = 0.04f * deltaFrame / 60.0f;
            for (int player = 0; player < 4; player++) {
                if (unk78 & (1 << player)) {
                    float target = scale == 0.0f ? 0.0f : strength * ((unk7C[player] - lowest) / scale - 1.0f);
                    unk9CValues[player] = target;
                    float current = unk8C[player];
                    if (current != target) {
                        if (current < target) {
                            current += step;
                            if (current > target) current = target;
                        } else {
                            current -= step;
                            if (current < target) current = target;
                        }
                        unk8C[player] = current;
                    }
                }
            }
        }
        for (int player = 0; player < 4; player++) {
            if (unk78 & (1 << player)) {
                int entryId = g_ftManager->getEntryId(player);
                if (g_ftManager->isValidEntryId(entryId)) {
                    g_ftManager->getOwner(entryId)->setAutoHandi(unk8C[player]);
                }
            }
        }
    }
}
