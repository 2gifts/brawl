#include <types.h>
#include <ft/poke/ft_poke_lizardon_article_pools.h>
#include <so/so_module_accesser.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

void wnPokeLizardonRock::shoot(bool unk) {
    if (unk == true) {
        getModuleAccesser()->getStatusModule().changeStatusForce(3, getModuleAccesser());
        getModuleAccesser()->getWorkManageModule().offFlag(0x12000000);
        return;
    }
    getModuleAccesser()->getStatusModule().changeStatusForce(2, getModuleAccesser());
}
