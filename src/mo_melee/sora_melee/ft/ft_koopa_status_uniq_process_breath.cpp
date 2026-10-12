#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftKoopaStatusUniqProcessBreath : public soStatusUniqProcess {
public:
    virtual ~ftKoopaStatusUniqProcessBreath() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftKoopaStatusUniqProcessBreath g_ftKoopaStatusUniqProcessBreath;

void ftKoopaStatusUniqProcessBreath::initStatus(soModuleAccesser* moduleAccesser) { }
void ftKoopaStatusUniqProcessBreath::exitStatus(soModuleAccesser* moduleAccesser, int) { }
void ftKoopaStatusUniqProcessBreath::execStatus(soModuleAccesser* moduleAccesser) { }
