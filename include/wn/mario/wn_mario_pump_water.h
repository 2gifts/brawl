#pragma once
// wnMarioPumpWater article (article kind 0x48).
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(3);
typedef soTypeList<soIntToType<4>, soTypeListNullType> wnMarioPumpWaterStatusTransitionTypeList;

// Six float and two integer parameters; roles follow activation and kinetic setup.
// HYPOTHESIS: original field spelling.
struct wnMarioPumpWaterParam {
    float m_baseSpeed;
    float m_chargeSpeed;
    float m_gravity;
    float m_targetSpeedY;
    s32 m_life;
    float m_minScale;
    float m_maxScale;
    s32 m_chargeReactionAdd;
};
// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnMarioPumpWaterData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnMarioPumpWaterParam* m_param; // +0x34
};
class wnMarioPumpWaterParamAccesser : public wnExtendParamAccesser<3999, 6, 23999, 2, 0, 0> {
public:
    wnMarioPumpWaterParamAccesser(const wnMarioPumpWaterParam* param) : wnExtendParamAccesser<3999, 6, 23999, 2, 0, 0>(0x48) {
        m_floats[0] = &param->m_baseSpeed;
        m_floats[1] = &param->m_chargeSpeed;
        m_floats[2] = &param->m_gravity;
        m_floats[3] = &param->m_targetSpeedY;
        m_floats[4] = &param->m_minScale;
        m_floats[5] = &param->m_maxScale;
        m_ints[0] = &param->m_life;
        m_ints[1] = &param->m_chargeReactionAdd;
    }
    virtual ~wnMarioPumpWaterParamAccesser() { }
};
class wnMarioPumpWaterModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<1, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfigNull, 3, soTransitionModuleBuildConfig<wnMarioPumpWaterStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<6, 2, 1> GeneralWorkBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};
class wnMarioPumpWater : public wnWeaponBuilder<wnMarioPumpWaterModuleAccesserBuildConfig> {
public:
    const wnMarioPumpWaterParam* m_param;
    wnMarioPumpWaterParamAccesser m_paramAccesser;
    wnMarioPumpWater(s32 articleId, const wnArticleBuildInfo& info, const wnMarioPumpWaterData* data);
    virtual ~wnMarioPumpWater() { }
    void activate(int founderTaskId, int resourceId, int team, Vec3f* position,
                  float lr, float charge, Vec3f* direction);
    virtual void updateNodeSRT();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool reflect();
};

static_assert(sizeof(wnMarioPumpWater) == 0x1E50, "Mario water article layout");
