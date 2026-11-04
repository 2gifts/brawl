// Local BrawlHeaders shadow: adds soEffectScreen friendship.
#pragma once

#include <StaticAssert.h>
#include <types.h>

class efScreenHandle {
    s8 unk0;
    friend class soEffectScreen;
public:
    bool isValid() const;
};
static_assert(sizeof(efScreenHandle) == 1, "Class is the wrong size!");
