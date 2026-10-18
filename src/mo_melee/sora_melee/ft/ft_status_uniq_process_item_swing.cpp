#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_smash_attack.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Swing of a held item (bat, fan, ...). The smash swing statuses (0xA1 swing, 0xA2 swing hold) borrow the fighter
// smash attack charge handling.
class ftStatusUniqProcessItemSwing : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessItemSwing() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void checkAttack(soModuleAccesser* moduleAccesser, void*, float);
};

ftStatusUniqProcessItemSwing g_ftStatusUniqProcessItemSwing;

void ftStatusUniqProcessItemSwing::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xA1:
    case 0xA2:
        static_cast<soStatusUniqProcess*>(&g_ftStatusUniqProcessSmashAttack)->initStatus(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemSwing::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xA1:
    case 0xA2:
        static_cast<soStatusUniqProcess*>(&g_ftStatusUniqProcessSmashAttack)->exitStatus(moduleAccesser, nextStatusKind);
        break;
    }
    moduleAccesser->getItemManageModule().setHaveItemAction(5, 1.0f, 0);
}

void ftStatusUniqProcessItemSwing::checkAttack(soModuleAccesser* moduleAccesser, void*, float) {
    switch (moduleAccesser->getMotionModule().getKind()) {
    case 0x12C:
    case 0x132:
        moduleAccesser->getItemManageModule().setHaveItemAction(4, 1.0f, 0);
        break;
    }
}
