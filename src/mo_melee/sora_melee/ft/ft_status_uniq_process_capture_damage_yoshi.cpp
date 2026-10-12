#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureDamageYoshi : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureDamageYoshi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureDamageYoshi g_ftStatusUniqProcessCaptureDamageYoshi;

void ftStatusUniqProcessCaptureDamageYoshi::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptureDamageYoshi::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessCaptureDamageYoshi::execFixPos(soModuleAccesser* moduleAccesser) { }
bool ftStatusUniqProcessCaptureDamageYoshi::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
