#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Test-motion status process. Keeps no work of its own; both hooks are empty.
class ftStatusUniqProcessTestMotion : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessTestMotion() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessTestMotion g_ftStatusUniqProcessTestMotion;

void ftStatusUniqProcessTestMotion::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessTestMotion::execStatus(soModuleAccesser* moduleAccesser) {
}
