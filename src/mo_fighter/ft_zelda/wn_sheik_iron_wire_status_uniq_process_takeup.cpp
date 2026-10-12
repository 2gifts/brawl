#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSheikIronWireStatusUniqProcessTakeup : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessTakeup() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessTakeup g_wnSheikIronWireStatusUniqProcessTakeup;

void wnSheikIronWireStatusUniqProcessTakeup::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSheikIronWireStatusUniqProcessTakeup::execStatus(soModuleAccesser* moduleAccesser) { }
