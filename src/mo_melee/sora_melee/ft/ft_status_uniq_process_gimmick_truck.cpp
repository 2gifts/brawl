#include <ft/ft_status_uniq_process_gimmick.h>
#include <so/so_module_accesser.h>
#include <types.h>

void ftStatusUniqProcessGimmickTruck::notifyEvent(soGimmickEventArgs* eventInfo, soModuleAccesser* moduleAccesser) {
    if (eventInfo->m_kind <= 0x1B || eventInfo->m_kind >= 0x22) {
        ftStatusUniqProcessGimmick::notifyEvent(eventInfo, moduleAccesser);
    } else {
        switch (eventInfo->m_kind) {
        case Gimmick::Truck_Event_Start_Run:
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
            break;
        case Gimmick::Truck_Event_Pos: {
            soGimmickTruckEventArgs* args = static_cast<soGimmickTruckEventArgs*>(eventInfo);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            moduleAccesser->getWorkManageModule().setFloat(args->m_pos.m_x - pos.m_x, 0x21000004);
            moduleAccesser->getWorkManageModule().setFloat(args->m_pos.m_y - pos.m_y, 0x21000005);
            pos.m_x = args->m_pos.m_x;
            pos.m_y = args->m_pos.m_y;
            moduleAccesser->getPostureModule().setPos(&pos);
            break;
        }
        case Gimmick::Truck_Event_Crash:
            moduleAccesser->getWorkManageModule().offFlag(0x22000010);
            moduleAccesser->getStatusModule().changeStatusRequest(0x49, moduleAccesser);
            break;
        }
    }
}
