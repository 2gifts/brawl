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
