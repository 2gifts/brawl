// Local BrawlHeaders shadow: byte 0x0C of gmGlobalCorps is a plain u8 inside the _2 padding.
#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <gm/gm_lib.h>

struct gmPlayerCorpsInitData {
    gmCharacterKind m_characterKind : 8;      // 0x00
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

class gmGlobalCorps {
public:
    u8 _0;                                   // 0x00
    s8 m_numFightersToBeat;                  // 0x01
    u8 _1;                                   // 0x02
    s8 m_numFightersInCurrentStage;          // 0x03
    union {
        char _2[0x19];                       // 0x04
        struct {
            char _pad4[8];
            u8 m_unkC; // HYPOTHESIS: a count decreased by 10
            char _padD[0x10];
        };
    };
    s8 m_remainingFightersToBeat;            // 0x1D
    char _3[0x06];                           // 0x1E
    gmPlayerCorpsInitData m_playersInitData[0x24]; // 0x24
};
static_assert(sizeof(gmGlobalCorps) == 0x2f4, "Class is wrong size!");
static_assert(offsetof(gmGlobalCorps, m_remainingFightersToBeat) == 0x1D, "Class is wrong size!");
