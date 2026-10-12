#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftFoxStatusUniqProcessFire : public soStatusUniqProcess {
public:
    virtual ~ftFoxStatusUniqProcessFire() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftFoxStatusUniqProcessFire g_ftFoxStatusUniqProcessFire;

void ftFoxStatusUniqProcessFire::initStatus(soModuleAccesser* moduleAccesser) { }
void ftFoxStatusUniqProcessFire::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftFoxStatusUniqProcessFire::execStatus(soModuleAccesser* moduleAccesser) { }
void ftFoxStatusUniqProcessFire::execStop(soModuleAccesser* moduleAccesser) { }
void ftFoxStatusUniqProcessFire::execFixPos(soModuleAccesser* moduleAccesser) { }
