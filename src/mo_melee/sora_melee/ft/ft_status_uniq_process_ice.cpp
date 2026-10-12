#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessIce : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessIce() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessIce g_ftStatusUniqProcessIce;

void ftStatusUniqProcessIce::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessIce::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessIce::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessIce::execStop(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessIce::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessIce::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
