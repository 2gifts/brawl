#define MT_VEC3F_MUL_INLINE
#include <ft/ft_util.h>
#include <math.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <mt/mt_common.h>
#include <so/so_external_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <it/it_create.h>
#include <so/kinetic/so_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_simple.h>
#include <so/so_value_accesser.h>

// Tilts the fighter model around the X axis (node 0x12E, HYPOTHESIS: the model's root of the tilt), e.g. while sliding on a slope.
void ftUtil::setXAngle(soModuleAccesser* acc, float angle) {
    u32 node = acc->getModelModule().getCorrectNodeId(0x12E);
    if (node != -1) {
        Vec3f rotation(angle, 0.0f, 0.0f);
        acc->getModelModule().setNodeRotate(node, &rotation);
    }
}

// Tilts the model to lie along the knockback direction of a fighter tumbling in a damage fly roll. Returns the angle in
// degrees (0 when the fighter is too slow to define a direction).
float ftUtil::setDamageFlyRollAngle(soModuleAccesser* acc) {
    float lr = acc->getPostureModule().getLr();
    float angle = 0.0f;
    Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(-1));
    if (speed.length() > 0.00001f) {
        float radians = atan2(speed.m_x, speed.m_y);
        angle = 57.29578f * (lr * radians);
        setXAngle(acc, angle);
    }
    return angle;
}

// Tilts the model to lie along the wall (or ceiling) the fighter touches. Returns the angle in degrees.
// HYPOTHESIS: the parameter is the facing / side the tilt follows.
float ftUtil::setAngleWallNormal(soModuleAccesser* acc, float lr) {
    Vec2f rotated;
    Vec2f normal(0.0f, 1.0f);
    if (acc->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(4), 0)) {
        normal = acc->getGroundModule().getTouchNormal(4, 0);
    } else if (acc->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(2), 0)) {
        normal = acc->getGroundModule().getTouchNormal(2, 0);
    }
    normal.rot(&rotated, 0.017453292f * (90.0f * lr));
    float y = rotated.m_y;
    float x = rotated.m_x;
    float radians = atan2(x, y);
    float angle = 57.29578f * (lr * radians);
    setXAngle(acc, angle);
    return angle;
}

// Whether the fighter's body is above its feet (the sign of the horizontal offset between the centre of two body nodes
// and node 0x12D, times the facing). HYPOTHESIS: the node meanings.
bool ftUtil::isBodyUp(soModuleAccesser* acc) {
    Vec3f base;
    Vec3f left;
    Vec3f right;
    base = acc->getModelModule().getNodeGlobalPosition(acc->getModelModule().getCorrectNodeId(0x12D), false);
    left = acc->getModelModule().getNodeGlobalPosition(acc->getModelModule().getCorrectNodeId(0x132), false);
    right = acc->getModelModule().getNodeGlobalPosition(acc->getModelModule().getCorrectNodeId(0x133), false);
    Vec3f offset;
    offset = left + right;
    offset = offset * 0.5f;
    offset = offset - base;
    return offset.m_x * acc->getPostureModule().getLr() < 0.0f;
}

// Applies a "scaling" to a value, used for the knockback / damage multipliers. A negative exponent is the inverse of the
// positive one. For a positive exponent n and factor s the result is value * (1 + n * (s - 1)) (linear interpolation of the
// multiplier) unless the factor is below 1 and n is above 1, where it is value * s / n instead.
// HYPOTHESIS: meaning of the parameters; the formula is read from the code.
float ftUtil::calcScalingParam(float value, float factor, float exponent) {
    if (exponent == 0.0f) {
        return value;
    }
    if (exponent < 0.0f) {
        return value / calcScalingParam(1.0f, factor, -exponent);
    }
    if (factor >= 1.0f || exponent <= 1.0f) {
        return value + exponent * ((factor - 1.0f) * value);
    }
    return value * factor / exponent;
}

