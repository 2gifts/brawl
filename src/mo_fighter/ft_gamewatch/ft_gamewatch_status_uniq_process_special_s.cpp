#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftGamewatchStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftGamewatchStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftGamewatchStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {}
void ftGamewatchStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {}

ftGamewatchStatusUniqProcessSpecialS g_ftGamewatchStatusUniqProcessSpecialS;
