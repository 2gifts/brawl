#include <ft/ft_status_uniq_process_hammer.h>
#include <so/so_module_accesser.h>
#include <types.h>

ftStatusUniqProcessHammer g_ftStatusUniqProcessHammer;

void ftStatusUniqProcessHammer::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched.
}

void ftStatusUniqProcessHammer::execStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched.
}

void ftStatusUniqProcessHammer::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched.
}

void ftStatusUniqProcessHammer::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftStatusUniqProcessHammer::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: not yet matched (calls ftUtil::isHammerStatus and the item/hammer module updates).
}

bool ftStatusUniqProcessHammer::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return false;
}
