#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <so/catch/so_catch_module.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: class shape from the donkey catch-wait TU (exitStatus override, base catch-wait class).
class ftStatusUniqProcessCatchWait : public soStatusUniqProcess {
public:
    ftStatusUniqProcessCatchWait() { }
    virtual ~ftStatusUniqProcessCatchWait() { }
    virtual void initStatus(soModuleAccesser*);
    virtual void exitStatus(soModuleAccesser*, int);
};

// MATCH-ONLY: not yet decoded: the link-module branch (link slot 0x50, then result slot 0xa4 == 1) that
// sets the work flag 0x22000010 is omitted from this draft.
void ftStatusUniqProcessCatchWait::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) == Fighter::Status::Catch_Pull) {
        moduleAccesser->getControllerModule().setRumble(4, 0, true, -1);
    }
}

void ftStatusUniqProcessCatchWait::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    // Exiting into a catch-related status (0x39..0x3c) or 0xf1 keeps the catch alive.
    if (nextStatus == 0xf1) {
        return;
    }
    if (nextStatus < 0x3d) {
        if (nextStatus >= 0x39) {
            return;
        }
    }
    static_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, false);
}
ftStatusUniqProcessCatchWait g_ftStatusUniqProcessCatchWait;
