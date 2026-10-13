#pragma once

#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

struct wnGameWatchRescueParam {
    float unk0;
};

struct wnGameWatchRescueData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnGameWatchRescueParam* m_param;
};

class wnGameWatchRescueParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    explicit wnGameWatchRescueParamAccesser(const wnGameWatchRescueParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(118) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnGameWatchRescueParamAccesser() { }
};
static_assert(sizeof(wnGameWatchRescueParamAccesser) == 0x2C, "Rescue parameter accesser size");

class wnGameWatchRescueModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

class wnGameWatchRescue : public wnWeaponBuilder<wnGameWatchRescueModuleAccesserBuildConfig> {
    const wnGameWatchRescueParam* m_param;
    wnGameWatchRescueParamAccesser m_paramAccesser;
    soAnimObj m_animObj;
public:
    wnGameWatchRescue(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchRescueData* data);
    virtual ~wnGameWatchRescue() { }
    // HYPOTHESIS: source reference qualification; the native argument is a position pointer.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f& position, float color);
    void deactivate(bool force);
};

// Factory and holder accessors place the article immediately after the holder prefix.
static_assert(sizeof(wnWeaponBuilder<wnGameWatchRescueModuleAccesserBuildConfig>) == 0x2088, "Rescue builder extent");
static_assert(sizeof(wnGameWatchRescue) == 0x20E0, "Rescue object extent");
