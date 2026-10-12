#include <so/status/so_status_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftLuigiStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftLuigiStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftLuigiStatusUniqProcessSpecialLw::initStatus(soModuleAccesser*) { }

void ftLuigiStatusUniqProcessSpecialLw::execStatus(soModuleAccesser*) { }

ftLuigiStatusUniqProcessSpecialLw g_ftLuigiStatusUniqProcessSpecialLw;
