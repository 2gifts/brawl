#pragma once

#include <wn/wn_weapon_builder.h>

struct wnMarioMantleData;

// The cape follows Mario's animation; collision and special-move state belong to the fighter.
class wnMarioMantleModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soPostureModuleBuildConfig<0, soPostureModuleSimple> PostureModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soAnimCmdModuleBuildConfigNull AnimCmdModuleBuildConfig;
    typedef soStatusModuleBuildConfigNull StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigNoUniq<0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

// The cape follows Mario without its own movement energies. Keep the selection
// specific to this config so the fighter can own other article types as well.
template <>
class wnWeaponBuilder<wnMarioMantleModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnMarioMantleModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in the no-common-energies builder.
    u32 m_modulesPad;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), weaponFlag), m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};

class wnMarioMantle : public wnWeaponBuilder<wnMarioMantleModuleAccesserBuildConfig> {
public:
    wnMarioMantle(s32 articleId, const wnArticleBuildInfo& info, const wnMarioMantleData* data);
    virtual ~wnMarioMantle() { }
    void activate(int founderTaskId, int resourceId, int team, int situationKind);
};

static_assert(sizeof(wnMarioMantle) == 0x1358, "Mario cape article layout");
