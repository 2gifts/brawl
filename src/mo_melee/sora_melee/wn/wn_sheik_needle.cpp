#include <wn/sheik/wn_sheik_needle.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
#include <math.h>

// Registers the Sheik needle article class (kind 68) in the class info table during static initialization.
wnClassInfoImpl<68, wnSheikNeedle, wnSheikNeedleData> g_wnSheikNeedleClassInfo;

void wnSheikNeedle::updateNodeSRT() {
    Vec3f scale;
    Vec3f rot;
    Vec3f pos;
    scale.m_x = scale.m_y = scale.m_z = m_moduleAccesser->getPostureModule().getScale();
    pos = m_moduleAccesser->getPostureModule().getPos();
    rot = m_moduleAccesser->getPostureModule().getRot(0);
    rot.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rot, &pos);
}

template class wnExtendParamAccesser<3999, 2, 23999, 2, 0, 0>;

bool wnSheikNeedle::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 3) {
        if (randi(3) == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
            return false;
        }
        deactivate(false);
        return true;
    }
    if (flags & 4) {
        if (randi(3) == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
            return false;
        }
        deactivate(false);
        return true;
    }
    if (flags & 0x10) {
        deactivate(false);
        return true;
    }
    if ((flags & 8) && m_moduleAccesser->getReflectModule().isReflect() == true) {
        reflectSettingTeam();
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
        const wnSheikNeedleParam* param = dynamic_cast<wnSheikNeedle&>(m_moduleAccesser->getStageObject()).m_param;
        Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
        double speedX = -speed.m_x;
        double speedY = -speed.m_y;
        float angle = (float)atan2(speedY, speedX);
        Vec3f rot(57.29578f * (angle * -posture.getLr()), 0.0f, 0.0f);
        posture.setRot(&rot, 0);
        speed.m_x = param->unkC * (float)cos(angle);
        speed.m_y = param->unkC * (float)sin(angle);
        wnKineticEnergyNormal& movement = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
        movement.m_speed = speed;
        movement.m_accel = Vec2f(0.0f, 0.0f);
        movement.m_speedTarget = speed;
        movement.m_speedLimit = speed;
        movement.enable();
        float life = m_moduleAccesser->getWorkManageModule().getInt(0x10000003);
        life *= param->unk8;
        m_moduleAccesser->getWorkManageModule().setInt((int)life, 0x10000004);
        return false;
    }
    return false;
}
