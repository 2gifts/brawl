#pragma once

#include <wn/wn_weapon_builder.h>
#if defined(WN_LINK_BOW_ARROW_FULL_BUILDER) && !defined(WN_MARIO_FIREBALL_UNIT) // the article unit itself expands the vector
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 3);
#endif

// HYPOTHESIS: field names from their use in the arrow's status process (wn_link_bow_arrow_status_uniq_process.cpp).
struct wnLinkBowArrowParam {
    s32 unk0;
    s32 m_holdLife;     // +0x04: life while the arrow is nocked (status 0)
    float m_speedMin;   // +0x08: flight speed without charge
    float m_speedMax;   // +0x0C: flight speed at full charge
    float m_flyLifeMin; // +0x10
    float m_flyLifeMax; // +0x14
    s32 m_stuckLife;    // +0x18: life once the arrow sticks (status 2)
    float m_gravity;    // +0x1C
};

struct wnLinkBowArrowData {
    s32 m_resGroupNo; // HYPOTHESIS: as the other articles' data blocks.
    u8 unk4[0x30];
    const wnLinkBowArrowParam* m_param; // +0x34
};
struct wnArticleBuildInfo;

#ifdef WN_LINK_BOW_ARROW_FULL_BUILDER
// HYPOTHESIS: drafted from the Pit arrow and Fox illusion configurations; only the module builders named by the
// original's builder symbols (motion, situation, stop, visibility, status, general work, anim cmd) are fitted.
class wnLinkBowArrowModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<5, 3, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
};

#ifdef WN_ZELDA_LIGHTING_BOW_UNIT
template <>
class soModuleAccesserBuilder<wnLinkBowArrowModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnLinkBowArrowModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};
#endif

class wnLinkBowArrow : public wnWeaponBuilder<wnLinkBowArrowModuleAccesserBuildConfig> {
public:
    soKineticEnergyGroundMovement m_energyGround; // +0x1FA8: kinetic energy 2, the outside ground movement
    wnLinkBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnLinkBowArrowData* data);
#ifdef FT_LINK_BOW_ARROW_DTOR_EXTERNAL
    // The fighter holder calls this. This object does not emit the destructor.
    virtual ~wnLinkBowArrow();
#else
    virtual ~wnLinkBowArrow() { }
#endif
    void activate(int founderTaskId, int resourceId, int team, float lr);
    virtual void deactivate(bool force);
    const wnLinkBowArrowData* getData();
    void toFly(float lr, float unk2, float unk1);
    void setFlyPose();
    virtual void updateNodeSRT();
    virtual bool reflect();
};
static_assert(sizeof(wnLinkBowArrow) == 0x1FD8, "Link bow arrow article extent");
#else
// Extent known from the class info createArray allocation (0x1FD8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnLinkBowArrow : public Weapon {
    u8 m_storage[0x1FD8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnLinkBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnLinkBowArrowData* data);
    virtual ~wnLinkBowArrow();
    // HYPOTHESIS: parameter names; the same activation as Toon Link's arrow (founder, resource, team, facing).
    void activate(int founderTaskId, int resourceId, int team, float lr);
    virtual void deactivate(bool force);
    const wnLinkBowArrowData* getData();
    // HYPOTHESIS: parameter names; the two floats go to work floats 0x11000002 and 0x11000001.
    void toFly(float lr, float unk2, float unk1);
    void setFlyPose();
    virtual void updateNodeSRT();
    virtual bool reflect();
};
static_assert(sizeof(wnLinkBowArrow) == 0x1FD8, "Link bow arrow article extent");
#endif
