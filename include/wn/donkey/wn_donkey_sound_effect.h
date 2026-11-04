#pragma once

// The musical-note article emitted by Donkey Kong's Konga Beat (article kind 0x72).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(2);

// The parameter block of the article (1 int and 4 floats, id 4000).
// HYPOTHESIS: meaning of the parameters.
struct wnDonkeySoundEffectParam {
    s32 unk0;
    float missSpeed;
    float hitSpeed;
    float scaleGrowthTime;
    float targetScaleMul;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnDonkeySoundEffectData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnDonkeySoundEffectParam* m_param; // +0x34
};

class wnDonkeySoundEffectParamAccesser : public wnExtendParamAccesser<3999, 4, 23999, 1, 0, 0> {
public:
    wnDonkeySoundEffectParamAccesser(const wnDonkeySoundEffectParam* param) : wnExtendParamAccesser<3999, 4, 23999, 1, 0, 0>(0x72) {
        m_floats[0] = &param->missSpeed;
        m_floats[1] = &param->hitSpeed;
        m_floats[2] = &param->scaleGrowthTime;
        m_floats[3] = &param->targetScaleMul;
        m_ints[0] = &param->unk0;
    }
    virtual ~wnDonkeySoundEffectParamAccesser() { }
};

class wnDonkeySoundEffectModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 2, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

class wnDonkeySoundEffect : public wnWeaponBuilder<wnDonkeySoundEffectModuleAccesserBuildConfig> {
public:
    const wnDonkeySoundEffectParam* m_param; // +0x1F8C
private:
    wnDonkeySoundEffectParamAccesser m_paramAccesser; // +0x1F90
public:
    wnDonkeySoundEffect(s32 articleId, const wnArticleBuildInfo& info, const wnDonkeySoundEffectData* data);
    virtual ~wnDonkeySoundEffect() { }
    void activate(int founderTaskId, int team, Vec3f* position, float angle,
                  float powerMul, float speedMul, float scale, bool goodStrike);
    virtual void updateNodeSRT();
    virtual bool notifyEventCollisionAttackCheck(u32) { return false; }
};
