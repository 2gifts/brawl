#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_util.h>
#include <so/controller/so_controller_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessDamageFall : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessDamageFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    void setEnableTransition(soModuleAccesser* moduleAccesser, bool enable);
};

ftStatusUniqProcessDamageFall g_ftStatusUniqProcessDamageFall;

// HYPOTHESIS: work flag 0x22000010 records that the transition term groups 0xB, 0xC, 0xF, 0x11, 0x12 and 0xE are enabled;
// work int 0x20000000 is a countdown that keeps them disabled while it is positive.
void ftStatusUniqProcessDamageFall::setEnableTransition(soModuleAccesser* moduleAccesser, bool enable) {
    if (enable == true) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000010) && !moduleAccesser->getWorkManageModule().isFlag(0x12000018) &&
            moduleAccesser->getWorkManageModule().getInt(0x20000000) <= 0) {
            moduleAccesser->getStatusModule().enableTransitionTermGroup(0xB);
            moduleAccesser->getStatusModule().enableTransitionTermGroup(0xC);
            moduleAccesser->getStatusModule().enableTransitionTermGroup(0xF);
            moduleAccesser->getStatusModule().enableTransitionTermGroup(0x11);
            moduleAccesser->getStatusModule().enableTransitionTermGroup(0x12);
            moduleAccesser->getStatusModule().enableTransitionTermGroup(0xE);
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        }
    } else {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == true) {
            moduleAccesser->getStatusModule().unableTransitionTermGroup(0xB);
            moduleAccesser->getStatusModule().unableTransitionTermGroup(0xC);
            moduleAccesser->getStatusModule().unableTransitionTermGroup(0xF);
            moduleAccesser->getStatusModule().unableTransitionTermGroup(0x11);
            moduleAccesser->getStatusModule().unableTransitionTermGroup(0x12);
            moduleAccesser->getStatusModule().unableTransitionTermGroup(0xE);
            moduleAccesser->getWorkManageModule().offFlag(0x22000010);
        }
    }
}

void ftStatusUniqProcessDamageFall::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) == 0xEA) {
        int delay = soValueAccesser::getConstantInt(moduleAccesser, 0x5A82, 0);
        moduleAccesser->getWorkManageModule().setInt(delay, 0x20000000);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_speedY = -soValueAccesser::getConstantFloat(moduleAccesser, 0xDAA, 0);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200000B) == true) {
        int counter = soValueAccesser::getConstantInt(moduleAccesser, 0x5A70, 0);
        moduleAccesser->getWorkManageModule().setInt(counter, 0x1000001C);
    } else {
        setEnableTransition(moduleAccesser, true);
    }
    moduleAccesser->getControllerModule().setRumble(9, 0, true, -1);
}

void ftStatusUniqProcessDamageFall::execStatus(soModuleAccesser* moduleAccesser) {
    ftUtil::setCloudThroughCheck(moduleAccesser);
}

void ftStatusUniqProcessDamageFall::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200000B) == true) {
        if ((moduleAccesser->getControllerModule().getTrigger() & soController::getButtonMask(soController::Pad_Button_Attack)) ||
            (moduleAccesser->getControllerModule().getTrigger() & soController::getButtonMask(soController::Pad_Button_Special))) {
            moduleAccesser->getControllerModule().resetTrigger();
            moduleAccesser->getStatusModule().changeStatusRequest(0xE4, moduleAccesser);
        } else {
            setEnableTransition(moduleAccesser, false);
        }
    } else {
        ftUtil::checkCloudThroughOut(moduleAccesser);
        setEnableTransition(moduleAccesser, true);
    }
}
