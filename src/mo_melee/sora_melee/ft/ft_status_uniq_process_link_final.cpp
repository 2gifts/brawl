#include <ft/ft_status_uniq_process_link_final.h>
#include <so/so_module_accesser.h>
#include <types.h>

ftStatusUniqProcessLinkFinal g_ftStatusUniqProcessLinkFinal;

void ftStatusUniqProcessLinkFinal::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched.
}

void ftStatusUniqProcessLinkFinal::execFixPos(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: not yet matched.
}

void ftStatusUniqProcessLinkFinal::exitStatus(soModuleAccesser* moduleAccesser, int) {
    // HYPOTHESIS: not yet matched.
}

bool ftStatusUniqProcessLinkFinal::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    moduleAccesser->getStatusModule().changeStatusRequest(0xed, moduleAccesser);
    return true;
}
