// The map names of this unit take the accesser first in reqQuake (and reqCommonEffect), as the sora_enemy effector does.
#define SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
#include <so/damage/so_damage_effector_impl.h>
#include <so/so_module_accesser.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <math.h>
#include <types.h>

// The common damage effector: the effects every damaged object shows (the invincible and tip hit sparks, the ground
// beat-down dust). The fighter and article effectors derive from it.
// HYPOTHESIS: reqDamageEffectParam is pure here (the vtable slot is empty). ft/ft_damage_effector_impl.h and
// wn/wn_damage_effector_impl.h declare the same class for their units.
// TODO: clean up: one declaration of soDamageEffectorImpl.
class soDamageEffectorImpl : public soDamageEffector {
public:
    virtual ~soDamageEffectorImpl() { }
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData,
                          int hitStopFrame) { }
    virtual void reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr,
                                 soCollisionAttackData* attackData, soCollisionLog* collisionLog) { }
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData) { }
    virtual void reqInvincibleEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser,
                                      soCollisionAttackData* attackData) = 0;
    virtual void reqDamageGroundBeatDownEffect(soModuleAccesser* moduleAccesser, Vec2f* normal);
    virtual void reqQuake(soModuleAccesser* moduleAccesser, float frameReaction, int level) { }
    virtual void reqStop() { }
    virtual void reqTipEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
};

// HYPOTHESIS: effect ids (common effect table): 9 the beat-down dust, 0x20 the invincible hit, 0x26 the tip hit.
void soDamageEffectorImpl::reqDamageGroundBeatDownEffect(soModuleAccesser* moduleAccesser, Vec2f* normal) {
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    Vec3f rot(0.0f, 0.0f, atan2(-normal->m_x, normal->m_y));
    moduleAccesser->getEffectModule().req((EfID)9, &pos, &rot, 1.0f, 0, -1);
}

// NONMATCHING (both hit effects): the original reads the hit position into registers before it stores the rotation.
void soDamageEffectorImpl::reqInvincibleEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    Vec3f pos;
    Vec3f rot(0.0f, 0.0f, 0.0f);
    pos = collisionLog->m_pos;
    moduleAccesser->getEffectModule().req((EfID)0x20, &pos, &rot, 1.0f, 0, -1);
}

void soDamageEffectorImpl::reqTipEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    Vec3f pos;
    Vec3f rot(0.0f, 0.0f, 0.0f);
    pos = collisionLog->m_pos;
    moduleAccesser->getEffectModule().req((EfID)0x26, &pos, &rot, 1.0f, 0, -1);
}
