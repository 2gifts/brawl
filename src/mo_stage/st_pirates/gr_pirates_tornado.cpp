#include <gr/gr_calc_world_callback.h>
#include <gr/gr_pirates_tornado.h>
#include <memory.h>
#include <snd/snd_id.h>
#include <so/so_world.h>
#include <st_pirates/gr_pirates_util.h>

// MATCH-ONLY: preserve the original shared constant pool.
extern const float g_piratesTornadoConstants[];

grPiratesTornado* grPiratesTornado::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesTornado* ground = new (Heaps::StageInstance) grPiratesTornado(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesTornado::grPiratesTornado(const char* taskName) : grPirates(taskName), m_soundGenerator() {
    m_ctrlFrameAdvanced = 0;
    m_motionPhase = 0;
    unk164 = NULL;
    m_stateWork = NULL;
    m_previousShipState = 0;
    m_motionId = 1;
    m_motionTimer = g_piratesTornadoConstants[0];
    m_reducedGravity = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
    unk180 = -1;
    unk184 = 0;
}

grPiratesTornado::~grPiratesTornado() {
}

void grPiratesTornado::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        m_ctrlFrameAdvanced = 0;
        if (m_timer < *m_ctrlFrame) m_ctrlFrameAdvanced = 1;
        m_timer = *m_ctrlFrame;
        updateMotion(deltaFrame);
        updateSE(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grPiratesTornado::updateMotion(float deltaFrame) {
    const float* constants = g_piratesTornadoConstants;
    if (m_stateWork == NULL) return;
    m_motionTimer -= deltaFrame;
    if (m_motionTimer < constants[0]) m_motionTimer = constants[0];
    switch (m_state) {
    case 0:
        setMotion(1, 0, true, NULL);
        setVisibility(false);
        // Reset restores both global gravity multipliers.
        g_soWorld->m_gravityUp = constants[1];
        g_soWorld->m_gravityDown = constants[1];
        m_reducedGravity = 0;
        m_state = 1;
        // Fall through: process the current ship event after resetting.
    case 1:
        if (m_ctrlFrameAdvanced == 1) {
            u32 motion = 1;
            bool loop = true;
            switch (m_motionId) {
            case 0:
                --m_motionPhase;
                switch (m_motionPhase) {
                case 0:
                    m_state = 0;
                    break;
                case 1:
                    if (*m_ctrlFrame > constants[2]) {
                        g_soWorld->m_gravityUp = constants[3];
                        g_soWorld->m_gravityDown = constants[3];
                        m_reducedGravity = 1;
                    } else {
                        g_soWorld->m_gravityUp = constants[1];
                        g_soWorld->m_gravityDown = constants[1];
                        m_reducedGravity = 0;
                    }
                    break;
                }
            }
            if (*m_stateWork != m_previousShipState) {
                switch (*m_stateWork) {
                case 6:
                    // Ship event 6 starts the tornado appearance sequence.
                    m_motionPhase = 3;
                    motion = 0;
                    setVisibility(true);
                    break;
                }
                if (motion != 1) loop = false;
                m_previousShipState = *m_stateWork;
            }
            if (motion != 1) {
                setMotion(motion, loop, true, &m_motionTimer);
                setMotionFrame(constants[4] - *m_ctrlFrame, 0);
                unk184 = 0;
            }
        } else {
            switch (m_motionPhase) {
            case 1:
                if (*m_ctrlFrame > constants[2]) {
                    g_soWorld->m_gravityUp = constants[3];
                    g_soWorld->m_gravityDown = constants[3];
                    m_reducedGravity = 1;
                } else {
                    g_soWorld->m_gravityUp = constants[1];
                    g_soWorld->m_gravityDown = constants[1];
                    m_reducedGravity = 0;
                }
                break;
            }
        }
        break;
    }
}

// The sounds of the tornado: they follow the node of the tornado and start when the motion reaches a frame.
void grPiratesTornado::updateSE(float deltaFrame) {
    const float* constants = g_piratesTornadoConstants;
    if (m_motionId == 0) {
        u32 node;
        if (getNodeIndex(&node, 0, "TatsumakiN")) {
            Vec3f pos;
            getNodePosition(&pos, 0, node);
            m_soundGenerator.setPos(&pos);
            switch (unk184) {
            case 0:
                unk180 = m_soundGenerator.playSE(snd_se_stage_Pirates_04, 0, 0, -1);
                unk184++;
                break;
            case 1:
                if (getMotionFrame(0) < constants[5]) {
                    break;
                }
                if (unk180 != -1) {
                    m_soundGenerator.stopSE(unk180, 90);
                }
                unk180 = m_soundGenerator.playSE(snd_se_stage_Pirates_05, 0, 0, -1);
                unk184++;
                break;
            case 2:
                if (getMotionFrame(0) < constants[6]) {
                    break;
                }
                if (unk180 != -1) {
                    m_soundGenerator.stopSE(unk180, 90);
                }
                unk180 = m_soundGenerator.playSE(snd_se_stage_Pirates_06, 0, 0, -1);
                unk184++;
                break;
            case 3:
                if (getMotionFrame(0) < constants[7]) {
                    break;
                }
                if (unk180 != -1) {
                    m_soundGenerator.stopSE(unk180, 90);
                }
                unk180 = m_soundGenerator.playSE(snd_se_stage_Pirates_07, 0, 0, -1);
                unk184++;
                break;
            case 4:
                if (getMotionFrame(0) < constants[8]) {
                    break;
                }
                if (unk180 != -1) {
                    m_soundGenerator.stopSE(unk180, 90);
                }
                unk180 = -1;
                m_soundGenerator.playSE(snd_se_stage_Pirates_08, 0, 0, -1);
                unk184++;
                break;
            }
        }
    }
}

// The model follows the matrix the stage hands to the tornado.
void grPiratesTornado::updateCallBack(float deltaFrame) {
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
            if (unk164 != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *static_cast<Matrix*>(unk164);
            }
        }
    }
}

void grPiratesTornado::setMotion(u32 motion, bool loop, bool force, float* endFrame) {
    PIRATES_SET_MOTION(m_motionId, 1, motion, loop, force, endFrame)
}
