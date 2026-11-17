#pragma once
// wnSamusLaser2: the second beam article of Samus's final smash (article kind 0x57), without movement energies. The class is built in sora_melee; the fighter module
// sees the call-side view below.
#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;

// The static data block of the article. HYPOTHESIS: layout of the untouched part and the meaning of the value.
struct wnSamusLaser2Data {
    s32 m_resGroupNo; // the constructor reads the low byte of this first word
    u8 unk4[0x30];
    int* m_value; // +0x34: copied to work int 0x10000005 by the status process
};

#if defined(WN_WEAPON_BUILDER) && defined(WN_MARIO_FIREBALL_UNIT) // the article unit (a fighter unit with builders sees the call-side view)
// BYTES: the sora_melee unit mode of the module accesser builder is shared with the Light Bow (wn/zelda/wn_zelda_lighting_bow.h).
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>

class wnSamusLaser2ModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig; // no movement energies
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 2, 0, 0, 2, 1, 2, 0, 0, 2, 1, 2, 0, 0, 2, 1, 2, 0, 0, 2> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnSamusLaser2ModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSamusLaser2ModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnSamusLaser2 : public wnWeaponBuilder<wnSamusLaser2ModuleAccesserBuildConfig> {
#else
// Call-side view keeps the native extent without instantiating the builder.
class wnSamusLaser2 : public Weapon {
    u8 m_storage[0x17C8 - sizeof(Weapon)];
#endif
public:
    wnSamusLaser2(s32 articleId, const wnArticleBuildInfo& info, const wnSamusLaser2Data* data);
#if defined(WN_WEAPON_BUILDER) && defined(WN_MARIO_FIREBALL_UNIT)
    virtual ~wnSamusLaser2() { }
#else
    virtual ~wnSamusLaser2();
#endif
    // HYPOTHESIS: the float is the height offset kept in work float 0x11000001.
    void activate(int founderTaskId, int etcResourceId, int team, float height);
    const wnSamusLaser2Data* getData();
    void toShoot(float lr);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnSamusLaser2) == 0x17C8, "Samus second laser article extent");
