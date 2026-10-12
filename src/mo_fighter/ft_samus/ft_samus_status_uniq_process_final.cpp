#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// Samus's Charge Shot final smash. Only the class shape is in place; the status bodies are not reconstructed yet.
class ftSamusStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftSamusStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftSamusStatusUniqProcessFinal g_ftSamusStatusUniqProcessFinal;

void ftSamusStatusUniqProcessFinal::initStatus(soModuleAccesser*) {}

void ftSamusStatusUniqProcessFinal::execStatus(soModuleAccesser*) {}

void ftSamusStatusUniqProcessFinal::exitStatus(soModuleAccesser*, int) {}
