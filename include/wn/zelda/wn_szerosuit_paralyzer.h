#pragma once
// wnSZerosuitParalyzer: Zero Suit Samus's Paralyzer pistol in the hand (article kind 98; the shots are
// wn/zelda/wn_szerosuit_paralyzer_bullet.h). The class is built in sora_melee; units without WN_WEAPON_BUILDER (the
// transactor) see the call-side view at the end of this file.

#include <wn/wn_weapon_builder.h>

struct wnArticleBuildInfo;

// Only the article's own unit (the sora_melee unit mode of wn/zelda/wn_zelda_lighting_bow.h) builds it; the fighter module
// (WN_WEAPON_BUILDER through ft_szerosuit_article_pools.h) keeps the opaque view.
#if defined(WN_WEAPON_BUILDER) && defined(WN_ZELDA_LIGHTING_BOW_UNIT)
#include <wn/wn_extend_param_accesser.h>
#include <wn/zelda/wn_szerosuit_anim_cmd.h>
#include <wn/zelda/wn_szerosuit_status_builder.h>

// HYPOTHESIS: layout of the untouched part of the static article data (as the other articles' data blocks).
struct wnSZerosuitParalyzerData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const float* m_param; // +0x34, HYPOTHESIS: one float parameter
};

class wnSZerosuitParalyzerParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnSZerosuitParalyzerParamAccesser(const float* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(98) {
        m_floats[0] = param;
    }
    virtual ~wnSZerosuitParalyzerParamAccesser(); // BYTES: defined in the article unit, after the article constructor (emission order)
};
static_assert(sizeof(wnSZerosuitParalyzerParamAccesser) == 0x2C, "Zero Suit paralyzer parameter accesser extent");

// The Blaster's configuration (wn/fox/wn_fox_blaster.h) with twenty motions, a two-part visibility module, a shadow,
// reflect and glow module, a nine-flag general work and full-property command threads (template arguments from the
// link map names of the builders the unit keeps or calls).
class wnSZerosuitParalyzerModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<20, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 2> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfigNoUniq<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
                                      soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
        StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<9, 6, 1> GeneralWorkBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullPropertyNoUniqStack<1, 1, 0, 0, 2, 1, 20, 0, 0, 2, 1, 20, 0, 0, 2, 1, 20, 0, 0, 2>
        AnimCmdModuleSubBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

// The article's own unit (the sora_melee unit mode of wn/zelda/wn_zelda_lighting_bow.h): the generic builder's member
// layout, spelled out so that the constructor can be defined by hand (wn_zelda_lighting_bow_builder_ctor.h).
template <>
class soModuleAccesserBuilder<wnSZerosuitParalyzerModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSZerosuitParalyzerModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    // The destructor is too big to expand in the weapon builder's: the unit keeps one weak copy.
    ~soModuleAccesserBuilder() { }
    soModuleAccesser* getModuleAccesser(); // BYTES: defined in wn_zelda_lighting_bow_builder_ctor.h (emission order)
};

class wnSZerosuitParalyzer : public wnWeaponBuilder<wnSZerosuitParalyzerModuleAccesserBuildConfig> {
    const float* m_param;                                 // +0x1B58
    wnSZerosuitParalyzerParamAccesser m_paramAccesser;    // +0x1B5C
public:
    wnSZerosuitParalyzer(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitParalyzerData* data);
    virtual ~wnSZerosuitParalyzer();
#else

struct wnSZerosuitParalyzerData;

// Call-side view: keeps the native extent without instantiating the builder.
class wnSZerosuitParalyzer : public Weapon {
    u8 m_storage[0x1B88 - sizeof(Weapon)];
public:
    wnSZerosuitParalyzer(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitParalyzerData* data);
    virtual ~wnSZerosuitParalyzer();
#endif
    // HYPOTHESIS: parameter types follow the Samus bomb and missile activators (founder task, resource, team).
    void activate(int founderTaskId, int resourceId, int team);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int selector);
    virtual void processFixPosition();
};
static_assert(sizeof(wnSZerosuitParalyzer) == 0x1B88, "Zero Suit paralyzer article extent");
