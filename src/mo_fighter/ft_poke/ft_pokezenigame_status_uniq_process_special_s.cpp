#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/controller/so_controller_module_impl.h>
#include <types.h>
#include <so/so_value_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_views.h>
#include <math.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// Squirtle's Withdraw (special S): status 0x113 starts the shell, 0x119 slides (the motion rate follows the speed and the
// attack power grows with it) and 0x11c is the stuck-in-shell clatter: the player mashes out of it and every mash plays the
// spin faster. exitStatus ends the controller clatter when the status changes out of 0x11c.
// HYPOTHESIS: the status meanings come from the motion kind (0x1dd) and the clatter calls; the BrawlRE status table names
// are not checked.
class ftPokeZenigameStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftPokeZenigameStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo);
};

ftPokeZenigameStatusUniqProcessSpecialS g_ftPokeZenigameStatusUniqProcessSpecialS;

void ftPokeZenigameStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x113: {
        // A flick of the stick when the move starts gives the fast start (work flag 0x22000011).
        soControllerModule& controller = moduleAccesser->getControllerModule();
        int flickFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a04, 0);
        if (controller.getFlickNoResetX() < flickFrames) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000011);
        } else {
            moduleAccesser->getWorkManageModule().offFlag(0x22000011);
        }
        break;
    }
    case 0x11c: {
        soMotionChangeParam motion;
        motion.m_kind = 0x1dd;
        motion.m_frame = 0.0f;
        motion.m_rate = 1.0f;
        motion._12 = 0;
        motion._13 = 0;
        motion._14 = 0;
        motion._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&motion);
        float clatterTime = moduleAccesser->getMotionModule().getEndFrame();
        reinterpret_cast<ftControllerClatterView*>(&moduleAccesser->getControllerModule())
            ->startClatter(clatterTime, 0.0f, 1.0f, 0, -1, 0, false);
        moduleAccesser->getWorkManageModule().setFloat(clatterTime, 0x21000004);
        break;
    }
    }
}

// MATCH-ONLY: the fighter units are built without inlining of the nw4r FSelect asm helper (as in the Pikachu units).
static inline float clampf(register float value, register float low, register float high) {
    register float bounded; // BYTES: declared before the difference so the intermediate takes f2, as in the original.
    register float difference;
    register float result;
    difference = value - low;
    asm { fsel bounded, difference, value, low }
    difference = bounded - high;
    asm { fsel result, difference, high, bounded }
    return result;
}

void ftPokeZenigameStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    switch (statusKind) {
    case 0x113:
    case 0x119: {
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        if (statusKind == 0x119) {
            // The slide's motion rate goes from the slow to the fast rate as the speed goes from the low to the high speed.
            float lowSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb2, 0);
            float highSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0);
            float slowRate = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
            float fastRate = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb5, 0);
            float ratio = ((float)fabs(speed.m_x) - lowSpeed) / (highSpeed - lowSpeed);
            float rate = slowRate + ratio * (fastRate - slowRate);
            moduleAccesser->getMotionModule().setRate(clampf(rate, slowRate, fastRate));
        }
        float powerPerSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb9, 0);
        float absSpeed = fabs(speed.m_x);
        moduleAccesser->getCollisionAttackModule().setPowerAddStatus(powerPerSpeed * absSpeed);
        break;
    }
    case 0x11c: {
        // Each mash lowers the clatter time: play the spin at double rate on the frames it changed.
        float clatterTime = moduleAccesser->getControllerModule().getClatterTime(0);
        if (clatterTime != moduleAccesser->getWorkManageModule().getFloat(0x21000004)) {
            moduleAccesser->getMotionModule().setRate(2.0f);
            moduleAccesser->getWorkManageModule().setFloat(clatterTime, 0x21000004);
        } else {
            moduleAccesser->getMotionModule().setRate(1.0f);
        }
        break;
    }
    }
}

void ftPokeZenigameStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11c:
        moduleAccesser->getControllerModule().endClatter(0);
        break;
    default:
        break;
    }
}

bool ftPokeZenigameStatusUniqProcessSpecialS::checkDamage(soModuleAccesser* moduleAccesser, void* damageInfo) {
    soDamage* damage = static_cast<soDamage*>(damageInfo);
    if (moduleAccesser->getDamageModule().isDamageLock() == true) {
        switch (damage->m_attackData.m_attribute) {
        case soCollisionAttackData::Attribute_Ice:
        case soCollisionAttackData::Attribute_Bury:
        case soCollisionAttackData::Attribute_Stun:
        case soCollisionAttackData::Attribute_Water:
        case soCollisionAttackData::Attribute_Paralyze:
        case soCollisionAttackData::Attribute_Pitfall:
            break;
        default: {
            ftKineticEnergyStop& stopEnergy = dynamic_cast<ftKineticEnergyStop&>(moduleAccesser->getKineticModule().getEnergy(3)[0]);
            Vec2f speed = stopEnergy.getSpeed();
            float damageSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
            speed.m_x -= damage->m_lr * (damage->m_attackData.m_power * damageSpeed);
            stopEnergy.m_speed = speed;
            return true;
        }
        }
    }
    return false;
}
