#include <gr/collision/gr_collision_linework.h>
#include <so/ground/so_ground_tracer_impl.h>

bool soGroundTracerImpl::canStart(u32 arg0, u8 arg1, soGroundModule* groundModule, u32 index, float arg4) {
    grCollisionLineWork work;
    unk8 = 0xffff;
    unk10 = 0xffff;
    return getStartLineWorkId(&unk8, &work, arg0, arg1, groundModule, index, arg4);
}

Vec2f soGroundTracerImpl::getGroundMovementSpeed() {
    if (!isTraced()) {
        return Vec2f(0.0f, 0.0f);
    }
    return m_groundMovementSpeed;
}
