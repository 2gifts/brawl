#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftDededeStatusUniqProcessSpecialNDrinkItem : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialNDrinkItem() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

ftDededeStatusUniqProcessSpecialNDrinkItem g_ftDededeStatusUniqProcessSpecialNDrinkItem;

void ftDededeStatusUniqProcessSpecialNDrinkItem::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNDrinkItem::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNDrinkItem::execStop(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNDrinkItem::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftDededeStatusUniqProcessSpecialNDrinkItem::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
