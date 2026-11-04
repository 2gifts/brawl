#include <so/reflect/so_reflect_module.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/poke/wn_poke_lizardon_breath.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"

bool wnPokeLizardonBreath::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

bool wnPokeLizardonBreath::reflect() {
    int life = m_moduleAccesser->getWorkManageModule().getInt(0x10000005);
    if (life > 0) {
        float multiplier = m_moduleAccesser->getReflectModule().getLifeMul();
        life = (int)(life * multiplier);
        m_moduleAccesser->getWorkManageModule().setInt(life, 0x10000005);
    }
    return Weapon::reflect();
}
