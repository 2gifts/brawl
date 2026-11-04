#include <gf/gf_camera.h>
#include <st/st_shadow.h>
#include <st/stage.h>
#include <types.h>

stShadow::ShadowObject::ShadowObject() : unk1c(0xFFFF) { }

void stShadow::addShadowObject(u32 id, const Vec3f* pos, float size, u32 arg3, float height, u32 arg5) {
    gfCamera::ClipSphere sphere;
    sphere.center.set(pos->m_x, pos->m_y - height, pos->m_z);
    sphere.radius = 1.5f * size;
    if (gfCameraManager::getManager()->getCamera(0)->isClipIn(&sphere) && unk40 && !unk41) {
        ShadowObject* object = static_cast<ShadowObject*>(unk90.getHead());
        if (object) {
            unk90.remove(object);
            object->unk8 = id;
            object->unkC = pos->m_x;
            object->unk10 = pos->m_y;
            object->unk14 = pos->m_z;
            object->unk18 = size;
            object->unk20 = NULL;
            object->unk24 = 0;
            object->unk1c = 0xFFFF;
            object->unk28 = arg3;
            object->unk2C = height;
            object->unk30 = arg5;
            unk84.addTail(object);
            ++unk7c;
        }
    }
}

void stShadow::advDrawShadowProc(nw4r::g3d::ScnProc*, bool opa) {
    if (!opa) {
        stShadow* shadow = g_Stage->m_shadow;
        if (shadow && shadow->unk43_0 == 1) {
            shadow->drawADVShadow(0);
        }
    }
}

void stShadow::advDrawShadowProc_gr2(nw4r::g3d::ScnProc*, bool opa) {
    if (!opa) {
        stShadow* shadow = g_Stage->m_shadow;
        if (shadow && shadow->unk43_0 == 1) {
            shadow->drawADVShadow(1);
        }
    }
}
