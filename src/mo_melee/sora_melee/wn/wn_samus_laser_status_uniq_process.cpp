#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusLaserStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusLaserStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusLaserStatusUniqProcess g_wnSamusLaserStatusUniqProcess;

void wnSamusLaserStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusLaserStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnSamusLaserStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusLaserStatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
