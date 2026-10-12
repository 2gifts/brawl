#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftDonkeyStatusUniqProcessSpecialNAttack : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessSpecialNAttack() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDonkeyStatusUniqProcessSpecialNAttack g_ftDonkeyStatusUniqProcessSpecialNAttack;

void ftDonkeyStatusUniqProcessSpecialNAttack::initStatus(soModuleAccesser* moduleAccesser) { }
void ftDonkeyStatusUniqProcessSpecialNAttack::exitStatus(soModuleAccesser* moduleAccesser, int) { }
