#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSheikIronWireStatusUniqProcessHad : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessHad() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessHad g_wnSheikIronWireStatusUniqProcessHad;

void wnSheikIronWireStatusUniqProcessHad::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSheikIronWireStatusUniqProcessHad::execStatus(soModuleAccesser* moduleAccesser) { }
