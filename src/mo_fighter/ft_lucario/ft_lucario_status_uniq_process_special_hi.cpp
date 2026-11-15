#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// BYTES: Extreme Speed calls the out-of-line Vec2f assignment.
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_COPY_MEMBERWISE // BYTES: Vec2f::copy moves the two floats one by one in this unit (speed_mul_energy)
#include <ft/lucario/ft_lucario.h>
#include <ft/ft_kinetic_energy.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <math.h>
#include <so/so_kinetic_utility.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/turn/so_turn_module.h>
#include <so/motion/so_motion_module_impl.h>
#include <types.h>

class ftLucarioStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void execStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessSpecialHiRush : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialHiRush() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execStatus(soModuleAccesser*);
    virtual void execFixPos(soModuleAccesser*);
    static void turn(soModuleAccesser* moduleAccesser);
    static void set_pose(soModuleAccesser* moduleAccesser);
};

class ftLucarioStatusUniqProcessSpecialHiRushEnd : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialHiRushEnd() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
    virtual void execFixPos(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessSpecialHiBound : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialHiBound() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};

void set_end_param(soModuleAccesser* moduleAccesser);
float set_end_lr(soModuleAccesser* moduleAccesser);
// HYPOTHESIS: the map gives no parameter types; these follow the register use.
bool check_dir(int touchMask, bool attach, float lr, soModuleAccesser* moduleAccesser);
bool check_attach(float lr, soModuleAccesser* moduleAccesser);
Vec2f get_vector(float angle, float length);
void set_landing_status(int frame, float rate, soModuleAccesser* moduleAccesser);
void speed_mul(float x, float y, soModuleAccesser* moduleAccesser);
void speed_mul_energy(soKineticModule* kineticModule, int energy, float x, float y);

extern ftLucarioStatusUniqProcessSpecialHiRush g_ftLucarioStatusUniqProcessSpecialHiRush;

// The map name is Vec2f::dotProduct; the symbol keeps the free-function spelling it has in symbols.txt.
inline float dotProduct(Vec2f* a, Vec2f* b) {
    return a->m_x * b->m_x + a->m_y * b->m_y;
}


// The status the rush leaves for is kept in work int 0x20000001 while the flag 0x22000014 is set.
static inline void setNextStatus(soModuleAccesser* moduleAccesser, int status) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.onFlag(0x22000014);
    work.setInt(status, 0x20000001);
}

static inline void clearNextStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.offFlag(0x22000014);
    work.setInt(-1, 0x20000001);
}

// BYTES: nw4r::math::FSelect is never expanded at the optimization level of the fighter units (-O2,s), so the two
// selects of the clamp are written with the instruction itself, as in the Pikachu units.
static inline float clampRate(register float value, register float low, register float high) {
    register float difference;
    register float bounded;
    register float result;
    difference = value - low;
    asm { fsel bounded, difference, value, low }
    difference = bounded - high;
    asm { fsel result, difference, high, bounded }
    return result;
}

// The start of the move: the horizontal speed is divided by the constant 0xfb8, the fall stops.
void ftLucarioStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
    speed.m_x /= soValueAccesser::getConstantFloat(moduleAccesser, 0xfb8, 0);
    stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.enable();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    gravity.m_speedY = 0.0f;
    gravity.enable();
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(0, moduleAccesser);
    work.setFloat(moduleAccesser->getPostureModule().getLr(), 0x21000004);
    ftLucario::setMachValidity(moduleAccesser, false);
}

// In the air the fall is switched on by the flag 0x22000011 of the animation; the stop energy brakes with 0xfb9.
void ftLucarioStatusUniqProcessSpecialHi::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        if (work.isFlag(0x22000011)) {
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
            gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
        } else {
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
            gravity.m_gravity = 0.0f;
        }
        soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
        stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfb9, 0), 0.0f);
    }
}


