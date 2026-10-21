#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <types.h>

#include <st_mansion/gr_mansion_area.h>
#include <st_mansion/gr_mansion_util.h>

grMansionArea::grMansionArea(const char* taskName) : grMansion(taskName), unk17C() {
    unk158 = NULL;
    unk15C = NULL;
    unk160 = NULL;
    unk164 = NULL;
    unk168 = 0.875f;
    unk16C = 0;
    unk16D = 0;
    unk16E = 2;
    unk170 = 0;
    unk174 = 1;
    unk178 = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->setup(1, 4);
}

grMansionArea::~grMansionArea() { }

// The place of the node of the pillar (that the area is on) is read every frame.
void grMansionArea::processAnim() {
    Ground::processAnim();
    if (unk158 != NULL) {
        getNodePosition(unk158, m_unk1, unk170);
    }
}

void grMansionArea::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    setNode();
    if (m_isUpdate) {
        updateDestroy(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grMansionArea::updateDestroy(float deltaFrame) { }

// The node of the area is scaled by how far the collapse is.
void grMansionArea::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = m_nodeIndex;
            }
            float scale;
            float rate;
            if (unk16C == 1) {
                rate = 1.0f - *unk164;
            } else {
                rate = *unk164;
            }
            scale = unk168;
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            float clamped = mansionClamp(rate, 0.001f, 1.0f);
            data->m_scale.m_x = scale;
            data->m_scale.m_y = scale * clamped;
            data->m_scale.m_z = scale;
            Vec3f soundPos(0.0f, 0.0f, 0.0f);
            unk17C.setPos(&soundPos);
        }
    }
}

void grMansionArea::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    MANSION_SET_MOTION(unk174, animId, loop, force, frameCount)
}
