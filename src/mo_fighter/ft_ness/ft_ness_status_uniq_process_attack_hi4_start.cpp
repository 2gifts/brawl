#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftNessStatusUniqProcessAttackHi4Start : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessAttackHi4Start() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftNessStatusUniqProcessAttackHi4Start::initStatus(soModuleAccesser* moduleAccesser) {
    // TODO: the yoyo link setup (status 0x2d / 0x30 branch, two ftNessLinkEventYoyo objects) is not reconstructed yet.
}

void ftNessStatusUniqProcessAttackHi4Start::execFixPos(soModuleAccesser* moduleAccesser) {
    // TODO: not reconstructed yet.
}

ftNessStatusUniqProcessAttackHi4Start g_ftNessStatusUniqProcessAttackHi4Start;
