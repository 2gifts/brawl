#include <ft/ft_status_uniq_process_air_lasso_rewind.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

ftStatusUniqProcessAirLassoRewind g_ftStatusUniqProcessAirLassoRewind;

void ftStatusUniqProcessAirLassoRewind::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: the rewind speed is the float at +4 of the constant block 0xA80E (same block as the lasso frames).
    float rewindSpeed = *(float*)((u8*)soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0) + 4);
    moduleAccesser->getWorkManageModule().setFloat(rewindSpeed, 0x21000004);
}

void ftStatusUniqProcessAirLassoRewind::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched (model node positions clamped against the fighter position).
}

void ftStatusUniqProcessAirLassoRewind::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: not yet matched.
}
