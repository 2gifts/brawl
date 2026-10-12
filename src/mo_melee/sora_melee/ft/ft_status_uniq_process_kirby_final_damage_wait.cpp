#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessKirbyFinalDamageWait : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessKirbyFinalDamageWait() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessKirbyFinalDamageWait g_ftStatusUniqProcessKirbyFinalDamageWait;

void ftStatusUniqProcessKirbyFinalDamageWait::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessKirbyFinalDamageWait::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessKirbyFinalDamageWait::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessKirbyFinalDamageWait::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessKirbyFinalDamageWait::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
