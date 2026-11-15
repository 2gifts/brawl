#include <StaticAssert.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <ft/purin/ft_purin_status_uniq_process_special_n.h>
#include <ft/purin/ft_purin_transactor.h>
#include <so/damage/so_damage.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>

ftPurinTransactor::ftPurinTransactor() { }
ftPurinTransactor::~ftPurinTransactor() { }

// Rolling (statuses 0x119-0x11a): hitting a fighter sets work flag 0x22000012.
void ftPurinTransactor::notifyEventCollisionAttackFighter(soCollisionLog* log, soModuleAccesser* moduleAccesser) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true)) {
    case 0x119:
    case 0x11a:
        moduleAccesser->getWorkManageModule().onFlag(0x22000012);
        break;
    }
}

// Rolling: a pending fighter hit bounces the roll back.
bool ftPurinTransactor::notifyEventCollisionAttackCheck(u32 flags, soModuleAccesser* moduleAccesser) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true)) {
    case 0x119:
    case 0x11a:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            ftPurinStatusUniqProcessSpecialNUtility::ftProcHitSpecialNPurin(moduleAccesser);
            moduleAccesser->getWorkManageModule().offFlag(0x22000012);
            return true;
        }
        break;
    }
    return false;
}

// Special N statuses (0x117-0x11d): attribute 7 damage turns the roll around.
void ftPurinTransactor::notifyEventOnDamage(soDamage* damage, bool isDamage, soModuleAccesser* moduleAccesser) {
    switch (g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, moduleAccesser->getStatusModule().getStatusKind(), moduleAccesser, true)) {
    case 0x117:
    case 0x118:
    case 0x119:
    case 0x11a:
    case 0x11b:
    case 0x11c:
    case 0x11d:
        if (damage->m_attackData.m_attribute == 7) {
            ftPurinStatusUniqProcessSpecialNUtility::ftProcDamageTurnPurinSpecialN(moduleAccesser);
        }
        break;
    }
}
