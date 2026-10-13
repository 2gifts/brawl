#pragma once

#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

struct wnGameWatchPanelParam {
    float unk0;
};

struct wnGameWatchPanelData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnGameWatchPanelParam* m_param;
};

class wnGameWatchPanelParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    explicit wnGameWatchPanelParamAccesser(const wnGameWatchPanelParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(120) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnGameWatchPanelParamAccesser() { }
};
static_assert(sizeof(wnGameWatchPanelParamAccesser) == 0x2C, "Panel parameter accesser size");

class wnGameWatchPanelModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

class wnGameWatchPanel : public wnWeaponBuilder<wnGameWatchPanelModuleAccesserBuildConfig> {
    const wnGameWatchPanelParam* m_param;
    wnGameWatchPanelParamAccesser m_paramAccesser;
    soAnimObj m_animObj;
public:
    wnGameWatchPanel(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchPanelData* data);
    virtual ~wnGameWatchPanel() { }
    // HYPOTHESIS: source argument order for the interleaved integer/float parameters.
    void activate(int founderTaskId, int resourceId, int team, float lr, int selector, float frame);
    virtual void updateNodeSRT();
    void deactivate(bool force);
};

// The enclosing instance holder ends immediately after the animation object.
static_assert(sizeof(wnWeaponBuilder<wnGameWatchPanelModuleAccesserBuildConfig>) == 0x20A4, "Panel builder extent");
static_assert(sizeof(wnGameWatchPanel) == 0x20FC, "Panel object extent");
