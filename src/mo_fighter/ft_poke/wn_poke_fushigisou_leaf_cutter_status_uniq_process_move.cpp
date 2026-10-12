#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnPokeFushigisouLeafCutterStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnPokeFushigisouLeafCutterStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnPokeFushigisouLeafCutterStatusUniqProcessMove g_wnPokeFushigisouLeafCutterStatusUniqProcessMove;

void wnPokeFushigisouLeafCutterStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) { }
void wnPokeFushigisouLeafCutterStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) { }
