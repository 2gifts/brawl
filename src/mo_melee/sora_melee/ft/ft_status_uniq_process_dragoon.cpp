// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of getOwner.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <cm/cm_camera_controller.h>
#include <ft/ft_status_uniq_process_dragoon.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <st/stage.h>
#include <types.h>

ftStatusUniqProcessDragoon g_ftStatusUniqProcessDragoon;

void ftStatusUniqProcessDragoon::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftItemManageSlotView*>(&moduleAccesser->getItemManageModule())->slot40(1);
}

void ftStatusUniqProcessDragoon::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessDragoon::execFixPos(soModuleAccesser* moduleAccesser) {
}

// HYPOTHESIS: on leaving the dragoon the fighter is put back at its restart point (zero pos from the stage) with the
// camera's height, and the invincibility frames of the hit module are set from the status constant 0x5a68.
void ftStatusUniqProcessDragoon::exitStatus(soModuleAccesser* moduleAccesser, int) {
    Fighter& fighter = dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject);
    Stage* stage = g_Stage;
    if (stage != NULL) {
        Vec3f pos;
        pos.m_x = pos.m_y = pos.m_z = 0.0f;
        stage->getFighterReStartPos(&pos, (s8)fighter.getOwner()->getStartPointIndex());
        moduleAccesser->getPostureModule().initPos(&pos);
        pos.m_y = CameraController::getInstance()->unk160;
        moduleAccesser->getPostureModule().setPos(&pos);
    }
    int invincibleFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a68, 0);
    moduleAccesser->getCollisionHitModule().setInvincibleFrameGlobal(invincibleFrames, false, 0);
}
