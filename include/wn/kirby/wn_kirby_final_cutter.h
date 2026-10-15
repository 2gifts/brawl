#pragma once

// Kirby's Final Cutter article (article kind 7): the cutter blade that is thrown up and comes back down.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the article (3 floats and 1 int).
// HYPOTHESIS: meaning of the parameters.
struct wnKirbyFinalCutterParam {
    float unk0;
    float unk4;
    s32 unk8;
    float unkC;
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnKirbyFinalCutterData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnKirbyFinalCutterParam* m_param; // +0x34
};

class wnKirbyFinalCutterParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 1, 0, 0> {
public:
    wnKirbyFinalCutterParamAccesser(const wnKirbyFinalCutterParam* param) : wnExtendParamAccesser<3999, 3, 23999, 1, 0, 0>(7) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unkC;
        m_ints[0] = &param->unk8;
    }
    virtual ~wnKirbyFinalCutterParamAccesser() { }
};

class wnKirbyFinalCutterModuleAccesserBuildConfig : public wnCommonBuildConfig {
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
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnKirbyFinalCutter : public wnWeaponBuilder<wnKirbyFinalCutterModuleAccesserBuildConfig> {
public:
    const wnKirbyFinalCutterParam* m_param; // +0x1FA0
private:
    wnKirbyFinalCutterParamAccesser m_paramAccesser; // +0x1FA4
public:
    wnKirbyFinalCutter(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyFinalCutterData* data);
    virtual ~wnKirbyFinalCutter() { }
    // HYPOTHESIS: argument meanings.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float angle);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool hop();
};
