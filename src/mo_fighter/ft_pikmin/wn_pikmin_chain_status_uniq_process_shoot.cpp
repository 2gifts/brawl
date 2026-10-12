#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPikminChainStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnPikminChainStatusUniqProcessShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPikminChainStatusUniqProcessShoot g_wnPikminChainStatusUniqProcessShoot;

void wnPikminChainStatusUniqProcessShoot::initStatus(soModuleAccesser* moduleAccesser) { }
void wnPikminChainStatusUniqProcessShoot::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
void wnPikminChainStatusUniqProcessShoot::execFixPos(soModuleAccesser* moduleAccesser) { }
