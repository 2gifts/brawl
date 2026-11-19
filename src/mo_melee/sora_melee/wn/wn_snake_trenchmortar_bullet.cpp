#include <wn/snake/wn_snake_trench_mortar_bullet.h>
#include <so/collision/so_collision_log.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/reflect/so_reflect_module.h>
#include <so/team/so_team_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

void wnSnakeTrenchMortarBullet::updateNodeSRT() {
    float scale = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scaleVec(scale, scale, scale);
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rot1 = m_moduleAccesser->getPostureModule().getRot(1);
    Vec3f rot0 = m_moduleAccesser->getPostureModule().getRot(0);
    Vec3f rot2 = m_moduleAccesser->getPostureModule().getRot(2);
    m_moduleAccesser->getModelModule().setNodeRotate(2, &rot0);
    m_moduleAccesser->getModelModule().setNodeRotate(1, &rot1);
    m_moduleAccesser->getModelModule().setNodeRotate(0, &rot2);
    m_moduleAccesser->getModelModule().setNodeScale(0, &scaleVec);
    m_moduleAccesser->getModelModule().setNodeTranslate(0, &pos);
}

void wnSnakeTrenchMortarBullet::processHit() {
    StageObject::processHit();
}

void wnSnakeTrenchMortarBullet::processCollision() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000004);
    }
    StageObject::processCollision();
}

void wnSnakeTrenchMortarBullet::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser*) {
    if (collisionLog == nullptr) {
        return;
    }
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        if (collisionLog->m_kind == 3) {
            if (collisionLog->m_teamNo != m_moduleAccesser->getTeamModule().getTeam()->getNo() &&
                m_moduleAccesser->getReflectModule().isReflect() == 1) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000005);
            }
        } else {
            m_moduleAccesser->getWorkManageModule().onFlag(0x22000004);
        }
    }
}

bool wnSnakeTrenchMortarBullet::notifyEventCollisionSearchCheck() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x22000005);
            m_moduleAccesser->getPostureModule().reverseLr();
            m_moduleAccesser->getPostureModule().updateRotYLr();
            int team = m_moduleAccesser->getReflectModule().getTeam();
            m_moduleAccesser->getTeamModule().setTeam(team, true);
            m_moduleAccesser->getTeamModule().setHitTeam(team);
            m_moduleAccesser->getTeamModule().setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
            m_moduleAccesser->getKineticModule().mulSpeed(&Vec3f(-m_moduleAccesser->getReflectModule().getSpeedMul(), 1.0f, 1.0f),
                                                          soKineticEnergy::AttributeFlag(1));
        }
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000004)) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
            return true;
        }
    }
    return false;
}

bool wnSnakeTrenchMortarBullet::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// TODO: clean up, give variables real names
namespace jj_3AF5F0 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__45wnSnakeTrenchMortarBulletStatusUniqProcessFlyFv(void* ptr, int size) {
    if (ptr != 0 && size > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_3AF630 {
extern "C" void* fn_27_3AF630(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_3B0198 {
extern "C" void* fn_27_3B0198(void* self) {
    return reinterpret_cast<u8*>(self) + 0x9c;
}
}

// TODO: clean up, give variables real names
namespace jj_3B01A0 {
extern "C" void* fn_27_3B01A0(void* self) {
    return reinterpret_cast<u8*>(self) + 0x4;
}
}

// TODO: clean up, give variables real names
namespace jj_3B01A8 {
extern "C" void fn_27_3B01A8(void) {}
}

// TODO: clean up, give variables real names
namespace jj_3B0318 {
extern "C" void __ct__19soReflectModuleImplFP16soModuleAccesseri(void*, void*, int);

extern "C" void* fn_27_3B0318(void* a, void* b) {
    __ct__19soReflectModuleImplFP16soModuleAccesseri(a, b, 1);
    return a;
}
}

// TODO: clean up, give variables real names
namespace jj_3B0D74 {
extern "C" void fn_27_3AF338(void*);

extern "C" void fn_27_3B0D74(void* self) {
    fn_27_3AF338(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_3B0D7C {
extern "C" void fn_27_3AF338(void*);

extern "C" void fn_27_3B0D7C(void* self) {
    fn_27_3AF338(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_3B0D84 {
extern "C" void fn_27_3AF338(void*);

extern "C" void fn_27_3B0D84(void* self) {
    fn_27_3AF338(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3B0DA4 {
extern "C" void fn_27_3B0CC8(void*);

extern "C" void fn_27_3B0DA4(void* self) {
    fn_27_3B0CC8(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3B0DB4 {
extern "C" void notifyEventCollisionSearchCheck__25wnSnakeTrenchMortarBulletFv(void*);

extern "C" void fn_27_3B0DB4(void* self) {
    notifyEventCollisionSearchCheck__25wnSnakeTrenchMortarBulletFv(reinterpret_cast<u8*>(self) - 0x21f0);
}
}

// TODO: clean up, give variables real names
namespace jj_3B0DBC {
extern "C" void notifyEventCollisionSearch__25wnSnakeTrenchMortarBulletFP14soCollisionLogP16soModuleAccesser(void*);

extern "C" void fn_27_3B0DBC(void* self) {
    notifyEventCollisionSearch__25wnSnakeTrenchMortarBulletFP14soCollisionLogP16soModuleAccesser(reinterpret_cast<u8*>(self) - 0x21f0);
}
}

