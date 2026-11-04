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
