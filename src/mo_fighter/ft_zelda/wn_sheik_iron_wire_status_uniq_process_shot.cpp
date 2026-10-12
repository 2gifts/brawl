#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSheikIronWireStatusUniqProcessShot : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessShot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessShot g_wnSheikIronWireStatusUniqProcessShot;

void wnSheikIronWireStatusUniqProcessShot::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSheikIronWireStatusUniqProcessShot::execStatus(soModuleAccesser* moduleAccesser) { }
