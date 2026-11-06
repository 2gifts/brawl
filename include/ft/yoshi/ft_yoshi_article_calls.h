#pragma once

#include <StaticAssert.h>
#include <wn/weapon.h>
#include <wn/wn_article_build_info.h>

struct wnYoshiFireBreathData;
struct wnYoshiFireCannonData;

// Fighter-side views used only for nonvirtual activation calls and, in ft_yoshi.cpp, for building the
// article holders in place. The article units retain their full builder declarations.
// Extents agree with the owning constructors and their compiled builder types.
class wnYoshiFireBreath : public Weapon {
    u8 unkTail[0x1F30 - sizeof(Weapon)];
public:
    wnYoshiFireBreath(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiFireBreathData* data);
    virtual ~wnYoshiFireBreath();
    void activate(int founderTaskId, int team, const Vec3f* position, float lr, float angle);
};
static_assert(sizeof(wnYoshiFireBreath) == 0x1F30, "Fire breath article extent");

class wnYoshiFireCannon : public Weapon {
    u8 unkTail[0x1FD8 - sizeof(Weapon)];
public:
    wnYoshiFireCannon(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiFireCannonData* data);
    virtual ~wnYoshiFireCannon();
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float angle);
};
static_assert(sizeof(wnYoshiFireCannon) == 0x1FD8, "Fire cannon article extent");
