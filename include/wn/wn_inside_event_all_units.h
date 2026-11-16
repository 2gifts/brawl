#pragma once
// Inside event manage module of the articles that can enable every event unit (Samus's and Zero Suit Samus's articles):
// a unit with capacity asks the module for the manage id, a unit without capacity is the one-byte null unit and is
// built with manage id -1. HYPOTHESIS: the source spelling of the selection.
#include <wn/wn_module_accesser_builder.h>

struct wnAllInsideEventManageModuleTypes {
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

template <bool HasCapacity>
struct wnEventUnitManageId {
    static s16 get(soEventManageModuleImpl& module) { return module.getManageId(); }
};
template <>
struct wnEventUnitManageId<false> {
    static s16 get(soEventManageModuleImpl&) { return -1; }
};

template <typename B>
class soInsideEventManageModuleBuilder<B, wnAllInsideEventManageModuleTypes> {
    typedef wnAllInsideEventManageModuleTypes T;
    soInsideEventManageModuleBuilder_MEMBER_LIST
public:
    soInsideEventManageModuleBuilder() :
        m_entity(),
        m_module(this),
        m_status(wnEventUnitManageId<B::StatusCap != 0>::get(m_module), B::StatusEvtId),
        m_animCmd(wnEventUnitManageId<B::AnimCmdCap != 0>::get(m_module), B::AnimCmdEvtId),
        m_damage(wnEventUnitManageId<B::DamageCap != 0>::get(m_module), B::DamageEvtId),
        m_link(wnEventUnitManageId<B::LinkCap != 0>::get(m_module), B::LinkEvtId),
        m_situation(wnEventUnitManageId<B::SituationCap != 0>::get(m_module), B::SituationEvtId),
        m_collisionHit(wnEventUnitManageId<B::CollisionHitCap != 0>::get(m_module), B::CollisionHitEvtId),
        m_collisionAttack(wnEventUnitManageId<B::CollisionAttackCap != 0>::get(m_module), B::CollisionAttackEvtId),
        m_model(wnEventUnitManageId<B::ModelCap != 0>::get(m_module), B::ModelEvtId),
        m_article(wnEventUnitManageId<B::ArticleCap != 0>::get(m_module), B::ArticleEvtId),
        m_capture(wnEventUnitManageId<B::CaptureCap != 0>::get(m_module), B::CaptureEvtId),
        m_collisionShield(wnEventUnitManageId<B::CollisionShieldCap != 0>::get(m_module), B::CollisionShieldEvtId),
        m_collisionReflector(wnEventUnitManageId<B::CollisionReflectorCap != 0>::get(m_module), B::CollisionReflectorEvtId),
        m_collisionAbsorber(wnEventUnitManageId<B::CollisionAbsorberCap != 0>::get(m_module), B::CollisionAbsorberEvtId),
        m_collisionSearch(wnEventUnitManageId<B::CollisionSearchCap != 0>::get(m_module), B::CollisionSearchEvtId),
        m_itemManage(wnEventUnitManageId<B::ItemManageCap != 0>::get(m_module), B::ItemManageEvtId),
        m_motion(wnEventUnitManageId<B::MotionCap != 0>::get(m_module), B::MotionEvtId),
        m_gimmick(wnEventUnitManageId<B::GimmickCap != 0>::get(m_module), B::GimmickEvtId),
        m_turn(wnEventUnitManageId<B::TurnCap != 0>::get(m_module), B::TurnEvtId),
        m_collisionCatch(wnEventUnitManageId<B::CollisionCatchCap != 0>::get(m_module), B::CollisionCatchEvtId) { }
    ~soInsideEventManageModuleBuilder() { }
    soEventManageModuleImpl* getModule() { return &m_module; }
};
