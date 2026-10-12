#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminPikminStatusUniqProcessWaterFollow : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessWaterFollow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessWaterFollow g_wnPikminPikminStatusUniqProcessWaterFollow;

void wnPikminPikminStatusUniqProcessWaterFollow::initStatus(soModuleAccesser* moduleAccesser) { }
void wnPikminPikminStatusUniqProcessWaterFollow::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
