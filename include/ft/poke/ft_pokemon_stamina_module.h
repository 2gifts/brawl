#pragma once

#include <types.h>

class soModuleAccesser;

// The stamina block of the first extend parameter set of the Pokemon (offsets from the stamina code).
// HYPOTHESIS: field roles follow how the module uses them.
struct ftPokemonStaminaParam {
    float m_max;          // 0x00 stamina at full health (before the trainer's bonus)
    float m_drain;        // 0x04 lost every 60 frames while the Pokemon is out
    float m_attackCost;   // 0x08 lost per attack
    float m_regen;        // 0x0C regained every 60 frames while resting
    float m_minPower;     // 0x10 attack power multiplier at zero stamina
    float m_tiredBelow;   // 0x14 below this the attacks weaken
    float m_deadKeep;     // 0x18 share of the lost stamina that a stock loss keeps lost
    u8 unk1C[0x24 - 0x1C];
    float unk24;          // 0x24
};

// Pokemon Trainer stamina (only while g_ftPokemonStaminaSystem is on): drains while the Pokemon is out, regenerates while it
// rests, and weakens its attacks when tired.
class ftPokemonStaminaModule {
    soModuleAccesser* m_moduleAccesser;   // 0x00
    const ftPokemonStaminaParam* m_param; // 0x04
    int m_frame;                          // 0x08 frame counter, wraps every 60 frames
public:
    // HYPOTHESIS: unkC / unk14 are copied into the work floats 0x11000013 / 0x11000014 at the start.
    float unkC;  // 0x0C current stamina
    float unk10; // 0x10 maximum stamina
    float unk14; // 0x14
    ftPokemonStaminaModule(const void* extendParam, soModuleAccesser* moduleAccesser);
    ~ftPokemonStaminaModule() { }
    void activate(float bonus);
    void initStamina();
    void decStaminaUpdate();
    void decStaminaAttack();
    void incStaminaDead();
    void update();
};

extern u8 g_ftPokemonStaminaSystem;
