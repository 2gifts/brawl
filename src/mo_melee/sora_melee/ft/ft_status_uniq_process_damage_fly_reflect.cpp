#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessDamageFlyReflect : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessDamageFlyReflect() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessDamageFlyReflect g_ftStatusUniqProcessDamageFlyReflect;

void ftStatusUniqProcessDamageFlyReflect::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessDamageFlyReflect::execStatus(soModuleAccesser* moduleAccesser) { }
