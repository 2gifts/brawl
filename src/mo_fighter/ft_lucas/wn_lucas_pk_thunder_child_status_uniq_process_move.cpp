#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLucasPKThunderChildStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKThunderChildStatusUniqProcessMove() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLucasPKThunderChildStatusUniqProcessMove g_wnLucasPKThunderChildStatusUniqProcessMove;

void wnLucasPKThunderChildStatusUniqProcessMove::execFixPos(soModuleAccesser* moduleAccesser) { }
