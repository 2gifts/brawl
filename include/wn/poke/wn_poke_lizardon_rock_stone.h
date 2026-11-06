#pragma once

// The stones the Lizardon rock breaks into (article kind 0xF). The full class is only built by the unit that defines it
// (WN_WEAPON_BUILDER); everyone else sees the opaque shell declared in ft/poke/ft_poke_lizardon_article_pools.h.
// HYPOTHESIS: all names that are not in the module map, and the data layout.

#include <wn/wn_weapon_builder.h>

struct wnPokeLizardonRockStoneData;

#ifdef WN_WEAPON_BUILDER
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_kinetic_energy_rot_normal.h>

struct wnPokeLizardonRockStoneData {
    s32 m_resGroupNo;
};

// HYPOTHESIS: this article's command threads keep short interpreter stacks (the map names give 2 for threads 1 to 3, 0 for
// thread 0); the uniq thread is as in the default.
// HYPOTHESIS: thread 0 of this article owns no interpreter stack (it hands the null array in), so no storage is left between
// the table word and the address pack.
template <>
class soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<0, 1, 2, 0, 0, 0, 0, 0> > {
    typedef soAnimCmdControlUnitBuildConfig<0, 1, 2, 0, 0, 0, 0, 0> BC;
    typedef soArrayContractibleTable<const acAnimCmdConv*> TableT;
    soAnimCmdFirstMember<TableT> m_table;                 // +0
    u32 unk10;                                            // +0x10
    soAnimCmdAddressPackArraySeparate m_pack;             // +0x14
    soAnimCmdInterpreter m_interpreter;                   // +0x30
public:
    soAnimCmdControlUnitBuilder(soModuleAccesser* acc, void* data0, void* data1, void* data2) :
        m_table((const acAnimCmdConv**)data0, BC::Capacity),
        m_pack(&m_table.m_member, (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull, (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull),
        m_interpreter(ftGetManageId(acc),
                      (soArrayVector<acCmdInterpreterStackData, 8>*)soSingletonHolder<soArrayNull<acCmdInterpreterStackData> >::getInstance(),
                      BC::UnitId, BC::InterpMode, 1.0f, ftAddressOf((u8)0)) {
        soAttributeFlag attr(BC::AttrMask);
        soAnimCmdControlUnit unit = { &m_interpreter, &m_pack };
        acc->getAnimCmdModule().registInterpreter(unit, attr);
    }
    ~soAnimCmdControlUnitBuilder() { }
};

class wnPokeLizardonRockStoneAnimCmdSubBuildConfig {
public:
    typedef soAnimCmdControlUnitBuildConfig<0, 1, 2, 0, 0, 0, 0, 0> Unit0Config;
    typedef soAnimCmdControlUnitBuildConfig<1, 2, 2, 0, 0, 1, 0, 2> Unit1Config;
    typedef soAnimCmdControlUnitBuildConfig<2, 2, 2, 0, 0, 1, 0, 2> Unit2Config;
    typedef soAnimCmdControlUnitBuildConfig<3, 2, 2, 0, 0, 1, 0, 2> Unit3Config;
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 2> Unit4Config;
    enum { Unit0Enabled = 1, Unit1Enabled = 1, Unit2Enabled = 1, Unit3Enabled = 1, Unit4Enabled = 1 };
};

// An effect module with no continual or screen arrays of its own (the null singletons are handed in).
// HYPOTHESIS: source spelling of the selection; the only member left is the word list.
template <typename T>
class soEffectModuleBuilder<wnEffectModuleBuildConfig<0, 0, 0, 1, -1, T> > {
    u8 unk0[4];
    soArrayVector<u32, 1> m_u32s; // +0x4
    T m_effectModule;             // +0x14
public:
    soEffectModuleBuilder(soModuleAccesser* acc, void* nodeData, void* emitData, void* commonData, void* screenData,
                          soEventObserverRegistrationDesc* regDesc) :
        m_u32s(1, 0, 0),
        m_effectModule(acc, soSingletonHolder<soArrayNull<soEffectContinual> >::getInstance(), nodeData, &m_u32s,
                       soSingletonHolder<soArrayNull<soEffectTime> >::getInstance(), emitData, regDesc, commonData, -1,
                       screenData, soSingletonHolder<soArrayNull<efScreenHandle> >::getInstance()) { }
    T* getModule() { return &m_effectModule; }
};

// HYPOTHESIS: the status module owns no unique process list (it hands in the null array), no work bank and no transitions.
typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfigNull, 2, soTransitionModuleBuildConfigNull> wnPokeLizardonRockStoneStatusBuildConfig;
template <>
class soStatusModuleBuilder<wnPokeLizardonRockStoneStatusBuildConfig> {
    typedef wnPokeLizardonRockStoneStatusBuildConfig BC;
    soArrayContractibleTable<const soStatusData> m_statusTable;                         // +0x00
    soGeneralWorkBuilder<BC::GeneralWorkBuildConfig> m_generalWorkBuilder;              // +0x10
    soTransitionModuleBuilder<BC::TransitionBuildConfig> m_transitionBuilder;           // +0x11
    soSingletonHolder<soArrayNull<soStatusUniqProcess*> > m_uniqProcs;                  // +0x12
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;                          // +0x14
    soStatusModuleImpl m_module;                                                        // +0x28
    u32 unkD4;                                                                          // +0xD4
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, BC::DataCount),
        m_generalWorkBuilder(),
        m_transitionBuilder(),
        m_uniqProcs(),
        m_changeRequests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)soSingletonHolder<soArrayNull<soStatusUniqProcess*> >::getInstance(),
                 m_generalWorkBuilder.getModule(), m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, BC::StatusKindCap, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

class wnPokeLizardonRockStoneModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<11, 18, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, wnTurnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnTurnInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<2, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnPokeLizardonRockStoneStatusBuildConfig StatusModuleBuildConfig;
    typedef soSoundModuleBuildConfigNull SoundModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig;
    typedef wnEffectModuleBuildConfig<0, 0, 0, 1, -1, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<9, 7, 1> GeneralWorkBuildConfig;
    typedef wnPokeLizardonRockStoneAnimCmdSubBuildConfig AnimCmdModuleSubBuildConfig;
};

class wnPokeLizardonRockStone : public wnWeaponBuilder<wnPokeLizardonRockStoneModuleAccesserBuildConfig> {
public:
    wnPokeLizardonRockStone(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonRockStoneData* data);
    virtual ~wnPokeLizardonRockStone() { }
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int index);
    // Same declaration as the opaque shell in ft/poke/ft_poke_lizardon_article_pools.h (wnPokeLizardonRock calls it).
    void activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float angle, float speed, int life, int variant);
};
static_assert(sizeof(wnPokeLizardonRockStone) == 0x16DC, "wnPokeLizardonRockStone layout is wrong!");
#endif
