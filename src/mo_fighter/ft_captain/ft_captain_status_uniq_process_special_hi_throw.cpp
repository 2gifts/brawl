#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

class ftCaptainStatusUniqProcessSpecialHiThrow : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessSpecialHiThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

void ftCaptainStatusUniqProcessSpecialHiThrow::initStatus(soModuleAccesser* moduleAccesser) {
}

ftCaptainStatusUniqProcessSpecialHiThrow g_ftCaptainStatusUniqProcessSpecialHiThrow;
