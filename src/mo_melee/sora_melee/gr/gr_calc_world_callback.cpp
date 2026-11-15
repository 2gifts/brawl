#include <gr/gr_calc_world_callback.h>
#include <nw4r/g3d/g3d_calcworld.h>

// The per-node transform overrides a ground object installs on its scene model: every node callback either replaces the
// animated translation, rotation, scale or matrix of its node, or adds an offset to them.

struct ChrAnmResult;
// HYPOTHESIS: a game helper that reads the scale of an animation result (main, the symbol takes a global ChrAnmResult)
void ChrAnmResult_GetScale(const ChrAnmResult* result, Vec3f* scale);

grCalcWorldCallBack::grCalcWorldCallBack() {
    m_index = 0;
    m_numNodeCallbackData = 0;
    m_nodeCallbackDatas = NULL;
}

grCalcWorldCallBack::~grCalcWorldCallBack() {
    if (m_nodeCallbackDatas != NULL) {
        delete[] m_nodeCallbackDatas;
        m_nodeCallbackDatas = NULL;
    }
}

int grCalcWorldCallBack::initialize(bool unk1, HeapType heapType) {
    grNodeCallbackData* datas;
    if (!unk1) {
        datas = m_nodeCallbackDatas = new (heapType) grNodeCallbackData[m_numNodeCallbackData];
    } else {
        datas = NULL;
    }
    if (datas == NULL) {
        return false;
    }
    int count = m_numNodeCallbackData;
    for (int i = 0; i != count; i++) {
        grNodeCallbackData* data = &m_nodeCallbackDatas[i];
        data->m_nodeIndex = 0;
        data->m_flags = 0;
        data->m_pos = Vec3f(0.0f, 0.0f, 0.0f);
        data->m_rot = Vec3f(0.0f, 0.0f, 0.0f);
        data->m_scale = Vec3f(1.0f, 1.0f, 1.0f);
        data->m_offsetPos = Vec3f(0.0f, 0.0f, 0.0f);
        data->m_offsetRot = Vec3f(0.0f, 0.0f, 0.0f);
        data->m_offsetScale = Vec3f(1.0f, 1.0f, 1.0f);
        data->m_matrix.setIdentity();
    }
    return true;
}

int grCalcWorldCallBack::clearAll() {
    if (m_nodeCallbackDatas != NULL) {
        delete[] m_nodeCallbackDatas;
    }
    m_index = 0;
    m_numNodeCallbackData = 0;
    m_nodeCallbackDatas = NULL;
    return true;
}

void grCalcWorldCallBack::ExecCallbackA(nw4r::g3d::ChrAnmResult* result, nw4r::g3d::ResMdl mdl, nw4r::g3d::FuncObjCalcWorld* funcObj) {
    if ((int)m_index < (int)m_numNodeCallbackData && m_nodeCallbackDatas != NULL) {
        if (m_nodeCallbackDatas[m_index].m_flags & 0x1) {
            result->SetTranslate(reinterpret_cast<nw4r::math::VEC3*>(&m_nodeCallbackDatas[m_index].m_pos));
        }
        if (m_nodeCallbackDatas[m_index].m_flags & 0x2) {
            result->SetRotateDeg(reinterpret_cast<nw4r::math::VEC3*>(&m_nodeCallbackDatas[m_index].m_rot));
        }
        if (m_nodeCallbackDatas[m_index].m_flags & 0x4) {
            result->SetScale(reinterpret_cast<nw4r::math::VEC3*>(&m_nodeCallbackDatas[m_index].m_scale));
        }
        if (m_nodeCallbackDatas[m_index].m_flags & 0x8) {
            result->SetRotTrans(reinterpret_cast<nw4r::math::MTX34*>(&m_nodeCallbackDatas[m_index].m_matrix));
        }
        if (m_nodeCallbackDatas[m_index].m_flags & 0x10) {
            nw4r::math::VEC3 pos;
            result->GetTranslate(&pos);
            pos = pos + *reinterpret_cast<nw4r::math::VEC3*>(&m_nodeCallbackDatas[m_index].m_offsetPos);
            result->SetTranslate(&pos);
        }
        if (m_nodeCallbackDatas[m_index].m_flags & 0x20) {
            nw4r::math::VEC3 rot;
            result->GetRotateDeg(&rot);
            rot = rot + *reinterpret_cast<nw4r::math::VEC3*>(&m_nodeCallbackDatas[m_index].m_offsetRot);
            result->SetRotateDeg(&rot);
        }
        if (m_nodeCallbackDatas[m_index].m_flags & 0x40) {
            nw4r::math::VEC3 scale;
            ChrAnmResult_GetScale(reinterpret_cast<ChrAnmResult*>(result), reinterpret_cast<Vec3f*>(&scale));
            scale = scale + *reinterpret_cast<nw4r::math::VEC3*>(&m_nodeCallbackDatas[m_index].m_offsetScale);
            result->SetScale(&scale);
        }
        if ((int)++m_index < (int)m_numNodeCallbackData) {
            funcObj->SetCallbackNodeID(m_nodeCallbackDatas[m_index].m_nodeIndex);
        } else {
            m_index = 0;
        }
    }
}
