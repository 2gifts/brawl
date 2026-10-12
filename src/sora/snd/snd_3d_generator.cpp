#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <types.h>

extern "C" void SetPosition__Q34nw4r3snd12Sound3DActorFRCQ34nw4r4math4VEC3(nw4r::snd::Sound3DActor* actor, Vec3f* pos);
extern f32 lbl_805A1B08;

static inline int isFinite(float f) {
    int res = 1;
    u32 i;
    __memcpy(&i, &f, 4);
    if ((i >> 23 & 0xFF) == 0xFF)
        res = 0;
    return res;
}

void snd3DGenerator::setPos(Vec3f* pos) {
    if (m_sndActor != NULL) {
        if (!(isFinite(pos->m_x) && isFinite(pos->m_y) && isFinite(pos->m_z))) {
            f32 zero = lbl_805A1B08;

            pos->m_x = zero;
            pos->m_y = zero;
            pos->m_z = zero;
        }
        SetPosition__Q34nw4r3snd12Sound3DActorFRCQ34nw4r4math4VEC3(m_sndActor, pos);
    }
}
