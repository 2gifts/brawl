#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessAdvFollow : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessAdvFollow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessAdvFollow g_ftStatusUniqProcessAdvFollow;

void ftStatusUniqProcessAdvFollow::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessAdvFollow::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessAdvFollow::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessAdvFollow::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) { }
