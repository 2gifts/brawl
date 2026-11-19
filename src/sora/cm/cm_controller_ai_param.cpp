#include <types.h>
#include "nw4r/math/math_arithmetic.h"
#include <cm/cm_controller_ai_param.h>

void cmCameraParam::clear() {
    m_pos.m_x = 0.0f;
    m_pos.m_y = 10.0f;
    m_pos.m_z = 0.0f;
    m_targetPos.m_x = 0.0f;
    m_targetPos.m_y = 10.0f;
    m_targetPos.m_z = 0.0f;
    m_eye.m_x = 0.0f;
    m_eye.m_y = 8.241425f;
    m_eye.m_z = 90.0f;
    m_targetEye.m_x = 0.0f;
    m_targetEye.m_y = 8.241425f;
    m_targetEye.m_z = 90.0f;
    m_fovy = m_targetFovy = 38.0f;
    unk38 = true;
}

void cmCameraParam::updateFovyNormal(float fovy, float rate) {
    m_targetFovy = fovy;
    m_fovy = m_fovy + rate * (fovy - m_fovy);
}

// TODO: clean up, give variables real names
namespace jj_800A323C {

extern "C" double fn_80400AB8(double);
extern "C" const float lbl_805A1F3C;
extern "C" const float lbl_805A1F40;
extern "C" const float lbl_805A1F50;

struct Cam {
    char pad0[0x94];
    float unk94;
    float unk98;
};

extern "C" Cam* getInstance__16CameraControllerFv();

struct Src {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    char pad10[4];
    float unk14;
};

struct Dst {
    char pad0[0x34];
    float fov;
};

// TODO: clean up, give variables real names
extern "C" void fn_800A323C(Dst* dst, Src* src, float farg0) {
    float prod = lbl_805A1F40 * (lbl_805A1F3C * dst->fov);
    float tanf = (float)fn_80400AB8(prod);
    float a = (src->unkC - src->unk4) / (lbl_805A1F50 * tanf);
    float tanf2 = (float)fn_80400AB8(prod);
    float b = (src->unk8 - src->unk0) / (farg0 * (lbl_805A1F50 * tanf2));
    float m = b > a ? b : a;
    Cam* cam = getInstance__16CameraControllerFv();
    float d = m - cam->unk94;
    float t = nw4r::math::FSelect(d, m, cam->unk94);
    t = nw4r::math::FSelect(t - cam->unk98, cam->unk98, t);
    src->unk14 = t;
}
}

