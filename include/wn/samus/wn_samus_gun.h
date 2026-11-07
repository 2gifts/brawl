#pragma once

#include <so/so_value_accesser.h>
#include <wn/wn_module_accesser_builder.h>
// After the weapon builder scaffolding (its fighter chain defines the helpers the builder headers use).
#include <ft/builder/ft_module_accesser_builder.h>

// HYPOTHESIS: the article data type of the class (kind 0x58); the constructor reads the resource group number from the first
// word; the rest is unreconstructed.
struct wnSamusGunData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
};
struct wnArticleBuildInfo;

// Four command threads (each: enabled, table capacity, two unknown values, one more property) and the uniq interpreter thread 4.
// HYPOTHESIS: meaning of the per-thread values beyond the command table capacity; the fifth property is 2 in every thread here
// and is kept only to spell the original configuration (the stacks keep the shared capacity 8).
template <int E0, int C0, int A0, int B0, int P0, int E1, int C1, int A1, int B1, int P1, int E2, int C2, int A2, int B2, int P2, int E3,
          int C3, int A3, int B3, int P3>
class wnAnimCmdModuleSubBuildConfigFullProperty {
public:
    typedef soAnimCmdControlUnitBuildConfig<0, 1, C0, A0, B0, 0, 0, 8> Unit0Config;
    typedef soAnimCmdControlUnitBuildConfig<1, 2, C1, A1, B1, 1, 0, 8> Unit1Config;
    typedef soAnimCmdControlUnitBuildConfig<2, 2, C2, A2, B2, 1, 0, 8> Unit2Config;
    typedef soAnimCmdControlUnitBuildConfig<3, 2, C3, A3, B3, 1, 0, 8> Unit3Config;
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 8> Unit4Config;
    enum { Unit0Enabled = E0, Unit1Enabled = E1, Unit2Enabled = E2, Unit3Enabled = E3, Unit4Enabled = 1 };
};

class wnSamusGunModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 5, 1, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnTurnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnTurnInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<8, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 1, 0, 0, 2, 1, 8, 0, 0, 2, 1, 8, 0, 0, 2, 1, 8, 0, 0, 2> AnimCmdModuleSubBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
};

// The gun article: the generic weapon builder with nothing added (the extent is the builder's 0x1834).
class wnSamusGun : public wnWeaponBuilder<wnSamusGunModuleAccesserBuildConfig> {
public:
    wnSamusGun(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGunData* data);
    virtual ~wnSamusGun() { }
    // HYPOTHESIS: unk1..unk4 are the resource ids of the activation descriptor (+0x4/+0x8, +0xC, +0x10, +0x14).
    void activate(int founderTaskId, int unk1, int unk2, int unk3, int unk4, int team);
};
