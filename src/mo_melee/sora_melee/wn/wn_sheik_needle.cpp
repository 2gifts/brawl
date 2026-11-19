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

// TODO: clean up, give variables real names
namespace jj_33DEA8 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__26wnSheikNeedleParamAccesserFv(void* ptr, int size) {
    if (ptr != 0 && size > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_33DEE8 {
extern "C" void* fn_27_33DEE8(void* self) {
    return reinterpret_cast<u8*>(self) + 0x4b4;
}
}

// TODO: clean up, give variables real names
namespace jj_33EC7C {
extern "C" void fn_27_33EC7C(void) {}
}

// TODO: clean up, give variables real names
namespace jj_33EC80 {
extern "C" void* fn_27_33EC80(void* self) {
    return reinterpret_cast<u8*>(self) + 0x9c;
}
}

// TODO: clean up, give variables real names
namespace jj_33EC88 {
extern "C" void* fn_27_33EC88(void* self) {
    return reinterpret_cast<u8*>(self) + 0xdc;
}
}

// TODO: clean up, give variables real names
namespace jj_33EC90 {
extern "C" void* fn_27_33EC90(void* self) {
    return reinterpret_cast<u8*>(self) + 0xb8;
}
}

// TODO: clean up, give variables real names
namespace jj_3414F8 {
extern "C" void fn_27_33D590(void*);

extern "C" void fn_27_3414F8(void* self) {
    fn_27_33D590(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_341500 {
extern "C" void fn_27_33D590(void*);

extern "C" void fn_27_341500(void* self) {
    fn_27_33D590(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_341508 {
extern "C" void fn_27_33D590(void*);

extern "C" void fn_27_341508(void* self) {
    fn_27_33D590(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_341510 {
extern "C" void __dt__13wnSheikNeedleFv(void*);

extern "C" void fn_27_341510(void* self) {
    __dt__13wnSheikNeedleFv(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_341518 {
extern "C" void notifyEventCollisionAttackCheck__13wnSheikNeedleFUl(void*);

extern "C" void fn_27_341518(void* self) {
    notifyEventCollisionAttackCheck__13wnSheikNeedleFUl(reinterpret_cast<u8*>(self) - 0x70);
}
}

// TODO: clean up, give variables real names
namespace jj_341520 {
extern "C" void __dt__13wnSheikNeedleFv(void*);

extern "C" void fn_27_341520(void* self) {
    __dt__13wnSheikNeedleFv(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_341528 {
extern "C" void fn_27_341280(void*);

extern "C" void fn_27_341528(void* self) {
    fn_27_341280(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_341530 {
extern "C" void __dt__13wnSheikNeedleFv(void*);

extern "C" void fn_27_341530(void* self) {
    __dt__13wnSheikNeedleFv(reinterpret_cast<u8*>(self) - 0x88);
}
}

