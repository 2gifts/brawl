#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <types.h>

class ftWarioStatusUniqProcessSpecialNEat : public soStatusUniqProcess {
public:
    virtual ~ftWarioStatusUniqProcessSpecialNEat() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftWarioStatusUniqProcessSpecialNEat g_ftWarioStatusUniqProcessSpecialNEat;

void ftWarioStatusUniqProcessSpecialNEat::execFixPosCounter(soModuleAccesser* moduleAccesser) {
}

void ftWarioStatusUniqProcessSpecialNEat::exitStatus(soModuleAccesser* moduleAccesser, int) {
    for (int index = 3; index < 7; index++) {
        if (moduleAccesser->getItemManageModule().isHaveItem(index)) {
            moduleAccesser->getItemManageModule().removeItem(index);
        }
    }
}
