#include <wn/snake/wn_snake_rgb6_bullet.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_energy_3f.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>

void wnSnakeRgb6Bullet::onDeactivate() {
    m_moduleAccesser->getEffectModule().killAll(0, false, true);
}

void wnSnakeRgb6Bullet::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                                soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == 1) {
        Vec3f pos(m_moduleAccesser->getWorkManageModule().getFloat(0x11000001),
                  m_moduleAccesser->getWorkManageModule().getFloat(0x11000002), 0.0f);
        m_moduleAccesser->getPostureModule().setPos(&pos);
        updateNodeSRT();
        soKineticEnergy3f* energy = dynamic_cast<soKineticEnergy3f*>(m_moduleAccesser->getKineticModule().getEnergy(2));
        if (energy != nullptr) {
            energy->clearSpeed();
            energy->disable();
        }
    }
}

bool wnSnakeRgb6Bullet::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) {
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

// TODO: clean up, give variables real names
namespace jj_3B4FA8 {
extern "C" void __dl__FPv(void*);

extern "C" void* __dt__30wnSnakeRgb6BulletParamAccesserFv(void* ptr, int count) {
    if (ptr != 0 && count > 0)
        __dl__FPv(ptr);
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_3B4FE8 {
extern "C" void* fn_27_3B4FE8(void* self) {
    return reinterpret_cast<u8*>(self) + 0x7f8;
}
}

// TODO: clean up, give variables real names
namespace jj_3B6638 {
extern "C" void fn_27_3B4D20(void*);

extern "C" void fn_27_3B6638(void* self) {
    fn_27_3B4D20(reinterpret_cast<u8*>(self) - 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_3B6640 {
extern "C" void fn_27_3B4D20(void*);

extern "C" void fn_27_3B6640(void* self) {
    fn_27_3B4D20(reinterpret_cast<u8*>(self) - 0x90);
}
}

// TODO: clean up, give variables real names
namespace jj_3B6648 {
extern "C" void fn_27_3B4D20(void*);

extern "C" void fn_27_3B6648(void* self) {
    fn_27_3B4D20(reinterpret_cast<u8*>(self) - 0x88);
}
}

// TODO: clean up, give variables real names
namespace jj_3B6670 {
extern "C" void fn_27_3B658C(void*);

extern "C" void fn_27_3B6670(void* self) {
    fn_27_3B658C(reinterpret_cast<u8*>(self) - 0x88);
}
}

