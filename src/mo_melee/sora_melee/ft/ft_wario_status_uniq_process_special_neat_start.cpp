#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftWarioStatusUniqProcessSpecialNEatStart : public soStatusUniqProcess {
public:
    virtual ~ftWarioStatusUniqProcessSpecialNEatStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

ftWarioStatusUniqProcessSpecialNEatStart g_ftWarioStatusUniqProcessSpecialNEatStart;

void ftWarioStatusUniqProcessSpecialNEatStart::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soItemManageModule& items = moduleAccesser->getItemManageModule();

    // Scan the four item slots (3..6) for a held item and collect its size and trait flags.
    // HYPOTHESIS: the flags select which eat result (0x11d..0x120) is stored below.
    bool sizeSmall = false;
    bool traitFlag2000 = false;
    bool traitFlag8000 = false;
    for (int index = 3; index < 7; index++) {
        if (items.isHaveItem(index)) {
            soItemInfo info;
            items.getHaveItemInfo(&info, index);
            if (items.getHaveItemSize(index) == 1) {
                sizeSmall = true;
            }
            if (info.m_trait.m_mask & 0x8000) {
                traitFlag8000 = true;
            }
            if (info.m_trait.m_mask & 0x2000) {
                traitFlag2000 = true;
            }
        }
    }

    if (traitFlag8000) {
        work.setInt(0x11e, 0x20000005);
    } else if (sizeSmall) {
        work.setInt(0x11f, 0x20000005);
    } else if (traitFlag2000) {
        work.setInt(0x11d, 0x20000005);
    } else {
        work.setInt(0x120, 0x20000005);
    }
}

void ftWarioStatusUniqProcessSpecialNEatStart::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    // HYPOTHESIS: the item manage object is downcast from the module interface to its implementation.
    soItemManageModuleImpl* implItems = dynamic_cast<soItemManageModuleImpl*>(moduleAccesser->m_enumerationStart->m_itemManageModule);
    soItemManageModule* items = implItems;
    if (work.isFlag(0x22000012)) {
        for (int index = 3; index < 7; index++) {
            if (items->isHaveItem(index)) {
                items->setHaveItemVisibility(0, index);
            }
        }
    }
}
