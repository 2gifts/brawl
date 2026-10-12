#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessDamageFall : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessDamageFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessDamageFall g_ftStatusUniqProcessDamageFall;

void ftStatusUniqProcessDamageFall::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessDamageFall::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessDamageFall::execFixPos(soModuleAccesser* moduleAccesser) { }