// The rush starts along the stick (straight up without one): on the ground along the surface unless the stick
// points away from it, in which case Lucario takes off.
void ftLucarioStatusUniqProcessSpecialHiRush::initStatus(soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float stickX = moduleAccesser->getControllerModule().getStickX();
    float stickY = moduleAccesser->getControllerModule().getStickY();
    float direction = 1.5707964f;
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbd, 0);
    float stickMin = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbc, 0);
    clearNextStatus(moduleAccesser);
    if (static_cast<float>(fabs(stickX)) > soValueAccesser::getConstantFloat(moduleAccesser, 0xfbb, 0)) {
        moduleAccesser->getPostureModule().setStickLR(0.0f);
        moduleAccesser->getPostureModule().updateRotYLr();
    }
    float lr = moduleAccesser->getPostureModule().getLr();
    Vec2f velocity;
    if (situation.getKind() == 0) {
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
        bool takeOff = true;
        float absY = fabs(stickY);
        float absX = fabs(stickX);
        if (!(absX + absY < stickMin)) {
            if (normal.angle(&Vec2f(stickX, stickY)) >= 1.5707964f) {
                takeOff = false;
            }
        }
        if (!takeOff) {
            float slope = atan2f(normal.m_y, normal.m_x);
            float side;
            if (lr > 0.0f) {
                side = -1.0f;
            } else {
                side = 1.0f;
            }
            direction = slope + 1.5707964f * side;
            if (direction >= 3.1415927f) {
                direction -= 6.2831855f;
            } else if (direction <= -3.1415927f) {
                direction += 6.2831855f;
            }
            // A direction within 0.05 rad of level is made exactly level.
            double wide = fabs(direction);
            float magnitude = wide;
            if (magnitude <= 0.05f) {
                direction = 0.0f;
            } else {
                magnitude = 3.1415927f - magnitude;
                magnitude = fabs(magnitude);
                if (magnitude <= 0.05f) {
                    direction = 3.1415927f;
                }
            }
            Vec2f::copy(velocity, get_vector(direction, speed));
        } else {
            situation.setKind(static_cast<SituationKind>(2), false);
            moduleAccesser->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        }
    }
    kinetic.unableEnergy(1);
    kinetic.unableEnergy(0);
    kinetic.unableEnergy(2);
    kinetic.enableEnergy(3);
    if (situation.getKind() == 2) {
        float absY = fabs(stickY);
        float absX = fabs(stickX);
        if (absX + absY >= stickMin) {
            direction = atan2(stickY, stickX);
        }
        Vec2f::copy(velocity, get_vector(direction, speed));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        gravity.m_gravity = 0.0f;
        gravity.disable();
    }
    soGroundModule& ground = moduleAccesser->getGroundModule();
    // The rush follows the ground while it touches it and flies free otherwise; its speeds are capped at 10.
    if (ground.isTouch(8, 0)) {
        ground.setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(1), 0);
        kinetic.changeKinetic(0xd, moduleAccesser);
    } else {
        ground.setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        kinetic.changeKinetic(0xd, moduleAccesser);
    }
    soKineticEnergyNormal& limited = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    limited.m_speedLimit = Vec2f(10.0f, 10.0f);
    limited.m_speedTarget = Vec2f(10.0f, 10.0f);
    ftKineticEnergyGravity& fall = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    fall.unk1C = 10.0f;
    fall.m_fallSpeedMax = 10.0f;
    soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    stop.m_speed = velocity;
    stop.enable();
    work.setFloat(velocity.m_x, 0x21000006);
    work.setFloat(velocity.m_y, 0x21000007);
    work.setFloat(direction, 0x21000005);
    set_pose(moduleAccesser);
}

// The stick steers the flight: its component across the flight direction turns it by up to the constant 0xfbe.
static inline void steer(soModuleAccesser* moduleAccesser) {
    soControllerModule& controller = moduleAccesser->getControllerModule();
    Vec2f stick(controller.getStickX(), controller.getStickY());
    if (stick.length() > 1.1920929e-7f) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        Vec2f velocity(work.getFloat(0x21000006), work.getFloat(0x21000007));
        Vec2f side = get_vector(1.5707964f + atan2f(velocity.m_y, velocity.m_x), 1.0f);
        float amount = dotProduct(&side, &stick);
        float deadZone = soValueAccesser::getConstantFloat(moduleAccesser, 0xc2d, 0);
        float magnitude = static_cast<float>(fabs(amount));
        if (magnitude >= deadZone) {
            soWorkManageModule& turnWork = moduleAccesser->getWorkManageModule();
            float rate = (magnitude - deadZone) / (1.0f - deadZone);
            rate = clampRate(rate, 0.0f, 1.0f);
            if (amount < 0.0f) {
                rate *= -1.0f;
            }
            float current = turnWork.getFloat(0x21000005);
            float turnMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbe, 0);
            rate = current + 0.017453292f * turnMax * rate;
            turnWork.setFloat(rate, 0x21000005);
            soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
            Vec2f newSpeed;
            Vec2f::copy(newSpeed, get_vector(rate, stop.getSpeed().length()));
            stop.m_speed = newSpeed;
        }
    }
}

