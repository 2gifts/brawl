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

// MATCH-ONLY: the original reads the attack data bitfields as whole words (lwz) with unsigned extraction.
static inline u32 getAttackDataWord(soDamage* damage, int offset) {
    return *(u32*)((u8*)&damage->m_attackData + offset);
}

// The article version of the damage status: no attribute statuses, a ground hit only keeps the article's own direction
// and the stop frames are applied after the status request.
void wnDamageTransactorActor::setupDamageStatusNormal(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog, int unk) {
    float speedMul;
    Vec2f speed;
    Vec2f groundNormal;
    int statusKind;
    bool doHitStop = false;
    bool isDamageAir = true;
    moduleAccesser->getSituationModule().getKind();
    getDamageStatusKind(moduleAccesser);
    moduleAccesser->getPostureModule().getLr();
    float constMul = soValueAccesser::getConstantFloat(moduleAccesser, 2010, 0);
    int situation = moduleAccesser->getSituationModule().getKind();
    float reaction = damage->m_reaction;
    float frameReaction = reaction * soValueAccesser::getConstantFloat(moduleAccesser, 2002, 0);
    int level = soDamageUtilActor::getDamageLevel(moduleAccesser, frameReaction);
    Vec2f attackSpeed;
    Vec2f::copy(attackSpeed, damage->m_speed);
    float angle = soDamageUtilActor::getDamageAngle(moduleAccesser, reaction, damage->m_lr, damage->m_attackData.m_vector, &attackSpeed);
    soDamageUtilActor::checkDamageMeteor(moduleAccesser, damage->m_attackData.m_vector);
    int height = getDamageHeight(moduleAccesser, damage->m_collisionLog.m_damageIndex);
    float damageLr = damage->m_lr;
    speedMul = reaction * constMul;
    speed.m_x = -damageLr * (speedMul * (float)cos(angle));
    speed.m_y = speedMul * (float)sin(angle);
    if (situation == 0) {
        if (0.0f == angle && level > 2) {
            level = 2;
        }
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
        float groundAngle;
        float lenProduct = (normal.m_x * normal.m_x + normal.m_y * normal.m_y) * (speed.m_x * speed.m_x + speed.m_y * speed.m_y);
        if (0.0f == lenProduct) {
            groundAngle = 0.0f;
        } else {
            float dot = normal.m_x * speed.m_x + normal.m_y * speed.m_y;
            float c = dot * rsqrtf(lenProduct);
            const float hi = 1.0f;
            const float lo = -1.0f;
            float clamped = nw4r::math::FSelect(c - lo, c, lo);
            clamped = nw4r::math::FSelect(clamped - hi, hi, clamped);
            groundAngle = acos(clamped);
        }
        if (groundAngle < 1.5707964f) {
            if (level == 3) {
                statusKind = 4;
            } else {
                statusKind = 7;
            }
        } else if (level == 3) {
            statusKind = 4;
            if (groundAngle > 1.5707964f + soValueAccesser::getConstantFloat(moduleAccesser, 2013, 0)) {
                speed.m_y *= -soValueAccesser::getConstantFloat(moduleAccesser, 2014, 0);
            }
        } else {
            statusKind = 8;
            isDamageAir = false;
        }
    } else {
        if (level == 3) {
            statusKind = 4;
        } else {
            statusKind = 7;
        }
    }
    if (statusKind == 4) {
        statusKind = soDamageUtilActor::getDamageFlyStatus(moduleAccesser, damage->m_damage, angle);
    }
    int hitStopFrame = soDamageUtilActor::getDamageHitStopFrame(moduleAccesser, damage, 1.0f, false);
    float hitStopMul = getHitStopMul(moduleAccesser);
    if (1.0f != hitStopMul) {
        hitStopFrame = (int)((float)hitStopFrame * hitStopMul);
    }
    if (hitStopFrame > 0) {
        doHitStop = true;
    }
    if (situation == 2) {
        if (onCompositionDamageSpeed(moduleAccesser, damage, &speed, level)) {
            statusKind = 0;
        }
    }
    if (statusKind != 0) {
        damageLog->m_reaction = reaction;
        damageLog->m_level = (soDamage::Level)level;
        damageLog->m_height = height;
        damageLog->m_speed.m_x = speed.m_x;
        damageLog->m_speed.m_y = speed.m_y;
        damageLog->m_angle = angle;
        damageLog->m_lr = damageLr;
        damageLog->m_frame = frameReaction;
        damageLog->m_hitStopFrame = hitStopFrame;
        damageLog->m_attribute = (soCollisionAttackData::Attribute)damage->m_attackData.m_attribute;
        damageLog->m_damageAdd = damage->m_damageAdd;
        damageLog->m_attackerTeamNo = damage->m_collisionLog.m_teamNo;
        damageLog->m_hitStopDelay = damage->m_attackData.m_hitStopDelay;
        groundNormal.m_x = 0.0f;
        groundNormal.m_y = 1.0f;
        Vec2f groundNormalCopy;
        Vec2f::copy(groundNormalCopy, groundNormal);
        damageLog->m_groundTouchNormal.m_x = groundNormalCopy.m_x;
        damageLog->m_groundTouchNormal.m_y = groundNormalCopy.m_y;
        damageLog->m_attackerTaskId = damage->m_collisionLog.m_taskId;
        damageLog->m_attackerTeamOwnerId = damage->m_attackerTeamOwnerId;
        *((u8*)damageLog + 0x44) = *((u8*)damage + 0x32);
        damageLog->m_isSituationGround = false;
        damageLog->m_isDamageAir = isDamageAir;
        damageLog->m_unk26 = false;
        damageLog->m_isCollisionAbsolute = damage->m_collisionLog.m_isAbsolute;
        damageLog->m_isMeteor = false;
        damageLog->m_isAttackDirect = (getAttackDataWord(damage, 0x38) >> 13) & 1;
        damageLog->m_isVector365 = damage->m_attackData.m_vector == 365;
        if (isUseTurn(moduleAccesser)) {
            moduleAccesser->getPostureModule().setLr(damageLr);
        }
        onDamageChangeStatusRequest(statusKind, moduleAccesser, damageLog);
    }
    moduleAccesser->getSituationModule().getKind();
    if (doHitStop == true) {
        moduleAccesser->getStopModule().setHitStopFrame(hitStopFrame, true);
    }
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
