#pragma once

#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>

struct wnGameWatchNormalWeaponParam {
    float unk0;
};

struct wnGameWatchNormalWeaponConstraintData {
    s32 unk0;
    s32 unk4;
};

struct wnGameWatchNormalWeaponData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnGameWatchNormalWeaponParam* m_param;
};

static_assert(sizeof(wnGameWatchNormalWeaponData) == 0x38, "NormalWeapon fixed data prefix");
static_assert(sizeof(wnGameWatchNormalWeaponConstraintData) == 8, "NormalWeapon constraint record");

class wnGameWatchNormalWeaponParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    explicit wnGameWatchNormalWeaponParamAccesser(const wnGameWatchNormalWeaponParam* param) :
        wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(123) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnGameWatchNormalWeaponParamAccesser() { }
};
static_assert(sizeof(wnGameWatchNormalWeaponParamAccesser) == 0x2C, "NormalWeapon parameter accesser size");

class wnGameWatchNormalWeaponModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<11, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
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
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 11, 0, 0, 1, 11, 0, 0, 1, 11, 0, 0>
        AnimCmdModuleSubBuildConfig;
};

class wnGameWatchNormalWeapon : public wnWeaponBuilder<wnGameWatchNormalWeaponModuleAccesserBuildConfig> {
    const wnGameWatchNormalWeaponParam* m_param;
    wnGameWatchNormalWeaponParamAccesser m_paramAccesser;
    soAnimObj m_animObj;
public:
    wnGameWatchNormalWeapon(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchNormalWeaponData* data);
    virtual ~wnGameWatchNormalWeapon() { }
    // HYPOTHESIS: selector and frame names follow their table-index and animation uses.
    void activate(int founderTaskId, int resourceId, int team, int selector, float frame);
    void deactivate(bool force);
};

// Factory and holder accessors place the article immediately after the holder prefix.
static_assert(sizeof(wnWeaponBuilder<wnGameWatchNormalWeaponModuleAccesserBuildConfig>) == 0x2088, "NormalWeapon builder extent");
static_assert(sizeof(wnGameWatchNormalWeapon) == 0x20E0, "NormalWeapon object extent");
