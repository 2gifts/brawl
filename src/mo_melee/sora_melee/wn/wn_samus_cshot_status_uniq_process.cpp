#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusCShotStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusCShotStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnSamusCShotStatusUniqProcess g_wnSamusCShotStatusUniqProcess;

void wnSamusCShotStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusCShotStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnSamusCShotStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) { }
