#include <types.h>
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>

inline soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) { }

void ftStatusUniqProcessGimmickLadder::initLadderEnd(soModuleAccesser* moduleAccesser, soGimmickLadderEventArgs* info) {
    if (1.0f == moduleAccesser->getPostureModule().getLr()) {
        moduleAccesser->getMotionModule().changeMotionRequest(&soMotionChangeParam(0x186, 0.0f, 1.0f, 0, 0, 0, 0));
    } else {
        moduleAccesser->getMotionModule().changeMotionRequest(&soMotionChangeParam(0x187, 0.0f, 1.0f, 0, 0, 0, 0));
    }
    Vec3f pos(moduleAccesser->getPostureModule().getPos());
    moduleAccesser->getWorkManageModule().setFloat(info->m_upperNodePos.m_x, 0x21000006);
    moduleAccesser->getWorkManageModule().setFloat(info->m_upperNodePos.m_y, 0x21000007);
    moduleAccesser->getWorkManageModule().setFloat(pos.m_x, 0x21000008);
    moduleAccesser->getWorkManageModule().setFloat(pos.m_y, 0x21000009);
}

void ftStatusUniqProcessGimmickLadder::fixPosLadderEnd(soModuleAccesser* moduleAccesser, soGimmickLadderEventArgs* info) {
    Vec3f upperPos(info->m_upperNodePos.m_x, info->m_upperNodePos.m_y, 0.0f);
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    if (moduleAccesser->getMotionModule().isEnd() == true) {
        moduleAccesser->getPostureModule().setPos(&upperPos);
        if (moduleAccesser->getGroundModule().attachGround(0) == true) {
            moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
            moduleAccesser->getStatusModule().changeStatusRequest(0, moduleAccesser);
        } else {
            moduleAccesser->getSituationModule().setKind(Situation_Air, false);
            moduleAccesser->getStatusModule().changeStatusRequest(0xE, moduleAccesser);
        }
    } else if (1.0f + upperPos.m_y < pos.m_y) {
        moduleAccesser->getSituationModule().setKind(Situation_Air, false);
        moduleAccesser->getStatusModule().changeStatusRequest(0xE, moduleAccesser);
    }
}
