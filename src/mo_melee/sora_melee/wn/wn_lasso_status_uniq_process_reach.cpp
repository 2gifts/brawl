#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnLassoStatusUniqProcessReach : public soStatusUniqProcess {
public:
    virtual ~wnLassoStatusUniqProcessReach() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnLassoStatusUniqProcessReach g_wnLassoStatusUniqProcessReach;

void wnLassoStatusUniqProcessReach::initStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessReach::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void wnLassoStatusUniqProcessReach::execStatus(soModuleAccesser* moduleAccesser) { }
void wnLassoStatusUniqProcessReach::execFixPos(soModuleAccesser* moduleAccesser) { }
