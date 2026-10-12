#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminPikminStatusUniqProcessSpecialLwComp : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessSpecialLwComp() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnPikminPikminStatusUniqProcessSpecialLwComp g_wnPikminPikminStatusUniqProcessSpecialLwComp;

void wnPikminPikminStatusUniqProcessSpecialLwComp::exitStatus(soModuleAccesser* moduleAccesser, int) { }
