#pragma once

// soDamageEffectorNull: the damage effector of stage objects that show no damage reaction (weapons). Every request is
// ignored; the virtuals are inline (weak copies where the vtable is needed).

#include <so/damage/so_damage_effector_impl.h>

class soDamageEffectorNull : public soDamageEffector {
public:
    virtual ~soDamageEffectorNull() { }
    virtual void reqShake(soModuleAccesser*, int, Vec2f*, soCollisionAttackData*, int) { }
    virtual void reqCommonEffect(float, float, float, soModuleAccesser*, soCollisionAttackData*, soCollisionLog*) { }
    virtual void reqUniqEffect(soModuleAccesser*, int, soCollisionAttackData*) { }
    virtual void reqInvincibleEffect(soModuleAccesser*, soCollisionLog*) { }
    // HYPOTHESIS: the module maps name this slot reqEffectScreen (all implementations).
    virtual void reqDamageEffectParam(float, float, soModuleAccesser*, soCollisionAttackData*) { }
    virtual void reqDamageGroundBeatDownEffect(soModuleAccesser*, Vec2f*) { }
    virtual void reqQuake(float, soModuleAccesser*, int) { }
    virtual void reqStop() { }
    virtual void reqTipEffect(soModuleAccesser*, soCollisionLog*) { }
};
