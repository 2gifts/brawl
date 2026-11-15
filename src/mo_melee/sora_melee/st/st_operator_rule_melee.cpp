#include <st/operator/st_operator_rule_melee.h>
#include <gm/gm_global.h>
#include <gm/gm_global_mode_melee.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <types.h>

// TODO: the rule processing, result and corps members.

bool stOperatorRuleMelee::isEnd() {
    return m_state == 5;
}

void stOperatorRuleMelee::updateExtraInfo() { }

bool stOperatorRuleMelee::checkExtraRule() {
    return false;
}

void stOperatorRuleMelee::decisionExtraRule() { }

void stOperatorRuleMelee::calcScore() {
    setNormalResultInfo();
}

// The corps rule (HYPOTHESIS: corps kind 4) is set up from the global corps data.
void stOperatorRuleMelee::notifyEventSetupCorps() {
    if (m_condition->m_meleeInitData.m_corpsKind == 4) {
        initCorpsRule(g_GameGlobal->m_corps);
    }
}

static inline int getStock(gmPlayerResultInfo* info) {
    s8 stock = info->m_startStockCount;
    if (stock < 0) {
        return 1;
    }
    return stock;
}

// The stocks left of the human players with a controller (a negative count counts as one).
int stOperatorRuleMelee::getLocalHumanTotalStock() {
    int total = 0;
    for (int i = 0; i < m_numPlayers; i++) {
        gmPlayerResultInfo* info = &m_resultInfo->m_playersResultInfo[i];
        if (info->m_state == 0 && info->m_hasController) {
            total += getStock(info);
        }
    }
    return total;
}

int stOperatorRuleMelee::getPlayerStock(int playerNo) {
    return getStock(&m_resultInfo->m_playersResultInfo[m_condition->m_playersInitData[playerNo].m_playerNo]);
}

// Per team: the members, and the stocks left of the members still in (and the count of teams with stocks left).
void stOperatorRuleMelee::getTeamTotalStock(int* numTeams, int* teamStocks, int* teamMembers) {
    *numTeams = 0;
    teamStocks[0] = 0;
    teamMembers[0] = 0;
    teamStocks[1] = 0;
    teamMembers[1] = 0;
    teamStocks[2] = 0;
    teamMembers[2] = 0;
    for (int i = 0; i < m_numPlayers; i++) {
        gmPlayerResultInfo* info = &m_resultInfo->m_playersResultInfo[i];
        if (info->m_state != 3) {
            u8 team = info->m_teamNo;
            teamMembers[team]++;
            if (info->m_startStockCount != 0) {
                if (teamStocks[team] == 0) {
                    (*numTeams)++;
                }
                int stock = info->m_startStockCount;
                teamStocks[team] += stock < 1 ? 1 : stock;
            }
        }
    }
}

void stOperatorRuleMelee::getExistTeam(int* numTeams) {
    *numTeams = 0;
    u32 teams = 0;
    for (int i = 0; i < m_numPlayers; i++) {
        gmPlayerResultInfo* info = &m_resultInfo->m_playersResultInfo[i];
        if (info->m_state != 3) {
            u32 bit = 1 << info->m_teamNo;
            if (!(teams & bit)) {
                teams |= bit;
                (*numTeams)++;
            }
        }
    }
}

// A tie in the time-type modes goes to sudden death (not in corps matches).
bool stOperatorRuleMelee::isSuddenDeath() {
    switch (m_condition->m_meleeInitData.m_gameMode) {
    case 0:
    case 1:
    case 0xe:
    case 0x17:
    case 0x18:
    case 0x1c:
        if (m_condition->m_meleeInitData.m_corpsKind == 0) {
            if (m_resultInfo->m_isTeams == false) {
                return m_resultInfo->m_numWinners > 1;
            }
            return m_resultInfo->m_unk10 > 1;
        }
        break;
    }
    return false;
}

// HYPOTHESIS: the high score record of the solo modes 2 and 4: the time in seconds, the damage given (by both players when
// the second one is in) and the time in frames.
void stOperatorRuleMelee::calcHighScore() {
    u32 mode = m_condition->m_meleeInitData.m_gameMode;
    if (mode == 4 || mode == 2) {
        gmResultInfo* info = m_resultInfo;
        u32 frames = m_remainingFrameTime;
        u32 damage = info->m_playersResultInfo[0].m_giveDamage;
        if (info->m_playersResultInfo[1].m_state == 0) {
            damage += info->m_playersResultInfo[1].m_giveDamage;
        }
        info->m_playersResultInfo[0].m_unk240 = (int)frames / 60;
        info->m_playersResultInfo[0].m_unk244 = damage;
        *reinterpret_cast<u32*>(info->m_playersResultInfo[0]._248) = 0;
        *reinterpret_cast<u32*>(info->m_playersResultInfo[0]._248 + 4) = m_remainingFrameTime;
    }
}

// HYPOTHESIS: +0x224 is the network flag of the match; byte +0xF of each player's result goes to the Wi-Fi task's bytes at +0xFDF.
void stOperatorRuleMelee::networkWriteRanking() {
    if (*(reinterpret_cast<u8*>(this) + 0x224) != 0) {
        u8* rank = reinterpret_cast<u8*>(g_muWifiInterfaceTask) + 0xFDF;
        for (int i = 0; i < m_numPlayers; i++, rank++) {
            gmPlayerInitData* player = &m_condition->m_playersInitData[i];
            if (player->m_state != 3) {
                int playerNo = player->m_playerNo;
                if (playerNo >= 0 && playerNo < 4) {
                    *rank = m_resultInfo->m_playersResultInfo[playerNo].m_unkF;
                }
            }
        }
    }
}