// After 0x5dc2 frames the rush slows down (once, by the constant 0xfc0) and brakes with 0xfc1.
void ftLucarioStatusUniqProcessSpecialHiRush::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
    int frame = work.getInt(0x20000000);
    if (frame > soValueAccesser::getConstantInt(moduleAccesser, 0x5dc2, 0)) {
        if (!work.isFlag(0x22000013)) {
            float mul = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc0, 0);
            speed_mul(mul, mul, moduleAccesser);
            work.onFlag(0x22000013);
        }
        float brake = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc1, 0);
        stop.m_brake = Vec2f(brake, brake);
    } else {
        stop.m_brake = Vec2f(0.0f, 0.0f);
    }
    steer(moduleAccesser);
    float stickX = moduleAccesser->getControllerModule().getStickX();
    if (static_cast<float>(fabs(stickX)) >= soValueAccesser::getConstantFloat(moduleAccesser, 0xc2d, 0)) {
        float side = stickX > 0.0f ? 1.0f : -1.0f;
        moduleAccesser->getWorkManageModule().setFloat(side, 0x21000008);
    }
}

void ftLucarioStatusUniqProcessSpecialHiRush::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        int frame = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc3, 0);
        set_landing_status(frame, soValueAccesser::getConstantFloat(moduleAccesser, 0xfc3, 0), moduleAccesser);
        set_landing_status(5, 1.0f, moduleAccesser);
    }
}


// Leaves for the kept status; the bound status 0x11d starts without vertical speed.
static inline void changeNextStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x22000014)) {
        int status = work.getInt(0x20000001);
        moduleAccesser->getStatusModule().changeStatusRequest(status, moduleAccesser);
        if (status == 0x11d) {
            speed_mul(1.0f, 0.0f, moduleAccesser);
        }
        clearNextStatus(moduleAccesser);
    }
}

// The rush ends when its motion ends (0x11c), at a ledge (0x73) or against a surface (check_dir).
void ftLucarioStatusUniqProcessSpecialHiRush::execFixPos(soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    if (moduleAccesser->getMotionModule().isEnd()) {
        setNextStatus(moduleAccesser, 0x11c);
    }
    // The rush follows the ground while it touches it and flies free otherwise; its speeds are capped at 10.
    if (ground.isTouch(8, 0)) {
        ground.setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(1), 0);
        kinetic.changeKinetic(0xd, moduleAccesser);
    } else {
        ground.setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
        kinetic.changeKinetic(0xd, moduleAccesser);
    }
    soKineticEnergyNormal& limited = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    limited.m_speedLimit = Vec2f(10.0f, 10.0f);
    limited.m_speedTarget = Vec2f(10.0f, 10.0f);
    ftKineticEnergyGravity& fall = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    fall.unk1C = 10.0f;
    fall.m_fallSpeedMax = 10.0f;
    if (moduleAccesser->getSituationModule().getKind() == 0 && ground.isTouch(8, 0)) {
        Vec2f speed = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3)).getSpeed();
        if (speed.length() > 1.1920929e-7f) {
            Vec2f normal = ground.getTouchNormal(8, 0);
            normal.normalize();
            if (dotProduct(&normal, &speed) > 1.1920929e-7f) {
                moduleAccesser->getSituationModule().setKind(static_cast<SituationKind>(2), false);
            }
        }
    }
    if (ground.isStCliff(0)) {
        setNextStatus(moduleAccesser, 0x73);
    } else if (!check_dir(8, false, 0.0f, moduleAccesser) && !check_dir(2, true, 1.0f, moduleAccesser) &&
               !check_dir(4, true, -1.0f, moduleAccesser)) {
        check_dir(1, false, 0.0f, moduleAccesser);
    }
    changeNextStatus(moduleAccesser);
}

void ftLucarioStatusUniqProcessSpecialHiRushEnd::initStatus(soModuleAccesser* moduleAccesser) {
    set_end_lr(moduleAccesser);
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f brake;
    brake.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc1, 0);
    brake.m_y = 0.0f;
    energy.m_brake = brake;
    energy.enable();
}

void ftLucarioStatusUniqProcessSpecialHiRushEnd::execFixPos(soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if ((ground.isTouch(static_cast<grCollStatus::TouchMask>(2), 0) && check_attach(1.0f, moduleAccesser)) ||
        (ground.isTouch(static_cast<grCollStatus::TouchMask>(4), 0) && check_attach(-1.0f, moduleAccesser))) {
        moduleAccesser->getStatusModule().changeStatusRequest(0x68, moduleAccesser);
    }
}

