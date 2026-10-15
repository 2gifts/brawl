#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of getOwner.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_log_transactor.h>
#include <ft/ft_status_uniq_process_item_shoot.h>
#include <ft/ft_status_uniq_process_shoot.h>
#include <ft/ft_util.h>
#include <math.h>
#include <nw4r/math/math_arithmetic.h>
#include <so/controller/so_controller_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftStatusUniqProcessItemShoot::initStatus_FFlower(soModuleAccesser* moduleAccesser) {
    if (!ftUtil::isItemShootStatus(moduleAccesser, moduleAccesser->getStatusModule().getPrevStatusKind(0))) {
        detachHoldAnim(moduleAccesser);
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        moduleAccesser->getWorkManageModule().setInt(0, 0x2000000C);
        ftLogTransactor::resetLogActionInfo(2, 0x63, 1, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
    }
    ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x161, 0x162);
}

// HYPOTHESIS: work int 0x2000000C is the fire flower state (0 aiming, 1 spraying, 2 stopping), 0x2000000D the spray
// frame counter and 0x2000000E the delay counter before the spray stops.
void ftStatusUniqProcessItemShoot::execStatus_FFlower(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x2000000C)) {
    case 0:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
                moduleAccesser->getMotionModule().setPartialRate(0.0f, 1);
            } else {
                moduleAccesser->getMotionModule().setRate(0.0f);
            }
            moduleAccesser->getWorkManageModule().setInt(1, 0x2000000C);
            if ((int)moduleAccesser->getItemManageModule().getShootItemBullet(0) > 0) {
                dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner()->addLogInt(1, 0x1D, 0);
            }
        }
        break;
    case 1:
        if (!(moduleAccesser->getControllerModule().getButton() & soController::getButtonMask(soController::Pad_Button_Attack))) {
            int holdFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5A63, 0);
            if ((u8)moduleAccesser->getControllerModule().getTriggerCount(0) >= holdFrames) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000014);
                moduleAccesser->getWorkManageModule().onFlag(0x22000012);
            }
        }
        break;
    }
    ftStatusUniqProcessShoot::execShoot_Common(moduleAccesser);
}

void ftStatusUniqProcessItemShoot::execFixPosCounter_FFlower(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x2000000C)) {
    case 1: {
        int counter = moduleAccesser->getWorkManageModule().getInt(0x2000000D);
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
            counter = moduleAccesser->getWorkManageModule().getInt(0x2000000D);
            if (counter >= soValueAccesser::getConstantInt(moduleAccesser, 0x5A61, 0)) {
                if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
                    moduleAccesser->getMotionModule().setPartialRate(1.0f, 1);
                } else {
                    moduleAccesser->getMotionModule().setRate(1.0f);
                }
                moduleAccesser->getWorkManageModule().setInt(2, 0x2000000C);
            }
        }
        int delay = moduleAccesser->getWorkManageModule().getInt(0x2000000E);
        moduleAccesser->getWorkManageModule().setInt(delay - 1, 0x2000000E);
        if (delay - 1 <= 0) {
            int reload = soValueAccesser::getConstantInt(moduleAccesser, 0x5A62, 0);
            moduleAccesser->getWorkManageModule().setInt(reload, 0x2000000E);
            moduleAccesser->getWorkManageModule().incInt(0x2000000D);
            if (moduleAccesser->getItemManageModule().getShootItemBullet(0) == 0) {
                moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1FA2), false, false, 0);
            } else {
                moduleAccesser->getItemManageModule().shootItemBullet(0, 0.0f, 0);
                moduleAccesser->getControllerModule().setRumble(0, 0, false, -1);
                if (counter % 2 == 0) {
                    struct {
                        Vec3f rotRange;
                        Vec3f posRange;
                        Vec3f rot;
                        Vec3f pos;
                    } zero;
                    zero.rotRange.m_x = 0.0f;
                    zero.rotRange.m_y = 0.0f;
                    zero.rotRange.m_z = 0.0f;
                    zero.posRange.m_x = 0.0f;
                    zero.posRange.m_y = 0.0f;
                    zero.posRange.m_z = 0.0f;
                    zero.rot.m_x = 0.0f;
                    zero.rot.m_y = 0.0f;
                    zero.rot.m_z = 0.0f;
                    zero.pos.m_x = 0.0f;
                    zero.pos.m_y = 0.0f;
                    zero.pos.m_z = 0.0f;
                    moduleAccesser->getEffectModule().req(static_cast<EfID>(0xA), 0, &zero.pos, &zero.rot, 1.0f, &zero.posRange, &zero.rotRange, false, 0);
                }
            }
        }
        break;
    }
    }
}

void ftStatusUniqProcessItemShoot::execFixPos_FFlower(soModuleAccesser*) { }

void ftStatusUniqProcessItemShoot::exit_FFlower(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (!ftUtil::isItemShootStatus(moduleAccesser, nextStatusKind)) {
        attachHoldAnim(moduleAccesser, 2);
    }
    ftStatusUniqProcessShoot::exitShoot_Common(moduleAccesser, nextStatusKind);
}
