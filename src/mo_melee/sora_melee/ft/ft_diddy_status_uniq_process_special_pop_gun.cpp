#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDiddyStatusUniqProcessSpecialPopGun : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialPopGun() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftDiddyStatusUniqProcessSpecialPopGun g_ftDiddyStatusUniqProcessSpecialPopGun;

void ftDiddyStatusUniqProcessSpecialPopGun::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialPopGun::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialPopGun::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialPopGun::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDiddyStatusUniqProcessSpecialPopGun::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
}
