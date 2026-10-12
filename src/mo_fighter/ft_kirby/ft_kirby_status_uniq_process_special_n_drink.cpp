#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: Kirby's Special N drink (swallowed item drink) status process; names are unknown.
class ftKirbyStatusUniqProcessSpecialNDrink : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialNDrink() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessSpecialNDrink::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().onFlag(0x22000018);
    moduleAccesser->getWorkManageModule().setInt(1, 0x1000004b);
    int prevKind = moduleAccesser->getStatusModule().getPrevStatusKind(0);
    moduleAccesser->getWorkManageModule().setInt(prevKind, 0x20000006);
    moduleAccesser->getWorkManageModule().setInt(0, 0x20000007);
    moduleAccesser->getWorkManageModule().setInt(0, 0x20000008);
}

void ftKirbyStatusUniqProcessSpecialNDrink::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: exit path not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessSpecialNDrink::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialNDrink::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessSpecialNDrink::execFixPos(soModuleAccesser* moduleAccesser) {
}

ftKirbyStatusUniqProcessSpecialNDrink g_ftKirbyStatusUniqProcessSpecialNDrink;
