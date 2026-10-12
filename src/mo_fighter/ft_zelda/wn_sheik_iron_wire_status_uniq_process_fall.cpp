#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSheikIronWireStatusUniqProcessFall : public soStatusUniqProcess {
public:
    virtual ~wnSheikIronWireStatusUniqProcessFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikIronWireStatusUniqProcessFall g_wnSheikIronWireStatusUniqProcessFall;

void wnSheikIronWireStatusUniqProcessFall::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSheikIronWireStatusUniqProcessFall::execFixPos(soModuleAccesser* moduleAccesser) { }
