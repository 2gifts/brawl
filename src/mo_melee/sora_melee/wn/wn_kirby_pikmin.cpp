#include <so/so_value_accesser.h>
#include <wn/pikmin/wn_kirby_pikmin.h>
#include <wn/wn_class_info_impl.h>

// Registers the Kirby's Pikmin article class (kind 150) in the class info table during static initialization.
wnClassInfoImpl<150, wnKirbyPikmin, wnKirbyPikminData> g_wnKirbyPikminClassInfo;

bool wnKirbyPikmin::notifyEventCollisionAttackCheck(u32 flags) {
    if ((flags & 7) != 0) {
        return false;
    }
    int reflected = 0;
    if ((flags & 8) != 0) {
        if (m_moduleAccesser->getReflectModule().isReflect() == 1) {
            reflected = 1;
        }
    }
    if (reflected != 0) {
        Vec2f accel(1.0f, 1.0f);
        float speedY = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
        Vec2f speed(-m_moduleAccesser->getReflectModule().getSpeedMul(), speedY);
        return reflectRequest(&speed, &accel);
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}
