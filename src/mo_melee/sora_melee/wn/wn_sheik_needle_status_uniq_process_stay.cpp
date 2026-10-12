#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSheikNeedleStatusUniqProcessStay : public soStatusUniqProcess {
public:
    virtual ~wnSheikNeedleStatusUniqProcessStay() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

wnSheikNeedleStatusUniqProcessStay g_wnSheikNeedleStatusUniqProcessStay;

void wnSheikNeedleStatusUniqProcessStay::exitStatus(soModuleAccesser* moduleAccesser, int) { }
