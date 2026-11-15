#include <st/operator/st_operator_rule_melee.h>
#include <gm/gm_global.h>
#include <gm/gm_global_mode_melee.h>
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
