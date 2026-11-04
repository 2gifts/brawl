#include <math.h>
#include <so/so_module_accesser.h>
#include <wn/pikachu/wn_pikachu_thunder_jolt2.h>
#include <wn/wn_class_info_impl.h>

// Registers the Pikachu thunder jolt 2 article class (kind 10) in the class info table during static initialization.
wnClassInfoImpl<10, wnPikachuThunderJolt2, wnPikachuThunderJolt2Data> g_wnPikachuThunderJolt2ClassInfo;

void wnPikachuThunderJolt2::setPosition(Vec3f* position, const Vec2f* direction, bool resetFrame, float lr) {
    soModuleAccesser* accesser = m_moduleAccesser;
    accesser->getPostureModule().setPos(position);
    if (resetFrame) {
        accesser->getMotionModule().setFrame(0.0f);
    }
    if (lr == 0.0f) {
        lr = accesser->getPostureModule().getLr();
    }
    float angle = atan2f(direction->m_x, direction->m_y);
    Vec3f rotation(57.29578f * (lr * angle), 0.0f, 0.0f);
    accesser->getPostureModule().setRot(&rotation, 0);
}
