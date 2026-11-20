#pragma once

// The Pokemon Trainer's Final Smash article (article kind 0x12), built in sora_melee. Only wn_poke_trainer_final.cpp includes
// this header; the fighter unit sees the opaque shell of ft/poke/ft_poke_trainer_article_pools.h.
// HYPOTHESIS: all class names and the data/param layouts.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/zelda/wn_szerosuit_status_builder.h>
#include <wn/poke/wn_poke_lizardon_rock_stone.h> // the effect module builder without continual and screen arrays
#include <wn/wn_inside_event_all_units.h>
#include <wn/wn_article_build_info.h>

// The parameter block of the article: one integer.
struct wnPokeTrainerFinalParam {
    s32 unk0;
};

// The static data block of the article. HYPOTHESIS: layout of the untouched part.
struct wnPokeTrainerFinalData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPokeTrainerFinalParam* m_param; // +0x34
    u8 unk38[8]; // HYPOTHESIS: a table the build data is handed the address of
};

class wnPokeTrainerFinalParamAccesser : public wnExtendParamAccesser<3999, 0, 23999, 1, 0, 0> {
public:
    explicit wnPokeTrainerFinalParamAccesser(const wnPokeTrainerFinalParam* param) :
        wnExtendParamAccesser<3999, 0, 23999, 1, 0, 0>(0x12) {
        m_ints[0] = &param->unk0;
    }
    virtual ~wnPokeTrainerFinalParamAccesser();
};
static_assert(sizeof(wnPokeTrainerFinalParamAccesser) == 0x2C, "Final Smash parameter accesser extent");

// The status module's work area holds one integer and one flag, no float. The native builder keeps a word of padding where
// the (empty) float array starts. HYPOTHESIS: source spelling of the configuration.
template <>
class soGeneralWorkBuilder<soGeneralWorkBuildConfig<1, 0, 1> > {
    s32 m_ints[1];
    float m_floats[1];
    u32 m_flags[1];
    soGeneralWorkSimple m_module;
public:
    soGeneralWorkBuilder() : m_module(m_ints, 1, m_floats, 0, m_flags, 1) {
        soGeneralWorkAbstract* work = &m_module;
        work->clearWorkAll();
    }
    soGeneralWorkSimple* getModule() { return &m_module; }
    soGeneralWorkAbstract* getWork() { return &m_module; }
};

// The full-property command threads with a one-entry stack on the uniq thread.
// HYPOTHESIS: in the original this is one more value of wnAnimCmdModuleSubBuildConfigFullProperty.
class wnPokeTrainerFinalAnimCmdSubBuildConfig
    : public wnAnimCmdModuleSubBuildConfigFullProperty<1, 1, 0, 0, 2, 1, 19, 0, 0, 3, 1, 19, 0, 0, 2, 1, 19, 0, 0, 2> {
public:
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 1> Unit4Config;
};

class wnPokeTrainerFinalModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<12, 20, 0, 1, 0, 0, 3, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0,
                                                 wnAllInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<4, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<19, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 7, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soStopModuleBuildConfigNull StopModuleBuildConfig;
    typedef wnEffectModuleBuildConfig<0, 0, 0, 1, -1, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 2> VisibilityModuleBuildConfig;
    typedef soCameraModuleBuildConfig<soCameraModuleImpl> CameraModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<16, 10, 1> GeneralWorkBuildConfig;
    typedef wnStatusModuleBuildConfigNoUniq<1, soGeneralWorkBuildConfig<1, 0, 1>, 1, soTransitionModuleBuildConfigNull>
        StatusModuleBuildConfig;
    typedef wnPokeTrainerFinalAnimCmdSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
};

template <>
class soModuleAccesserBuilder<wnPokeTrainerFinalModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnPokeTrainerFinalModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

static_assert(sizeof(soModuleAccesserBuilder<wnPokeTrainerFinalModuleAccesserBuildConfig>) == 0x1BC4, "Final module builder extent");
static_assert(sizeof(wnModuleAccesserBuilder<wnPokeTrainerFinalModuleAccesserBuildConfig>) == 0x1F1C, "Final modules extent");
static_assert(sizeof(wnWeaponBuilder<wnPokeTrainerFinalModuleAccesserBuildConfig>) == 0x1FEC, "Final builder extent");

class wnPokeTrainerFinal : public wnWeaponBuilder<wnPokeTrainerFinalModuleAccesserBuildConfig> {
    const wnPokeTrainerFinalParam* m_param;      // +0x1FEC
    wnPokeTrainerFinalParamAccesser m_paramAccesser; // +0x1FF0
    u8 unk201C[0x2070 - 0x201C]; // HYPOTHESIS: the article's working data, untouched by the constructor
public:
    wnPokeTrainerFinal(s32 articleId, const wnArticleBuildInfo& info, const wnPokeTrainerFinalData* data);
    virtual ~wnPokeTrainerFinal();
    Vec3f getFinalPokemonPos(int index);
};
static_assert(sizeof(wnPokeTrainerFinal) == 0x2070, "Final Smash article extent");
