#include <types.h>
#include <mt/mt_vector.h>
#include <gf/gf_3d_scene.h>
#include <nw4r/g3d/g3d_camera.h>
#include <nw4r/g3d/g3d_scnroot.h>

namespace nw4r { namespace math {
VEC3* VEC3TransformCoord(VEC3* out, const MTX44* matrix, const VEC3* position);
} }

extern nw4r::math::MTX34 lbl_8049E030;
extern nw4r::math::MTX44 lbl_8049E060;

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A02B0[0x8] __attribute__((aligned(4))) = { 0 };
float lbl_805A02B8[2] = { 0 };
float lbl_805A02C0[2] = { 0 };
unsigned char lbl_805A02C8[0x8] __attribute__((aligned(4))) = { 0 };

void Vec3f::IfCommon_exchangeStageObjPosToInfoPosFastCursor(const Vec3f* position, Vec3f* out) {
    nw4r::math::VEC3 cameraPosition;
    nw4r::math::VEC3 projected;
    PSMTXMultVec(lbl_8049E030, reinterpret_cast<const Vec*>(position), cameraPosition);
    nw4r::math::VEC3TransformCoord(&projected, &lbl_8049E060, &cameraPosition);
    out->m_x = projected.x * lbl_805A02B8[0];
    out->m_y = projected.y * lbl_805A02B8[1];
    out->m_z = 0.0f;
}

Vec3f Vec3f::IfCommon_exchangeStageObjPosToInfoPosFastLoupe(const Vec3f* position) {
    nw4r::math::VEC3 cameraPosition;
    nw4r::math::VEC3 projected;
    PSMTXMultVec(lbl_8049E030, reinterpret_cast<const Vec*>(position), cameraPosition);
    cameraPosition.z = -40.0f;
    nw4r::math::VEC3TransformCoord(&projected, &lbl_8049E060, &cameraPosition);
    Vec3f out;
    out.m_x = projected.x * lbl_805A02C0[0];
    out.m_y = projected.y * lbl_805A02C0[1];
    out.m_z = 0.0f;
    return out;
}

Vec3f Vec3f::IfCommon_exchangeStageObjPosToInfoPos(const Vec3f* position, int unk1) {
    nw4r::g3d::ScnRoot* infoScene = g_gfSceneRoot->unk44;
    nw4r::g3d::Camera stageCamera = g_gfSceneRoot->unk40->GetCamera(0);
    nw4r::math::VEC3 projected;
    stageCamera.Project(&projected, *reinterpret_cast<const nw4r::math::VEC3*>(position));
    nw4r::g3d::Camera infoCamera = infoScene->GetCamera(0);
    nw4r::g3d::CameraData& infoData = infoCamera.ref();
    nw4r::math::VEC3 cameraPosition;
    infoCamera.GetPosition(&cameraPosition);
    Vec3f out;
    out.m_x = infoData.projAspect * (0.07432901114225388f * (projected.x - 320.0f));
    out.m_y = 0.1036885678768158f * ((480.0f - projected.y) - 240.0f);
    out.m_z = 0.0f;
    if (static_cast<int>(g_gfSceneRoot->m_transformFlag.m_mask) >> 24) {
        out.m_x = -out.m_x;
    }
    return out;
}

// TODO: clean up, give variables real names
namespace jj_800DB674 {
#pragma push
#pragma opt_propagation off

extern "C" void fn_801ECD70(void*, int, void*);
extern "C" void fn_80162C48(void*, void*, void*);
extern "C" void fn_8003E388(void*, void*);

struct Globals8049 {
    unsigned char first[0x30];
};
extern Globals8049 lbl_8049E030;
extern float lbl_805A02B8[2];
extern const float lbl_805A2344;

struct Vec3 { float x, y, z; };

// TODO: clean up, give variables real names
extern "C" void fn_800DB674(int arg0, float* arg1) {
    Globals8049* base = &lbl_8049E030;
    unsigned char (Globals8049::*first)[0x30] = &Globals8049::first;
    Vec3 vec;
    Vec3 v;
    Vec3 out;
    fn_801ECD70(&(base->*first), arg0, &vec);
    fn_80162C48(&v, (unsigned char*)base + 0x30, &vec);
    out.x = v.x * lbl_805A02B8[0];
    out.y = v.y * lbl_805A02B8[1];
    out.z = lbl_805A2344;
    fn_8003E388((unsigned char*)base + 0x70, arg1);
    arg1[3] = out.x;
    arg1[7] = out.y;
    arg1[11] = out.z;
}

#pragma pop
}

