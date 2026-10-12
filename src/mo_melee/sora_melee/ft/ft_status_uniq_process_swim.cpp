#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessSwim : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwim() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSwim g_ftStatusUniqProcessSwim;

void ftStatusUniqProcessSwim::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwim::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessSwim::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwim::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSwim::execFixPos(soModuleAccesser* moduleAccesser) { }
