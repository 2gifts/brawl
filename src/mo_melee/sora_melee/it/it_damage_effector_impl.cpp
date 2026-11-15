#include <wn/wn_damage_effector_impl.h>
#include <types.h>

// TODO: reqShake, reqUniqEffect, reqCommonEffect, reqQuake and the static initializer.

// The damage effector shared by the items (declared here: include/wn/wn_damage_effector_impl.h has the base class).
class itDamageEffectorImpl : public soDamageEffectorImpl {
public:
    virtual ~itDamageEffectorImpl();
    virtual void reqShake(soModuleAccesser* moduleAccesser, int situation, Vec2f* normal, soCollisionAttackData* attackData, int hitStopFrame);
    virtual void reqCommonEffect(float power, float reaction, float lr, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData, soCollisionLog* collisionLog);
    virtual void reqUniqEffect(soModuleAccesser* moduleAccesser, int level, soCollisionAttackData* attackData);
    // HYPOTHESIS: the module map names this slot reqEffectScreen.
    virtual void reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData);
    virtual void reqQuake(float frameReaction, soModuleAccesser* moduleAccesser, int level);
    virtual void reqStop();
};

void itDamageEffectorImpl::reqStop() { }

void itDamageEffectorImpl::reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData) { }

itDamageEffectorImpl::~itDamageEffectorImpl() { }
