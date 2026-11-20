// BYTES: this unit owns the out-of-line copies of the soStatusUniqProcess default virtuals that the other Dedede status
// units (and the Jet Hammer vtable) refer to.
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// TODO(W6): all 18 functions are exact and the .text of this unit links byte-identical (with the out-of-line base copies
// below in the original order), but it stays NonMatching: the original unit's .data carve starts at 0x84E4 (4 mod 8) while
// our compiled .data section is 8-aligned (weak kinetic-energy RTTI objects), so the linker pads the section and every
// RTTI name string of the following units lands 4 bytes off. Needs a 4-aligned .data (or a carve that starts 8-aligned).
#include <ft/ft_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

class ftDededeStatusUniqProcessSpecialSuperJump : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJump();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

// MATCH-ONLY: the vector temporaries go through this helper so that their addresses are formed at the call (the native
// code does not cache them in saved registers).
static inline void resetKinetic(soKineticEnergy& energy, int mode, Vec3f* accel, const Vec2f& speed, soModuleAccesser* moduleAccesser) {
    energy.resetEnergy(mode, const_cast<Vec2f*>(&speed), accel, moduleAccesser);
}

// MATCH-ONLY: retain reset mode across the sum-speed query and vector construction, as in the native register lifetime.
void ftDededeStatusUniqProcessSpecialSuperJump::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
    ftKineticEnergyController::ResetType mode = ftKineticEnergyController::ResetType_11;
    Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    if (sumSpeed.m_x > soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0)) {
        sumSpeed.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
    }
    resetKinetic(controller, mode, &Vec3f(0.0f, 0.0f, 0.0f), Vec2f(sumSpeed.m_x, 0.0f), moduleAccesser);
    controller.m_accelMul = 1.0f;
    controller.m_unk40 = 0.0f;
    controller.enable();

    ftKineticEnergyGravity& energy = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    float a = -1.0f * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb5, 0);
    float b = -1.0f * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb6, 0);
    resetKinetic(energy, 0, &Vec3f(0.0f, 0.0f, 0.0f), Vec2f(0.0f, a), moduleAccesser);
    energy.m_gravity = b;
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        energy.enable();
    } else {
        energy.disable();
    }
}

void ftDededeStatusUniqProcessSpecialSuperJump::execStatus(soModuleAccesser* moduleAccesser) {
    float stickX = moduleAccesser->getControllerModule().getStickX();
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
    Vec2f speed;
    speed.m_x = stickX * scale;
    speed.m_y = 0.0f;
    controller.m_speed = speed;
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        moduleAccesser->getKineticModule().getEnergy(1)->enable();
    } else {
        moduleAccesser->getKineticModule().getEnergy(1)->disable();
    }
}

void ftDededeStatusUniqProcessSpecialSuperJump::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialSuperJump::exitStatus(soModuleAccesser* moduleAccesser, int) {
}

bool soStatusUniqProcess::checkTransitionPrecede(soModuleAccesser*, void*, int) {
    return true;
}

void soStatusUniqProcess::leaveStop(soModuleAccesser*, int, bool) { }

bool soStatusUniqProcess::onChangeLr(soModuleAccesser*, float, float) {
    return false;
}

void soStatusUniqProcess::checkAttack(soModuleAccesser*, void*, float) { }

bool soStatusUniqProcess::checkDamage(soModuleAccesser*, void*) {
    return false;
}

void soStatusUniqProcess::execFixCamera(soModuleAccesser*) { }

void soStatusUniqProcess::execFixPos(soModuleAccesser*) { }

void soStatusUniqProcess::execFixPosCounter(soModuleAccesser*) { }

void soStatusUniqProcess::execMapCorrection(soModuleAccesser*) { }

ftDededeStatusUniqProcessSpecialSuperJump::~ftDededeStatusUniqProcessSpecialSuperJump() { }

void soStatusUniqProcess::execStop(soModuleAccesser*) { }

void soStatusUniqProcess::exitStatus(soModuleAccesser*, int) { }

ftDededeStatusUniqProcessSpecialSuperJump g_ftDededeStatusUniqProcessSpecialSuperJump;
