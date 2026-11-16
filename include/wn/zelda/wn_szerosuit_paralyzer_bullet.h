#pragma once
// wnSZerosuitParalyzerBullet: the shot of Zero Suit Samus's Paralyzer (article kind 64). The class is built in
// sora_melee; units without WN_WEAPON_BUILDER (the transactor) see the call-side view at the end of this file.

#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;

// The parameter block of the shot: six floats (ids 4000-4005) and two ints (ids 24000-24001).
// HYPOTHESIS: meaning of the values other than the two horizontal shot speeds.
struct wnSZerosuitParalyzerBulletParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float m_speed;          // +0x10: horizontal speed of a partly charged shot
    float m_fullChargeSpeed; // +0x14: horizontal speed of a fully charged shot
    s32 unk18;
    s32 unk1C;
};

// Only the article's own unit (the sora_melee unit mode of wn/zelda/wn_zelda_lighting_bow.h) builds it; the fighter module
// (WN_WEAPON_BUILDER through ft_szerosuit_article_pools.h) keeps the opaque view.
#if defined(WN_WEAPON_BUILDER) && defined(WN_ZELDA_LIGHTING_BOW_UNIT)
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>
#include <wn/zelda/wn_szerosuit_status_builder.h>

// HYPOTHESIS: layout of the untouched part of the static article data (as the other articles' data blocks).
struct wnSZerosuitParalyzerBulletData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSZerosuitParalyzerBulletParam* m_param; // +0x34
};

class wnSZerosuitParalyzerBulletParamAccesser : public wnExtendParamAccesser<3999, 6, 23999, 2, 0, 0> {
public:
    wnSZerosuitParalyzerBulletParamAccesser(const wnSZerosuitParalyzerBulletParam* param) :
        wnExtendParamAccesser<3999, 6, 23999, 2, 0, 0>(64) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->m_speed;
        m_floats[5] = &param->m_fullChargeSpeed;
        m_ints[0] = &param->unk18;
        m_ints[1] = &param->unk1C;
    }
    virtual ~wnSZerosuitParalyzerBulletParamAccesser(); // BYTES: defined in the article unit (emission order)
};
static_assert(sizeof(wnSZerosuitParalyzerBulletParamAccesser) == 0x48, "Zero Suit paralyzer bullet parameter accesser extent");

// The Samus laser's configuration (wn/samus/wn_samus_laser.h) with a one-node model, no sound module, the common
// visibility and glow modules (template arguments from the link map names of the builders the unit keeps or calls).
class wnSZerosuitParalyzerBulletModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<1, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soSoundModuleBuildConfigNull SoundModuleBuildConfig;
    typedef wnStatusModuleBuildConfigNoUniq<2, soGeneralWorkBuildConfig<2, 2, 1>, 2, soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 2, 0, 0, 2, 1, 2, 0, 0, 1, 1, 2, 0, 0, 1, 1, 2, 0, 0, 1> AnimCmdModuleSubBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnSZerosuitParalyzerBulletModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSZerosuitParalyzerBulletModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnSZerosuitParalyzerBullet : public wnWeaponBuilder<wnSZerosuitParalyzerBulletModuleAccesserBuildConfig> {
    const wnSZerosuitParalyzerBulletParam* m_param;           // +0x17A0
    wnSZerosuitParalyzerBulletParamAccesser m_paramAccesser; // +0x17A4
public:
    wnSZerosuitParalyzerBullet(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitParalyzerBulletData* data);
#else

struct wnSZerosuitParalyzerBulletData;

// Call-side view: keeps the native extent without instantiating the builder.
class wnSZerosuitParalyzerBullet : public Weapon {
    u8 m_storage[0x17EC - sizeof(Weapon)];
public:
    wnSZerosuitParalyzerBullet(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitParalyzerBulletData* data);
#endif
    virtual ~wnSZerosuitParalyzerBullet();
    void activate(int founderTaskId, int team, const Vec3f& position, bool fullCharge, float lr);
};
static_assert(sizeof(wnSZerosuitParalyzerBullet) == 0x17EC, "Zero Suit paralyzer bullet article extent");
