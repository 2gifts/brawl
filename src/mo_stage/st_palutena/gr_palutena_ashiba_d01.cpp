#define MT_VEC3F_ADD_FRESH_REGS // MATCH-ONLY: register choice of the inline Vec3f adds in updateCallBack, see mt_vector.h
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>

#include <st_palutena/gr_palutena.h>

inline grPalutenaAshibaD01::grPalutenaAshibaD01(const char* taskName) : grPalutenaAshiba(taskName) {
    m_landing.m_x = 0.0f;
    m_landing.m_y = 0.0f;
    m_landing.m_z = 0.0f;
    m_translate.m_x = 0.0f;
    m_translate.m_y = 0.0f;
    m_translate.m_z = 0.0f;
    m_unk1 = -1;
    m_nodeIndex = -1;
    unk1D1 = 0;
    m_landState = 0;
    m_landTimer = 0.0f;
    m_landTime = 0.0f;
}

grPalutenaAshibaD01* grPalutenaAshibaD01::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaAshibaD01* ground = new (Heaps::StageInstance) grPalutenaAshibaD01(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaAshibaD01::~grPalutenaAshibaD01() {
}

void grPalutenaAshibaD01::update(float deltaFrame) {
    grPalutenaAshiba::update(deltaFrame);
    if (m_isUpdate) {
        updateLanding(deltaFrame);
    }
}

// The dip when a fighter lands on the platform: it is pushed down (sine, 8 frames) and springs back (12 frames).
void grPalutenaAshibaD01::updateLanding(float deltaFrame) {
    m_landTimer = m_landTimer - deltaFrame;
    if (m_landTimer < 0.0f) {
        m_landTimer = 0.0f;
    }
    switch (m_landState) {
    case 0:
        break;
    case 1: {
        m_landTime = m_landTime + deltaFrame;
        float rate = m_landTime / 8.0f;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (rate > 1.0f) {
            rate = 1.0f;
        }
        if (rate == 1.0f) {
            m_landState = 2;
            m_landTime = 0.0f;
        }
        float sine = nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))));
        m_landing.m_y = sine * -3.0f;
        break;
    }
    case 2: {
        m_landTime = m_landTime + deltaFrame;
        float rate = m_landTime / 12.0f;
        if (rate < 0.0f) {
            rate = 0.0f;
        }
        if (rate > 1.0f) {
            rate = 1.0f;
        }
        if (rate == 1.0f) {
            m_landState = 0;
            m_landTime = 0.0f;
        }
        m_landing.m_y = -0.5f - (1.0f - nw4r::math::SinFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(rate * 16384.0f))))) * 2.5f;
        break;
    }
    }
    if (m_landTimer == 0.0f) {
        m_landing.m_y = m_landing.m_y + deltaFrame * 0.025f;
        if (m_landing.m_y > 0.0f) {
            m_landing.m_y = 0.0f;
        }
    }
}

// The model follows the platform (shake, landing dip and the translation of the cloud under it).
// TODO: clean up: MATCH-ONLY copy of the inline Vec3f add (mt_vector.h) for the first sum of updateCallBack: its paired-single
// registers are declared in operand order, the two later sums use the MT_VEC3F_ADD_FRESH_REGS form.
static inline void grPalutenaD01VecAdd(register Vec3f* pOut, register const Vec3f* lhs, register const Vec3f* rhs) {
    register f32 leftXY, rightXY, leftZ, rightZ, resultXY, resultZ;
    // clang-format off
    asm {
        psq_l leftXY, Vec3f.m_x(lhs), 0, 0
        psq_l rightXY, Vec3f.m_x(rhs), 0, 0
        psq_l leftZ, Vec3f.m_z(lhs), 1, 0
        psq_l rightZ, Vec3f.m_z(rhs), 1, 0
        ps_add resultXY, leftXY, rightXY
        ps_add resultZ, leftZ, rightZ
        psq_st resultXY, Vec3f.m_x(pOut), 0, 0
        psq_st resultZ, Vec3f.m_z(pOut), 1, 0
    }
    // clang-format on
}

static inline Vec3f grPalutenaD01AddVec(const Vec3f* lhs, const Vec3f* rhs) {
    Vec3f result;
    grPalutenaD01VecAdd(&result, lhs, rhs);
    return result;
}

void grPalutenaAshibaD01::updateCallBack(float deltaFrame) {

    Vec3f pos;
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            calcWorldCallBack->m_index = 0;
            scnMdl->m_calcWorldCallBack = calcWorldCallBack;
            scnMdl->EnableScnMdlCallbackTiming(1);
            scnMdl->m_nodeIndex = m_nodeIndex;
            grNodeCallbackData* data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            Vec3f offset = grPalutenaD01AddVec(&m_offset, &m_shakePos) + m_landing + m_translate;
            data->m_offsetPos.m_x = offset.m_x;
            data->m_offsetPos.m_y = offset.m_y;
            data->m_offsetPos.m_z = offset.m_z;
            data = &calcWorldCallBack->m_nodeCallbackDatas[0];
            data->m_scale.m_x = m_scale.m_x;
            data->m_scale.m_y = m_scale.m_y;
            data->m_scale.m_z = m_scale.m_z;
            getNodePosition(&pos, 0, m_nodeIndex);
            m_snd.setPos(&pos);
        }
    }
}

void grPalutenaAshibaD01::getTranslate_Delta(Vec3f* out) {
    if (out == NULL) {
        return;
    }
    out->m_x = m_translate.m_x;
    out->m_y = m_translate.m_y;
    out->m_z = m_translate.m_z;
}

void grPalutenaAshibaD01::setTranslate_Delta(float x, float y, float z) {
    m_translate.m_x = x;
    m_translate.m_y = y;
    m_translate.m_z = z;
}

void grPalutenaAshibaD01::getLanding_Delta(Vec3f* out) {
    if (out == NULL) {
        return;
    }
    out->m_x = m_landing.m_x;
    out->m_y = m_landing.m_y;
    out->m_z = m_landing.m_z;
}

// A fighter lands on the platform: it starts to dip when the platform is whole.
void grPalutenaAshibaD01::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    bool start = false;
    if (m_landTimer == 0.0f) {
        start = true;
    }
    if (isFirstContact == 1) {
        start = true;
    }
    m_landTimer = 5.0f;
    if (m_scale.m_y != 1.0f) {
        return;
    }
    if (start == 1) {
        m_landState = 1;
    }
}
