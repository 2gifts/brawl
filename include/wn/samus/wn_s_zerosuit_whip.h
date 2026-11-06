#pragma once

// Zero Suit Samus's whip (article kind 0x41): the plasma whip of her side special. HYPOTHESIS: all field names; offsets follow
// the native code.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);
WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(9);

// HYPOTHESIS: meaning of the three parameters (ids 4000..).
struct wnSZerosuitWhipParam {
    float unk0, unk4, unk8;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnSZerosuitWhipData {
    s32 m_resGroupNo; // The constructor reads the low byte of this first word.
    u8 unk4[0x30];
    const wnSZerosuitWhipParam* m_param; // +0x34
};

class wnSZerosuitWhipParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0> {
public:
    wnSZerosuitWhipParamAccesser(const wnSZerosuitWhipParam* param) : wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>(0x41) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
    }
    virtual ~wnSZerosuitWhipParamAccesser() { }
};

// The four command threads of the whip: the stack capacity of every thread is 2 and threads 1 to 3 hold 15 commands.
template <int E0, int C0, int A0, int B0, int S0, int E1, int C1, int A1, int B1, int S1, int E2, int C2, int A2, int B2, int S2, int E3,
          int C3, int A3, int B3, int S3>
class wnAnimCmdModuleSubBuildConfigFullProperty {
public:
    typedef soAnimCmdControlUnitBuildConfig<0, 1, C0, A0, B0, 0, 0, S0> Unit0Config;
    typedef soAnimCmdControlUnitBuildConfig<1, 2, C1, A1, B1, 1, 0, S1> Unit1Config;
    typedef soAnimCmdControlUnitBuildConfig<2, 2, C2, A2, B2, 1, 0, S2> Unit2Config;
    typedef soAnimCmdControlUnitBuildConfig<3, 2, C3, A3, B3, 1, 0, S3> Unit3Config;
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 2> Unit4Config;
    enum { Unit0Enabled = E0, Unit1Enabled = E1, Unit2Enabled = E2, Unit3Enabled = E3, Unit4Enabled = 1 };
};

struct wnSZerosuitWhipModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<15, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 4, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soPhysicsModuleBuildConfigCap<0, soPhysicsModuleImpl> PhysicsModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<14, 9, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<3, 3, 1>, 9,
                                      soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 9, 0, 0, 2, 1, 15, 0, 0, 2, 1, 15, 0, 0, 2, 1, 15, 0, 0, 2>
        AnimCmdModuleSubBuildConfig;
};

class wnSZerosuitWhip : public wnWeaponBuilder<wnSZerosuitWhipModuleAccesserBuildConfig> {
public:
    const wnSZerosuitWhipParam* m_param;
    wnSZerosuitWhipParamAccesser m_paramAccesser;

    wnSZerosuitWhip(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitWhipData* data);
    virtual ~wnSZerosuitWhip() { }
    void activate(int founderTaskId, u32 etcResId, int team, float lr, Vec3f* pos, int isSpecial, int isAerial, int logGroup);
    virtual void deactivate(bool);
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);
    virtual void processFixPosition();
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32);
};
