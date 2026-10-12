#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftMarioStatusUniqProcessSpecialHi : public soStatusUniqProcess {
public:
    virtual ~ftMarioStatusUniqProcessSpecialHi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftMarioStatusUniqProcessSpecialHi g_ftMarioStatusUniqProcessSpecialHi;

void ftMarioStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) { }
void ftMarioStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftMarioStatusUniqProcessSpecialHi::execStatus(soModuleAccesser* moduleAccesser) { }
