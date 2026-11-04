#include <types.h>
#include <ft/koopa/ft_koopa_article_pools.h>
#include <so/so_module_accesser.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <math.h>

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

} // extern "C"


void wnKoopaBreath::updateNodeSRT() {
    Vec3f scaling;
    scaling.m_x = scaling.m_y = scaling.m_z = m_moduleAccesser->getPostureModule().getScale();
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    Vec2f sumSpeed = m_moduleAccesser->getKineticModule().getSumSpeed(1);
    Vec2f speed = sumSpeed;
    float lr;
    Vec3f rotation;
    // HYPOTHESIS: the breath leans along its velocity and faces its direction.
    lr = m_moduleAccesser->getPostureModule().getLr();
    float facingX = speed.m_x * lr;
    float velY = speed.m_y;
    rotation.m_x = -(float)atan2(velY, facingX) * 57.29578f;
    rotation.m_y = lr == 1.0f ? 90.0f : -90.0f;
    rotation.m_z = 0.0f;
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scaling, &rotation, &position);
}

bool wnKoopaBreath::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
