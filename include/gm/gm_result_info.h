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
    u8 m_unk5;         // HYPOTHESIS: the rumble setting of the controller (ifVsResultTask; also passed to the auto handicap lookup);
                       // GameGlobal::initResultInfoData copies the name index of the init data here
    u8 m_teamNo;
    u8 : 1;
    u8 m_unk7_6 : 1;   // HYPOTHESIS: copy of gmPlayerInitData::m_0x1B_6 (GameGlobal::initResultInfoData)
    u8 m_isStamina : 1; // HYPOTHESIS: copy of gmPlayerInitData::m_isStamina (GameGlobal::initResultInfoData)
    u8 m_hasController : 1; // HYPOTHESIS: a human player with a controller (ifVsResultTask; also gets coins in gmAddMeleeCoin)
    u8 : 4;
    u16 m_hitPointMax; // unsigned: GameGlobal::initResultInfoData copies it with lhz
    s8 m_startStockCount; // signed: ifVsResultTask tests it with > 0 on the byte
    char _11[1];
    u16 m_startDamage; // unsigned, as m_hitPointMax
    u8 m_rank;
    u8 m_unkF; // HYPOTHESIS: zero for players that lost (auto handicap)
    u32 m_koCount;
    u32 m_deadCount;
    u16 m_suicideCount;
    u16 m_playerBeatCounts[MAX_PLAYERS];
    u32 m_coins;
    u32 m_pickupCoins;
    u32 m_lostCoins;
    u8 m_koTargets[100];   // 0x34 HYPOTHESIS: one entry per KO (m_koCount of them): the player that was knocked out
    u8 m_koCauses[100];    // 0x98
    u8 m_deadTargets[100]; // 0xFC HYPOTHESIS: one entry per death (m_deadCount of them)
    u8 m_deadCauses[100];  // 0x160
    char _1C4[8];
    // HYPOTHESIS: the names of 0x1CC-0x1DC, 0x228 and 0x284-0x2A0 are those of the gmDiagramResult members that
    // gmAddDiagramResultCommon adds them into.
    u32 m_giveDamage;    // 0x1CC
    u32 m_takeDamage;    // 0x1D0 (also summed into the record's damage total)
    u32 m_recoverDamage; // 0x1D4
    u32 m_maxDamage;     // 0x1D8
    u32 m_blowDistance;  // 0x1DC
    u32 m_unk1E0;
    u32 m_unk1E4;
    u32 m_unk1E8;
    u8 m_unk1EC;
    char _1ED[3];
    u32 m_unk1F0;
    u32 m_unk1F4;
    u32 m_unk1F8;
    u32 m_unk1FC;
    u32 m_unk200;
    u32 m_unk204;
    u32 m_unk208;
    u32 m_unk20C;
    u32 m_damageSpeedMaxDisplay; // 0x210 HYPOTHESIS: m_damageSpeedMax in ft/h, truncated (ifVsResultTask::adjustResultinfo)
    u32 m_attackSpeedMaxDisplay; // 0x214 HYPOTHESIS: as m_damageSpeedMaxDisplay for m_attackSpeedMax
    u32 m_unk218; // frames, converted to seconds by ifVsResultTask::adjustResultinfo (as 0x1E0-0x1E8 and 0x21C)
    u32 m_unk21C;
    u32 m_unk220;
    u32 m_unk224;
    u32 m_makeDropCoin; // 0x228
    int m_unk22C; // input of the coin rate: gmCountMeleeCoin(kind, m_unk22C)
    char _230[4];
    u32 m_unk234;
    u32 m_unk238;
    u32 m_unk23C;
    u32 m_unk240;
    u32 m_unk244;
    char _248[0x10];
    int m_rankCount;
    char _25C[0x28];
    u32 m_totalBeat;     // 0x284
    u32 m_totalFall;     // 0x288
    u32 m_walkDistance;  // 0x28C
    u32 m_jumpDistance;  // 0x290
    u32 m_fallDistance;  // 0x294
    u32 m_swimDistance;  // 0x298
    u32 m_swimFall;      // 0x29C
    u32 m_fallButtocks;  // 0x2A0
    float m_damageSpeedMax; // 0x2A4 HYPOTHESIS: from the "_damagespeedmax" report of ifVsResultTask::adjustResultinfo
    float m_attackSpeedMax; // 0x2A8 HYPOTHESIS: "_attackspeedmax"
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
    u8 m_unk10;
    u8 m_unk11;
    u8 m_unk12;
    u8 m_unk13; // HYPOTHESIS: the first player whose rank is zero (GameGlobal::initResultInfoData)
    char _14[6];
    u8 m_unk1A;
    char _1B[3];
    u8 m_winningTeam; // 0x1E
    u8 m_winningPlayer; // 0x1F: a plain byte (ifVsResultTask reads it without sign extension)
    char _32[4];
    gmPlayerResultInfo m_playersResultInfo[MAX_PLAYERS];
    gmTeamResultInfo m_teamResultInfo[3]; // 0x12D8 HYPOTHESIS: red, blue, green
    u32 m_unk1368;
    u8 m_unk136C;
    char _136D[3];
    u32 m_unk1370;
    u32 m_unk1374;
    u8 m_decisionKind;
    char _0x1379[11];
    int m_framesElapsed;
};
static_assert(sizeof(gmResultInfo) == 0x1388, "Class is wrong size!");

namespace static_checks {

} // namespace static_checks
