#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminPikminStatusUniqProcessSpecialSCling : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessSpecialSCling() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnPikminPikminStatusUniqProcessSpecialSCling g_wnPikminPikminStatusUniqProcessSpecialSCling;

void wnPikminPikminStatusUniqProcessSpecialSCling::initStatus(soModuleAccesser* moduleAccesser) { }
void wnPikminPikminStatusUniqProcessSpecialSCling::exitStatus(soModuleAccesser* moduleAccesser, int) { }
