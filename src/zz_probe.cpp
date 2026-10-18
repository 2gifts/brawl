#include <so/so_module_accesser.h>
#include <so/item/so_item_manage_module_impl.h>
void p0_activate(soModuleAccesser* a) { a->getItemManageModule().activate(); }
void p1_deactivate(soModuleAccesser* a) { a->getItemManageModule().deactivate(); }
void p2_begin(soModuleAccesser* a) { a->getItemManageModule().begin(); }
void p3_check(soModuleAccesser* a) { a->getItemManageModule().check(); }
void p4_haveItem(soModuleAccesser* a) { a->getItemManageModule().haveItem((itKind)0, 0, 0, 0); }
void p5_createThrowItem(soModuleAccesser* a) { a->getItemManageModule().createThrowItem((itKind)0, 0, 0, 0.0f, 0.0f, 0.0f); }
void p6_haveItem(soModuleAccesser* a) { a->getItemManageModule().haveItem((BaseItem*)0, 0, 0, 0); }
void p7_haveItem(soModuleAccesser* a) { a->getItemManageModule().haveItem(0, (BaseItem*)0, 0, 0, 0); }
void p8_useItem(soModuleAccesser* a) { a->getItemManageModule().useItem((BaseItem*)0); }
void p9_attachItem(soModuleAccesser* a) { a->getItemManageModule().attachItem((BaseItem*)0, 0); }
void p10_pickupItem(soModuleAccesser* a) { a->getItemManageModule().pickupItem(0, 0, 0, 0); }
void p11_isSuccessPickupItem(soModuleAccesser* a) { a->getItemManageModule().isSuccessPickupItem(); }
void p12_isHaveItem(soModuleAccesser* a) { a->getItemManageModule().isHaveItem(0); }
void p13_getHaveItemSize(soModuleAccesser* a) { a->getItemManageModule().getHaveItemSize(0); }
void p14_getHaveItemKind(soModuleAccesser* a) { a->getItemManageModule().getHaveItemKind(0); }
void p15_getHaveItemTrait(soModuleAccesser* a) { a->getItemManageModule().getHaveItemTrait(0); }
void p16_getHaveItemHoldKind(soModuleAccesser* a) { a->getItemManageModule().getHaveItemHoldKind(0); }
void p17_getHaveItemParamInt(soModuleAccesser* a) { a->getItemManageModule().getHaveItemParamInt(0, 0); }
void p18_getHaveItemParamFloat(soModuleAccesser* a) { a->getItemManageModule().getHaveItemParamFloat(0, 0); }
void p19_getHaveItemInfo(soModuleAccesser* a) { a->getItemManageModule().getHaveItemInfo((soItemInfo*)0, 0); }
void p20_getPickableItemSize(soModuleAccesser* a) { a->getItemManageModule().getPickableItemSize(); }
void p21_getPickableItemKind(soModuleAccesser* a) { a->getItemManageModule().getPickableItemKind(); }
void p22_getPickableItemTrait(soModuleAccesser* a) { a->getItemManageModule().getPickableItemTrait(); }
void p23_getPickableItemInfo(soModuleAccesser* a) { a->getItemManageModule().getPickableItemInfo((soItemInfo*)0); }
void p24_removeItem(soModuleAccesser* a) { a->getItemManageModule().removeItem((BaseItem*)0); }
void p25_removeItem(soModuleAccesser* a) { a->getItemManageModule().removeItem(0); }
void p26_removeAll(soModuleAccesser* a) { a->getItemManageModule().removeAll(); }
void p27_throwItem(soModuleAccesser* a) { a->getItemManageModule().throwItem(0.0f, 0.0f, 0.0f, 0); }
void p28_throwItem(soModuleAccesser* a) { a->getItemManageModule().throwItem(0.0f, 0.0f, 0.0f, (Vec2f*)0, 0); }
void p29_dropItem(soModuleAccesser* a) { a->getItemManageModule().dropItem(0.0f, 0.0f, 0); }
void p30_useItem(soModuleAccesser* a) { a->getItemManageModule().useItem(0); }
void p31_warpItem(soModuleAccesser* a) { a->getItemManageModule().warpItem((Vec3f*)0, 0); }
void p32_bornItem(soModuleAccesser* a) { a->getItemManageModule().bornItem(0); }
void p33_shootItemBullet(soModuleAccesser* a) { a->getItemManageModule().shootItemBullet(0, 0.0f, 0); }
void p34_shootItemBulletBlanks(soModuleAccesser* a) { a->getItemManageModule().shootItemBulletBlanks(0, 0); }
void p35_getShootItemBullet(soModuleAccesser* a) { a->getItemManageModule().getShootItemBullet(0); }
void p36_updateHaveItemActionInfo(soModuleAccesser* a) { a->getItemManageModule().updateHaveItemActionInfo(0); }
void p37_setHaveItemScaleAnim(soModuleAccesser* a) { a->getItemManageModule().setHaveItemScaleAnim(0.0f, 0, 0); }
void p38_setHaveItemAction(soModuleAccesser* a) { a->getItemManageModule().setHaveItemAction(0, 0.0f, 0); }
void p39_setHaveItemVisibility(soModuleAccesser* a) { a->getItemManageModule().setHaveItemVisibility(0, 0); }
void p40_setHaveItemHoldAnim(soModuleAccesser* a) { a->getItemManageModule().setHaveItemHoldAnim(0, 0); }
void p41_setHaveItemConstraintNode(soModuleAccesser* a) { a->getItemManageModule().setHaveItemConstraintNode(0, 0); }
void p42_resetHaveItemConstraintNode(soModuleAccesser* a) { a->getItemManageModule().resetHaveItemConstraintNode(0); }
void p43_attachItem(soModuleAccesser* a) { a->getItemManageModule().attachItem((itKind)0, 0, 0); }
void p44_isAttachItem(soModuleAccesser* a) { a->getItemManageModule().isAttachItem((itKind)0); }
void p45_getAttachItemCount(soModuleAccesser* a) { a->getItemManageModule().getAttachItemCount(); }
void p46_getAttachItemInfo(soModuleAccesser* a) { a->getItemManageModule().getAttachItemInfo((soItemInfo*)0, 0); }
void p47_setAttachItemVisibility(soModuleAccesser* a) { a->getItemManageModule().setAttachItemVisibility(0, (itParam::AttachGroup)0); }
void p48_getAttachItemVisibility(soModuleAccesser* a) { a->getItemManageModule().getAttachItemVisibility(); }
void p49_ejectHaveItem(soModuleAccesser* a) { a->getItemManageModule().ejectHaveItem(0, 0, 0); }
void p50_ejectAttachItem(soModuleAccesser* a) { a->getItemManageModule().ejectAttachItem(0, 0, 0); }
void p51_eject(soModuleAccesser* a) { a->getItemManageModule().eject(0); }
void p52_ejectAttach(soModuleAccesser* a) { a->getItemManageModule().ejectAttach((itKind)0, 0, 0); }
void p53_dropAttach(soModuleAccesser* a) { a->getItemManageModule().dropAttach((itKind)0, 0.0f, 0.0f); }
void p54_dropAttachGroup(soModuleAccesser* a) { a->getItemManageModule().dropAttachGroup((itParam::AttachGroup)0, 0.0f, 0.0f); }
void p55_scaleAttach(soModuleAccesser* a) { a->getItemManageModule().scaleAttach((itKind)0, 0.0f); }
void p56_scaleItem(soModuleAccesser* a) { a->getItemManageModule().scaleItem((BaseItem*)0, 0.0f); }
void p57_setHaveItemTeam(soModuleAccesser* a) { a->getItemManageModule().setHaveItemTeam(0, 0); }
