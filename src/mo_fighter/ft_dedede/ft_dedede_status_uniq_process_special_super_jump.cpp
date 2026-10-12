#include <ft/ft_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
// Constant table shared with ft_dedede.cpp data (lbl_117_rodata_18): 0.0, 1.0, -1.0.
static const float s_superJumpConst[3] = { 0.0f, 1.0f, -1.0f };

class ftDededeStatusUniqProcessSpecialSuperJump : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialSuperJump() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftDededeStatusUniqProcessSpecialSuperJump::initStatus(soModuleAccesser* moduleAccesser) {
    const float* k = s_superJumpConst;
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(2));
    int mode = 0xb;
    soKineticEnergy::AttributeFlag flag(1);
    Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(flag);
    if (sumSpeed.m_x > soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0)) {
        sumSpeed.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
    }
    Vec2f speed(sumSpeed.m_x, k[0]);
    motion.resetEnergy(mode, &speed, &Vec3f(k[0], k[0], k[0]), moduleAccesser);
    motion.unk3C = k[1];
    motion.unk40 = k[0];
    motion.enable();

    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(1));
    float a = k[2] * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb5, 0);
    float b = k[2] * soValueAccesser::getConstantFloat(moduleAccesser, 0xfb6, 0);
    Vec2f speed2(k[0], a);
    energy.resetEnergy(0, &speed2, &Vec3f(k[0], k[0], k[0]), moduleAccesser);
    energy.m_accel.m_x = b;
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        energy.enable();
    } else {
        energy.disable();
    }
}

void ftDededeStatusUniqProcessSpecialSuperJump::execStatus(soModuleAccesser* moduleAccesser) {
    float stickX = moduleAccesser->getControllerModule().getStickX();
    ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(2));
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
    motion.m_speed.m_x = stickX * scale;
    motion.m_speed.m_y = s_superJumpConst[0];
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

ftDededeStatusUniqProcessSpecialSuperJump g_ftDededeStatusUniqProcessSpecialSuperJump;
