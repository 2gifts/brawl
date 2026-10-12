#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSheikIronWireStatusUniqProcessShotFall : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessShotFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessShotFall g_wnSheikIronWireStatusUniqProcessShotFall;

void wnSheikIronWireStatusUniqProcessShotFall::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSheikIronWireStatusUniqProcessShotFall::execStatus(soModuleAccesser* moduleAccesser) { }
