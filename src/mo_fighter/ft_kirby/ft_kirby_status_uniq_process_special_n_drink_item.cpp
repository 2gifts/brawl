#define FT_FIGHTER_ANIMCMD_LONG
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <types.h>

class soModuleAccesser;

#include <so/status/so_status_uniq_process.h>

// BYTES: defined ahead of the other headers, as a unit that includes its own class header first would have it. Vtables and
// RTTI come out in reverse order of class definition, and the original has this vtable after the RTTI of
// soItemManageModuleImpl, its event presenter and soItemManageModule.
// HYPOTHESIS: Kirby's Special N drink-item status process (the inhaled item is eaten, status kinds 0x11d-0x120 of the mouth
// statuses): it picks the motions by the kind of item in the mouth slots 3..6, then eats the items.
class ftKirbyStatusUniqProcessSpecialNDrinkItem : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialNDrinkItem() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

#include <ft/fighter.h>
#include <ft/kirby/ft_kirby_unk8.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

static ftKirbyUnk8 g_ftKirbyUnk8A(0xff, 0);
static ftKirbyUnk8 g_ftKirbyUnk8B(0xff, 1);

// The motions follow what Kirby holds: bombs, heavy items, food, or anything else (the work 0x20000005 gives the same
// choice when the item was already known).
void ftKirbyStatusUniqProcessSpecialNDrinkItem::initStatus(soModuleAccesser* moduleAccesser) {
    bool heavy = false;
    bool food = false;
    bool bomb = false;
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200001c)) {
        switch (moduleAccesser->getWorkManageModule().getInt(0x20000005)) {
        case 1:
            heavy = true;
            break;
        case 2:
            bomb = true;
            break;
        }
    } else {
        soItemManageModule& items = moduleAccesser->getItemManageModule();
        for (int i = 3; i < 7; ++i) {
            if (items.isHaveItem(i)) {
                soItemInfo info;
                items.getHaveItemInfo(&info, i);
                if (items.getHaveItemSize(i) == itParam::Size_Heavy) {
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
    }
    if (bomb) {
        moduleAccesser->getWorkManageModule().setInt(0x1d4, 0x20000000);
        moduleAccesser->getWorkManageModule().setInt(0x1de, 0x20000001);
    } else if (heavy) {
        moduleAccesser->getWorkManageModule().setInt(0x1d5, 0x20000000);
        moduleAccesser->getWorkManageModule().setInt(0x1df, 0x20000001);
    } else if (food) {
        moduleAccesser->getWorkManageModule().setInt(0x1d3, 0x20000000);
        moduleAccesser->getWorkManageModule().setInt(0x1dd, 0x20000001);
    } else {
        moduleAccesser->getWorkManageModule().setInt(0x1d6, 0x20000000);
        moduleAccesser->getWorkManageModule().setInt(0x1e0, 0x20000001);
    }
}

void ftKirbyStatusUniqProcessSpecialNDrinkItem::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialNDrinkItem::execStop(soModuleAccesser* moduleAccesser) {
}

// BYTES: the item pointer is read through an inline accessor; reading info.m_item directly loads it after the event's first
// stores instead of before them. HYPOTHESIS: the original's accessor name and owner are not known.
static inline BaseItem* getEatenItem(const soItemInfo& info) { return info.m_item; }

// Animation flags first hide the mouth items, then eat them once: the fighter hook applies the effects of the item, and the damage
// changes by the amount of the item (a bomb adds damage, other items heal up to a limit).
void ftKirbyStatusUniqProcessSpecialNDrinkItem::execFixPos(soModuleAccesser* moduleAccesser) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x2200001c)) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            soItemManageModuleImpl& items = dynamic_cast<soItemManageModuleImpl&>(moduleAccesser->getItemManageModule());
            for (int i = 3; i < 7; ++i) {
                if (items.isHaveItem(i)) {
                    items.setHaveItemVisibility(0, i);
                }
            }
            moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
            soItemManageModuleImpl& items = dynamic_cast<soItemManageModuleImpl&>(moduleAccesser->getItemManageModule());
            Fighter& fighter = dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
            for (int i = 3; i < 7; ++i) {
                if (items.isHaveItem(i)) {
                    soItemInfo info;
                    items.getHaveItemInfo(&info, i);
                    float damage = moduleAccesser->getDamageModule().getDamage(0);
                    float amount = 0.0f;
                    if (info.m_trait.m_mask & (itParam::TRAIT_MASK_FOOD | itParam::TRAIT_MASK_RECOVER | itParam::TRAIT_MASK_QUICK |
                                               itParam::TRAIT_MASK_TOUCH)) {
                        items.useItem(i);
                    } else if (info.m_trait.m_mask & itParam::TRAIT_MASK_BOMB) {
                        amount = soValueAccesser::getConstantFloat(moduleAccesser, 0xfaa, 0);
                    } else if (damage - soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0) < 0.0f) {
                        amount = -damage;
                    } else {
                        amount = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0);
                    }
                    Vec3f position = moduleAccesser->getModelModule().getNodeGlobalPosition(0x196, false);
                    soLinkTouchItemEventArgs event(-1, getEatenItem(info), position, 0.0f, false);
                    fighter.eatItem(&event);
                    moduleAccesser->getDamageModule().addDamage(amount, 0);
                    moduleAccesser->getItemManageModule().removeItem(i);
                }
            }
            moduleAccesser->getWorkManageModule().offFlag(0x22000014);
        }
    }
}

void ftKirbyStatusUniqProcessSpecialNDrinkItem::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x2200001c)) {
        for (int i = 3; i < 7; ++i) {
            if (moduleAccesser->getItemManageModule().isHaveItem(i)) {
                moduleAccesser->getItemManageModule().removeItem(i);
            }
        }
    }
}

ftKirbyStatusUniqProcessSpecialNDrinkItem g_ftKirbyStatusUniqProcessSpecialNDrinkItem;
