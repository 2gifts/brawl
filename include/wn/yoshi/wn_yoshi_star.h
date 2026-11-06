#pragma once

#include <wn/wn_weapon_builder.h>
#include <wn/wn_article_build_info.h>

struct wnYoshiStarData;

struct wnYoshiStarModuleAccesserBuildConfig;
struct wnYoshiStarParam {
    // Activation multiplies the horizontal components by the spawn facing,
    // negates gravity, and preserves the energy's existing horizontal limit.
    float horizontalSpeed;
    float horizontalAcceleration;
    float verticalSpeed;
    float verticalSpeedLimit;
    float gravity;
};
static_assert(sizeof(wnYoshiStarParam) == 0x14, "Star float parameter record");

// Original RTTI identifies the natural builder base. Its embedded modules and
// kinetic energies remain opaque while the constructor is unresolved.
template <>
class wnWeaponBuilder<wnYoshiStarModuleAccesserBuildConfig> : public Weapon {
    u8 unkD0[0x1F9C - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnYoshiStarModuleAccesserBuildConfig>) == 0x1F9C, "Star builder extent");

class wnYoshiStar : public wnWeaponBuilder<wnYoshiStarModuleAccesserBuildConfig> {
    wnYoshiStarParam* m_param;
    u8 unk1FA0[0x3C]; // Embedded parameter accesser; the article holders of ft_yoshi.cpp establish the 0x1FDC extent.
public:
    // HYPOTHESIS: the constructor is the same (id, build info, data) form as the other articles; its body is not in this tree.
    wnYoshiStar(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiStarData* data);
    virtual ~wnYoshiStar();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool notifyEventAnimCmd(acAnimCmd*, soModuleAccesser*, int);
    // HYPOTHESIS: relative source ordering of lr. Native registers establish
    // founder r4, team r5, position r6, and facing f1 independently.
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
};
static_assert(sizeof(wnYoshiStar) == 0x1FDC, "Star article layout");
