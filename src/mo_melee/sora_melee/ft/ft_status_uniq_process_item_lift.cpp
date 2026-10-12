#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftStatusUniqProcessItemLift : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessItemLift() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftStatusUniqProcessItemLift g_ftStatusUniqProcessItemLift;

// TODO: initStatus (status 0x97 entry: builds a 4-field info record and calls the motion module) is not decoded.
void ftStatusUniqProcessItemLift::initStatus(soModuleAccesser*) {}

// TODO: execStatus (status 0x9A: clamps a float from fn_27_194390 and calls the motion module) is not decoded.
void ftStatusUniqProcessItemLift::execStatus(soModuleAccesser*) {}

void ftStatusUniqProcessItemLift::exitStatus(soModuleAccesser* moduleAccesser, int kind) {
    if (kind == 0x9d) {
        return;
    }
    if (kind >= 0x97 && kind < 0x9b) {
        return;
    }
    if (!moduleAccesser->getItemManageModule().isHaveItem(0)) {
        return;
    }
    // HYPOTHESIS: the drop-velocity constants are lbl_27_rodata_236C / lbl_27_rodata_2360 (values not read).
    moduleAccesser->getItemManageModule().dropItem(0.0f, 0.0f, 0);
}
