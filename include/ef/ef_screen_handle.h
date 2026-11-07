// Local BrawlHeaders shadow: adds soEffectScreen and efScreen friendship.
#pragma once

#include <StaticAssert.h>
#include <types.h>

class efScreenHandle {
    s8 unk0;
    friend class soEffectScreen;
    friend class efScreen;
public:
#ifdef WN_MARIO_FIREBALL_UNIT
    efScreenHandle() : unk0(-1) { }
#endif
    bool isValid() const;
};
static_assert(sizeof(efScreenHandle) == 1, "Class is the wrong size!");
