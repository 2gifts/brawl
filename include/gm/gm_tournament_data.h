// Local BrawlHeaders shadow: adds the byte cleared by sqToMelee::start.
#pragma once

#include <StaticAssert.h>
#include <types.h>

struct gmTournamentData {
    union {
        char _spacer[0x510]; // 0x00
        struct {
            u8 unk0[0x509];
            u8 unk509;
        };
    };
};
static_assert(sizeof(gmTournamentData) == 0x510, "Class is wrong size!");