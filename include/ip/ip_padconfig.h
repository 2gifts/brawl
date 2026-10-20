#pragma once
// Local BrawlHeaders shadow: partial layout used by local/online input routing.
#include <types.h>
#include <StaticAssert.h>
#include <stddef.h>

struct gfPadStatus;
struct ipPadStatus;
struct ipPadConfig {
    u8 unk0[0xB5];
    u8 m_playerPads[4];
    u8 unkB9[0xF3];

    static ipPadConfig* getInstance();
    s32 getPlayerNo(s32 pad);
    // Map names; conversion body and index arithmetic corroborate their roles.
    int getPlayerNoFast(int pad);
    void getPadInput(int player, const gfPadStatus* input, ipPadStatus* output);
};
static_assert(sizeof(ipPadConfig) == 0x1AC, "Pad configuration size");
static_assert(offsetof(ipPadConfig, m_playerPads) == 0xB5, "Player pad mapping offset");
