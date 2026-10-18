#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <ft/ft_status_uniq_process_catch_pull_impl.h>
#include <so/catch/so_catch_module.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: class shape from the donkey catch-wait TU (exitStatus override, base catch-wait class).
class ftStatusUniqProcessCatchWait : public ftStatusUniqProcessCatchPull {
public:
    ftStatusUniqProcessCatchWait() { }
    virtual ~ftStatusUniqProcessCatchWait() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};

// Catching an enemy sets the work flag 0x22000010.
void ftStatusUniqProcessCatchWait::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().getNode(0)->soGetKind() == StageObject_Enemy) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    }
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) == Fighter::Status::Catch_Pull) {
        moduleAccesser->getControllerModule().setRumble(4, 0, true, -1);
    }
}

void ftStatusUniqProcessCatchWait::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    // Exiting into a catch-related status (0x39..0x3c) or 0xf1 keeps the catch alive.
    switch (nextStatus) {
    case 0x39:
    case 0x3A:
    case 0x3B:
    case 0x3C:
    case 0xF1:
        break;
    default:
        static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
        break;
    }
}
ftStatusUniqProcessCatchWait g_ftStatusUniqProcessCatchWait;