// In adventure mode the speed multipliers grow with the distance to the main unit's fighter: 1 inside the near distance
// (or when it is not the main unit), rising linearly up to the maximum multiplier at the far distance.
float ftUtil::calcAdventureMulValue(soModuleAccesser* acc, u32 nearDistanceId, u32 farDistanceId, u32 maxMulId) {
    if (g_ftManager->getOwner(acc->getWorkManageModule().getInt(0x10000000))->getAdventureUnitKind() == 1) {
        int entryId = g_ftManager->getEntryId(0);
        if (g_ftManager->isValidEntryId(entryId) == true) {
            float nearDistance = soValueAccesser::getConstantFloat(acc, nearDistanceId, 0);
            if (nearDistance != 0.0f) {
                Vec3f target = soExternalValueAccesser::getPos(g_ftManager->getFighter(entryId, -1));
                Vec3f offset = target - acc->getPostureModule().getPos();
                float distanceSq = offset.m_x * offset.m_x + offset.m_y * offset.m_y;
                if (distanceSq > nearDistance * nearDistance) {
                    float distance = mtSqrtf(distanceSq);
                    float farDistance = soValueAccesser::getConstantFloat(acc, farDistanceId, 0);
                    float maxMul = soValueAccesser::getConstantFloat(acc, maxMulId, 0);
                    if (distance > farDistance) {
                        return maxMul;
                    }
                    return 1.0f + (maxMul - 1.0f) * ((distance - nearDistance) / (farDistance - nearDistance));
                }
            }
        }
    }
    return 1.0f;
}

bool ftUtil::isItemShootStatus(soModuleAccesser*, int status) {
    switch (status) {
    case 0xA4:
    case 0xA5:
    case 0xA6:
    case 0xA7:
    case 0xA8:
    case 0xA9:
    case 0xAA:
    case 0xAB:
    case 0xAC:
    case 0xAD:
    case 0xAE:
        return true;
    default:
        return false;
    }
}

bool ftUtil::isHammerStatus(soModuleAccesser*, int status) {
    switch (status) {
    case 0xDF:
    case 0xE0:
    case 0xE1:
    case 0xE2:
    case 0xE3:
    case 0xE4:
    case 0xE5:
        return true;
    default:
        return false;
    }
}

bool ftUtil::isHammerMotion(soModuleAccesser*, int motion) {
    switch (motion) {
    case 0x141:
    case 0x142:
    case 0x143:
        return true;
    default:
        return false;
    }
}

bool ftUtil::isSleepStatus(soModuleAccesser*, int status) {
    switch (status) {
    case 0x5C:
    case 0x5D:
    case 0x5E:
        return true;
    default:
        return false;
    }
}

bool ftUtil::isBindStatus(soModuleAccesser*, int status) {
    switch (status) {
    case 0x5F:
        return true;
    default:
        return false;
    }
}

bool ftUtil::isBuryStatus(soModuleAccesser*, int status) {
    switch (status) {
    case 0xC7:
    case 0xC8:
        return true;
    default:
        return false;
    }
}

// HYPOTHESIS: ftManager byte 0x68 is the adventure-mode (Subspace Emissary) flag.
float ftUtil::getWalkSpeedMul(soModuleAccesser* acc) {
    float mul = acc->getParamCustomizeModule().getWalkSpeedMul();
    if ((u8)g_ftManager->m_mode == 1) {
        mul *= calcAdventureMulValue(acc, 0xD25, 0xD27, 0xD26);
    }
    return mul;
}

float ftUtil::getRunSpeedMul(soModuleAccesser* acc) {
    float mul = 1.0f;
    if ((u8)g_ftManager->m_mode == 1) {
        mul *= calcAdventureMulValue(acc, 0xD25, 0xD27, 0xD26);
    }
    return mul;
}

float ftUtil::getJumpSpeedMul(soModuleAccesser* acc) {
    float mul = 1.0f;
    if ((u8)g_ftManager->m_mode == 1) {
        mul *= calcAdventureMulValue(acc, 0xD28, 0xD2A, 0xD29);
    }
    return mul;
}

