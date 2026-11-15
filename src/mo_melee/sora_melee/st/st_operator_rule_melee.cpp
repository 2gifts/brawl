#include <st/operator/st_operator_rule_melee.h>
#include <ai/ai_mgr.h>
#include <ft/ft_manager.h>
#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <math.h>
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

// HYPOTHESIS: the pause/stall bits at the top of the application word at +0xEC.
struct stRuleMeleeAppFlagsView {
    u32 m_stop : 5;
    u32 unk : 27;
};

static inline stRuleMeleeAppFlagsView* getAppFlags() {
    return reinterpret_cast<stRuleMeleeAppFlagsView*>(reinterpret_cast<u8*>(g_gfApplication) + 0xEC);
}

// HYPOTHESIS: the init data flag byte at +0x1D (0x04: removed) and the CPU type and rank after it.
struct stRuleMeleePlayerFlagsView {
    char _0[0x1D];
    u8 unk80 : 5;
    u8 m_isRemoved : 1;
    u8 unk02 : 2;
    u8 m_cpuType;
    u8 m_cpuRank;
};

// HYPOTHESIS: flag 0x08 of the mode byte at +0xF.
struct stRuleMeleeModeFlagsView {
    char _0[0xF];
    u8 unkF0 : 4;
    u8 m_flag08 : 1;
    u8 unkF7 : 3;
};

static inline u8* byteAt(void* object, int offset) {
    return reinterpret_cast<u8*>(object) + offset;
}

// HYPOTHESIS: +0xF4 is the time step (60 over the application frame rate at +0xF8, a halfword), +0xD5 is set unless flag
// 0x08 of the mode byte +0xF is.
void stOperatorRuleMelee::startOperator() {
    stOperatorRule::startOperator();
    m_isStart = true;
    *reinterpret_cast<float*>(byteAt(this, 0xF4)) = 60.0f / *reinterpret_cast<u16*>(byteAt(g_gfApplication, 0xF8));
    *reinterpret_cast<bool*>(byteAt(this, 0xD5)) = reinterpret_cast<stRuleMeleeModeFlagsView*>(m_condition)->m_flag08 == 0;
}

// The frame counters advance by whole frames of the accumulated time step (paused games and HYPOTHESIS: the network
// stall flags at +0x75 and the application's +0xEC word stop them); they saturate at six hours.
void stOperatorRuleMelee::updateFrameCounter() {
    if (*byteAt(this, 0x75) != 0 && (getAppFlags()->m_stop & 4)) {
        return;
    }
    if (!m_isStart) {
        return;
    }
    if (!(*byteAt(m_condition, 0xC) & 1) && getAppFlags()->m_stop != 0) {
        return;
    }
    float* time = reinterpret_cast<float*>(byteAt(this, 0xEC));
    *time += *reinterpret_cast<float*>(byteAt(this, 0xF4));
    float whole = floor(*time);
    int frames = whole;
    *time -= whole;
    if (frames != 0) {
        if (m_remainingFrameTime > frames) {
            m_remainingFrameTime -= frames;
        } else {
            m_remainingFrameTime = 0;
        }
        m_framesElapsed += frames;
        m_framesElapsed = m_framesElapsed < 21599999 ? m_framesElapsed : 21599999;
        if (*byteAt(this, 0xD7) != 0) {
            m_frameCounter += frames;
            m_frameCounter = m_frameCounter < 21599999 ? m_frameCounter : 21599999;
        }
    }
}

// Removes the players still in that are not in the mask: as a CPU (mode 1) or leaving (state 3).
// HYPOTHESIS: flag 0x04 of +0x1D of the init data marks a removed player; +0x1E/+0x1F are its CPU type and rank.
void stOperatorRuleMelee::deletePlayer(u32 mode, u32 keepMask) {
    for (int i = 0; i < m_numPlayers; i++) {
        gmPlayerInitData* player = &m_condition->m_playersInitData[i];
        if (player->m_state == 0) {
            stRuleMeleePlayerFlagsView* flags = reinterpret_cast<stRuleMeleePlayerFlagsView*>(player);
            if (!flags->m_isRemoved && !(keepMask & (1 << i))) {
                flags->m_isRemoved = true;
                if (mode == 1) {
                    int entryId = g_ftManager->getEntryId(i);
                    if (entryId != -1) {
                        g_ftManager->setFighterOperationType(entryId, 1);
                        g_ftManager->setFighterOperationCpuType(entryId, flags->m_cpuType);
                        g_aiMgr->setCpuRank(i, flags->m_cpuRank);
                    }
                } else {
                    player->m_state = 3;
                }
            }
        }
    }
}

// HYPOTHESIS: +0x74 stops the bookkeeping (a replay or a network spectator).
void stOperatorRuleMelee::notifyEventKnockout(u32 playerIndex) {
    if (*byteAt(this, 0x74) == 0 && (int)playerIndex < m_numPlayers) {
        m_0x124[playerIndex] = m_framesElapsed;
        m_playersLastDeadSleepFrame[playerIndex] = 0;
    }
}

// Logs the KO in both players' results (the last 100 of each), then the common bookkeeping.
void stOperatorRuleMelee::notifyEventBeat(u32 playerIndex1, u32 playerIndex2) {
    if (*byteAt(this, 0x74) == 0) {
        gmPlayerResultInfo* winner = &m_resultInfo->m_playersResultInfo[m_condition->m_playersInitData[playerIndex1].m_playerNo];
        gmPlayerResultInfo* loser = &m_resultInfo->m_playersResultInfo[m_condition->m_playersInitData[playerIndex2].m_playerNo];
        int koIndex = m_0x15C[playerIndex1]++ % 100;
        winner->m_koTargets[koIndex] = loser->m_characterKind;
        winner->m_koCauses[koIndex] = loser->_3[0];
        int deadIndex = m_0x178[playerIndex2]++ % 100;
        loser->m_deadTargets[deadIndex] = winner->m_characterKind;
        loser->m_deadCauses[deadIndex] = winner->_3[0];
        stOperatorRule::notifyEventBeat(playerIndex1, playerIndex2);
    }
}
