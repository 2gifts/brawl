#pragma once

// Local shadow of the header of the shared headers (this directory comes first in the include paths): it names the kind of
// the select screen that the character select screen reads.

#include <StaticAssert.h>
#include <gm/gm_global_mode_melee.h>
#include <types.h>

struct gmSelCharData {
    char _0[0x4];
    u8 m_selCharType;   // 0x04 the kind of the select screen (versus, a solo mode, a minigame ...)
    union {
        char _5[0xb3];
        struct {
            char _pad5[0x29];
            u8 m_unk2E_7 : 3; // HYPOTHESIS: top 3 bits of byte 0x2E
            u8 m_unk2E_4 : 3; // HYPOTHESIS: the next 3 bits of byte 0x2E
            u8 : 2;
        };
    };
    gmPlayerInitData m_playersInitData[MAX_PLAYERS];
    char _0x33c[4];
};
static_assert(sizeof(gmSelCharData) == 0x340, "Class is wrong size!");
static_assert(offsetof(gmSelCharData, m_playersInitData) == 0xB8, "Class is wrong size!");
