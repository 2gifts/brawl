#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessPKThunderStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnNessPKThunderStatusUniqProcessMove() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnNessPKThunderStatusUniqProcessMove g_wnNessPKThunderStatusUniqProcessMove;

void wnNessPKThunderStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnNessPKThunderStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) { }
void wnNessPKThunderStatusUniqProcessMove::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
