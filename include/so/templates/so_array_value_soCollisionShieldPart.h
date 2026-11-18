#pragma once

#include <StaticAssert.h>
#include <types.h>

// Local copy declaration for array instantiations that only need the type's size; the full class is
// include/so/collision/so_collision_shield_part.h. SDK unchanged.
class soCollisionShieldPart {
public:
    u8 unk0[0x60];
    soCollisionShieldPart();
    ~soCollisionShieldPart();
};
static_assert(sizeof(soCollisionShieldPart) == 0x60, "Class is wrong size!");
