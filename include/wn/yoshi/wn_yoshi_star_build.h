#pragma once

// wnYoshiStar article (article kind 0x3b) with its module accesser build configuration, for the unit that constructs it.
// HYPOTHESIS: the module counts of the build configuration below are guesses refined against the original object.
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

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

// HYPOTHESIS: layout of the untouched part.
struct wnYoshiStarData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    wnYoshiStarParam* m_param; // +0x34
};

class wnYoshiStarParamAccesser : public wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0> {
public:
    wnYoshiStarParamAccesser(const wnYoshiStarParam* param) : wnExtendParamAccesser<3999, 5, 23999, 0, 0, 0>(0x3b) {
        m_floats[0] = &param->horizontalSpeed;
        m_floats[1] = &param->horizontalAcceleration;
        m_floats[2] = &param->verticalSpeed;
        m_floats[3] = &param->verticalSpeedLimit;
        m_floats[4] = &param->gravity;
    }
    virtual ~wnYoshiStarParamAccesser() { }
};

class wnYoshiStarModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

static_assert(sizeof(wnWeaponBuilder<wnYoshiStarModuleAccesserBuildConfig>) == 0x1F9C, "builder size");
class wnYoshiStar : public wnWeaponBuilder<wnYoshiStarModuleAccesserBuildConfig> {
    wnYoshiStarParam* m_param;
    wnYoshiStarParamAccesser m_paramAccesser;
public:
    wnYoshiStar(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiStarData* data);
    virtual ~wnYoshiStar() { }
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool notifyEventAnimCmd(acAnimCmd*, soModuleAccesser*, int);
    // HYPOTHESIS: relative source ordering of lr. Native registers establish
    // founder r4, team r5, position r6, and facing f1 independently.
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
};
// HYPOTHESIS: the original reads the accesser tables at +0x1C/+0x34/+0x38, so the generic accesser extent (0x3C) applies; the
// opaque declaration in wn_yoshi_star.h states 0x1FD0.
static_assert(sizeof(wnYoshiStar) == 0x1FDC, "Star article layout");
