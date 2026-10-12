#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPitIkarusStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~wnPitIkarusStatusUniqProcessTurn() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnPitIkarusStatusUniqProcessTurn g_wnPitIkarusStatusUniqProcessTurn;

void wnPitIkarusStatusUniqProcessTurn::exitStatus(soModuleAccesser* moduleAccesser, int) { }