void ftLucarioStatusUniqProcessSpecialHiRushEnd::exitStatus(soModuleAccesser* moduleAccesser, int) {
    set_end_param(moduleAccesser);
}

void ftLucarioStatusUniqProcessSpecialHiBound::initStatus(soModuleAccesser* moduleAccesser) {
    set_end_lr(moduleAccesser);
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        moduleAccesser->getKineticModule().changeKinetic(6, moduleAccesser);
    } else {
        moduleAccesser->getKineticModule().changeKinetic(0xd, moduleAccesser);
    }
    speed_mul(soValueAccesser::getConstantFloat(moduleAccesser, 0xfc4, 0), 1.0f, moduleAccesser);
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f brake;
    brake.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc1, 0);
    brake.m_y = 0.0f;
    energy.m_brake = brake;
    energy.enable();
}

void ftLucarioStatusUniqProcessSpecialHiBound::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    soStatusUniqProcess& rush = g_ftLucarioStatusUniqProcessSpecialHiRush;
    rush.exitStatus(moduleAccesser, status);
    set_end_param(moduleAccesser);
}

void set_end_param(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setFloat(0.0f, 0x11000000);
    work.setFloat(1.0f, 0x11000001);
    work.onFlag(0x12000003);
}

// Faces the way the fighter was last steered (work float 0x21000008), or its stored facing without one.
float set_end_lr(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float speed = work.getFloat(0x21000008);
    float lr;
    double wide = fabs(speed);
    float magnitude = wide;
    if (magnitude <= 1.1920929e-7f) {
        lr = work.getFloat(0x21000004);
    } else if (speed > 0.0f) {
        lr = 1.0f;
    } else {
        lr = -1.0f;
    }
    moduleAccesser->getPostureModule().setLr(lr);
    moduleAccesser->getPostureModule().updateRotYLr();
    return lr;
}

// The flight meets the touched surface head-on when it comes in steeper than the constant 0xfbf (degrees past 90
// from the surface normal).
static inline bool isHeadOn(int touchMask, soModuleAccesser* moduleAccesser) {
    Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(touchMask, 0);
    Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float angle = normal.angle(&speed);
    return angle >= 0.017453292f * (90.0f + soValueAccesser::getConstantFloat(moduleAccesser, 0xfbf, 0));
}

// The angle from one direction to another in 16-bit units (0x8000 is pi).
static inline int getAngleDifference(const Vec2f& from, Vec2f to) {
    int fromAngle = static_cast<int>(32768.0f * (static_cast<float>(atan2(from.m_y, from.m_x)) / 3.1415927f));
    int toAngle = static_cast<int>(32768.0f * (static_cast<float>(atan2(to.m_y, to.m_x)) / 3.1415927f));
    int difference = fromAngle - toAngle;
    difference = (difference << 16) >> 16; // wrapped to 16 bits
    return difference;
}

// A surface met head-on ends the rush (in a wall cling when allowed, else in the bound status 0x11d); a surface met
// at a shallow angle bends the flight along it by at most the constant 0xfbe.
bool check_dir(int touchMask, bool attach, float lr, soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if (ground.isTouch(touchMask, 0)) {
        if (isHeadOn(touchMask, moduleAccesser)) {
            if (attach && check_attach(lr, moduleAccesser)) {
                setNextStatus(moduleAccesser, 0x68);
            } else {
                setNextStatus(moduleAccesser, 0x11d);
            }
            return true;
        }
        soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
        Vec2f velocity = stop.getSpeed();
        if (velocity.length() > 1.1920929e-7f) {
            Vec2f wall = ground.getTouchNormal(touchMask, 0);
            if (dotProduct(&velocity, &wall) < 0.0f) {
                soWorkManageModule& work = moduleAccesser->getWorkManageModule();
                float turnMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbe, 0);
                int difference = getAngleDifference(velocity, wall);
                int quarter = difference + 0x4000;
                if (difference >= 0) {
                    quarter = difference - 0x4000;
                }
                float delta = 3.1415927f * (static_cast<float>(quarter) / 32768.0f);
                float limit = 0.017453292f * turnMax;
                if (delta >= limit) {
                    delta = -limit;
                } else if (delta <= -limit) {
                    delta = limit;
                } else {
                    delta = -delta;
                }
                float current = work.getFloat(0x21000005);
                float direction = current + delta;
                work.setFloat(direction, 0x21000005);
                Vec2f newSpeed;
                Vec2f::copy(newSpeed, get_vector(direction, velocity.length()));
                stop.m_speed = newSpeed;
            }
        }
    }
    return false;
}

