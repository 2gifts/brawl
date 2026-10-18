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

// HYPOTHESIS: work int 0x2000000C is the scope state (0 idle, 1 charging, 2 ..., 3 hold fire, 4 end), 0x2000000D a frame
// counter reloaded from status constant 0x5A60, 0x2000000E a second counter; work flag 0x22000013 records the release.
void ftStatusUniqProcessItemShoot::initStatus_SScope(soModuleAccesser* moduleAccesser) {
    if (!ftUtil::isItemShootStatus(moduleAccesser, moduleAccesser->getStatusModule().getPrevStatusKind(0))) {
        detachHoldAnim(moduleAccesser);
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        moduleAccesser->getWorkManageModule().setInt(0, 0x2000000C);
        int reload = soValueAccesser::getConstantInt(moduleAccesser, 0x5A60, 0);
        moduleAccesser->getWorkManageModule().setInt(reload, 0x2000000D);
        moduleAccesser->getWorkManageModule().setInt(0, 0x2000000E);
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1F91), false, false, 0);
    }
    int bullets = moduleAccesser->getItemManageModule().getShootItemBullet(0);
    switch (moduleAccesser->getWorkManageModule().getInt(0x2000000C)) {
    case 0:
        if (bullets == 0) {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x16B, 0x16F);
        } else {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x163, 0x167);
        }
        break;
    case 1:
        if (bullets == 0) {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x16C, 0x170);
        } else {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x164, 0x168);
        }
        break;
    case 2:
        if (bullets == 0) {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x16B, 0x16F);
        } else {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x163, 0x167);
        }
        break;
    case 3:
        if (bullets == 0) {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x16D, 0x171);
        } else {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x165, 0x169);
        }
        moduleAccesser->getSoundModule().stopSE(0x1F93, 0);
        break;
    case 4:
        if (bullets == 0) {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x16E, 0x172);
        } else {
            ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x166, 0x16A);
        }
        break;
    }
}

void ftStatusUniqProcessItemShoot::execStatus_SScope(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x2000000C)) {
    case 0:
        if (moduleAccesser->getControllerModule().getRelease() & soController::getButtonMask(soController::Pad_Button_Attack)) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000013);
        }
        break;
    case 1:
        if (moduleAccesser->getControllerModule().getTrigger() & soController::getButtonMask(soController::Pad_Button_Attack)) {
            int reload = soValueAccesser::getConstantInt(moduleAccesser, 0x5A60, 0);
            moduleAccesser->getWorkManageModule().setInt(reload, 0x2000000D);
        }
        break;
    }
    ftStatusUniqProcessShoot::execShoot_Common(moduleAccesser);
}

void ftStatusUniqProcessItemShoot::execFixPosCounter_SScope(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x2000000C)) {
    case 0: {
        moduleAccesser->getWorkManageModule().incInt(0x2000000E);
        bool motionEnd;
        if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
            motionEnd = moduleAccesser->getMotionModule().isEndPartial(1);
        } else {
            motionEnd = moduleAccesser->getMotionModule().isEnd();
        }
        if (motionEnd == true) {
            if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
                toRapid_SScope(moduleAccesser);
                if ((int)moduleAccesser->getItemManageModule().getShootItemBullet(0) > 0) {
                    dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner()->addLogInt(1, 0x1D, 0);
                }
            } else {
                moduleAccesser->getWorkManageModule().setInt(2, 0x2000000C);
                moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1F93), false, false, 0);
                ftLogTransactor::resetLogActionInfo(2, 0x66, 1, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
            }
        }
        break;
    }
    case 1: {
        bool motionEnd;
        if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
            motionEnd = moduleAccesser->getMotionModule().isEndPartial(1);
        } else {
            motionEnd = moduleAccesser->getMotionModule().isEnd();
        }
        if (motionEnd == true) {
            int remaining = moduleAccesser->getWorkManageModule().getInt(0x2000000D);
            moduleAccesser->getWorkManageModule().setInt(remaining - 1, 0x2000000D);
            if (remaining - 1 <= 0) {
                toEnd_SScope(moduleAccesser);
            } else {
                if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
                    moduleAccesser->getMotionModule().setPartialFrameSyncAnimCmd(0.0f, 1);
                } else {
                    moduleAccesser->getMotionModule().setFrameSyncAnimCmd(0.0f);
                }
                if (moduleAccesser->getItemManageModule().getShootItemBullet(0) == 0) {
                    moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1F9D), false, false, 0);
                }
            }
        }
        break;
    }
    case 2: {
        moduleAccesser->getWorkManageModule().incInt(0x2000000E);
        int elapsed = moduleAccesser->getWorkManageModule().getInt(0x2000000E);
        if (!(moduleAccesser->getControllerModule().getButton() & soController::getButtonMask(soController::Pad_Button_Attack)) ||
            elapsed >= soValueAccesser::getConstantInt(moduleAccesser, 0x5A5F, 0)) {
            toHoldFire_SScope(moduleAccesser);
        }
        break;
    }
    }
}

