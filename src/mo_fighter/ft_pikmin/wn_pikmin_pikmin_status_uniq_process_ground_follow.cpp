#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminPikminStatusUniqProcessGroundFollow : public soStatusUniqProcess {
public:
    virtual ~wnPikminPikminStatusUniqProcessGroundFollow() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnPikminPikminStatusUniqProcessGroundFollow g_wnPikminPikminStatusUniqProcessGroundFollow;

void wnPikminPikminStatusUniqProcessGroundFollow::execStatus(soModuleAccesser* moduleAccesser) { }
void wnPikminPikminStatusUniqProcessGroundFollow::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
