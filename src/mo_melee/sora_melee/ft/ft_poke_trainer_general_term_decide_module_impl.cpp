#include <types.h>
#include <ft/ft_poke_trainer_general_term_decide_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"



bool ftPokeTrainerGeneralTermDecideModuleImpl::checkEstablishSub(
    int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser,
    u32* returnWord, soGeneralTermCache* cache)
{
    if (moduleAccesser->getWorkManageModule().getInt(0x10000045) <= 0 &&
        moduleAccesser->getWorkManageModule().isFlag(0x12000040) &&
        moduleAccesser->getWorkManageModule().isFlag(0x12000041) != true) {
        float distance;
        float remaining;
        float direction;
        double distanceWide = fabs(moduleAccesser->getWorkManageModule().getFloat(0x1100001B));
        distance = distanceWide;
        if (distance > moduleAccesser->getWorkManageModule().getFloat(0x11000020)) {
            direction = moduleAccesser->getWorkManageModule().getFloat(0x11000016) < 0.0f
                ? -1.0f : 1.0f;
            float facing = -1.0f == moduleAccesser->getWorkManageModule().getFloat(0x1100001F)
                ? -1.0f : 1.0f;
            if (direction == facing) {
                double remainingWide = fabs(moduleAccesser->getWorkManageModule().getFloat(0x1100001E));
                remaining = remainingWide;
                if (remaining < soValueAccesser::getConstantFloat(moduleAccesser, 0xFA5, 0)) {
                    return false;
                }
                float cliffDistance = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA5, 0);
                if (moduleAccesser->getGroundModule().isNearCliff(direction, cliffDistance, 0) == true) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}
