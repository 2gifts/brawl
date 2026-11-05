#include <so/reflect/so_reflect_module.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/poke/wn_poke_lizardon_breath.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
#include <math.h>

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

void wnPokeLizardonBreath::activate(int founderTaskId, int team, const Vec3f& position, int variation,
                                    float lr, float speed, float scale, float speedMul, float angle) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, position,
                        lr, team, unk1648->unk4, 5, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(variation, 0x10000006);
    m_moduleAccesser->getWorkManageModule().setInt(unk1648->unk8, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setFloat(scale, 0x11000001);
    m_moduleAccesser->getGroundModule().setOffsetY(unk1648->unk0, 0);
    m_moduleAccesser->getPostureModule().setScale(scale);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*m_moduleAccesser->getKineticModule().getEnergy(0));
    float randomSpeed = unk1648->unkC + (unk1648->unk10 - unk1648->unkC) * randf();
    speedMul *= speed * randomSpeed;
    energy.m_speed = Vec2f(lr * (speedMul * (float)cos(angle)), speedMul * (float)sin(angle));
    updateNodeSRT();
}
