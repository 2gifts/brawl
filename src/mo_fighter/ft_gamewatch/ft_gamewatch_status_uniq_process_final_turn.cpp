#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftGameWatchStatusUniqProcessFinalTurn : public soStatusUniqProcess {
public:
    virtual ~ftGameWatchStatusUniqProcessFinalTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftGameWatchStatusUniqProcessFinalTurn::initStatus(soModuleAccesser* moduleAccesser) {}
void ftGameWatchStatusUniqProcessFinalTurn::execStatus(soModuleAccesser* moduleAccesser) {}

ftGameWatchStatusUniqProcessFinalTurn g_ftGameWatchStatusUniqProcessFinalTurn;
