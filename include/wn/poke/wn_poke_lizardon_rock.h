#pragma once

// The rock of Lizardon's down special (article kind 0xE). It replaces the opaque shell of
// ft/poke/ft_poke_lizardon_article_pools.h inside the unit that builds it (WN_POKE_LIZARDON_ROCK_DEFINED).

#include <wn/wn_weapon_builder.h>
#include <wn/wn_article_manage_module.h>
#include <wn/wn_article_build_info.h>
#include <so/status/so_status_module_impl.h>

class wnPokeLizardonRockStone;
struct wnPokeLizardonRockData;

#define WN_POKE_LIZARDON_ROCK_DEFINED

WN_STATUS_PROC_VECTOR_DEFAULT_CTOR(5);
FT_DOL_ARRAY_VECTOR(soArticle*, 1);
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 2);
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 1);
// The uniq thread's stack is built by the no-argument constructor.
template <>
class soArrayVector<acCmdInterpreterStackData, 1> : public soArrayVectorAbstract<acCmdInterpreterStackData> {
    s32 m_topIndex : sizeof(bit_width<1>) + 1;
    s32 m_lastIndex : sizeof(bit_width<1>) + 1;
    s32 m_size : sizeof(bit_width<1>) + 1;
    u32 m_isFull : 1;
    acCmdInterpreterStackData m_elements[1];
public:
    soArrayVector();
    virtual s32 size() const;
    virtual ~soArrayVector();
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual acCmdInterpreterStackData& atFastAbstractSub(s32 index) const;
    virtual acCmdInterpreterStackData& getArrayValueConst(s32 index);
    virtual s32 getTopIndex() const;
    virtual s32 getLastIndex() const;
    virtual void setSize(s32 size);
    virtual void setTopIndex(s32 topIndex);
    virtual void setLastIndex(s32 lastIndex);
    virtual void onFull();
    virtual void offFull();
};
template <>
class soArraySelectHolder<1, soArrayVector<acCmdInterpreterStackData, 1>, soSingletonHolder<soArrayNull<acCmdInterpreterStackData> > > {
public:
    soArrayVector<acCmdInterpreterStackData, 1> m_array;
    soArraySelectHolder() : m_array() { }
    ~soArraySelectHolder() { }
    soArrayVector<acCmdInterpreterStackData, 1>* get() { return &m_array; }
};
FT_DOL_ARRAY_VECTOR(acCmdInterpreterStackData, 3);
WN_ARTICLE_EVENT_OBSERVER_VECTOR(1);
typedef wnNullMediatorArticleManageModuleBuilder<1> wnPokeLizardonRockArticleManageModuleBuilder;

// HYPOTHESIS: the rock builds neither the damage nor the situation event unit.
struct wnPokeLizardonRockInsideEventManageModuleTypes {
    enum {
        hasStatus = true,
        hasAnimCmd = true,
        hasDamage = true,
        hasLink = true,
        hasSituation = true,
        hasCollisionHit = true,
        hasCollisionAttack = true,
        hasModel = true,
        hasArticle = true,
        hasCapture = true,
        hasCollisionShield = true,
        hasCollisionReflector = true,
        hasCollisionAbsorber = true,
        hasCollisionSearch = true,
        hasItemManage = true,
        hasMotion = true,
        hasGimmick = true,
        hasTurn = true,
        hasCollisionCatch = true,
    };
};

template <typename B>
class soInsideEventManageModuleBuilder<B, wnPokeLizardonRockInsideEventManageModuleTypes> {
    typedef wnPokeLizardonRockInsideEventManageModuleTypes T;
    soInsideEventManageModuleBuilder_MEMBER_LIST
public:
    soInsideEventManageModuleBuilder() :
        m_entity(),
        m_module(this),
        m_status(m_module.getManageId(), B::StatusEvtId),
        m_animCmd(m_module.getManageId(), B::AnimCmdEvtId),
        m_link(m_module.getManageId(), B::LinkEvtId),
        m_collisionHit(m_module.getManageId(), B::CollisionHitEvtId),
        m_collisionAttack(m_module.getManageId(), B::CollisionAttackEvtId),
        m_model(m_module.getManageId(), B::ModelEvtId),
        m_article(m_module.getManageId(), B::ArticleEvtId),
        m_collisionShield(m_module.getManageId(), B::CollisionShieldEvtId),
        m_turn(m_module.getManageId(), B::TurnEvtId) { }
    ~soInsideEventManageModuleBuilder() { }
    soEventManageModuleImpl* getModule() { return &m_module; }
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnPokeLizardonRockParam {
    float unk0; // model scale
    float unk4;
    float unk8;
    int unkC;
    int unk10;
    float unk14;
    int unk18;
};

struct wnPokeLizardonRockData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPokeLizardonRockParam* m_param; // +0x34
    u8 unk38[8];                            // the collision hit data set handed to the hit module
};

