#pragma once

// Ivysaur's Razor Leaf (article kind 73). HYPOTHESIS: all names that are not in the module map.

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_article_build_info.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_effect_module_builder_null_arrays.h>
#include <ft/builder/ft_dol_instances.h>
#include <wn/samus/wn_szerosuit_stack_vector.h>

// HYPOTHESIS: descriptive parameter names. The activation reads life at 0 and
// spin limits at 8/C; the constructor registers the others with its accessor.
struct wnPokeFushigisouLeafCutterParam {
    s32 lifeFrames;       // +0x00
    float horizontalBrake; // +0x04
    float spinMin;        // +0x08
    float spinMax;        // +0x0C
    float unk10;
    s32 unk14;
    float unk18;
    float unk1C;
};

// HYPOTHESIS: layout of the untouched part of the static data block.
struct wnPokeFushigisouLeafCutterData {
    s32 m_resGroupNo; // the constructor reads the low byte of this first word
    u8 unk4[0x30];
    const wnPokeFushigisouLeafCutterParam* m_param; // +0x34
};

class wnPokeFushigisouLeafCutterParamAccesser : public wnExtendParamAccesser<3999, 6, 23999, 2, 0, 0> {
public:
    explicit wnPokeFushigisouLeafCutterParamAccesser(const wnPokeFushigisouLeafCutterParam* param) :
        wnExtendParamAccesser<3999, 6, 23999, 2, 0, 0>(73) {
        m_floats[0] = &param->horizontalBrake;
        m_floats[1] = &param->spinMin;
        m_floats[2] = &param->spinMax;
        m_floats[3] = &param->unk10;
        m_floats[4] = &param->unk18;
        m_floats[5] = &param->unk1C;
        m_ints[0] = &param->lifeFrames;
        m_ints[1] = &param->unk14;
    }
    virtual ~wnPokeFushigisouLeafCutterParamAccesser() { }
};


// The status unique process list is built by the vector constructor taking an unused zero word.
WN_STATUS_PROC_VECTOR_ARG_CTOR(2);

// HYPOTHESIS: thread 0 keeps a one-entry interpreter stack built by the constructor taking an unused zero word; the others keep two.
template <>
class soArrayVector<acCmdInterpreterStackData, 1> : public soArrayVectorAbstract<acCmdInterpreterStackData> {
    s32 m_topIndex : sizeof(bit_width<1>) + 1;
    s32 m_lastIndex : sizeof(bit_width<1>) + 1;
    s32 m_size : sizeof(bit_width<1>) + 1;
    u32 m_isFull : 1;
    acCmdInterpreterStackData m_elements[1];
public:
    soArrayVector(s32);
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
    soArraySelectHolder() : m_array(0) { }
    ~soArraySelectHolder() { }
    soArrayVector<acCmdInterpreterStackData, 1>* get() { return &m_array; }
};

// HYPOTHESIS: the leaf constructs only the status, command, link, attack, model, shield and turn event units; the others
// have no capacity and keep their empty slots.
struct wnPokeFushigisouLeafCutterInsideEventManageModuleTypes {
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
class soInsideEventManageModuleBuilder<B, wnPokeFushigisouLeafCutterInsideEventManageModuleTypes> {
    typedef wnPokeFushigisouLeafCutterInsideEventManageModuleTypes T;
    soInsideEventManageModuleBuilder_MEMBER_LIST
public:
    soInsideEventManageModuleBuilder() :
        m_entity(),
        m_module(this),
        m_status(m_module.getManageId(), B::StatusEvtId),
        m_animCmd(m_module.getManageId(), B::AnimCmdEvtId),
        m_link(m_module.getManageId(), B::LinkEvtId),
        m_collisionAttack(m_module.getManageId(), B::CollisionAttackEvtId),
        m_model(m_module.getManageId(), B::ModelEvtId),
        m_collisionShield(m_module.getManageId(), B::CollisionShieldEvtId),
        m_turn(m_module.getManageId(), B::TurnEvtId) { }
    ~soInsideEventManageModuleBuilder() { }
    soEventManageModuleImpl* getModule() { return &m_module; }
};

class wnPokeFushigisouLeafCutterAnimCmdSubBuildConfig {
public:
    typedef soAnimCmdControlUnitBuildConfig<0, 1, 2, 0, 0, 0, 0, 1> Unit0Config;
    typedef soAnimCmdControlUnitBuildConfig<1, 2, 2, 0, 0, 1, 0, 2> Unit1Config;
    typedef soAnimCmdControlUnitBuildConfig<2, 2, 2, 0, 0, 1, 0, 2> Unit2Config;
    typedef soAnimCmdControlUnitBuildConfig<3, 2, 2, 0, 0, 1, 0, 2> Unit3Config;
    typedef soAnimCmdControlUnitBuildConfigUniq<4, 2> Unit4Config;
    enum { Unit0Enabled = 1, Unit1Enabled = 1, Unit2Enabled = 1, Unit3Enabled = 1, Unit4Enabled = 1 };
};

class wnPokeFushigisouLeafCutterModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<10, 18, 0, 1, 0, 0, 4, 4, 0, 0, 1, 0, 0, 0, 0, 0, 0, 6, 0, wnPokeFushigisouLeafCutterInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef wnPokeFushigisouLeafCutterInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<2, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfigNull,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<2, soGeneralWorkBuildConfigNull, 2, soTransitionModuleBuildConfigNull> StatusModuleBuildConfig;
    typedef soSoundModuleBuildConfigNull SoundModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 2> KineticModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<10, 10, 1> GeneralWorkBuildConfig;
    typedef wnEffectModuleBuildConfig<0, 0, 0, 1, -1, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef wnPokeFushigisouLeafCutterAnimCmdSubBuildConfig AnimCmdModuleSubBuildConfig;
};

class wnPokeFushigisouLeafCutter : public wnWeaponBuilder<wnPokeFushigisouLeafCutterModuleAccesserBuildConfig> {
public:
    const wnPokeFushigisouLeafCutterParam* m_param;              // +0x16B4
    wnPokeFushigisouLeafCutterParamAccesser m_paramAccesser;     // +0x16B8
    wnPokeFushigisouLeafCutter(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouLeafCutterData* data);
    virtual ~wnPokeFushigisouLeafCutter() { }
    void activate(int founderTaskId, int resourceId, int team, Vec3f* position,
                  float lr, float speed, float angle);
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd*, soModuleAccesser*, int);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
