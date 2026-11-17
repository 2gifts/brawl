#pragma once

// Zero Suit Samus's Power Suit pieces (article kind 0x42): the armor parts that fly off in her Final Smash ending and her
// down taunt. Each piece is a weapon that, when shot, turns into a "Power Suit piece" item (item kind 0x5A).
// HYPOTHESIS: field names; offsets follow the native code.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>

#include <ft/builder/ft_dol_instances.h>


// The interpreter stacks of the command threads (2 entries for thread 0, 1 for the others) are sora_melee vectors.
// HYPOTHESIS: the sora_melee map spells the 1-entry constructor without
// arguments (map names carry no signatures).
#ifndef FT_SZEROSUIT_ARTICLE_DTORS // the fighter unit declares the one-entry vectors itself (wn_module_accesser_builder.h, ft_builder_animcmd.h)
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 1);
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 1);
#endif
#include <wn/samus/wn_szerosuit_stack_vector.h>

// The energy list of the two kinetic energies is destroyed by sora_melee's instance of the vector destructor.
template <>
soArrayVector<soInstanceUnitFullProperty<soKineticEnergy*>, 2>::~soArrayVector();

// HYPOTHESIS: meaning of the parameter (id 4000).
struct wnSZerosuitArmorParam {
    float unk0;
};

// The static data block of the article (read by its wnModuleAccesserBuildData). HYPOTHESIS: layout of the untouched part.
struct wnSZerosuitArmorData {
    s32 m_resGroupNo; // The constructor reads the low byte of this first word.
    u8 unk4[0x30];
    const wnSZerosuitArmorParam* m_param; // +0x34
};

class wnSZerosuitArmorParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnSZerosuitArmorParamAccesser(const wnSZerosuitArmorParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x42) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnSZerosuitArmorParamAccesser() { }
};

// HYPOTHESIS: the status module owns no unique process list (it hands in the shared null array and a status kind count of
// 0, as wnPokeLizardonRockStone's does); the map name keeps 1 status kind and 1 status data entry.
typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<3, 2, 1>, 1,
                                  soTransitionModuleBuildConfig<soTypeList<soIntToType<2>, soTypeListNullType> > >
    wnSZerosuitArmorStatusBuildConfig;
template <>
class soStatusModuleBuilder<wnSZerosuitArmorStatusBuildConfig> {
    typedef wnSZerosuitArmorStatusBuildConfig BC;
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<BC::GeneralWorkBuildConfig> m_generalWorkBuilder;
    soTransitionModuleBuilder<BC::TransitionBuildConfig> m_transitionBuilder;
    soSingletonHolder<soArrayNull<soStatusUniqProcess*> > m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
#ifndef SO_STATUS_KINETIC_SLOT // a unit with the slot member keeps this word there (ft/builder/ft_module_accesser_builder.h)
    u32 m_tail;
#endif
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, BC::DataCount),
        m_generalWorkBuilder(),
        m_transitionBuilder(),
        m_uniqProcs(),
        m_changeRequests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)soSingletonHolder<soArrayNull<soStatusUniqProcess*> >::getInstance(),
                 m_generalWorkBuilder.getModule(), m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, 0, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

struct wnSZerosuitArmorModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soInsideEventManageModuleBuildConfig<12, 18, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 4, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<1, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnSZerosuitArmorStatusBuildConfig StatusModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig; // two energies
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 1, 0, 0, 2, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1> AnimCmdModuleSubBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soStopModuleBuildConfigNull StopModuleBuildConfig;
};

#if defined(FT_SZEROSUIT_ARTICLE_POOLS) && defined(WN_WEAPON_NO_COMMON_ENERGIES)
// Zero Suit Samus's fighter unit builds its whip and paralyzer without the common energies; the armor has them.
#include <wn/wn_weapon_builder_common_energies.h>
WN_WEAPON_BUILDER_COMMON_ENERGIES(wnSZerosuitArmorModuleAccesserBuildConfig);
#endif

class wnSZerosuitArmor : public wnWeaponBuilder<wnSZerosuitArmorModuleAccesserBuildConfig> {
public:
    const wnSZerosuitArmorParam* m_param;
    wnSZerosuitArmorParamAccesser m_paramAccesser;

    wnSZerosuitArmor(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitArmorData* data);
    virtual ~wnSZerosuitArmor() { }
    void activate(int founderTaskId, u32 etcResId, int index, int mdlResIndex, u32 node, u32 targetNode, int team);
    void shoot(float speed, float angle, float lr, int node);
};
