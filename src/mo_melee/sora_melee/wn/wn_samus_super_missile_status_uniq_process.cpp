#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnSamusSuperMissileStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnSamusSuperMissileStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnSamusSuperMissileStatusUniqProcess g_wnSamusSuperMissileStatusUniqProcess;

void wnSamusSuperMissileStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) { }
void wnSamusSuperMissileStatusUniqProcess::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
