#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessKirbyFinalDamageFly : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessKirbyFinalDamageFly() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessKirbyFinalDamageFly g_ftStatusUniqProcessKirbyFinalDamageFly;

void ftStatusUniqProcessKirbyFinalDamageFly::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessKirbyFinalDamageFly::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessKirbyFinalDamageFly::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessKirbyFinalDamageFly::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessKirbyFinalDamageFly::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
