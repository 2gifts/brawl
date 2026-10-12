#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLucasPKThunderStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKThunderStatusUniqProcessMove() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnLucasPKThunderStatusUniqProcessMove g_wnLucasPKThunderStatusUniqProcessMove;

void wnLucasPKThunderStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnLucasPKThunderStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) { }
void wnLucasPKThunderStatusUniqProcessMove::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
