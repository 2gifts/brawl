#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSheikNeedleStatusUniqProcessDrop : public soStatusUniqProcess {
public:
    virtual ~wnSheikNeedleStatusUniqProcessDrop() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSheikNeedleStatusUniqProcessDrop g_wnSheikNeedleStatusUniqProcessDrop;

void wnSheikNeedleStatusUniqProcessDrop::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSheikNeedleStatusUniqProcessDrop::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnSheikNeedleStatusUniqProcessDrop::execFixPos(soModuleAccesser* moduleAccesser) { }
