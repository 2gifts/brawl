#pragma once

#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything

#ifdef WN_POKE_LIZARDON_BREATH_BUILD
#include <wn/wn_weapon_builder.h>
// BYTES: the sora_melee unit mode of the module accesser builder is shared with the Light Bow (wn/zelda/wn_zelda_lighting_bow.h).
#define WN_ZELDA_LIGHTING_BOW_UNIT
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_anim_cmd_full_property.h>
#include <wn/wn_inside_event_all_units.h>

// HYPOTHESIS: same layout as the Bowser breath parameter block.
struct wnPokeLizardonBreathParam {
    float m_groundOffsetY;
    int m_work5;
    int m_work6;
    float m_speedRandMin;
    float m_speedRandMax;
};
struct wnPokeLizardonBreathData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPokeLizardonBreathParam* m_param; // +0x34
};

// HYPOTHESIS: thread 0 of this article owns no interpreter stack (it hands the null array in), so no storage is left between
// the table word and the address pack.
template <>
class soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<0, 1, 1, 0, 0, 0, 0, 0> > {
    typedef soAnimCmdControlUnitBuildConfig<0, 1, 1, 0, 0, 0, 0, 0> BC;
    typedef soArrayContractibleTable<const acAnimCmdConv*> TableT;
    soAnimCmdFirstMember<TableT> m_table;     // +0
    u32 unk10;                                // +0x10
    soAnimCmdAddressPackArraySeparate m_pack; // +0x14
    soAnimCmdInterpreter m_interpreter;       // +0x30
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

// HYPOTHESIS: an effect module with no continual or screen arrays of its own (the null singletons are handed in); the only
// member left is the word list.
template <typename T>
class soEffectModuleBuilder<wnEffectModuleBuildConfig<0, 1, 0, 0, -1, T> > {
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
    ~soEffectModuleBuilder() __attribute__((never_inline));
};

// HYPOTHESIS: the status module owns no unique process list (it hands in the null array), no work bank and no transitions.
typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfigNull, 1, soTransitionModuleBuildConfigNull> wnPokeLizardonBreathStatusBuildConfig;
template <>
class soStatusModuleBuilder<wnPokeLizardonBreathStatusBuildConfig> {
    typedef wnPokeLizardonBreathStatusBuildConfig BC;
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<BC::GeneralWorkBuildConfig> m_generalWorkBuilder;
    soTransitionModuleBuilder<BC::TransitionBuildConfig> m_transitionBuilder;
    soSingletonHolder<soArrayNull<soStatusUniqProcess*> > m_uniqProcs;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_changeRequests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* statusData, void* preCheckData) :
        m_statusTable(statusData, BC::DataCount),
        m_generalWorkBuilder(),
        m_transitionBuilder(),
        m_uniqProcs(),
        m_changeRequests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)soSingletonHolder<soArrayNull<soStatusUniqProcess*> >::getInstance(),
                 m_generalWorkBuilder.getModule(), m_transitionBuilder.getModule(), &m_changeRequests, preCheckData, (s32)0, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

class wnPokeLizardonBreathModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<11, 17, 0, 1, 0, 0, 4, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 7, 0, wnAllInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnAllInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<1, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soResourceModuleBuildConfigNull ResourceModuleBuildConfig;
    typedef soGroundModuleBuildConfig<1, soGroundModuleImpl> GroundModuleBuildConfig;
    typedef soGlowModuleBuildConfig<soGlowModuleSimple> GlowModuleBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfigNull VisibilityModuleBuildConfig;
    typedef soSoundModuleBuildConfigNull SoundModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<11, 7, 1> GeneralWorkBuildConfig;
    typedef wnPokeLizardonBreathStatusBuildConfig StatusModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig;
    typedef wnEffectModuleBuildConfig<0, 1, 0, 0, -1, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigFullProperty<1, 1, 0, 0, 0, 1, 1, 0, 0, 2, 1, 1, 0, 0, 2, 1, 1, 0, 0, 2>
        AnimCmdModuleSubBuildConfig;
};

template <>
class soModuleAccesserBuilder<wnPokeLizardonBreathModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnPokeLizardonBreathModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};

class wnPokeLizardonBreath : public wnWeaponBuilder<wnPokeLizardonBreathModuleAccesserBuildConfig> {
public:
    const wnPokeLizardonBreathParam* m_param;
    wnPokeLizardonBreath(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonBreathData* data);
    virtual ~wnPokeLizardonBreath() { }
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool reflect();
    void activate(int founderTaskId, int team, const Vec3f& position, int variation,
                  float lr, float speed, float scale, float speedMul, float angle);
};
#else
struct wnPokeLizardonBreathData;

// The flame breath of Lizardon. Opaque shell: its fields are not reconstructed yet.
class wnPokeLizardonBreath : public Weapon {
    union {
        u8 m_unreconstructed[0x164C - sizeof(Weapon)];
        struct {
            u8 unkCC[0x1648 - sizeof(Weapon)];
            const struct {
                float unk0;
                int unk4;
                int unk8;
                float unkC;
                float unk10;
            }* unk1648;
        };
    };
public:
    wnPokeLizardonBreath(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonBreathData* data);
    virtual ~wnPokeLizardonBreath();
    virtual void updateNodeSRT();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool reflect();
    void activate(int founderTaskId, int team, const Vec3f& position, int variation,
                  float lr, float speed, float scale, float speedMul, float angle);
};
#endif
//TMPASSERT
