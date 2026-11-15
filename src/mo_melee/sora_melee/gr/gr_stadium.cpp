#include <gr/gr_calc_world_callback.h>
#include <gr/gr_stadium.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>

// The stadium of the Pokemon Stadium stages: a model whose root node is scaled through the calc-world callback.

grStadium::grStadium(const char* taskName) : grYakumono(taskName) {
    m_scaleX = 1.0f;
    m_scaleY = 1.0f;
    m_scaleZ = 1.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_nodeIndex = 0;
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
}

grStadium::~grStadium() { }

void grStadium::update(float deltaFrame) {
    setNode();
    if (m_isUpdate) {
        updateCallBack(deltaFrame);
        grGimmick::update(deltaFrame);
    }
}

void grStadium::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        nw4r::g3d::ScnMdlSimple* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                callback->m_index = 0;
                scnMdl->m_calcWorldCallBack = callback;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = m_nodeIndex;
            }
            grNodeCallbackData* data = callback->m_nodeCallbackDatas;
            data->m_scale.m_x = m_scaleX;
            data->m_scale.m_y = m_scaleY;
            data->m_scale.m_z = m_scaleZ;
        }
    }
}

float grStadium::getScaleY() {
    return m_scaleY;
}

void grStadium::setScaleY(float scaleY) {
    m_scaleY = scaleY;
}
