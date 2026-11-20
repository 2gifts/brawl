#include <mt/mt_vector.h>
#include <math.h>
#include <types.h>

class soModuleAccesser;
float rsqrtf(float);

static inline float getLength(const Vec2f* v) {
    float lenSq = v->m_x * v->m_x + v->m_y * v->m_y;
    if ((float)fabs(lenSq) <= 1.1754944e-38f) {
        return 0.0f;
    }
    return lenSq * rsqrtf(lenSq);
}

// Map name soDamageUtil::getDamageAngle: the launch angle (radians) of a hit. Angles 363 and 365 follow the attacker's
// speed, 361 (the "Sakurai angle") is param0 for airborne targets and otherwise grows with the reaction from 1 up to
// param3 degrees between the reactions param1 and param2.
// HYPOTHESIS: parameter names; unk mirrors the result.
// TODO: clean up: keeps its placeholder name because sora_enemy and so_damage_util_actor.cpp call it by that name.
extern "C" float fn_27_60AF4(soModuleAccesser* moduleAccesser, float reaction, int attackVector, int situation, Vec2f* speed,
                             float lr, float param0, float param1, float param2, float param3, bool unk) {
    if (attackVector == 363 || attackVector == 365) {
        if (!(getLength(speed) <= 0.0f)) {
            float deg = 57.29578f * (float)atan2(speed->m_y, speed->m_x * -lr);
            if (deg < 0.0f) {
                deg += 360.0f;
            }
            return 0.017453292f * deg;
        }
    } else if (attackVector != 361) {
        return 0.017453292f * attackVector;
    } else if (situation != 0) {
        return param0;
    }
    if (reaction < param1) {
        return 0.0f;
    }
    float num = reaction - param1;
    float den = param2 - param1;
    float angle = 0.017453292f * (1.0f + param3 * (num / den));
    if (angle > 0.017453292f * param3) {
        angle = 0.017453292f * param3;
    }
    if (unk == true) {
        angle = 1.5707964f - angle;
    }
    return angle;
}
