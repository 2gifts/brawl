#pragma once

#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

struct wnGameWatchParachuteParam {
    float unk0;
};

struct wnGameWatchParachuteData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnGameWatchParachuteParam* m_param;
};

class wnGameWatchParachuteParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    explicit wnGameWatchParachuteParamAccesser(const wnGameWatchParachuteParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(119) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnGameWatchParachuteParamAccesser() { }
};
static_assert(sizeof(wnGameWatchParachuteParamAccesser) == 0x2C, "Parachute parameter accesser size");

class wnGameWatchParachuteModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<5, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
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
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

class wnGameWatchParachute : public wnWeaponBuilder<wnGameWatchParachuteModuleAccesserBuildConfig> {
    const wnGameWatchParachuteParam* m_param;
    wnGameWatchParachuteParamAccesser m_paramAccesser;
    soAnimObj m_animObj;
public:
    wnGameWatchParachute(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchParachuteData* data);
    virtual ~wnGameWatchParachute() { }
    void activate(int founderTaskId, int resourceId, int team, float frame);
    void deactivate(bool force);
};

// Factory and holder accessors place the article immediately after the holder prefix.
static_assert(sizeof(wnWeaponBuilder<wnGameWatchParachuteModuleAccesserBuildConfig>) == 0x2088, "Parachute builder extent");
static_assert(sizeof(wnGameWatchParachute) == 0x20E0, "Parachute object extent");
