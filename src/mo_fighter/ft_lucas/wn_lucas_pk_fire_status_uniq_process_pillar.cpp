#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLucasPKFireStatusUniqProcessPillar : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKFireStatusUniqProcessPillar() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnLucasPKFireStatusUniqProcessPillar g_wnLucasPKFireStatusUniqProcessPillar;

void wnLucasPKFireStatusUniqProcessPillar::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLucasPKFireStatusUniqProcessPillar::execStatus(soModuleAccesser* moduleAccesser) { }
