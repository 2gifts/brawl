#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <gm/gm_lib.h>

// One fighter of a corps (an All-Star or Subspace team list).
struct gmPlayerCorpsInitData {
    u8 m_characterKind;        // 0x00 (a gmCharacterKind; read with byte loads)
    u8 m_costumeId;            // 0x01
    s8 m_stockCount;           // 0x02
    u8 m_cpuType;              // 0x03
    s8 m_cpuRank;              // 0x04
    char _1[0x03];             // 0x05
    float m_attackReactionMul; // 0x08
    float m_damageReactionMul; // 0x0C
    float m_scale;             // 0x10
};
static_assert(sizeof(gmPlayerCorpsInitData) == 0x14, "Class is wrong size!");

// The corps of the running mode. The names of the all-star conversion fields and of the rest area fields are
// HYPOTHESIS (from gmGlobalCorpsConvertAllStarToCorps and stHeal).
class gmGlobalCorps {
public:
    u8 _0;                                   // 0x00
    u8 m_numFightersToBeat;                  // 0x01
    u8 _1;                                   // 0x02
    s8 m_numFightersInCurrentStage;          // 0x03
    u8 m_isAllStarList;                      // 0x04: 1 while the list is an all-star setup not converted yet
    u8 m_flags;                              // 0x05: 0x40 set by the conversion
    char _6[0x04];                           // 0x06
    u8 m_characterKind;                      // 0x0A: fighter, cpu and reactions shared by the whole corps
    u8 m_cpuType;                            // 0x0B
    s8 m_cpuRank;                            // 0x0C
    char _D[0x03];                           // 0x0D
    float m_attackReactionMul;               // 0x10
    float m_damageReactionMul;               // 0x14
    float m_scale;                           // 0x18
    u8 m_figureCount;                        // 0x1C: number of defeated fighters the rest area shows as figures
    s8 m_remainingFightersToBeat;            // 0x1D
    u8 m_heartMask;                          // 0x1E: bit n set while heart n of the rest area has not been taken
    u8 m_stageNo;                            // 0x1F: round number (the rest area's "Info_stgNN" sign)
    u16 m_extraFigureId;                     // 0x20
    char _22[0x02];                          // 0x22
    union {
        gmPlayerCorpsInitData m_playersInitData[0x24]; // 0x24: the all-star list
        u8 m_costumeIds[0x24 * 0x14];                  // 0x24: the corps list (one costume per fighter)
    };
};
static_assert(sizeof(gmGlobalCorps) == 0x2f4, "Class is wrong size!");
