#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusLaser2StatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusLaser2StatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusLaser2StatusUniqProcess g_wnSamusLaser2StatusUniqProcess;

void wnSamusLaser2StatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusLaser2StatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