float ftUtil::getWeightReactionMul(soModuleAccesser* acc) {
    float weight = soValueAccesser::getConstantFloat(acc, 0xBE1, 0) * 0.01f;
    return 2.0f - (weight * 2.0f) / (weight + 1.0f);
}

float ftUtil::getAttackReactionMul(soModuleAccesser* acc) {
    return soValueAccesser::getConstantFloat(acc, 0xDB0, 0);
}

float ftUtil::getAttackPowerMul(soModuleAccesser* acc) {
    return soValueAccesser::getConstantFloat(acc, 0xDAE, 0);
}

float ftUtil::getDamageMul(soModuleAccesser* acc) {
    return soValueAccesser::getConstantFloat(acc, 0xDAF, 0);
}

void ftUtil::setCloudThroughCheck(soModuleAccesser* acc) {
    float limit = soValueAccesser::getConstantFloat(acc, 0xCDB, 0);
    if ((float) fabs(acc->getKineticModule().getEnergy(4)->getSpeed().m_y) > limit) {
        acc->getGroundModule().setCloudThrough(true, 0);
        return;
    }
    acc->getGroundModule().setCloudThrough(false, 0);
}

// Statuses a Zako (Subspace Emissary enemy fighter) cannot enter: it has no tech, for example (HYPOTHESIS: the
// individual statuses are the ones the common status table marks as unused by Zako).
bool ftUtil::isValidStatusKindZako(soModuleAccesser*, int status) {
    switch (status) {
    case 0x1A:
    case 0x1B:
    case 0x1E:
    case 0x1F:
    case 0x20:
    case 0x21:
    case 0x4F:
    case 0x60:
    case 0x61:
    case 0x62:
    case 0x63:
    case 0x64:
    case 0x67:
    case 0x73:
    case 0x74:
    case 0x96:
    case 0x97:
    case 0x10C:
    case 0x10F:
    case 0x110:
    case 0x112:
    case 0x113:
    case 0x114:
    case 0x115:
        return false;
    default:
        return true;
    }
}

// HYPOTHESIS: work flag 0x12000039 lifts the limit on operated speed.
bool ftUtil::isEnableSpeedOperation(soModuleAccesser* acc) {
    if (acc->getWorkManageModule().isFlag(0x12000039) == false) {
        float limit = soValueAccesser::getConstantFloat(acc, 0xCE2, 0);
        float speedSq = acc->getKineticModule().getEnergy(4)->getSpeed().lengthSq();
        float limitSq = limit * limit;
        if (speedSq > limitSq)
            return false;
    }
    return true;
}

int ftUtil::getItemWaitMotion(soModuleAccesser* acc) {
    switch (acc->getItemManageModule().getHaveItemHoldKind(0)) {
    case itParam::Have_Normal:
    case itParam::Have_Pickup:
    case itParam::Have_Plate:
        if (acc->getItemManageModule().getHaveItemKind(0) != Item_MotionSensorBomb)
            return 5;
        break;
    case itParam::Have_Grip:
        switch (acc->getStageObject().soGetSubKind()) {
        case Fighter_KingDedede:
            return 5;
        }
        break;
    }
    return -1;
}

int ftUtil::getItemSquatWaitMotion(soModuleAccesser* acc) {
    switch (acc->getItemManageModule().getHaveItemHoldKind(0)) {
    case itParam::Have_Normal:
    case itParam::Have_Pickup:
    case itParam::Have_Plate:
        if (acc->getItemManageModule().getHaveItemKind(0) != Item_MotionSensorBomb)
            return 0x2D;
        break;
    case itParam::Have_Grip:
        switch (acc->getStageObject().soGetSubKind()) {
        case Fighter_KingDedede:
            return 0x2D;
        }
        break;
    }
    return -1;
}

