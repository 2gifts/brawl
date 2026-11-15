#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_COLL2D_RHOMBUS_CTOR_NOINLINE // BYTES: the empty rhombus constructor is called out of line here (weak copy in this unit)
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <ft/lucario/ft_lucario.h>
#include <so/so_module_accesser.h>
#include <so/so_kinetic_utility.h>
#include <so/so_value_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <mt/mt_coll2d.h>
#include <math.h>

class soModuleAccesser;

class ftLucarioStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessSpecialLwSplit : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialLwSplit() { }
    virtual void initStatus(soModuleAccesser*);
};

class ftLucarioStatusUniqProcessSpecialLwAppear : public soStatusUniqProcess {
public:
    virtual ~ftLucarioStatusUniqProcessSpecialLwAppear() { }
    virtual void initStatus(soModuleAccesser*);
};

// The start of the move keeps a share of the horizontal speed (constant 0xfc5) and falls with its own gravity.
void ftLucarioStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    Vec2f speed;
    Vec2f::copy(speed, kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    speed.m_x *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfc5, 0);
    stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.enable();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    gravity.m_speedY = 0.0f;
    gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfc6, 0);
    gravity.unk1C = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc7, 0);
    gravity.enable();
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(0, moduleAccesser);
}

void ftLucarioStatusUniqProcessSpecialLwSplit::initStatus(soModuleAccesser* moduleAccesser) {
    ftLucario& fighter = dynamic_cast<ftLucario&>(*moduleAccesser->m_stageObject);
    fighter.effect_suspend();
}

// BYTES: nw4r::math::FSelect is never expanded at the optimization level of the fighter units (-O2,s), so the two
// selects of the clamp are written with the instruction itself, as in the Pikachu units.
static inline float clampRate(register float value) {
    register float one;
    register float zero;
    register float result;
    register float bounded;
    register float difference;
    zero = 0.0f;
    one = 1.0f;
    difference = value - zero;
    asm { fsel bounded, difference, value, zero }
    difference = bounded - one;
    asm { fsel result, difference, one, bounded }
    return result;
}

// Moves Lucario to the spot where he reappears: the stored distance to the stored side, or up to a wall on the way.
static inline void moveToAppearPos(soModuleAccesser* moduleAccesser) {
    soPostureModule& posture = moduleAccesser->getPostureModule();
    Vec3f pos = posture.getPos();
    soGroundModule& ground = moduleAccesser->getGroundModule();
    clRhombus2D prevShape;
    clRhombus2D shape;
    ground.getRhombus(&prevShape, &shape, 0);
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float distance = work.getFloat(0x21000005);
    float direction = work.getFloat(0x21000004);
    Vec3f start(shape.m_centerPos.m_x, shape.m_centerPos.m_y, 0.0f);
    Vec3f move;
    move.m_x = distance * -direction;
    move.m_y = 0.0f;
    move.m_z = 0.0f;
    Vec3f hitPos(0.0f, 0.0f, 0.0f);
    Vec3f hitNormal(0.0f, 0.0f, 0.0f);
    if (ground.rayCheck(&start, &move, &hitPos, &hitNormal, true)) {
        float reach = hitPos.m_x - start.m_x;
        work.setFloat(reach, 0x21000005);
        pos.m_x += reach;
    } else {
        pos.m_x += move.m_x;
    }
    posture.setPos(&pos);
}

// The slide after the reappearance covers the stored distance in 0x5dc5 frames.
static inline void setAppearSlide(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
    int frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc5, 0);
    Vec2f speed(0.0f, 0.0f);
    if (frames != 0) {
        float distance = work.getFloat(0x21000005);
        float direction = work.getFloat(0x21000004);
        speed.m_x = direction * (distance / static_cast<float>(frames));
    }
    stop.m_speedLimit = speed;
    stop.m_speed = speed;
    stop.m_brake = Vec2f(0.0f, 0.0f);
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(0, moduleAccesser);
}

// The reappearance: the stick picks the side and, past the dead zone, how far (between the constants 0xfc8 and
// 0xfc9) Lucario shows up.
void ftLucarioStatusUniqProcessSpecialLwAppear::initStatus(soModuleAccesser* moduleAccesser) {
    ftLucario& fighter = dynamic_cast<ftLucario&>(moduleAccesser->getStageObject());
    soPostureModule& posture = moduleAccesser->getPostureModule();
    float stickX = moduleAccesser->getControllerModule().getStickX();
    float deadZone = soValueAccesser::getConstantFloat(moduleAccesser, 0xc2d, 0);
    float rate;
    float lr;
    float stickAbs = static_cast<float>(fabs(stickX));
    if (stickAbs >= deadZone) {
        rate = (stickAbs - deadZone) / (1.0f - deadZone);
        rate = clampRate(rate);
        lr = stickX > 0.0f ? -1.0f : 1.0f;
    } else {
        rate = 0.0f;
        lr = -1.0f * posture.getLr();
    }
    moduleAccesser->getWorkManageModule().setFloat(lr, 0x21000004);
    posture.setLr(lr);
    posture.updateRotYLr();
    moduleAccesser->getStageObject().updateNodeSRT();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float distanceMin = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc8, 0);
    float distanceMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc9, 0);
    float distance = distanceMin + rate * (distanceMax - distanceMin);
    work.setFloat(distance, 0x21000005);
    moveToAppearPos(moduleAccesser);
    setAppearSlide(moduleAccesser);
    fighter.effect_resume();
}

ftLucarioStatusUniqProcessSpecialLw g_ftLucarioStatusUniqProcessSpecialLw;
ftLucarioStatusUniqProcessSpecialLwSplit g_ftLucarioStatusUniqProcessSpecialLwSplit;
ftLucarioStatusUniqProcessSpecialLwAppear g_ftLucarioStatusUniqProcessSpecialLwAppear;
