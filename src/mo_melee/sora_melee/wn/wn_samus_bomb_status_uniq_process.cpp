#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusBombStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusBombStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSamusBombStatusUniqProcess g_wnSamusBombStatusUniqProcess;

void wnSamusBombStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusBombStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) { }
