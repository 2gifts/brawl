#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of getOwner.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_log_transactor.h>
#include <ft/ft_status_uniq_process_shoot.h>
#include <ft/ft_util.h>
#include <so/controller/so_controller_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Shoot statuses of the held shooting items. The work int 0x20000000 holds the item kind (0xD cracker launcher,
// 0x18 fire flower, 0x2C ray gun, 0x3D super scope) and selects the per-item routines below.
class ftStatusUniqProcessItemShoot : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessItemShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    void attachHoldAnim(soModuleAccesser* moduleAccesser, int holdKind) __attribute__((never_inline));
    void detachHoldAnim(soModuleAccesser* moduleAccesser) __attribute__((never_inline));

    void initStatus_SScope(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execStatus_SScope(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execFixPosCounter_SScope(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execFixPos_SScope(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void exit_SScope(soModuleAccesser* moduleAccesser, int nextStatusKind) __attribute__((never_inline));
    void toHoldFire_SScope(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void toRapid_SScope(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void toEnd_SScope(soModuleAccesser* moduleAccesser) __attribute__((never_inline));

    void initStatus_LGun(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execStatus_LGun(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execFixPosCounter_LGun(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execFixPos_LGun(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void exit_LGun(soModuleAccesser* moduleAccesser, int nextStatusKind) __attribute__((never_inline));

    void initStatus_FFlower(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execStatus_FFlower(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execFixPosCounter_FFlower(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execFixPos_FFlower(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void exit_FFlower(soModuleAccesser* moduleAccesser, int nextStatusKind) __attribute__((never_inline));

    void initStatus_CLauncher(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execStatus_CLauncher(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execFixPosCounter_CLauncher(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void execFixPos_CLauncher(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
    void exit_CLauncher(soModuleAccesser* moduleAccesser, int nextStatusKind) __attribute__((never_inline));

    void setHoldAngle(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
};

ftStatusUniqProcessItemShoot g_ftStatusUniqProcessItemShoot;

void ftStatusUniqProcessItemShoot::initStatus(soModuleAccesser* moduleAccesser) {
    int itemKind = moduleAccesser->getItemManageModule().getHaveItemKind(0);
    moduleAccesser->getWorkManageModule().setInt(itemKind, 0x20000000);
    moduleAccesser->getWorkManageModule().setInt(0xA4, 0x20000001);
    moduleAccesser->getWorkManageModule().setInt(0xA5, 0x20000002);
    moduleAccesser->getWorkManageModule().setInt(0xA7, 0x20000003);
    moduleAccesser->getWorkManageModule().setInt(0xA6, 0x20000004);
    moduleAccesser->getWorkManageModule().setInt(0xA8, 0x20000005);
    moduleAccesser->getWorkManageModule().setInt(0xA9, 0x20000006);
    moduleAccesser->getWorkManageModule().setInt(0xAA, 0x20000007);
    moduleAccesser->getWorkManageModule().setInt(0xAD, 0x20000008);
    moduleAccesser->getWorkManageModule().setInt(0xAC, 0x20000009);
    moduleAccesser->getWorkManageModule().setInt(0xAB, 0x2000000A);
    moduleAccesser->getWorkManageModule().setInt(0xAE, 0x2000000B);
    switch (itemKind) {
    case 0x2C:
        initStatus_LGun(moduleAccesser);
        break;
    case 0x3D:
        initStatus_SScope(moduleAccesser);
        break;
    case 0x18:
        initStatus_FFlower(moduleAccesser);
        break;
    case 0xD:
        initStatus_CLauncher(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemShoot::detachHoldAnim(soModuleAccesser* moduleAccesser) {
    dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).detachItemHoldAnim(true);
}

void ftStatusUniqProcessItemShoot::attachHoldAnim(soModuleAccesser* moduleAccesser, int holdKind) {
    dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).attachItemHoldAnim(holdKind, 0);
}

void ftStatusUniqProcessItemShoot::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0x2C:
        execStatus_LGun(moduleAccesser);
        break;
    case 0x3D:
        execStatus_SScope(moduleAccesser);
        break;
    case 0x18:
        execStatus_FFlower(moduleAccesser);
        break;
    case 0xD:
        execStatus_CLauncher(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemShoot::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0x2C:
        execFixPosCounter_LGun(moduleAccesser);
        break;
    case 0x3D:
        execFixPosCounter_SScope(moduleAccesser);
        break;
    case 0x18:
        execFixPosCounter_FFlower(moduleAccesser);
        break;
    case 0xD:
        execFixPosCounter_CLauncher(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemShoot::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0x2C:
        execFixPos_LGun(moduleAccesser);
        break;
    case 0x3D:
        execFixPos_SScope(moduleAccesser);
        break;
    case 0x18:
        execFixPos_FFlower(moduleAccesser);
        break;
    case 0xD:
        execFixPos_CLauncher(moduleAccesser);
        break;
    }
}

void ftStatusUniqProcessItemShoot::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
    case 0x2C:
        exit_LGun(moduleAccesser, nextStatusKind);
        break;
    case 0x3D:
        exit_SScope(moduleAccesser, nextStatusKind);
        break;
    case 0x18:
        exit_FFlower(moduleAccesser, nextStatusKind);
        break;
    case 0xD:
        exit_CLauncher(moduleAccesser, nextStatusKind);
        break;
    }
}

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

void ftStatusUniqProcessItemShoot::initStatus_LGun(soModuleAccesser* moduleAccesser) {
    detachHoldAnim(moduleAccesser);
    if (moduleAccesser->getItemManageModule().getShootItemBullet(0) == 0) {
        ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x15F, 0x160);
        if (!ftUtil::isItemShootStatus(moduleAccesser, moduleAccesser->getStatusModule().getPrevStatusKind(0))) {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1F9F), false, false, 0);
        }
    } else {
        ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x15D, 0x15E);
    }
    if (!ftUtil::isItemShootStatus(moduleAccesser, moduleAccesser->getStatusModule().getPrevStatusKind(0))) {
        ftLogTransactor::resetLogActionInfo(2, 0x62, 1, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
    }
}

void ftStatusUniqProcessItemShoot::execStatus_LGun(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessShoot::execShoot_Common(moduleAccesser);
}

void ftStatusUniqProcessItemShoot::execFixPosCounter_LGun(soModuleAccesser*) { }

void ftStatusUniqProcessItemShoot::execFixPos_LGun(soModuleAccesser*) { }

void ftStatusUniqProcessItemShoot::exit_LGun(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (!ftUtil::isItemShootStatus(moduleAccesser, nextStatusKind)) {
        attachHoldAnim(moduleAccesser, 2);
    }
    ftStatusUniqProcessShoot::exitShoot_Common(moduleAccesser, nextStatusKind);
}

void ftStatusUniqProcessItemShoot::initStatus_FFlower(soModuleAccesser* moduleAccesser) {
    if (!ftUtil::isItemShootStatus(moduleAccesser, moduleAccesser->getStatusModule().getPrevStatusKind(0))) {
        detachHoldAnim(moduleAccesser);
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        moduleAccesser->getWorkManageModule().setInt(0, 0x2000000C);
        ftLogTransactor::resetLogActionInfo(2, 0x63, 1, dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject).getOwner(), moduleAccesser);
    }
    ftStatusUniqProcessShoot::initShoot_Common(moduleAccesser, 0x161, 0x162);
}


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

void ftStatusUniqProcessItemShoot::exit_FFlower(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (!ftUtil::isItemShootStatus(moduleAccesser, nextStatusKind)) {
        attachHoldAnim(moduleAccesser, 2);
    }
    ftStatusUniqProcessShoot::exitShoot_Common(moduleAccesser, nextStatusKind);
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

// TODO: remaining bodies not decoded yet.
void ftStatusUniqProcessItemShoot::execFixPosCounter_SScope(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::execFixPos_SScope(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::toHoldFire_SScope(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::toRapid_SScope(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::toEnd_SScope(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::execFixPos_FFlower(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::initStatus_CLauncher(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::execStatus_CLauncher(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::execFixPosCounter_CLauncher(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::execFixPos_CLauncher(soModuleAccesser*) { }
void ftStatusUniqProcessItemShoot::exit_CLauncher(soModuleAccesser*, int) { }
void ftStatusUniqProcessItemShoot::setHoldAngle(soModuleAccesser*) { }
