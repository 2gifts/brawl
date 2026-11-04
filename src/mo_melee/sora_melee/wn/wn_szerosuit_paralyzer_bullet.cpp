#include <wn/zelda/wn_szerosuit_paralyzer_bullet.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>

// Registers the Zerosuit paralyzer bullet article class (kind 64) in the class info table during static initialization.
wnClassInfoImpl<64, wnSZerosuitParalyzerBullet, wnSZerosuitParalyzerBulletData> g_wnSZerosuitParalyzerBulletClassInfo;

void wnSZerosuitParalyzerBullet::activate(int founderTaskId, int team, const Vec3f& position, bool fullCharge, float lr) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, position, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);

    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    Vec2f speed;
    Vec2f accel(0.0f, 0.0f);
    speed.m_x = 0.0f;
    speed.m_y = 0.0f;
    if (fullCharge) {
        work.onFlag(0x22000000);
        speed.m_x = unk17A0->unk14 * lr;
    } else {
        work.offFlag(0x22000000);
        speed.m_x = unk17A0->unk10 * lr;
    }
    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    kinetic.changeKinetic(2, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    Vec3f rotation(0.0f, 0.0f, 0.0f);
    energy.resetEnergy(0, &speed, &rotation, moduleAccesser);
    energy.m_accel = accel;
    energy.m_speedLimit = speed;
    energy.m_speedTarget = speed;
    energy.enable();
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}
