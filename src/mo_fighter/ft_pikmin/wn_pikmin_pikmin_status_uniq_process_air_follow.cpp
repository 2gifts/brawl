#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminPikminStatusUniqProcessAirFollow : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessAirFollow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessAirFollow g_wnPikminPikminStatusUniqProcessAirFollow;

void wnPikminPikminStatusUniqProcessAirFollow::initStatus(soModuleAccesser* moduleAccesser) { }
void wnPikminPikminStatusUniqProcessAirFollow::execStatus(soModuleAccesser* moduleAccesser) { }
void wnPikminPikminStatusUniqProcessAirFollow::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
