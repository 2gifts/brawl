#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureBucyulus : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureBucyulus() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureBucyulus g_ftStatusUniqProcessCaptureBucyulus;

void ftStatusUniqProcessCaptureBucyulus::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCaptureBucyulus::exitStatus(soModuleAccesser* moduleAccesser, int) { }
bool ftStatusUniqProcessCaptureBucyulus::checkDamage(soModuleAccesser* moduleAccesser, void*) { return false; }
