#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnMarioPumpStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnMarioPumpStatusUniqProcessShoot() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
};

wnMarioPumpStatusUniqProcessShoot g_wnMarioPumpStatusUniqProcessShoot;

void wnMarioPumpStatusUniqProcessShoot::execStatus(soModuleAccesser* moduleAccesser) { }
void wnMarioPumpStatusUniqProcessShoot::execStop(soModuleAccesser* moduleAccesser) { }
