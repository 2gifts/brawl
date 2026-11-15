#define MT_VEC2F_MEMBER_COPY // MATCH-ONLY: see mt_vector.h.
#define WN_DAMAGE_ACTOR_INTERFACE
#include <wn/wn_damage_transactor_actor.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <nw4r/math/math_arithmetic.h>
#include <types.h>

// A hit with no reaction and no power is ignored. A hit without a transaction only pushes the article.
bool wnDamageTransactorActor::onDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) {
    float reaction = damage->m_reaction;
    if (0.0f == reaction && damage->m_powerMax <= 0.0f) {
        return false;
    }
    if (damage->m_attackData.m_noTransaction == 1) {
        setupSpeedDamage(moduleAccesser, damage, damageLog);
        return false;
    }
    setupDamageStatusNormal(moduleAccesser, damage, damageLog, -1);
    return true;
}

void wnDamageTransactorActor::setupDamageStatusTurn(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) { }

void wnDamageTransactorActor::setupSpeedDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) {
    Vec2f speed;
    Vec2f attackSpeed;
    Vec2f::copy(attackSpeed, damage->m_speed);
    float angle = soDamageUtilActor::getDamageAngle(moduleAccesser, damage->m_reaction, damage->m_lr, damage->m_attackData.m_vector, &attackSpeed);
    speed.m_x = -damage->m_lr * (float)cos(angle);
    speed.m_y = sin(angle);
    float mul = damage->m_reaction * soValueAccesser::getConstantFloat(moduleAccesser, 2010, 0);
    soKineticEnergy::AttributeFlag attr(soKineticEnergy::ATTRIBUTE_MASK_DAMAGE);
    speed.m_x *= mul;
    speed.m_y *= mul;
    Vec2f sum;
    sum = moduleAccesser->getKineticModule().getSumSpeed(attr);
    if (sum.m_x * speed.m_x + sum.m_y * speed.m_y >= 0.0f) {
        speed = speed - sum;
    }
    Vec3f speed3(speed.m_x, speed.m_y, 0.0f);
    moduleAccesser->getKineticModule().addSpeedOutside(soKineticEnergy::Outside_Attack, &speed3);
}

void wnDamageTransactorActor::setupDamageStatusNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) { }

void wnDamageTransactorActor::setupDamageFlyRollStatus(float angle, float speed, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) { }
