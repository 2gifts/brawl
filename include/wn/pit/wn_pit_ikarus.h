#pragma once

#include <so/damage/so_damage_event_presenter.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>

class wnPitIkarusModuleAccesserBuildConfig;
class soModuleAccesser;
struct wnArticleBuildInfo;
// HYPOTHESIS: the article data record the class info passes to the constructor (layout unreconstructed).
struct wnPitIkarusData;

#include <wn/pit/wn_pit_ikarus_kinetic_transactor.h>

#ifdef WN_PIT_IKARUS_FULL_BUILDER
#include <wn/wn_module_accesser_builder.h>
#include <so/controller/so_controller_module_link_ref.h>

// Module typedefs read off the weak builder destructor names (kinetic generic impl + mediator, status <5, work <2,2,1>, 5>,
// damage impl with singleton holders, collision hit <..., 1, 1, Impl, 10..>, motion <3, Impl, 0, 0>, anim command sub
// <1,5,0,0, 1,3,0,0, 1,3,0,0, 1,3,0,0>). The remaining arguments (model, link, visibility, attack, inside-event) are
// HYPOTHESIS, copied from the sibling pooled articles.
class wnPitIkarusModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnPitIkarusKineticEnergyTypeList, wnPitIkarusKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<soTypeList<soIntToType<4>, soTypeListNullType> >,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>,
                                      soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> VisibilityModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<5, soGeneralWorkBuildConfig<2, 2, 1>, 5,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 5, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};
#endif

#ifdef WN_PIT_IKARUS_FULL_BUILDER
// Field meanings unknown; offsets from the parameter accesser constructor (ints at 0x0/0x10/0x14, floats elsewhere).
struct wnPitIkarusParam {
    s32 unk0;
    float unk4;
    float unk8;
    float unkC;
    s32 unk10;
    s32 unk14;
    float unk18;
    float unk1C;
    float unk20;
    float unk24;
    float unk28;
    float unk2C;
    float unk30;
    float unk34;
};

struct wnPitIkarusData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPitIkarusParam* m_param; // +0x34
    u8 unk38[1];                     // +0x38, the attack collision set the constructor adds
};
#endif

class wnPitIkarusParamAccesser : public wnExtendParamAccesser<3999, 11, 23999, 3, 0, 0> {
public:
#ifdef WN_PIT_IKARUS_FULL_BUILDER
    explicit wnPitIkarusParamAccesser(const wnPitIkarusParam* param) __attribute__((never_inline)) :
        wnExtendParamAccesser<3999, 11, 23999, 3, 0, 0>(0x7d) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->unk8;
        m_floats[2] = &param->unkC;
        m_floats[3] = &param->unk18;
        m_floats[4] = &param->unk1C;
        m_floats[5] = &param->unk20;
        m_floats[6] = &param->unk24;
        m_floats[7] = &param->unk28;
        m_floats[8] = &param->unk2C;
        m_floats[9] = &param->unk30;
        m_floats[10] = &param->unk34;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk10;
        m_ints[2] = &param->unk14;
    }
#endif
    virtual ~wnPitIkarusParamAccesser() { }
};

// Ikarus keeps its energies in its own kinetic mediator (no appended common energies), unlike the other pooled articles.
// The Ikarus unit itself (WN_PIT_IKARUS_GENERIC_BUILDER) builds the generic wnWeaponBuilder<BC> template, unit-wide with
// WN_WEAPON_NO_COMMON_ENERGIES and WN_WEAPON_BUILDER_CTOR_NO_FLAG: BYTES, link order, the template's
// deactivateDescendantForce is then emitted with the other template members at the end of the unit, as in the original.
// The other Pit units that need the complete builder keep their common-energy weapon builders, so the Ikarus builder is
// spelled out for them here.
#if defined(WN_PIT_IKARUS_FULL_BUILDER) && !defined(WN_PIT_IKARUS_GENERIC_BUILDER)
template <>
class wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnPitIkarusModuleAccesserBuildConfig> m_modules; // +0xCC
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in the no-common-energies builder.
    u32 m_modulesPad;
public:
    // Defined in wn_pit_ikarus.cpp (the generic template's constructor there).
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd);
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};
#endif
#ifndef WN_PIT_IKARUS_FULL_BUILDER
template <>
class wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig> : public Weapon {
    u8 unkCC[0x22C8 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
};
#endif

class wnPitIkarus : public wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>, public soDamageEventObserver {
    const void* unk22D4; // the wnPitIkarusParam record (data->m_param)
    wnPitIkarusParamAccesser unk22D8;
public:
#ifdef WN_PIT_IKARUS_FULL_BUILDER
    wnPitIkarus(s32 articleId, const wnArticleBuildInfo& info, const wnPitIkarusData* data);
#endif
    // Weak in the native Ikarus unit (__dt__11wnPitIkarusFv), so defined in the class.
    virtual ~wnPitIkarus() { }
    void activate(u32 founderTaskId, u32 resourceId, int team, const Vec3f& pos, float unk1, float unk2);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
};

static_assert(sizeof(wnWeaponBuilder<wnPitIkarusModuleAccesserBuildConfig>) == 0x22C8, "Ikarus builder size");
static_assert(sizeof(wnPitIkarusParamAccesser) == 0x60, "Ikarus parameter accesser size");
static_assert(sizeof(wnPitIkarus) == 0x2338, "Ikarus size");
