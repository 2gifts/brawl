#pragma once
// wnKoopaBreath: Bowser's fire breath article (article kind 0x73). The class lives in sora_melee; Bowser's fighter module
// keeps weak copies of its destructor chain. Giga Bowser's breath (wn/koopa/wn_g_koopa_breath.h) is the same code.
#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
// The parameter block of the article.
struct wnKoopaBreathParam {
    float m_groundOffsetY; // +0x00: ground collision offset
    int m_work5;           // +0x04: HYPOTHESIS: copied to work int 0x10000005 on activation
    int m_work6;           // +0x08: HYPOTHESIS: copied to work int 0x10000006 on activation
    float m_speedRandMin;  // +0x0C: the launch speed is scaled by a random factor in [min, max]
    float m_speedRandMax;  // +0x10
};
// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnKoopaBreathData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnKoopaBreathParam* m_param; // +0x34
};
#ifdef WN_WEAPON_BUILDER
class wnKoopaBreathModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<8, 2, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
};

#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
template <>
class soModuleAccesserBuilder<wnKoopaBreathModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnKoopaBreathModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};
#endif

class wnKoopaBreath : public wnWeaponBuilder<wnKoopaBreathModuleAccesserBuildConfig> {
#else
// Call-side view keeps the native extent without instantiating the builder.
struct wnArticleBuildInfo;
class wnKoopaBreath : public Weapon {
    u8 m_storage[0x1F7C - sizeof(Weapon)];
#endif
public:
    const wnKoopaBreathParam* m_param; // +0x1F7C
    wnKoopaBreath(s32 articleId, const wnArticleBuildInfo& info, const wnKoopaBreathData* data);
#ifdef WN_WEAPON_BUILDER
    virtual ~wnKoopaBreath() { }
#else
    virtual ~wnKoopaBreath();
#endif
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    // Starts one flame: the launch speed is speedMul * speed * random(param range) along angle (radians), facing lr.
    void activate(int founderTaskId, int team, const Vec3f* position, float lr, int kind, float speed, float scale,
                  float speedMul, float angle);
};
static_assert(sizeof(wnKoopaBreath) == 0x1F80, "wnKoopaBreath layout is wrong!");
