#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <ft/ft_kinetic_energy.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessItemThrow : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessItemThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessItemThrow g_ftStatusUniqProcessItemThrow;

void ftStatusUniqProcessItemThrow::initStatus(soModuleAccesser*) {}

void ftStatusUniqProcessItemThrow::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
        return;
    }
    if (moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0x48) {
        if (moduleAccesser->getMotionModule().getKind() != 0xfe) {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x2293), false, false, 0);
        }
    }
    moduleAccesser->getWorkManageModule().onFlag(0x22000011);
}

void ftStatusUniqProcessItemThrow::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getItemManageModule().isHaveItem(0)) {
        return;
    }
    if (moduleAccesser->getItemManageModule().getHaveItemSize(0) == 1 ||
        moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0xd) {
        // HYPOTHESIS: the two drop-velocity constants are lbl_27_rodata_2370 / lbl_27_rodata_2374 (values not read).
        moduleAccesser->getItemManageModule().dropItem(0.0f, 0.0f, 0);
    }
}
