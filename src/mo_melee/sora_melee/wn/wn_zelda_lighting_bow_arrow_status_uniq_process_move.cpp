#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <ft/zelda/ft_zelda_article_pools.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>

// HYPOTHESIS: the arrow's data record holds a power scale at +8.
struct wnZeldaLightingBowArrowData {
    u8 _pad[8];
    float m_powerScale;
};

// Class shapes from the module map; the method bodies are not reconstructed yet.
class wnZeldaLightingBowArrowStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnZeldaLightingBowArrowStatusUniqProcessMove() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnZeldaLightingBowArrowStatusUniqProcessMove g_wnZeldaLightingBowArrowStatusUniqProcessMove;

void wnZeldaLightingBowArrowStatusUniqProcessMove::initStatus(soModuleAccesser* moduleAccesser) {
    dynamic_cast<wnZeldaLightingBowArrow&>(moduleAccesser->getStageObject());
}

void wnZeldaLightingBowArrowStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
    soStatusModule& status = moduleAccesser->getStatusModule();
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    wnZeldaLightingBowArrow& arrow = dynamic_cast<wnZeldaLightingBowArrow&>(moduleAccesser->getStageObject());
    const wnZeldaLightingBowArrowData* data = arrow.m_data;
    if (status.isCollisionAttackOccer()) {
        if (attack.isAttack(0, false)) {
            attack.setPower(0, static_cast<int>(data->m_powerScale * attack.getPower(0, false)), false);
        }
    }
}
