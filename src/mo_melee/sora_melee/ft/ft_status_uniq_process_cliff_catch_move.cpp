#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCliffCatchMove : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCliffCatchMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessCliffCatchMove g_ftStatusUniqProcessCliffCatchMove;

void ftStatusUniqProcessCliffCatchMove::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCliffCatchMove::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftStatusUniqProcessCliffCatchMove::execStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessCliffCatchMove::execFixPos(soModuleAccesser* moduleAccesser) { }
