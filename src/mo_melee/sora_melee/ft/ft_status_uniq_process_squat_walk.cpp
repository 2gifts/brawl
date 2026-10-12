#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessSquatWalk : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSquatWalk() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessSquatWalk g_ftStatusUniqProcessSquatWalk;

void ftStatusUniqProcessSquatWalk::initStatus(soModuleAccesser* moduleAccesser) { }
void ftStatusUniqProcessSquatWalk::execStatus(soModuleAccesser* moduleAccesser) { }
