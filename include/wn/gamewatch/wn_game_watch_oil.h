#pragma once

#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

struct wnGameWatchOilParam {
    float unk0;
};

struct wnGameWatchOilData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnGameWatchOilParam* m_param;
};

class wnGameWatchOilParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    explicit wnGameWatchOilParamAccesser(const wnGameWatchOilParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(121) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnGameWatchOilParamAccesser() { }
};
static_assert(sizeof(wnGameWatchOilParamAccesser) == 0x2C, "Oil parameter accesser size");

class wnGameWatchOilModuleAccesserBuildConfig : public wnCommonBuildConfig {
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

class wnGameWatchOil : public wnWeaponBuilder<wnGameWatchOilModuleAccesserBuildConfig> {
    const wnGameWatchOilParam* m_param;
    wnGameWatchOilParamAccesser m_paramAccesser;
    soAnimObj m_animObj;
public:
    wnGameWatchOil(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchOilData* data);
    virtual ~wnGameWatchOil() { }
    void activate(int founderTaskId, int resourceId, int team, float color);
    void deactivate(bool force);
};
