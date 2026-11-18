#include <gr/gr_pirates_sling.h>
#include <gr/gr_calc_world_callback.h>
#include <st_pirates/gr_pirates_util.h>

// The model follows the matrix the stage hands to the sling.
void grPiratesSling::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_mtxWork != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
            }
        }
    }
}

void grPiratesSling::setMotion(u32 motion, bool loop, bool force, float* endFrame) {
    PIRATES_SET_MOTION(m_motionId, 2, motion, loop, force, endFrame)
}
