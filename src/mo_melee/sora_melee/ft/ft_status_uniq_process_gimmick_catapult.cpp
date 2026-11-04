#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/damage/so_damage_module_actor.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftStatusUniqProcessGimmickCatapult::initStatus(soModuleAccesser* moduleAccesser) {
    soMotionChangeParam param;
    param.m_kind = 0x17E;
    param.m_frame = 1.0f;
    param.m_rate = 0.0f;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&param);
    ftStatusUniqProcessGimmick::initStatus(moduleAccesser);
    soGimmickCatapultEventArgs_On args;
    presentEvent(&args, moduleAccesser);
}

void ftStatusUniqProcessGimmickCatapult::execStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessGimmickCatapult::exitStatus(soModuleAccesser* moduleAccesser, int next) {
    ftStatusUniqProcessGimmick::exitStatus(moduleAccesser, next);
    moduleAccesser->getWorkManageModule().setInt(8, 0x1000000F);
}

void ftStatusUniqProcessGimmickCatapult::notifyEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: each gimmick owns a range of event kinds between two unnamed bounds.
    if (eventInfo->m_kind <= 0xB || eventInfo->m_kind >= 0xF) {
        ftStatusUniqProcessGimmick::notifyEvent(eventInfo, moduleAccesser);
    } else {
        switch (eventInfo->m_kind) {
        case Gimmick::Catapult_Event_Shoot: {
            soGimmickCatapultEventArgs* args = static_cast<soGimmickCatapultEventArgs*>(eventInfo);
            dynamic_cast<soDamageModuleActor&>(moduleAccesser->getDamageModule()).toDamageFlyRollStatus(args->m_vector, args->m_launchSpeed);
            ftManager* manager = g_ftManager;
            if (manager->m_mode == 1) {
                if (manager->getOwner(moduleAccesser->getWorkManageModule().getInt(0x10000000))->getAdventureUnitKind() == 1) {
                    moduleAccesser->getWorkManageModule().onFlag(0x12000034);
                }
            }
            break;
        }
        case Gimmick::Catapult_Event_Pos: {
            soGimmickCatapultEventArgs* args = static_cast<soGimmickCatapultEventArgs*>(eventInfo);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            pos.m_x = args->m_pos.m_x;
            pos.m_y = args->m_pos.m_y;
            moduleAccesser->getPostureModule().setPos(&pos);
            float lr = moduleAccesser->getPostureModule().getLr();
            if (args->m_lr != lr) {
                moduleAccesser->getPostureModule().setLr(args->m_lr);
                moduleAccesser->getPostureModule().updateRotYLr();
            }
            break;
        }
        }
    }
}