// HYPOTHESIS: the rock's motion transitions are of kind 1 only.
typedef soTypeList<soIntToType<1>, soTypeListNullType> wnPokeLizardonRockMotionTransitionTypeList;

// HYPOTHESIS: the rock's effect module takes the shared null continual, time and screen arrays; only the u32 list is stored.
class wnPokeLizardonRockEffectModuleBuildConfig { };

template <>
class soEffectModuleBuilder<wnPokeLizardonRockEffectModuleBuildConfig> {
    u8 unk0[4]; // HYPOTHESIS: the disabled time selection
    soArrayVector<u32, 1> m_u32s;
    soEffectModuleImpl m_effectModule;
public:
    soEffectModuleBuilder(soModuleAccesser* acc, void* nodeData, void* emitData, void* commonData, void* screenData,
                          soEventObserverRegistrationDesc* regDesc) :
        m_u32s(1, 0, 0),
        m_effectModule(acc, soSingletonHolder<soArrayNull<soEffectContinual> >::getInstance(), nodeData, &m_u32s,
                       soSingletonHolder<soArrayNull<soEffectTime> >::getInstance(), emitData, regDesc, commonData, -1, screenData,
                       soSingletonHolder<soArrayNull<efScreenHandle> >::getInstance()) { }
    soEffectModuleImpl* getModule() { return &m_effectModule; }
};

// The command threads of the rock: the last unit parameter is 3 for thread 0, 2 for the others, and the uniq thread has 1.
class wnPokeLizardonRockAnimCmdModuleSubBuildConfig {
public:
    typedef soAnimCmdControlUnitBuildConfig<0, 1, 5, 0, 0, 0, 0, 3> Unit0Config;
    typedef soAnimCmdControlUnitBuildConfig<1, 2, 5, 0, 0, 1, 0, 2> Unit1Config;
    typedef soAnimCmdControlUnitBuildConfig<2, 2, 5, 0, 0, 1, 0, 2> Unit2Config;
    typedef soAnimCmdControlUnitBuildConfig<3, 2, 5, 0, 0, 1, 0, 2> Unit3Config;
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 1> Unit4Config;
    enum { Unit0Enabled = 1, Unit1Enabled = 1, Unit2Enabled = 1, Unit3Enabled = 1, Unit4Enabled = 1 };
};

class wnPokeLizardonRockModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<11, 20, 0, 2, 0, 2, 2, 4, 1, 0, 1, 0, 0, 0, 0, 0, 0, 8, 0,
                                                 wnPokeLizardonRockInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef wnPokeLizardonRockInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soPostureModuleBuildConfig<2, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soModelModuleBuildConfig<2, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<5, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnPokeLizardonRockMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 1, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<5, soGeneralWorkBuildConfig<2, 1, 1>, 5, soTransitionModuleBuildConfig<wnPokeLizardonRockMotionTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<9, 8, 1> GeneralWorkBuildConfig;
    typedef wnPokeLizardonRockArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef soGlowModuleBuildConfig<soGlowModuleSimple> GlowModuleBuildConfig;
    typedef wnPokeLizardonRockEffectModuleBuildConfig EffectModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig;
    typedef wnPokeLizardonRockAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
};

class wnPokeLizardonRockStatusUniqProcessCrushed : public soStatusUniqProcess {
public:
    virtual ~wnPokeLizardonRockStatusUniqProcessCrushed() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};
extern wnPokeLizardonRockStatusUniqProcessCrushed g_wnPokeLizardonRockStatusUniqProcessCrushed;

class wnPokeLizardonRock : public wnWeaponBuilder<wnPokeLizardonRockModuleAccesserBuildConfig> {
public:
    const wnPokeLizardonRockParam* m_param; // +0x18E8
    wnPokeLizardonRock(s32 articleId, const wnArticleBuildInfo& info, const wnPokeLizardonRockData* data);
    virtual ~wnPokeLizardonRock() { }
    void shoot(bool unk);
    void activate(u32 founderTaskId, u32 resourceId, int team, const Vec3f& pos, float lr, float unk);
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, int index);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc, StageObject* sender, int index);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* acc);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData* data, u32 index, int kind,
                                         soModuleAccesser* acc, soCollisionLog* log);
    static bool activeArticle(wnPokeLizardonRockStone* stone, soModuleAccesser* acc);
};
static_assert(sizeof(wnPokeLizardonRock) == 0x18EC, "wnPokeLizardonRock layout is wrong!");
