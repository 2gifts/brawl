#pragma once

#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;
class soKineticEnergy;

// Shared braking, component limits and ground-relative velocity conversions.
class soKineticUtility {
public:
    static float brakeSpeedSub(float speed, float accel, float target, float brake);
    static float brakeSpeed(float speed, float accel, float target, float brake);
    static Vec2f brakeSpeed(const Vec2f& speed, const Vec2f& accel, const Vec2f& target, const Vec2f& brake);
    static Vec3f brakeSpeed(const Vec3f& speed, const Vec3f& accel, const Vec3f& target, const Vec3f& brake);
    static Vec2f limitSpeed(const Vec2f& speed, const Vec2f& limit);
    static float limitSpeed(float speed, float limit);
    static Vec3f limitSpeed(const Vec3f& speed, const Vec3f& limit);
    static Vec2f projectionNormalFollow(const Vec2f& vec, const Vec2f& normal);
    static Vec2f projectionGroundSpeed(const Vec2f& speed, soModuleAccesser* moduleAccesser);
    static void addSpeedOutsideMovementGround(soKineticEnergy* energy, Vec3f* speed, soModuleAccesser* moduleAccesser);
    static void resetEnableEnergy(int energyIndex, soModuleAccesser* moduleAccesser, int resetMode, Vec2f* speed, Vec3f* rotation);
};

// Clears velocity, then fetches the indexed energy again before disabling it.
void ftKineticEnergyDisableAndClear(int energyIndex, soModuleAccesser* moduleAccesser);
