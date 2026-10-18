#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <mt/mt_prng.h>
#include <types.h>

class ftPeachStatusUniqProcessAttackS4Start : public soStatusUniqProcess {
public:
    virtual ~ftPeachStatusUniqProcessAttackS4Start() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

// Picks one of three attack variants, never repeating the previous choice.
void ftPeachStatusUniqProcessAttackS4Start::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int previous = workManage.getInt(0x10000040);
    int next;
    do {
        next = randi(3);
    } while (next == previous);
    workManage.setInt(next, 0x10000040);
}

ftPeachStatusUniqProcessAttackS4Start g_ftPeachStatusUniqProcessAttackS4Start;
