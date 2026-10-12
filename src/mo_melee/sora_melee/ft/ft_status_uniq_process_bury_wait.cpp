#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessBuryWait : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessBuryWait() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessBuryWait g_ftStatusUniqProcessBuryWait;

void ftStatusUniqProcessBuryWait::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessBuryWait::execFixPosCounter(soModuleAccesser* moduleAccesser) { }
