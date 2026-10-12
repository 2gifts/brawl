#include <ft/ft_status_uniq_process_air_lasso.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

ftStatusUniqProcessAirLasso g_ftStatusUniqProcessAirLasso;

void ftStatusUniqProcessAirLasso::initStatus(soModuleAccesser* moduleAccesser) {
    int lassoFrames = *(int*)soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0);
    moduleAccesser->getWorkManageModule().setInt(lassoFrames, 0x20000001);
}

void ftStatusUniqProcessAirLasso::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getLinkModule().isLink(3) == 1) {
        reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->unlinkSlot164(0);
    }
}
