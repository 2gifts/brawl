#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessOttotto : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessOttotto() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessOttotto g_ftStatusUniqProcessOttotto;

void ftStatusUniqProcessOttotto::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessOttotto::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: work int 0x20000000 counts the frames (or hits) of the ottotto wobble.
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    moduleAccesser->getWorkManageModule().setInt(count + 1, 0x20000000);
}

void ftStatusUniqProcessOttotto::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessOttotto::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) { }
