#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLinkStatusUniqProcessSpecialRSlash : public soStatusUniqProcess {
public:
    virtual ~ftLinkStatusUniqProcessSpecialRSlash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftLinkStatusUniqProcessSpecialRSlash::initStatus(soModuleAccesser*) { }

void ftLinkStatusUniqProcessSpecialRSlash::execStatus(soModuleAccesser*) { }

ftLinkStatusUniqProcessSpecialRSlash g_ftLinkStatusUniqProcessSpecialRSlash;
