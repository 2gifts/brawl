#pragma once

#include <StaticAssert.h>
#include <wn/weapon.h>

// Fighter-side views used only for nonvirtual activation calls. The article
// units retain their full builder declarations; do not construct these views.
// Extents agree with the owning constructors and their compiled builder types.
class wnYoshiFireBreath : public Weapon {
    u8 unkTail[0x1F30 - sizeof(Weapon)];
public:
    void activate(int founderTaskId, int team, const Vec3f* position, float lr, float angle);
};
static_assert(sizeof(wnYoshiFireBreath) == 0x1F30, "Fire breath article extent");

class wnYoshiFireCannon : public Weapon {
    u8 unkTail[0x1FD8 - sizeof(Weapon)];
public:
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float angle);
};
static_assert(sizeof(wnYoshiFireCannon) == 0x1FD8, "Fire cannon article extent");
