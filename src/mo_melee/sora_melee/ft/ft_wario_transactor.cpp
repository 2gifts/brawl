#include <ft/wario/ft_wario_transactor.h>
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_log.h>
#include <wn/weapon.h>

ftWarioTransactor::ftWarioTransactor() { }
ftWarioTransactor::~ftWarioTransactor() { }

void ftWarioTransactor::notifyEventCollisionSearch(soCollisionLog* log, soModuleAccesser* acc) {
    int status = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(
        0, acc->getStatusModule().getStatusKind(), acc, true);
    if (status != 0x112 && status != 0x117) return;

    // A Chomp target has already been selected until the status consumes it.
    soWorkManageModule& work = acc->getWorkManageModule();
    if (work.getInt(0x20000007) != -1) return;

    gfTask* task = NULL;
    if (static_cast<u32>(log->m_taskId) != 0xFFFFFFFF)
        task = gfTask::getTask(log->m_taskId);
    if (task && task->m_taskCategory == gfTask::Category_Weapon) {
        Weapon* weapon = dynamic_cast<Weapon*>(task);
        work.setInt(0, 0x20000008);
        if (!weapon->isCanEat()) return;

        // Read the stored article kind through the verified Weapon implementation.
        // HYPOTHESIS: kind 0x33's identity and the consumer-side meaning of work 8
        // remain to be audited. Kind 0x37 is Diddy's Rocketbarrel (its build data).
        switch (weapon->Weapon::soGetSubKind()) {
        case 0x37:
            work.setInt(1, 0x20000008);
            // fall through
        case 0x33:
            work.setInt(log->m_taskId, 0x20000007);
            break;
        default:
            break;
        }
    }
}
