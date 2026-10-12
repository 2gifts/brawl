#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessSwallowedDrink : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwallowedDrink() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSwallowedDrink g_ftStatusUniqProcessSwallowedDrink;

void ftStatusUniqProcessSwallowedDrink::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwallowedDrink::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessSwallowedDrink::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwallowedDrink::execFixPos(soModuleAccesser* moduleAccesser) { }
