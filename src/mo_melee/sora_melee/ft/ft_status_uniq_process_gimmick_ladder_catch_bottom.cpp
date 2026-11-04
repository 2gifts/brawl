#include <types.h>
#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>

void ftStatusUniqProcessGimmickLadder::initLadderCatchBottom(soModuleAccesser* moduleAccesser, soGimmickLadderEventArgs* info) {
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    moduleAccesser->getWorkManageModule().setFloat(pos.m_x, 0x21000006);
    moduleAccesser->getWorkManageModule().setFloat(pos.m_y, 0x21000007);
    moduleAccesser->getWorkManageModule().setFloat(pos.m_x, 0x21000008);
    moduleAccesser->getWorkManageModule().setFloat(pos.m_y, 0x21000009);
}

void ftStatusUniqProcessGimmickLadder::execLadderCatchBottom(soModuleAccesser* moduleAccesser, soGimmickLadderEventArgs* info) {
    adjustLadderMovement(moduleAccesser, info->m_upperNodePos.m_x, info->m_upperNodePos.m_y);
    correctX(moduleAccesser, info->m_upperNodePos.m_x, 1);
}
