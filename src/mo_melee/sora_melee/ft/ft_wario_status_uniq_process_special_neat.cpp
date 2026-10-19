// MATCH-ONLY: use the existing Fighter virtual layout selected by the native eat-item call.
#define FT_FIGHTER_ANIMCMD_LONG
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <ft/fighter.h>
#include <so/model/so_model_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <types.h>

class ftWarioStatusUniqProcessSpecialNEat : public soStatusUniqProcess {
public:
    virtual ~ftWarioStatusUniqProcessSpecialNEat() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

ftWarioStatusUniqProcessSpecialNEat g_ftWarioStatusUniqProcessSpecialNEat;

// Animation flags first hide the stored mouth items, then consume them once.
// The fighter hook applies each item's eat effects at the mouth position.
void ftWarioStatusUniqProcessSpecialNEat::execFixPosCounter(soModuleAccesser* a) {
    soWorkManageModule& work = a->getWorkManageModule();
    soItemManageModuleImpl& items = dynamic_cast<soItemManageModuleImpl&>(a->getItemManageModule());
    if (work.isFlag(0x22000012)) {
        for (int i = 3; i < 7; ++i) {
            if (items.isHaveItem(i)) items.setHaveItemVisibility(0, i);
        }
    }
    if (work.isFlag(0x22000013)) {
        Fighter& fighter = dynamic_cast<Fighter&>(a->getStageObject());
        for (int i = 3; i < 7; ++i) {
            if (items.isHaveItem(i)) {
                soItemInfo info;
                items.getHaveItemInfo(&info, i);
                if (info.m_trait.m_mask & itParam::TRAIT_MASK_BOMB) {
                    // Bomb-type items apply the signed damage tuning before eating.
                    a->getDamageModule().addDamage(a->getConstantFloatKirby(0xFA5), 0);
                }
                a->getItemManageModule().useItem(i);
                int node = a->getModelModule().getCorrectNodeId(300);
                Vec3f position = a->getModelModule().getNodeGlobalPosition(node, false);
                soLinkTouchItemEventArgs event(-1, info.m_item, position, 0.0f, false);
                fighter.eatItem(&event);
            }
        }
        work.offFlag(0x22000013);
    }
}

void ftWarioStatusUniqProcessSpecialNEat::exitStatus(soModuleAccesser* moduleAccesser, int) {
    for (int index = 3; index < 7; index++) {
        if (moduleAccesser->getItemManageModule().isHaveItem(index)) {
            moduleAccesser->getItemManageModule().removeItem(index);
        }
    }
}
