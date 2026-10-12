#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnNessPKFireStatusUniqProcessPillar : public soStatusUniqProcess {
public:
    virtual ~wnNessPKFireStatusUniqProcessPillar() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnNessPKFireStatusUniqProcessPillar g_wnNessPKFireStatusUniqProcessPillar;

void wnNessPKFireStatusUniqProcessPillar::initStatus(soModuleAccesser* moduleAccesser) { }
void wnNessPKFireStatusUniqProcessPillar::execStatus(soModuleAccesser* moduleAccesser) { }
