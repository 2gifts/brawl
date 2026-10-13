#pragma once

#include <wn/wn_module_accesser_builder.h>

// Only the observed prefix of the article data is described here.
struct wnDiddyExplosionData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    // HYPOTHESIS: the payload type is unresolved; activation reads its first integer.
    const s32* unk34;
};
static_assert(sizeof(wnDiddyExplosionData) == 0x38, "Explosion observed data prefix");

class wnDiddyExplosionModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<1, 1, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0>
        AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};
static_assert(sizeof(wnWeaponBuilder<wnDiddyExplosionModuleAccesserBuildConfig>) == 0x1F94, "Explosion builder extent");

class wnDiddyExplosion : public wnWeaponBuilder<wnDiddyExplosionModuleAccesserBuildConfig> {
    // HYPOTHESIS: payload meaning is unresolved; its first integer supplies an activation work value.
    const s32* unk1F94;
public:
    wnDiddyExplosion(s32 articleId, const wnArticleBuildInfo& info, const wnDiddyExplosionData* data);
    virtual ~wnDiddyExplosion() { }
    void activate(int founderTaskId, int team, Vec3f* position, float lr);
    virtual void onDeactivate();
};
// The enclosing native instance holder places its next member immediately after this extent.
static_assert(sizeof(wnDiddyExplosion) == 0x1F98, "Explosion object extent");
