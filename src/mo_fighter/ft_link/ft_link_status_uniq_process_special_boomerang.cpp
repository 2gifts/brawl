#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLinkStatusUniqProcessSpecialBoomerang : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessSpecialBoomerang() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftLinkStatusUniqProcessSpecialBoomerang::initStatus(soModuleAccesser*) { }

void ftLinkStatusUniqProcessSpecialBoomerang::execStatus(soModuleAccesser*) { }

void ftLinkStatusUniqProcessSpecialBoomerang::exitStatus(soModuleAccesser*, int) { }

ftLinkStatusUniqProcessSpecialBoomerang g_ftLinkStatusUniqProcessSpecialBoomerang;
