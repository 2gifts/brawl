#pragma once
// wnSamusCShot: Samus's Charge Shot (article kind 0x53; Kirby's copy uses the same article). The class is built in
// sora_melee; the fighter module and the transactor see the call-side view below.
#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;

// HYPOTHESIS: the parameter block of the charge shot (read by the shot's status process).
struct wnSamusCShotParam {
    s32 unk0; // HYPOTHESIS: the life set into work int 0x10000005 when the shot is held
    float unk4; // HYPOTHESIS: the launch angle
    float unk8; // HYPOTHESIS: the speed at no charge
    float unkC; // HYPOTHESIS: the speed at full charge
    float unk10;
    float unk14;
    float unk18; // HYPOTHESIS: the effect scale at no charge
    float unk1C; // HYPOTHESIS: the effect scale at full charge
};

// The static data block of the article. HYPOTHESIS: layout of the untouched part.
struct wnSamusCShotData {
    s32 m_resGroupNo; // the constructor reads the low byte of this first word
    u8 unk4[0x30];
    const wnSamusCShotParam* m_param; // +0x34
};

#ifdef WN_WEAPON_BUILDER
// BYTES: the sora_melee unit mode of the module accesser builder is shared with the Light Bow (wn/zelda/wn_zelda_lighting_bow.h).
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>

class wnSamusCShotModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<1, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<9, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<9, soGeneralWorkBuildConfig<2, 2, 1>, 9, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 9, 0, 0, 2, 1, 9, 0, 0, 2, 1, 9, 0, 0, 2, 1, 9, 0, 0, 2> AnimCmdModuleSubBuildConfig;
    typedef soGeneralWorkBuildConfig<11, 8, 1> GeneralWorkBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig; // two energies
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnSamusCShotModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSamusCShotModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnSamusCShot : public wnWeaponBuilder<wnSamusCShotModuleAccesserBuildConfig> {
#else
// Call-side view keeps the native extent without instantiating the builder.
// Call-side view keeps the native extent without instantiating the builder.
class wnSamusCShot : public Weapon {
    u8 m_storage[0x1828 - sizeof(Weapon)];
#endif
public:
    wnSamusCShot(s32 articleId, const wnArticleBuildInfo& info, const wnSamusCShotData* data);
#ifdef WN_WEAPON_BUILDER
    virtual ~wnSamusCShot() { }
#else
    virtual ~wnSamusCShot();
#endif
    void activate(int founderTaskId, int etcResourceId, int team, int kind);
    void setCharge(float charge);
    void toShoot(int chargeFrame, float lr);
    const wnSamusCShotData* getData();
};
static_assert(sizeof(wnSamusCShot) == 0x1828, "Samus charge shot article extent");
