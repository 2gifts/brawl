#include <so/status/so_status_module_impl.h>
#include <types.h>

// Appeal (taunt-style) status process: the base status process supplies the defaults.
class ftStatusUniqProcessAppeal : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessAppeal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessAppeal g_ftStatusUniqProcessAppeal;

void ftStatusUniqProcessAppeal::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessAppeal::exitStatus(soModuleAccesser* moduleAccesser, int) { }
