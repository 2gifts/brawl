#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSheikNeedleStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnSheikNeedleStatusUniqProcessMove() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikNeedleStatusUniqProcessMove g_wnSheikNeedleStatusUniqProcessMove;

void wnSheikNeedleStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnSheikNeedleStatusUniqProcessMove::execFixPos(soModuleAccesser* moduleAccesser) { }
