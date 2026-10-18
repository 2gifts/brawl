#include <ft/ft_entry.h>
#include <ft/ft_status_uniq_process_smash_attack.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <types.h>

class ftNessStatusUniqProcessAttackHi4 : public ftStatusUniqProcessSmashAttack {
public:
    virtual ~ftNessStatusUniqProcessAttackHi4() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

struct ftNessAttackHi4LinkEvent : public soLinkEventArgs {
    float m_value;
    ftNessAttackHi4LinkEvent(int eventKind, float value) : soLinkEventArgs(eventKind), m_value(value) { }
};

void ftNessStatusUniqProcessAttackHi4::initStatus(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessSmashAttack::initStatus(moduleAccesser);
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000003)) {
        float value = moduleAccesser->getCollisionAttackModule().getPowerMulStatus();
        ftNessAttackHi4LinkEvent event(0x456, value);
        moduleAccesser->getLinkModule().sendEventParents(7, event);
    }
}

ftNessStatusUniqProcessAttackHi4 g_ftNessStatusUniqProcessAttackHi4;
