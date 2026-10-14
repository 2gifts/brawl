// Local BrawlHeaders shadow: expose the fighter collision-hit accessor.
#pragma once

#include <StaticAssert.h>
#include <ft/fighter.h>
#include <types.h>

namespace ftExternalValueAccesser {
    soCollisionHitModule* getsoCollisionHitModule(Fighter* fighter);
    float getWeight(Fighter* fighter);
    Vec3f getHipPos(Fighter* fighter);
    Vec3f getCursorPos(Fighter* fighter);
    int getNoDiscretionFinalCount();
    // HYPOTHESIS: maps a hit number of the attack module to the model node it hits (used by the Pikmin chain-up).
    int getNodeIdFromHitNo(u8 hitNo);
}