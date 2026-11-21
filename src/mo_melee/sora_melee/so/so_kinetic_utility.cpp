#define MT_VEC3F_LENGTH_SQ_PLAIN
#define MT_VEC2F_LENGTH_SQ_CONST
#include <so/so_kinetic_utility.h>
#include <ft/ft_kinetic_energy.h>
#include <math.h>
#include <revolution/OS.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>

extern const char lbl_27_data_13590[];

float soKineticUtility::brakeSpeed(float speed, float accel, float target, float brake) {
    return brakeSpeedSub(speed, accel, target, brake);
}

Vec2f soKineticUtility::brakeSpeed(const Vec2f& speed, const Vec2f& accel, const Vec2f& target, const Vec2f& brake) {
    Vec2f result = speed;
    result.m_x = brakeSpeedSub(speed.m_x, accel.m_x, target.m_x, brake.m_x);
    result.m_y = brakeSpeedSub(speed.m_y, accel.m_y, target.m_y, brake.m_y);
    return result;
}

Vec3f soKineticUtility::brakeSpeed(const Vec3f& speed, const Vec3f& accel, const Vec3f& target, const Vec3f& brake) {
    Vec3f result = speed;
    result.m_x = brakeSpeedSub(speed.m_x, accel.m_x, target.m_x, brake.m_x);
    result.m_y = brakeSpeedSub(speed.m_y, accel.m_y, target.m_y, brake.m_y);
    result.m_z = brakeSpeedSub(speed.m_z, accel.m_z, target.m_z, brake.m_z);
    return result;
}

// MATCH-ONLY: vector overloads and the scalar wrapper retain calls to this helper.
float soKineticUtility::brakeSpeedSub(float speed, float accel, float target, float brake)
    __attribute__((never_inline)) {
    // MATCH-ONLY: retain the native guard/prediction temporary lifetimes.
    float next = 0.0f;
    if (target >= next && brake > next) {
        next = speed + accel;
        if (next > target) {
            if (target == 0.0f) accel = -brake;
            else if (accel > -brake) accel = -brake;
            if (speed + accel < target) accel = target - speed;
        } else if (next < -target) {
            if (target == 0.0f) accel = brake;
            else if (accel < brake) accel = brake;
            if (speed + accel > -target) accel = -target - speed;
        }
        return accel;
    }
    return accel;
}

float soKineticUtility::limitSpeed(float speed, float limit) {
    if (speed != 0.0f && limit >= 0.0f) {
        double absolute = fabs(speed);
        float magnitude = absolute;
        if (limit < magnitude) speed = limit * (speed > 0.0f ? 1.0f : -1.0f);
    }
    return speed;
}

Vec2f soKineticUtility::limitSpeed(const Vec2f& speed, const Vec2f& limit) {
    Vec2f result = speed;
    result.m_x = limitSpeed(speed.m_x, limit.m_x);
    result.m_y = limitSpeed(speed.m_y, limit.m_y);
    return result;
}

Vec3f soKineticUtility::limitSpeed(const Vec3f& speed, const Vec3f& limit) {
    Vec3f result = speed;
    result.m_x = limitSpeed(speed.m_x, limit.m_x);
    result.m_y = limitSpeed(speed.m_y, limit.m_y);
    result.m_z = limitSpeed(speed.m_z, limit.m_z);
    return result;
}

static inline Vec2f projectOnto(const Vec2f& vec, const Vec2f& direction) {
    Vec2f unit = direction;
    unit.normalize();
    float along = vec.m_x * unit.m_x + vec.m_y * unit.m_y;
    return Vec2f(along * unit.m_x, along * unit.m_y);
}

Vec2f soKineticUtility::projectionNormalFollow(const Vec2f& speed, const Vec2f& normal) {
    if (speed.lengthSq() == 0.0f || normal.lengthSq() == 0.0f) return speed;
    Vec2f rotated = normal;
    rotated.rot(1.5707964f, &rotated);
    return projectOnto(speed, rotated);
}

static inline soGroundModule& groundOf(soModuleAccesser* acc) { soGroundModule* const p = &acc->getGroundModule(); return *p; }
static inline void reportNoGround(const char* taskName, soStatusModule* status) {
    OSReport(lbl_27_data_13590, taskName, status->getPrevStatusKind(0), status->getStatusKind());
}
Vec2f soKineticUtility::projectionGroundSpeed(const Vec2f& speed, soModuleAccesser* acc) {
    Vec2f result = speed;
    if (result.m_y != 0.0f) {
        soGroundModule& ground = groundOf(acc);
        if (!ground.isTouch(8, 0)) {
            reportNoGround(acc->getStageObject().m_taskName, &acc->getStatusModule());
        } else {
            result = projectionNormalFollow(result, ground.getTouchNormal(8, 0));
        }
        // Ground energies store signed speed along the surface in X, rather than world XY.
        float length = result.length();
        if (length != 0.0f) {
            result.m_x = result.m_x > 0.0f ? length : -length;
            result.m_y = 0.0f;
        }
    }
    return result;
}

void soKineticUtility::resetEnableEnergy(int index, soModuleAccesser* acc, int mode,
                                       Vec2f* speed, Vec3f* rotation) {
    acc->getKineticModule().getEnergy(index)->resetEnergy(mode, *speed, *rotation, acc);
    acc->getKineticModule().getEnergy(index)->enable();
}

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* acc) {
    acc->getKineticModule().getEnergy(index)->clearSpeed();
    acc->getKineticModule().getEnergy(index)->disable();
}

static inline void addOutside(soKineticEnergyGroundMovement* ground, float x, float y, float z) {
    ground->m_outsideSpeed.m_x += x;
    ground->m_outsideSpeed.m_y += y;
    ground->m_rotation += z;
}
static inline void addSpeedOutsideGroundMovement(soKineticEnergyGroundMovement* ground, Vec3f* speed,
                                                 soModuleAccesser* acc) {
    if (ground == NULL || !ground->isEnable() || speed->lengthSq() == 0.0f) return;
    if (acc->getSituationModule().getKind() != Situation_Ground) return;

    Vec2f planar(0.0f, 0.0f);
    Vec2f normal = acc->getGroundModule().getTouchNormal(8, 0);
    planar = soKineticUtility::projectionNormalFollow(Vec2f(speed->m_x, 0.0f), normal);
    if (planar.lengthSq() != 0.0f) {
        addOutside(ground, planar.m_x, planar.m_y, 0.0f);
    }
}

void soKineticUtility::addSpeedOutsideMovementGround(soKineticEnergy* energy, Vec3f* speed,
                                                   soModuleAccesser* acc) {
    if (!energy->isEnable() || speed->lengthSq() == 0.0f) return;
    addSpeedOutsideGroundMovement(dynamic_cast<soKineticEnergyGroundMovement*>(energy), speed, acc);
}
