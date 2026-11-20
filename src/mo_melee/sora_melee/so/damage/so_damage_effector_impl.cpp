// The map names of this unit take the accesser first in reqQuake (and reqCommonEffect), as the sora_enemy effector does.
#define SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
#include <so/damage/so_damage_effector_impl.h>
#include <so/so_module_accesser.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>
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

// TODO: clean up: MATCH-ONLY helper; the hit position components are read x, y, z before any store.
static inline void setPosReversed(Vec3f* v, float z, float y, float x) {
    v->m_x = x;
    v->m_y = y;
    v->m_z = z;
}

// Map name soDamageEffectorImpl::reqCommonEffectParam (static): the hit spark of the attack's element at the hit
// position, scaled with the power (clamped to [scaleMin, scaleMax]); normal hits at or above reactionStrong use the
// strong spark. The angle follows the attack's launch vector (361 counts as 0).
// HYPOTHESIS: parameter names and effect ids' meanings.
// NONMATCHING: float register assignment of the scale parameters and the order of the 0.0 load.
// TODO: clean up: keeps its placeholder name because the fighter, item, weapon and enemy effectors call it by that name.
extern "C" void fn_27_60CDC(float power, float reaction, float lr, float scaleMul, float scaleBase, float scaleMin, float scaleMax,
                            float reactionStrong, soModuleAccesser* moduleAccesser, soCollisionAttackData* attackData,
                            soCollisionLog* collisionLog) {
    Vec3f pos;
    setPosReversed(&pos, collisionLog->m_pos.m_z, collisionLog->m_pos.m_y, collisionLog->m_pos.m_x);
    Vec3f rot;
    rot.m_x = 0.0f;
    rot.m_y = 0.0f;
    rot.m_z = 2.0f * (3.1415927f * randf());
    float scale = scaleBase + power * scaleMul;
    if (scale < scaleMin) {
        scale = scaleMin;
    } else if (scale > scaleMax) {
        scale = scaleMax;
    }
    float angle;
    if (attackData->m_vector == 361) {
        angle = 0.0f;
    } else {
        angle = 0.017453292f * attackData->m_vector;
    }
    switch (attackData->m_attribute) {
        case soCollisionAttackData::Attribute_Normal:
        case soCollisionAttackData::Attribute_Turn:
        case soCollisionAttackData::Attribute_Water:
            if (reaction >= reactionStrong) {
                Vec3f zero(0.0f, 0.0f, 0.0f);
                moduleAccesser->getEffectModule().req((EfID)0x10, &pos, &zero, 1.0f, 0, -1);
            } else if (randi(soValueAccesser::getConstantInt(moduleAccesser, 0x55F4, 0)) != 0) {
                moduleAccesser->getEffectModule().req((EfID)1, &pos, &rot, scale, 0, -1);
            } else {
                moduleAccesser->getEffectModule().req((EfID)0x51, &pos, &rot, scale, 0, -1);
            }
            rot.m_y = 1.5707964f * -lr;
            rot.m_z = angle * -lr;
            moduleAccesser->getEffectModule().req((EfID)2, &pos, &rot, scale, 0, -1);
            break;
        case soCollisionAttackData::Attribute_Cutup:
        case soCollisionAttackData::Attribute_Grass:
            moduleAccesser->getEffectModule().req((EfID)5, &pos, &rot, scale, 0, -1);
            rot.m_y = 1.5707964f * -lr;
            rot.m_z = angle * -lr;
            moduleAccesser->getEffectModule().req((EfID)6, &pos, &rot, scale, 0, -1);
            break;
        case soCollisionAttackData::Attribute_Electric:
            moduleAccesser->getEffectModule().req((EfID)4, &pos, &rot, 1.0f, 0, -1);
            break;
        case soCollisionAttackData::Attribute_Fire:
            moduleAccesser->getEffectModule().req((EfID)3, &pos, &rot, 1.0f, 0, -1);
            break;
        case soCollisionAttackData::Attribute_Coin: {
            Vec3f zero(0.0f, 0.0f, 0.0f);
            moduleAccesser->getEffectModule().req((EfID)0x57, &pos, &zero, 1.0f, 0, -1);
            break;
        }
        case soCollisionAttackData::Attribute_Ice:
            moduleAccesser->getEffectModule().req((EfID)0x3F, &pos, &rot, 1.0f, 0, -1);
            rot.m_z = 0.0f;
            rot.m_y = 1.5707964f * -lr;
            moduleAccesser->getEffectModule().req((EfID)0x40, &pos, &rot, 1.0f, 0, -1);
            break;
    }
}

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
