#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftCaptainStatusUniqProcessFalconPunch : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconPunch() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

class ftCaptainStatusUniqProcessFalconPunchTurn : public soStatusUniqProcess {
public:
    virtual ~ftCaptainStatusUniqProcessFalconPunchTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftCaptainStatusUniqProcessFalconPunch g_ftCaptainStatusUniqProcessFalconPunch;
ftCaptainStatusUniqProcessFalconPunchTurn g_ftCaptainStatusUniqProcessFalconPunchTurn;

void ftCaptainStatusUniqProcessFalconPunch::initStatus(soModuleAccesser* moduleAccesser) { }
void ftCaptainStatusUniqProcessFalconPunch::execStatus(soModuleAccesser* moduleAccesser) { }
void ftCaptainStatusUniqProcessFalconPunchTurn::initStatus(soModuleAccesser* moduleAccesser) { }
void ftCaptainStatusUniqProcessFalconPunchTurn::execStatus(soModuleAccesser* moduleAccesser) { }
