#pragma once

// Local BrawlHeaders shadow: m_decisionKind (a DecisionKind) and m_characterKind (a gmCharacterKind) are plain bytes:
// stResult::createObj and ftManager::isExistLoseFighterResult read them with lbz. m_gameRule stays an enum bit field
// (gmAddDiagramResultCommon reads it with lwz and a signed extract).

#include <StaticAssert.h>
#include <types.h>
#include <gm/gm_lib.h>

struct gmPlayerResultInfo {
    u8 m_characterKind; // gmCharacterKind
    u8 m_state;
    s8 m_colorFileIdx;
    char _3[1];
    u8 m_controllerNo; // HYPOTHESIS: the number of the player's controller + 1 (0 for none)
    u8 m_unk5;         // HYPOTHESIS: the rumble setting of the controller (ifVsResultTask; also passed to the auto handicap lookup)
    u8 m_teamNo;
    u8 : 3;
    u8 m_hasController : 1; // HYPOTHESIS: a human player with a controller (ifVsResultTask; also gets coins in gmAddMeleeCoin)
    u8 : 4;
    s16 m_hitPointMax;
    s8 m_startStockCount; // signed: ifVsResultTask tests it with > 0 on the byte
    char _11[1];
    s16 m_startDamage;
    u8 m_rank;
    u8 m_unkF; // HYPOTHESIS: zero for players that lost (auto handicap)
    u32 m_koCount;
    u32 m_deadCount;
    u16 m_suicideCount;
    u16 m_playerBeatCounts[MAX_PLAYERS];
    u32 m_coins;
    u32 m_pickupCoins;
    u32 m_lostCoins;
    char _34[0x19C];
    u32 m_unk1D0; // HYPOTHESIS: damage dealt, summed into the record's damage total
    char _1D4[0x58];
    int m_unk22C; // input of the coin rate: gmCountMeleeCoin(kind, m_unk22C)
    char _230[0x28];
    int m_rankCount;
    char _604[80];
};
static_assert(sizeof(gmPlayerResultInfo) == 684, "Class is wrong size!");

class gmResultInfo {
public:
    enum DecisionKind {
        Decision_Timeup = 0x1,
        Decision_Win = 0x2,
        Decision_Team_Win = 0x3,
        Decision_Failure = 0x4,
        Decision_Complete = 0x5,
        Decision_Success = 0x6,
        Decision_Event_Success = 0x7,
        Decision_Event_Failure = 0x8,
        Decision_NoContest = 0x9,
    };

    char _0[1];
    GameRule m_gameRule : 8;
    bool m_isTeams;
    char _3[0xb];
    u8 m_isTournament : 1; // 0xE HYPOTHESIS: a tournament match (ifVsResultTask)
    u8 : 7;
    u8 m_numWinners;
    char _10[14];
    u8 m_winningTeam; // 0x1E
    u8 m_winningPlayer; // 0x1F: a plain byte (ifVsResultTask reads it without sign extension)
    char _32[4];
    gmPlayerResultInfo m_playersResultInfo[MAX_PLAYERS];
    char _4824[0xa0];
    u8 m_decisionKind;
    char _0x1379[11];
    int m_framesElapsed;
};
static_assert(sizeof(gmResultInfo) == 0x1388, "Class is wrong size!");

namespace static_checks {

} // namespace static_checks
