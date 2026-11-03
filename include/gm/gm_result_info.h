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

// HYPOTHESIS: the result of a team in a team battle; only the end of the record is read so far.
struct gmTeamResultInfo {
    char _0[0x2C];
    u8 m_rank;      // 0x2C
    u8 m_unk2D;     // 0x2D: zero for the winning team
    u8 m_isPlaying; // 0x2E: nonzero if the team has players
    char _2F[1];
};
static_assert(sizeof(gmTeamResultInfo) == 0x30, "Class is wrong size!");

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

    u8 m_gameMode;          // 0x00 GameMode: the tournament and the Wifi for-fun mode have their own result title
    GameRule m_gameRule : 8;
    bool m_isTeams;
    s8 m_suicidePenalty;    // 0x03 HYPOTHESIS: the points a self-destruct costs (ifVsResultTask multiplies the count by it)
    u32 m_timeOrCoins;      // 0x04 the limit of the time and coin rules
    u32 m_stockCount;       // 0x08 the limit of the stock rule
    u16 m_stageKind;        // 0x0C HYPOTHESIS: the stage that was played (its play count is bumped by gmSetMeleeResult)
    u8 m_isTournament : 1; // 0xE HYPOTHESIS: a tournament match (ifVsResultTask)
    u8 : 7;
    u8 m_numWinners;
    char _10[14];
    u8 m_winningTeam; // 0x1E
    u8 m_winningPlayer; // 0x1F: a plain byte (ifVsResultTask reads it without sign extension)
    char _32[4];
    gmPlayerResultInfo m_playersResultInfo[MAX_PLAYERS];
    gmTeamResultInfo m_teamResultInfo[3]; // 0x12D8 HYPOTHESIS: red, blue, green
    char _1368[0x10];
    u8 m_decisionKind;
    char _0x1379[11];
    int m_framesElapsed;
};
static_assert(sizeof(gmResultInfo) == 0x1388, "Class is wrong size!");

namespace static_checks {

} // namespace static_checks
