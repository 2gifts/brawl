#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: Kirby's Special N drink-item status process; names are unknown. Only the class shape is reconstructed.
class ftKirbyStatusUniqProcessSpecialNDrinkItem : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialNDrinkItem() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessSpecialNDrinkItem::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessSpecialNDrinkItem::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessSpecialNDrinkItem::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialNDrinkItem::execStop(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialNDrinkItem::execFixPos(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

ftKirbyStatusUniqProcessSpecialNDrinkItem g_ftKirbyStatusUniqProcessSpecialNDrinkItem;
