#pragma once

// Minimal view of the Subspace Emissary stage class (sora_adv_stage's stAdventure2). Only what the gimmick units need;
// the full class is not reconstructed yet. HYPOTHESIS: the member names.

#include <st/stage.h>
#include <types.h>

class stAdventure2 : public Stage {
    u8 m_unreconstructed[0x554 - sizeof(Stage)];

public:
    // 0x554, five rows of four floats indexed by background layer: x scale, y scale, x/y/z offset
    // (grAdventureBackGround::setBackGround).
    const float* m_backGroundParams;
};
