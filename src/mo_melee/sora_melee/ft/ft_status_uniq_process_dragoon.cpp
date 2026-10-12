#include <ft/ft_status_uniq_process_dragoon.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

ftStatusUniqProcessDragoon g_ftStatusUniqProcessDragoon;

void ftStatusUniqProcessDragoon::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftItemManageSlotView*>(&moduleAccesser->getItemManageModule())->slot40(1);
}

void ftStatusUniqProcessDragoon::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessDragoon::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessDragoon::exitStatus(soModuleAccesser* moduleAccesser, int) {
}
