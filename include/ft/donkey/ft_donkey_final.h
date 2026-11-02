#pragma once

#include <ft/ft_common_data_accesser.h>
#include <StaticAssert.h>
#include <stddef.h>
#include <types.h>

// HYPOTHESIS: source names for the Konga Beat parameter records. Their field
// offsets follow the FinalLoop consumers and the fighter's extend-param setup.
struct ftDonkeyFinalBonusParam {
    float gain;
    float maximum;
};
struct ftDonkeyFinalParam {
    int inputInterval;                  // 0x00: shortest accepted interval between strikes
    float fastInputBpm;                 // 0x04
    float slowInputBpm;                 // 0x08
    float maxMotionRate;                // 0x0C
    float motionRateDecay;              // 0x10
    int noteEffectInterval;             // 0x14
    ftDonkeyFinalBonusParam bonuses[3]; // 0x18: power, travel speed and scale, respectively
    int beatWindow;                     // 0x30
    int wideWindowCombo;                // 0x34
    int wideBeatWindow;                 // 0x38
    int repeatHitInterval;              // 0x3C
    int strongStrikeCombo;                  // 0x40
    int beatProgressLimit;              // 0x44
    int missAttackLife;                 // 0x48
    int hitAttackLife;                  // 0x4C
    int strongStrikeAttackLife;             // 0x50
    int duration;                       // 0x54
};
static_assert(sizeof(ftDonkeyFinalParam) == 0x58, "Konga Beat parameter extent");

struct ftDonkeyFinalBeat {
    int frame;
    u8 strongStrike; // Nonzero permits a strong strike once the combo reaches its threshold.
};
static_assert(sizeof(ftDonkeyFinalBeat) == 8, "Konga Beat beat entry extent");

// HYPOTHESIS: Donkey's moveset header extends the common header with these words.
// FinalLoop reads the table and count directly; the additional preceding word's
// role has not been identified.
struct ftDonkeyFinalData : ftData {
    const void* unk94;
    const ftDonkeyFinalBeat* beats;
    int beatCount;
};
static_assert(offsetof(ftDonkeyFinalData, beats) == 0x98, "Konga Beat beat table offset");
static_assert(offsetof(ftDonkeyFinalData, beatCount) == 0x9C, "Konga Beat beat count offset");
