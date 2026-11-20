#include <ft/ganon/ft_ganon_status_uniq_process_ganon_punch.h>
#include <ft/ft_kinetic_energy.h>
#include <math.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// TODO: clean up - the work ids below are bare numbers; name them once the Warlock Punch work layout is known.
// HYPOTHESIS: 0x20000000 is the punch variant (0 ground, 1 air charge, 2 air), 0x20000002 the last situation,
// 0x21000004 the facing, 0x22000012..0x22000015 one-shot flags.

// MATCH-ONLY: the original inlines these members here without emitting an out-of-line copy.
inline void soKineticEnergyNormal::setSpeed(Vec2f* speed) {
    m_speed.m_x = speed->m_x;
    m_speed.m_y = speed->m_y;
}
inline void soKineticEnergyNormal::setStableSpeed(Vec2f* speed) {
    m_speedTarget.m_x = speed->m_x;
    m_speedTarget.m_y = speed->m_y;
}
inline void soKineticEnergyNormal::setBrake(Vec2f* brake) {
    m_brake.m_x = brake->m_x;
    m_brake.m_y = brake->m_y;
}

static void updateAirSpeed(soModuleAccesser* moduleAccesser);
static void applyPunchEnd(soModuleAccesser* moduleAccesser);

void ftGanonStatusUniqProcessGanonPunch::initStatus(soModuleAccesser* moduleAccesser) {
    float lr = moduleAccesser->getPostureModule().getLr();
    moduleAccesser->getWorkManageModule().setFloat(lr, 0x21000004);
}

void ftGanonStatusUniqProcessGanonPunch::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        applyPunchEnd(moduleAccesser);
    }
}

void ftGanonStatusUniqProcessGanonPunchTurn::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftGanonStatusUniqProcessGanonPunchTurn::execStatus(soModuleAccesser* moduleAccesser) {
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int kind = situation.getKind();
    int prevKind = work.getInt(0x20000002);
    work.setInt(kind, 0x20000002);
    if (kind != prevKind) {
        ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
        float facing = work.getFloat(0x21000004);
        if (facing != motion.m_facing) {
            motion.m_prevMotionSpeed.m_x = -motion.m_prevMotionSpeed.m_x;
            motion.m_facing = facing;
        }
    }
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        applyPunchEnd(moduleAccesser);
    }
}

static void applyPunchEnd(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    updateAirSpeed(moduleAccesser);
    switch (work.getInt(0x20000000)) {
    case 0:
        if (!work.isFlag(0x22000014)) {
            kinetic.changeKinetic(10, moduleAccesser);
            work.onFlag(0x22000014);
        }
        break;
    case 1: {
        float scale = moduleAccesser->getConstantFloatKirby(0xfa4);
        Vec3f speed(scale, scale, 0.0f);
        soKineticEnergy::AttributeFlag all(-1);
        kinetic.mulSpeed(&speed, all);
        break;
    }
    case 2:
        if (!work.isFlag(0x22000015)) {
            kinetic.changeKinetic(0, moduleAccesser);
            work.onFlag(0x22000015);
        }
        break;
    }
}

static void updateAirSpeed(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x22000012) && !work.isFlag(0x22000013)) {
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        kinetic.changeKinetic(0, moduleAccesser);
        float stick = moduleAccesser->getControllerModule().getStickY();
        float maxStick = moduleAccesser->getConstantFloatKirby(0xfa1);
        float minStick = moduleAccesser->getConstantFloatKirby(0xfa0);
        float stickScale = moduleAccesser->getConstantFloatKirby(0xfa2);
        float mag = fabs(stick);
        if (mag > maxStick) {
            mag = maxStick;
        }
        float t = mag - minStick;
        if (t < 0.0f) {
            t = 0.0f;
        }
        if (stick < 0.0f) {
            t = -t;
        }
        float angle = 0.5f * ((t * stickScale) / (maxStick - minStick));
        float lr = moduleAccesser->getPostureModule().getLr();
        float speedScale = moduleAccesser->getConstantFloatKirby(0xfa3);
        float c = cos(angle);
        Vec2f brake(0.0f, 0.0f);
        Vec2f speed(c * speedScale * lr, 0.0f * speedScale);
        normal.setSpeed(&speed);
        normal.m_speedLimit.m_x = speed.m_x;
        normal.m_speedLimit.m_y = speed.m_y;
        normal.setStableSpeed(&speed);
        normal.setBrake(&brake);
        normal.enable();
        float s = sin(angle);
        gravity.m_speedY = speedScale * s;
        work.onFlag(0x22000013);
    }
}

ftGanonStatusUniqProcessGanonPunch g_ftGanonStatusUniqProcessGanonPunch;
ftGanonStatusUniqProcessGanonPunchTurn g_ftGanonStatusUniqProcessGanonPunchTurn;
