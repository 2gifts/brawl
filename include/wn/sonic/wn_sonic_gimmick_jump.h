#pragma once

#include <wn/wn_weapon_builder.h>
#include <so/so_gimmick_event_presenter.h>
#include <ft/ft_kinetic_energy.h>
#include <wn/wn_article_build_info.h>

// The fighter unit (ft_sonic.cpp) only holds the spring and sees the opaque builder layout; the spring's own unit defines
// WN_WEAPON_BUILDER (with the native builders) and gets the real one.
#ifdef WN_WEAPON_BUILDER
#include <wn/wn_extend_param_accesser.h>

// The parameter block of the spring (eleven floats, ids 4000..4010).
// HYPOTHESIS: meanings of the fields; the article reads them by id through the parameter accesser.
struct wnSonicGimmickJumpParam {
    float unk0[11];
};

// The static data block of the article (read by its wnModuleAccesserBuildData).
// HYPOTHESIS: layout of the untouched part.
struct wnSonicGimmickJumpData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSonicGimmickJumpParam* m_param; // +0x34
};

// The spring's event manager also builds the gimmick unit (the other articles leave it out).
// HYPOTHESIS: source spelling of the selection; the builder is built after the motion unit.
struct wnSonicGimmickInsideEventManageModuleTypes {
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
class soInsideEventManageModuleBuilder<B, wnSonicGimmickInsideEventManageModuleTypes> {
    typedef wnSonicGimmickInsideEventManageModuleTypes T;
    soInsideEventManageModuleBuilder_MEMBER_LIST
public:
    soInsideEventManageModuleBuilder() :
        m_entity(),
        m_module(this),
        m_status(m_module.getManageId(), B::StatusEvtId),
        m_animCmd(m_module.getManageId(), B::AnimCmdEvtId),
        m_damage(m_module.getManageId(), B::DamageEvtId),
        m_link(m_module.getManageId(), B::LinkEvtId),
        m_situation(m_module.getManageId(), B::SituationEvtId),
        m_collisionHit(m_module.getManageId(), B::CollisionHitEvtId),
        m_collisionAttack(m_module.getManageId(), B::CollisionAttackEvtId),
        m_model(m_module.getManageId(), B::ModelEvtId),
        m_article(m_module.getManageId(), B::ArticleEvtId),
        m_capture(m_module.getManageId(), B::CaptureEvtId),
        m_collisionShield(m_module.getManageId(), B::CollisionShieldEvtId),
        m_collisionReflector(m_module.getManageId(), B::CollisionReflectorEvtId),
        m_collisionAbsorber(m_module.getManageId(), B::CollisionAbsorberEvtId),
        m_collisionSearch(m_module.getManageId(), B::CollisionSearchEvtId),
        m_itemManage(m_module.getManageId(), B::ItemManageEvtId),
        m_motion(m_module.getManageId(), B::MotionEvtId),
        m_gimmick(m_module.getManageId(), B::GimmickEvtId) { }
    ~soInsideEventManageModuleBuilder() { }
    soEventManageModuleImpl* getModule() { return &m_module; }
};

class wnSonicGimmickJumpParamAccesser : public wnExtendParamAccesser<3999, 11, 23999, 0, 0, 0> {
public:
    // HYPOTHESIS: article kind id (0x70).
    wnSonicGimmickJumpParamAccesser(const wnSonicGimmickJumpParam* param) :
        wnExtendParamAccesser<3999, 11, 23999, 0, 0, 0>(0x70) {
        m_floats[0] = &param->unk0[0];
        m_floats[1] = &param->unk0[1];
        m_floats[2] = &param->unk0[2];
        m_floats[3] = &param->unk0[3];
        m_floats[4] = &param->unk0[4];
        m_floats[5] = &param->unk0[5];
        m_floats[6] = &param->unk0[6];
        m_floats[7] = &param->unk0[7];
        m_floats[8] = &param->unk0[8];
        m_floats[9] = &param->unk0[9];
        m_floats[10] = &param->unk0[10];
    }
    virtual ~wnSonicGimmickJumpParamAccesser() { }
};
static_assert(sizeof(wnSonicGimmickJumpParamAccesser) == 0x54, "Spring parameter accesser size");

class wnSonicGimmickJumpModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 8, 0, 0,
                                                  wnSonicGimmickInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef wnSonicGimmickInsideEventManageModuleTypes InsideEventManageModuleTypes;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<4, soGeneralWorkBuildConfig<2, 2, 1>, 4, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnKineticModuleBuildConfigSized<wnKineticModuleImpl, soKineticMediatorNull, 8> KineticModuleBuildConfig;
    typedef soAreaModuleBuildConfigWeapon<1, 4> AreaModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 4, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

static_assert(sizeof(wnWeaponBuilder<wnSonicGimmickJumpModuleAccesserBuildConfig>) == 0x2158, "Spring builder size");
class wnSonicGimmickJump : public wnWeaponBuilder<wnSonicGimmickJumpModuleAccesserBuildConfig>, public soGimmickEventObserver {
    const wnSonicGimmickJumpParam* m_param; // +0x2164
    wnSonicGimmickJumpParamAccesser m_paramAccesser; // +0x2168
    soKineticEnergyGroundMovement m_groundMovement;
public:
    wnSonicGimmickJump(s32 articleId, const wnArticleBuildInfo& info, const wnSonicGimmickJumpData* data);
    virtual ~wnSonicGimmickJump() { }
    virtual void onDeactivate();
    virtual void notifyEventChangeStatus(int, int, soStatusData*, soModuleAccesser*);
    virtual void updateNodeSRT();
    virtual void notifyEventGimmick(soGimmickEventArgs*, int*);
    // HYPOTHESIS: relative source ordering of lr; original caller establishes
    // f1 facing and r8 boolean independently of source ordering.
    void activate(int founderTaskId, int resourceId, int team, Vec2f* pos, float lr, bool unk);
    void shoot();
};
static_assert(sizeof(wnSonicGimmickJump) == 0x21EC, "Spring article layout");
#else

struct wnSonicGimmickJumpModuleAccesserBuildConfig;
struct wnSonicGimmickJumpData;

// Original RTTI gives this builder only Weapon as a base. The constructor
// places the spring's observer immediately after the embedded modules.
template <>
class wnWeaponBuilder<wnSonicGimmickJumpModuleAccesserBuildConfig> : public Weapon {
    u8 unkCC[0x2158 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnSonicGimmickJumpModuleAccesserBuildConfig>) == 0x2158, "Spring builder size");

class wnSonicGimmickJump : public wnWeaponBuilder<wnSonicGimmickJumpModuleAccesserBuildConfig>, public soGimmickEventObserver {
    void* unk2164;
    u8 unk2168[0x54]; // HYPOTHESIS: embedded parameter accesser, detailed fields unresolved.
    soKineticEnergyGroundMovement m_groundMovement;
public:
    // Built by the article pool of the fighter (the constructor lives in the article unit).
    wnSonicGimmickJump(s32 articleId, const wnArticleBuildInfo& info, const wnSonicGimmickJumpData* data);
    virtual ~wnSonicGimmickJump();
    virtual void onDeactivate();
    virtual void notifyEventChangeStatus(int, int, soStatusData*, soModuleAccesser*);
    virtual void updateNodeSRT();
    virtual void notifyEventGimmick(soGimmickEventArgs*, int*);
    // HYPOTHESIS: relative source ordering of lr; original caller establishes
    // f1 facing and r8 boolean independently of source ordering.
    void activate(int founderTaskId, int resourceId, int team, Vec2f* pos, float lr, bool unk);
    void shoot();
};
static_assert(sizeof(wnSonicGimmickJump) == 0x21EC, "Spring article layout");
#endif
