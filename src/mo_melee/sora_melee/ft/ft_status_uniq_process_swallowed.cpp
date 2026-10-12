#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessSwallowed : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwallowed() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessSwallowed g_ftStatusUniqProcessSwallowed;

void ftStatusUniqProcessSwallowed::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwallowed::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessSwallowed::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwallowed::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessSwallowed::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