// TODO: remaining bodies not decoded yet.
void ftStatusUniqProcessItemShoot::execFixPos_SScope(soModuleAccesser*) { }

void ftStatusUniqProcessItemShoot::exit_SScope(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (!ftUtil::isItemShootStatus(moduleAccesser, nextStatusKind)) {
        attachHoldAnim(moduleAccesser, 2);
        moduleAccesser->getSoundModule().stopSE(0x1F93, 0);
        if (moduleAccesser->getWorkManageModule().getInt(0x2000000C) == 2) {
            int bullets = moduleAccesser->getWorkManageModule().getInt(0x2000000E);
            moduleAccesser->getItemManageModule().shootItemBulletBlanks(bullets, 0);
        }
    }
    ftStatusUniqProcessShoot::exitShoot_Common(moduleAccesser, nextStatusKind);
}

// The three scope transitions below switch the aim motion (partial animation when one is playing) to the variant for
// charged / empty weapon: toRapid starts the rapid charge (state 1), toHoldFire the held shot (state 3), toEnd the wind-down (state 4).
void ftStatusUniqProcessItemShoot::toHoldFire_SScope(soModuleAccesser* moduleAccesser) {
    int bullets = moduleAccesser->getItemManageModule().getShootItemBullet(0);
    if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
        moduleAccesser->getMotionModule().addPartialAnimChr(1, bullets == 0 ? 0x16D : 0x165, 0x135, 0, 0.0f, 1.0f, 0);
    } else {
        soMotionChangeParam param;
        param.m_kind = bullets == 0 ? 0x171 : 0x169;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
    moduleAccesser->getSoundModule().stopSE(0x1F93, 0);
    if (bullets == 0) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1F9D), false, false, 0);
    }
    moduleAccesser->getWorkManageModule().setInt(3, 0x2000000C);
    if (bullets > 0) {
        dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner()->addLogInt(1, 0x1D, 0);
    }
    ftLogTransactor::resetLogActionInfo(2, 0x66, 0, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
}

void ftStatusUniqProcessItemShoot::toRapid_SScope(soModuleAccesser* moduleAccesser) {
    int bullets = moduleAccesser->getItemManageModule().getShootItemBullet(0);
    if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
        moduleAccesser->getMotionModule().addPartialAnimChr(1, bullets == 0 ? 0x16C : 0x164, 0x135, 0, 0.0f, 1.0f, 0);
    } else {
        soMotionChangeParam param;
        param.m_kind = bullets == 0 ? 0x170 : 0x168;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
    if (bullets == 0) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1F9D), false, false, 0);
    }
    moduleAccesser->getWorkManageModule().setInt(1, 0x2000000C);
    ftLogTransactor::resetLogActionInfo(2, 0x65, 1, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
}

void ftStatusUniqProcessItemShoot::toEnd_SScope(soModuleAccesser* moduleAccesser) {
    int bullets = moduleAccesser->getItemManageModule().getShootItemBullet(0);
    if (moduleAccesser->getMotionModule().getPartialKind(1) != -1) {
        moduleAccesser->getMotionModule().addPartialAnimChr(1, bullets == 0 ? 0x16E : 0x166, 0x135, 0, 0.0f, 1.0f, 0);
    } else {
        soMotionChangeParam param;
        param.m_kind = bullets == 0 ? 0x172 : 0x16A;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
    }
    moduleAccesser->getWorkManageModule().setInt(4, 0x2000000C);
    moduleAccesser->getWorkManageModule().onFlag(0x22000012);
}
