#pragma once

#include <wn/wn_weapon_builder.h>

// Static article data for kind 60; only the constructor-read fields are exposed.
struct wnYoshiTamagoData;
struct wnArticleBuildInfo;

// Partial parameter record reached through article data +0x34. Launch physics
// reads these floats; the first two words remain unreconstructed.
struct wnYoshiTamagoParam {
    s32 unk0;
    s32 unk4;
    float spinPercent;
    float gravity;
    float speedLimitY;
};

#ifdef WN_WEAPON_BUILDER
#include <wn/wn_extend_param_accesser.h>

struct wnYoshiTamagoData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnYoshiTamagoParam* m_param; // +0x34
};

class wnYoshiTamagoParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 2, 0, 0> {
public:
    wnYoshiTamagoParamAccesser(const wnYoshiTamagoParam* param) :
        wnExtendParamAccesser<3999, 3, 23999, 2, 0, 0>(60) {
        m_floats[0] = &param->spinPercent;
        m_floats[1] = &param->gravity;
        m_floats[2] = &param->speedLimitY;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnYoshiTamagoParamAccesser() { }
};
static_assert(sizeof(wnYoshiTamagoParamAccesser) == 0x3c, "Egg parameter accessor extent");

class wnYoshiTamagoModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
        wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0,
        soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
        soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0,
        soCollisionAttackModuleImpl, 1, false, true> CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2,
        soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};
static_assert(sizeof(wnWeaponBuilder<wnYoshiTamagoModuleAccesserBuildConfig>) == 0x1fb8, "Egg builder extent");
class wnYoshiTamago : public wnWeaponBuilder<wnYoshiTamagoModuleAccesserBuildConfig> {
    const wnYoshiTamagoParam* m_param;
    wnYoshiTamagoParamAccesser m_paramAccesser;
#else
// Call-side view retains the native extent without instantiating the builder.
class wnYoshiTamago : public Weapon {
    u8 m_storage[0x1FB8 - sizeof(Weapon)];
    const wnYoshiTamagoParam* m_param;
    u8 unk1FBC[0x1FF8 - 0x1FBC];
#endif
public:
    // The article unit constructs the modules; call-side views keep their native extent.
    wnYoshiTamago(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiTamagoData* data);
#ifdef WN_WEAPON_BUILDER
    virtual ~wnYoshiTamago() { }
#else
    virtual ~wnYoshiTamago();
#endif
    // captureNode is the captured fighter's corrected model node, or -1 when
    // this egg is being spawned for Egg Throw rather than around a victim.
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position,
                  u32 captureNode, float lr);
    void shoot(const Vec3f* position, const Vec2f* speed, float spinFactor);
    virtual void onDeactivate();
    virtual void notifyEventLink(soLinkEventArgs*, soModuleAccesser*, StageObject*, int);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnYoshiTamago) == 0x1FF8, "Yoshi egg article extent");
