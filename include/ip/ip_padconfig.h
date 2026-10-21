#pragma once
// Local BrawlHeaders shadow: button profiles and conversion history recovered
// from configuration setters, reset, and all four controller conversion paths.
#include <types.h>
#include <StaticAssert.h>
#include <stddef.h>

struct gfPadStatus;
struct ipPadStatus;

// HYPOTHESIS: original type/prototype names for these verified profile and
// history layouts. Reserved bytes retain their unknown names.
struct ipGcButtonConfig {
    u8 l, r, z, up, side, down, a, b, substick, y, x, flags;
};
// HYPOTHESIS: slot names for the extended Wii button bits. The byte order and
// each source mask are verified; Extra/Motion retain uncertain physical names.
struct ipWiiCoreConfig {
    u8 one, two, oneTwo, a, b, ab, up, extra;
};
struct ipNunchukConfig {
    u8 c, z, cz, a, b, ab, extra, motion, up, side, down, flags;
};
struct ipClassicConfig {
    u8 l, r, zl, zr, up, side, down, a, b, substick, x, y, flags;
};
struct ipWiiButtonConfig {
    ipWiiCoreConfig core;
    ipNunchukConfig nunchuk;
    ipClassicConfig classic;
};
struct ipButtonConfig {
    ipGcButtonConfig gc;
    ipWiiButtonConfig wii;
};

struct ipCorePadState {
    u8 attackHistory;
    u8 specialHistory;
    u8 downHold;
    u8 unk3;
    u16 leftHistory, rightHistory, upHistory, downHistory;
    s16 stickX, stickY;
    u8 downFrames;
    u8 unk11;
};
struct ipSubstickState {
    u8 history;
    u8 motionHistory;
    u8 unk2;
    u8 smashFrames;
    s8 smashX, smashY;
};

struct ipPadConfig {
    ipGcButtonConfig m_gc[4];
    ipWiiButtonConfig m_wii[4];
    u8 unkB4;
    u8 m_playerPads[4];
    u8 unkB9[0x11];
    ipCorePadState m_coreState[8];
    ipSubstickState m_substickState[8];
    u8 unk18A[2];
    int m_padPlayers[8];

    ipPadConfig() : unkB4(0) { reset(); }
    static ipPadConfig* getInstance();
    void reset();
    void setButtonConfig(int player, u32 pad, const ipButtonConfig* config);
    s32 getPlayerNo(s32 pad);
    int getPlayerNoFast(int pad);
    void createPadNo2PlayerNoTable();
    void getPadInput(int player, const gfPadStatus* input, ipPadStatus* output);

    static void setupPadSubstick(ipPadStatus*, const gfPadStatus*, int action, ipSubstickState*);
    static void setupPadStatusGc(ipPadStatus*, const gfPadStatus*, const ipGcButtonConfig*, ipSubstickState*);
    static bool check_key_flip(int history);
    static void setupPadStatusWiiCore(ipPadStatus*, const gfPadStatus*, const ipWiiCoreConfig*, ipCorePadState*);
    static void setupPadStatusWiiClassic(ipPadStatus*, const gfPadStatus*, const ipClassicConfig*, ipSubstickState*);
    static void setupPadStatusWiiFreeStyle(ipPadStatus*, const gfPadStatus*, const ipNunchukConfig*, ipSubstickState*);
};
// Map name, with the 45-byte extent independently verified in its memcpy.
void buttonConfigInit(ipButtonConfig* config);

static_assert(sizeof(ipButtonConfig) == 0x2D, "Combined button profile size");
static_assert(sizeof(ipWiiButtonConfig) == 0x21, "Wii button profile size");
static_assert(sizeof(ipCorePadState) == 0x12, "Wii core state size");
static_assert(sizeof(ipSubstickState) == 6, "Substick state size");
static_assert(sizeof(ipPadConfig) == 0x1AC, "Pad configuration size");
static_assert(offsetof(ipPadConfig, m_playerPads) == 0xB5, "Player pad mapping offset");
static_assert(offsetof(ipPadConfig, m_coreState) == 0xCA, "Wii core state offset");
static_assert(offsetof(ipPadConfig, m_substickState) == 0x15A, "Substick state offset");
static_assert(offsetof(ipPadConfig, m_padPlayers) == 0x18C, "Reverse mapping offset");
