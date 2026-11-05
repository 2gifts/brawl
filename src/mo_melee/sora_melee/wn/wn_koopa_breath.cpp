#include <types.h>
#include <ft/koopa/ft_koopa_article_pools.h>
#include <so/so_module_accesser.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <math.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>

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

void wnKoopaBreath::activate(int founderTaskId, int team, const Vec3f* position, int kind,
                            float lr, float speed, float scale, float speedMul, float angle) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0,
                        *position, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(kind, 0x10000007);
    m_moduleAccesser->getWorkManageModule().setInt(unk1F7C->unk4, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setInt(unk1F7C->unk8, 0x10000006);
    m_moduleAccesser->getWorkManageModule().setFloat(scale, 0x11000001);
    m_moduleAccesser->getGroundModule().setOffsetY(unk1F7C->unk0, 0);
    m_moduleAccesser->getPostureModule().setScale(scale);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    __typeof__(unk1F7C) param = unk1F7C;
    float magnitude = speedMul * (speed * (param->unkC + (param->unk10 - param->unkC) * randf()));
    energy.m_speed = Vec2f(lr * (magnitude * (float)cos(angle)), magnitude * (float)sin(angle));
    updateNodeSRT();
}
