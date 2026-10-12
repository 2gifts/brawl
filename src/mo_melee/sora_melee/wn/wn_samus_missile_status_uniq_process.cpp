#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusMissileStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusMissileStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusMissileStatusUniqProcess g_wnSamusMissileStatusUniqProcess;

void wnSamusMissileStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusMissileStatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
