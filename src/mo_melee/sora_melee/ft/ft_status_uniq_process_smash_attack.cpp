#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessSmashAttack : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSmashAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessSmashAttack g_ftStatusUniqProcessSmashAttack;

void ftStatusUniqProcessSmashAttack::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSmashAttack::exitStatus(soModuleAccesser* moduleAccesser, int) { }
