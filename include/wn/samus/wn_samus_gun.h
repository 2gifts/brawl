#pragma once

#include <wn/wn_weapon_builder.h>
// BYTES: the sora_melee unit mode of the module accesser builder is shared with the Light Bow (wn/zelda/wn_zelda_lighting_bow.h).
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>

// HYPOTHESIS: the article data type of the class (kind 0x58); the constructor reads the resource group number from the first
// word; the rest is unreconstructed.
struct wnSamusGunData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
};
struct wnArticleBuildInfo;

class wnSamusGunModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 5, 1, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<8, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 1, 0, 0, 2, 1, 8, 0, 0, 2, 1, 8, 0, 0, 2, 1, 8, 0, 0, 2> AnimCmdModuleSubBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnSamusGunModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSamusGunModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

// The gun article: the generic weapon builder with nothing added (the extent is the builder's 0x1834).
class wnSamusGun : public wnWeaponBuilder<wnSamusGunModuleAccesserBuildConfig> {
public:
    wnSamusGun(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGunData* data);
    virtual ~wnSamusGun() { }
    // HYPOTHESIS: unk1..unk4 are the resource ids of the activation descriptor (+0x4/+0x8, +0xC, +0x10, +0x14).
    void activate(int founderTaskId, int unk1, int unk2, int unk3, int unk4, int team);
};