// The wall can be clung to when its slope is between the constants 0xc9d and 0xc9c.
static inline bool isAttachAngle(soModuleAccesser* moduleAccesser, float angle) {
    if (angle >= soValueAccesser::getConstantFloat(moduleAccesser, 0xc9c, 0) ||
        angle <= soValueAccesser::getConstantFloat(moduleAccesser, 0xc9d, 0)) {
        return false;
    }
    return true;
}

// The slope in degrees of the wall touched on the given side (90 for an upright wall).
static inline float getWallSlope(soModuleAccesser* moduleAccesser, float lr) {
    Vec2f normal(0.0f, 1.0f);
    float sign;
    if (lr >= 0.0f) {
        Vec2f::copy(normal, moduleAccesser->getGroundModule().getTouchNormal(2, 0));
        sign = -1.0f;
    } else {
        Vec2f::copy(normal, moduleAccesser->getGroundModule().getTouchNormal(4, 0));
        sign = 1.0f;
    }
    return 90.0f + 57.29578f * (sign * atan2f(normal.m_x, normal.m_y));
}

// The rush ends in a wall cling when the stick is held away from a wall of the right slope on the given side.
bool check_attach(float lr, soModuleAccesser* moduleAccesser) {
    soGroundModule& ground = moduleAccesser->getGroundModule();
    int clingCount = moduleAccesser->getWorkManageModule().getInt(0x1000000b);
    float stickMin = soValueAccesser::getConstantFloat(moduleAccesser, 0xc97, 0);
    float stickX = moduleAccesser->getControllerModule().getStickX();
    if (clingCount <= 0 && static_cast<float>(fabs(stickX)) >= stickMin && stickX * lr < 0.0f) {
        if (isAttachAngle(moduleAccesser, getWallSlope(moduleAccesser, lr))) {
            if (!ground.isTouch(1, 0)) {
                if (lr >= 0.0f) {
                    if (ground.isAttachable(2, 0)) {
                        return true;
                    }
                } else {
                    if (ground.isAttachable(4, 0)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void set_landing_status(int frame, float rate, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setFloat(frame, 0x11000000);
    moduleAccesser->getWorkManageModule().setFloat(rate, 0x11000001);
    moduleAccesser->getWorkManageModule().onFlag(0x12000003);
}

static inline soTurnModule* getTurnModule(const soModuleEnumeration& modules) {
    soTurnModule* turnModule = static_cast<soTurnModule*>(modules.m_turnModule);
    return turnModule;
}

// When a turn ends the flight direction is mirrored and the rush restarts with the speed constant 0xfbd.
void ftLucarioStatusUniqProcessSpecialHiRush::turn(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration& modules = *moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules.m_workManageModule;
    soTurnModule* turnModule = getTurnModule(modules);
    soKineticModule& kinetic = *modules.m_kineticModule;
    bool turning = turnModule->isTurn();
    bool wasTurning = work.isFlag(0x22000015);
    work.setFlag(turning, 0x22000015);
    if (turning != wasTurning && !turning) {
        float direction = work.getFloat(0x21000005);
        float y = sin(direction);
        float x = cos(direction);
        direction = atan2(y, -x);
        // A direction within 0.05 rad of level is made exactly level.
        double wide = fabs(direction);
        float magnitude = wide;
        if (magnitude <= 0.05f) {
            direction = 0.0f;
        } else {
            magnitude = 3.1415927f - magnitude;
            magnitude = fabs(magnitude);
            if (magnitude <= 0.05f) {
                direction = 3.1415927f;
            }
        }
        work.setFloat(direction, 0x21000005);
        Vec2f speed;
        Vec2f::copy(speed, get_vector(direction, soValueAccesser::getConstantFloat(moduleAccesser, 0xfbd, 0)));
        kinetic.clearSpeedAll();
        kinetic.unableEnergyAll();
        soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
        stop.m_speed = speed;
        stop.enable();
        float lr = work.getFloat(0x21000004);
        work.setFloat(-lr, 0x21000004);
        float speedX = work.getFloat(0x21000008);
        work.setFloat(-speedX, 0x21000008);
        float stickX = work.getFloat(0x21000006);
        work.setFloat(-stickX, 0x21000006);
    }
}

// Leans the two body nodes 0x12e and 0x12f into the flight direction (work float 0x21000005, radians).
void ftLucarioStatusUniqProcessSpecialHiRush::set_pose(soModuleAccesser* moduleAccesser) {
    float direction = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    double wide = fabs(direction);
    float magnitude = wide;
    float lean;
    float tilt;
    float lr;
    double wideOff = fabs(magnitude - 1.5707964f);
    float off = wideOff;
    if (off > 1.1920929e-7f) {
        if (magnitude < 1.5707964f) {
            lean = 1.5707964f - direction;
            tilt = 1.5707964f + -direction;
            lr = 1.0f;
        } else {
            if (direction > 0.0f) {
                lean = 1.5707964f + -direction;
                tilt = -(direction - 1.5707964f);
            } else {
                lean = -direction - 4.712389f;
                tilt = lean;
            }
            lr = 1.0f;
        }
    } else if (direction > 0.0f) {
        lean = 0.0f;
        tilt = lean;
        lr = 1.0f;
    } else {
        lean = 3.1415927f;
        tilt = lean;
        lr = 1.0f;
    }
    soModelModule& model = moduleAccesser->getModelModule();
    int leanNode = model.getCorrectNodeId(0x12e);
    int tiltNode = model.getCorrectNodeId(0x12f);
    Vec3f leanRot;
    leanRot.m_x = 57.29578f * lean;
    leanRot.m_y = 0.0f;
    leanRot.m_z = 0.0f;
    model.setNodeRotate(leanNode, &leanRot);
    Vec3f tiltRot;
    tiltRot.m_x = 0.0f;
    tiltRot.m_y = 57.29578f * tilt;
    tiltRot.m_z = 0.0f;
    model.setNodeRotate(tiltNode, &tiltRot);
    if (!static_cast<soTurnModule*>(moduleAccesser->m_enumerationStart->m_turnModule)->isTurn() && 0.0f != lr) {
        moduleAccesser->getPostureModule().setLr(lr);
        moduleAccesser->getPostureModule().updateRotYLr();
    }
    turn(moduleAccesser);
}

void speed_mul(float x, float y, soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    speed_mul_energy(&kinetic, 3, x, y);
    speed_mul_energy(&kinetic, 2, x, y);
    speed_mul_energy(&kinetic, 0, x, y);
    speed_mul_energy(&kinetic, 1, x, y);
}

// Scales the speed of one kinetic energy (the gravity energy only has a vertical speed).
void speed_mul_energy(soKineticModule* kineticModule, int energy, float x, float y) {
    switch (energy) {
    case 1: {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kineticModule->getEnergy(energy));
        Vec2f speed;
        Vec2f::copy(speed, gravity.getSpeed());
        gravity.m_speedY = speed.m_y * y;
        break;
    }
    default: {
        soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*kineticModule->getEnergy(energy));
        Vec2f speed;
        Vec2f::copy(speed, normal.getSpeed());
        speed.m_x *= x;
        speed.m_y *= y;
        normal.m_speed = speed;
        break;
    }
    }
}

Vec2f get_vector(float angle, float length) {
    Vec2f dir;
    dir.m_x = cos(angle);
    dir.m_y = sin(angle);
    double wide = fabs(dir.m_y);
    float magnitude = wide;
    if (magnitude <= 0.01f) {
        dir.m_y = 0.0f;
    }
    Vec2f vector;
    vector.m_x = dir.m_x * length;
    vector.m_y = dir.m_y * length;
    return vector;
}

ftLucarioStatusUniqProcessSpecialHi g_ftLucarioStatusUniqProcessSpecialHi;
ftLucarioStatusUniqProcessSpecialHiRush g_ftLucarioStatusUniqProcessSpecialHiRush;
ftLucarioStatusUniqProcessSpecialHiRushEnd g_ftLucarioStatusUniqProcessSpecialHiRushEnd;
ftLucarioStatusUniqProcessSpecialHiBound g_ftLucarioStatusUniqProcessSpecialHiBound;

// The unit's read-only data ends with one more copy of pi/2 that no code reads: a constant with external linkage
// defined after the code (the code's own uses come from the literal pool).
// HYPOTHESIS: the name; the value is the upward direction the rush starts with.
extern const float g_ftLucarioSpecialHiRushUpDirection = 1.5707964f;
