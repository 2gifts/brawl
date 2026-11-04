#include <ft/ft_kinetic_energy.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>


// Skeleton: methods without bodies are stubs (not yet reconstructed).

class ftCaptainStatusUniqProcessFalconKick : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconKick() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFalconKickEnd : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconKickEnd() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftCaptainStatusUniqProcessFalconKick::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftCaptainStatusUniqProcessFalconKick::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftCaptainStatusUniqProcessFalconKickEnd::initStatus(soModuleAccesser* moduleAccesser) {
    int situation = moduleAccesser->getSituationModule().getKind();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    int variant = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    if (variant == 0 && situation == 0) {
        Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
        soKineticEnergy* stop = kinetic.getEnergy(3);
        stop->resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    } else if (variant == 2 && situation == 2) {
        Vec2f sumSpeed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
        kinetic.unableEnergyAll();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        soKineticEnergy* stop = kinetic.getEnergy(3);
        gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop->resetEnergy(9, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        stop->enable();
    }
}

void ftCaptainStatusUniqProcessFalconKickEnd::execStatus(soModuleAccesser* moduleAccesser) {
}

void soModuleAccesser::set_special_brake_coefficient(u32 airId, u32 groundId) {
    soWorkManageModule& workManage = getWorkManageModule();
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*getKineticModule().getEnergy(3));
    float coefficient;
    if (workManage.isFlag(0x22000010)) {
        coefficient = soValueAccesser::getConstantFloat(this, airId, 0);
    } else {
        coefficient = soValueAccesser::getConstantFloat(this, groundId, 0);
    }
    Vec2f brake;
    brake.m_x = coefficient;
    brake.m_y = 0.0f;
    energy.m_brake.m_x = brake.m_x;
    energy.m_brake.m_y = brake.m_y;
}

ftCaptainStatusUniqProcessFalconKick g_ftCaptainStatusUniqProcessFalconKick;
ftCaptainStatusUniqProcessFalconKickEnd g_ftCaptainStatusUniqProcessFalconKickEnd;
