#pragma once

#include <StaticAssert.h>
#include <so/damage/so_damage_effector_impl.h>
#include <types.h>

// HYPOTHESIS: soDamageEffectorImpl (sora_melee so_damage_effector_impl.o, no unit yet) declares the slots its vtable
// fills with map-named functions and leaves reqDamageEffectParam pure; wnDamageEffectorImpl derives from it (RTTI).
class soDamageEffectorImpl : public soDamageEffector {
public:
    virtual ~soDamageEffectorImpl() { }
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame);
#ifdef SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
    virtual void reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#else
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#endif
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    virtual void reqInvincibleEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData) = 0;
    virtual void reqDamageGroundBeatDownEffect(soModuleAccesser* moduleAccesser, Vec2f* normal);
#ifdef SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
    virtual void reqQuake(soModuleAccesser* moduleAccesser, float frameReaction, int level);
#else
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
#endif
    virtual void reqStop();
    virtual void reqTipEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
};

class wnDamageEffectorImpl : public soDamageEffectorImpl {
public:
    virtual ~wnDamageEffectorImpl();
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame);
#ifdef SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
    virtual void reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#else
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
#endif
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    // HYPOTHESIS: the module map names this slot reqEffectScreen.
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData);
#ifdef SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
    virtual void reqQuake(soModuleAccesser* moduleAccesser, float frameReaction, int level);
#else
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
#endif
    virtual void reqStop();
};
static_assert(sizeof(wnDamageEffectorImpl) == 0xC, "Class is wrong size!");
