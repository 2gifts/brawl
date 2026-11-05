#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>
#include <types.h>

void ftStatusUniqProcessGimmickDoor::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessGimmick::initStatus(moduleAccesser);
    soGimmickDoorEventArgs_In eventInfo;
    presentEvent(&eventInfo, moduleAccesser);
    moduleAccesser->getWorkManageModule().setFloat(eventInfo.m_pos.m_x, 0x21000004);
    moduleAccesser->getWorkManageModule().setFloat(eventInfo.m_pos.m_y, 0x21000005);
    if (eventInfo.m_noInto == true) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    }
    switch (moduleAccesser->getWorkManageModule().getInt(0x10000017)) {
    case 0x28:
    case 0x2A: {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        float distance = eventInfo.m_pos.m_x - pos.m_x;
        int motionKind;
        int kineticType;
        if ((float)__fabs(distance) < 1.0f) {
            kineticType = 0x32;
            if (!moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
                motionKind = 0xB;
                moduleAccesser->getWorkManageModule().setInt(2, 0x20000000);
            } else {
                motionKind = 0;
                moduleAccesser->getWorkManageModule().setInt(3, 0x20000000);
            }
        } else if (distance * moduleAccesser->getPostureModule().getLr() < 0.0f) {
            motionKind = 0x11;
            kineticType = 0x32;
            moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
        } else {
            motionKind = 0xB;
            kineticType = 0xE;
            moduleAccesser->getWorkManageModule().setInt(1, 0x20000000);
        }
        if (motionKind != moduleAccesser->getMotionModule().getKind()) {
            soMotionChangeParam motion;
            motion.m_kind = motionKind;
            motion.m_frame = 0.0f;
            motion.m_rate = 1.0f;
            motion._12 = 0;
            motion._13 = 0;
            motion._14 = 0;
            motion._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&motion);
        }
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground_Cliff_Stop, 0);
        moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        moduleAccesser->getKineticModule().changeKinetic(kineticType, moduleAccesser);
        break;
    }
    case 0x29: {
        soMotionChangeParam motion;
        motion.m_kind = 0x38;
        motion.m_frame = 0.0f;
        motion.m_rate = 1.0f;
        motion._12 = 0;
        motion._13 = 0;
        motion._14 = 0;
        motion._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&motion);
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
        moduleAccesser->getSituationModule().setKind(Situation_Air, false);
        moduleAccesser->getKineticModule().changeKinetic(0x32, moduleAccesser);
        break;
    }
    }
}

void ftStatusUniqProcessGimmickDoor::notifyEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: each gimmick owns a range of event kinds between two unnamed bounds.
    if (eventInfo->m_kind > 0x10 && eventInfo->m_kind < 0x12) {
    } else {
        ftStatusUniqProcessGimmick::notifyEvent(eventInfo, moduleAccesser);
    }
}