void ftUtil::getStickXRate(soModuleAccesser* acc, float threshold, Vec2f* out) {
    float stick = acc->getControllerModule().getStickX();
    if ((float) fabs(stick) < threshold) {
        out->m_x = 0.0f;
        out->m_y = 0.0f;
    } else if (stick > 0.0f) {
        out->m_y = 1.0f;
        out->m_x = (stick - threshold) / (1.0f - threshold);
    } else {
        float numerator = (float) fabs(stick) - threshold;
        float denominator = 1.0f - threshold;
        out->m_y = -1.0f;
        out->m_x = numerator / denominator;
    }
}

void ftUtil::getStickYRate(soModuleAccesser* acc, float threshold, Vec2f* out) {
    float stick = acc->getControllerModule().getStickY();
    if ((float) fabs(stick) < threshold) {
        out->m_x = 0.0f;
        out->m_y = 0.0f;
    } else if (stick > 0.0f) {
        out->m_y = 1.0f;
        out->m_x = (stick - threshold) / (1.0f - threshold);
    } else {
        float numerator = (float) fabs(stick) - threshold;
        float denominator = 1.0f - threshold;
        out->m_y = -1.0f;
        out->m_x = numerator / denominator;
    }
}

// HYPOTHESIS: the rumble kinds 6 (taking damage) and 0xB (dealing damage) select entries of the rumble table.
void ftUtil::setDamageRumble(soModuleAccesser* acc, float damage) {
    int amount = static_cast<int>(1.3f * damage);
    if (amount >= 1) {
        acc->getControllerModule().setRumble(6, amount, true, -1);
    }
}

void ftUtil::setAttackRumble(soModuleAccesser* acc, float power) {
    int amount = static_cast<int>(5.0f + 0.5f * power);
    if (amount >= 1) {
        acc->getControllerModule().setRumble(0xB, amount, true, -1);
    }
}

// Whether a collision was a hit on the shield of a fighter that can counter with it (Kirby, Peach, Marth, Lucario, Ike).
// HYPOTHESIS: log fields 0x21 == 2 mark a shield contact and 0x23 a valid collision part.
bool ftUtil::isCounterShield(soCollisionLog* collisionLog) {
    gfTask* task;
    if ((u8) collisionLog->_33 != 2) {
        return false;
    }
    if ((u8) collisionLog->_35 == 0) {
        return false;
    }
    task = gfTask::getTask(collisionLog->m_taskId);
    if (task == NULL) {
        return false;
    }
    StageObject& stageObject = dynamic_cast<StageObject&>(*task);
    if (stageObject.soGetKind() != StageObject_Fighter) {
        return false;
    }
    switch (stageObject.soGetSubKind()) {
    default:
        return false;
    case Fighter_Kirby:
    case Fighter_Peach:
    case Fighter_Marth:
    case Fighter_Lucario:
    case Fighter_Ike:
        return true;
    }
}

// Pushes the attacker back when its hit is set off by a shield: the speed grows with the attack power and is limited by a
// common constant. The direction is mirrored by the facing.
// HYPOTHESIS: parameter meanings and the constant roles.
void ftUtil::setShieldSetoffSpeed(soModuleAccesser* acc, float power, float lr) {
    float base = power * (1.0f - soValueAccesser::getConstantFloat(acc, 0xCB1, 0)) * soValueAccesser::getConstantFloat(acc, 0xCAA, 0);
    float speed = soValueAccesser::getConstantFloat(acc, 0xCAB, 0) + base;
    speed *= soValueAccesser::getConstantFloat(acc, 0xCAC, 0);
    if (speed > soValueAccesser::getConstantFloat(acc, 0xCAD, 0)) {
        speed = soValueAccesser::getConstantFloat(acc, 0xCAD, 0);
    }
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(4));
    Vec2f velocity(speed * -lr, 0.0f);
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    energy.resetEnergy(0xB, &velocity, &rotation, acc);
    energy.enable();
}
