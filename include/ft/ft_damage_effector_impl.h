#pragma once

#include <StaticAssert.h>
#include <so/damage/so_damage_effector_impl.h>
#include <types.h>

// HYPOTHESIS: soDamageEffectorImpl (sora_melee so_damage_effector_impl.o, no unit yet) declares the slots its vtable
// fills with map-named functions and leaves reqDamageEffectParam pure; ftDamageEffectorImpl derives from it (RTTI).
class soDamageEffectorImpl : public soDamageEffector {
public:
    virtual ~soDamageEffectorImpl() { }
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame);
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    virtual void reqInvincibleEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData) = 0;
    virtual void reqDamageGroundBeatDownEffect(soModuleAccesser* moduleAccesser, Vec2f* normal);
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
    virtual void reqStop();
    virtual void reqTipEffect(soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
};

// HYPOTHESIS: the fighter override of reqShake (fn_27_151140) reads a float argument that the soDamageEffector
// interface does not declare, so it is not declared here.
class ftDamageEffectorImpl : public soDamageEffectorImpl {
public:
    virtual ~ftDamageEffectorImpl();
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    // HYPOTHESIS: the module map names this slot reqEffectScreen.
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData);
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
    virtual void reqStop();
};
static_assert(sizeof(ftDamageEffectorImpl) == 0xC, "Class is wrong size!");
