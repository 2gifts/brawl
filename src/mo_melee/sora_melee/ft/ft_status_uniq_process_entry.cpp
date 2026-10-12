#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessEntry : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessEntry() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessEntry g_ftStatusUniqProcessEntry;

void ftStatusUniqProcessEntry::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessEntry::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessEntry::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) { }
