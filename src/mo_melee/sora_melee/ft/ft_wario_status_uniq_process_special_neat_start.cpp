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
    // Bombs take priority over heavy items, then food, then the default eat state.
    bool heavy = false;
    bool food = false;
    bool bomb = false;
    for (int index = 3; index < 7; index++) {
        if (items.isHaveItem(index)) {
            soItemInfo info;
            items.getHaveItemInfo(&info, index);
            if (items.getHaveItemSize(index) == itParam::Size_Heavy) {
                heavy = true;
            }
            if (info.m_trait.m_mask & itParam::TRAIT_MASK_BOMB) {
                bomb = true;
            }
            if (info.m_trait.m_mask & itParam::TRAIT_MASK_FOOD) {
                food = true;
            }
        }
    }

    if (bomb) {
        work.setInt(0x11e, 0x20000005);
    } else if (heavy) {
        work.setInt(0x11f, 0x20000005);
    } else if (food) {
        work.setInt(0x11d, 0x20000005);
    } else {
        work.setInt(0x120, 0x20000005);
    }
}

void ftWarioStatusUniqProcessSpecialNEatStart::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soItemManageModuleImpl* items = &dynamic_cast<soItemManageModuleImpl&>(moduleAccesser->getItemManageModule());
    if (work.isFlag(0x22000012)) {
        for (int index = 3; index < 7; index++) {
            if (items->isHaveItem(index)) {
                items->setHaveItemVisibility(0, index);
            }
        }
    }
}
