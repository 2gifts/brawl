#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE

#include <ft/ft_status_uniq_process_guard.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Local view of the shield helper. Slot offsets follow the original vtable (updateShield 0xC, setShieldScale 0x10,
// checkHitStopDelay 0x14 with a bool, checkHitStopDelayFlick 0x18); the bool overload is not in the shared header.
class ftStatusUniqProcessGuardFuncView {
public:
    virtual ~ftStatusUniqProcessGuardFuncView() { }
    virtual void updateShield(soModuleAccesser* moduleAccesser);
    virtual void setShieldScale(soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: the bool is the end-of-hitstop flag passed from leaveStop.
    virtual void checkHitStopDelay(soModuleAccesser* moduleAccesser, bool isHitStopEnd);
    virtual void checkHitStopDelayFlick(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessGuardOn g_ftStatusUniqProcessGuardOn;

void ftStatusUniqProcessGuardOn::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getCollisionShieldModule().setStatus(0, 1, 0);
    ((ftStatusUniqProcessGuardFuncView*)m_guardFunc)->updateShield(moduleAccesser);
}

void ftStatusUniqProcessGuardOn::execStatus(soModuleAccesser* moduleAccesser) {
    ((ftStatusUniqProcessGuardFuncView*)m_guardFunc)->updateShield(moduleAccesser);
}

void ftStatusUniqProcessGuardOn::execStop(soModuleAccesser* moduleAccesser) {
    ((ftStatusUniqProcessGuardFuncView*)m_guardFunc)->setShieldScale(moduleAccesser);
    ((ftStatusUniqProcessGuardFuncView*)m_guardFunc)->checkHitStopDelayFlick(moduleAccesser);
    if (moduleAccesser->getWorkManageModule().getInt(0x20000001) > 0) {
        moduleAccesser->getWorkManageModule().decInt(0x20000001);
    }
}

void ftStatusUniqProcessGuardOn::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 0);
}

void ftStatusUniqProcessGuardOn::leaveStop(soModuleAccesser* moduleAccesser, int, bool isHitStopEnd) {
    ((ftStatusUniqProcessGuardFuncView*)m_guardFunc)->checkHitStopDelay(moduleAccesser, isHitStopEnd);
}
