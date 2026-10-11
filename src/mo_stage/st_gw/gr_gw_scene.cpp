#include <memory.h>
#include <mt/mt_prng.h>

#include <st_gw/gr_gw_scene.h>

namespace {

enum State { // Name unknown
    State_0 = 0,
    State_1 = 1,
    State_2 = 2,
    State_3 = 3,
    State_4 = 4,
};

// Only the stage-data prefix read by this unit is described.
struct StageData { // Name unknown
    u8 unk00[0x2C];
    float unk2C;
};

}

grGWScene* grGWScene::create(int modelIndex, const char* nodeName, const char* taskName) {
    grGWScene* ground = new (Heaps::StageInstance) grGWScene(taskName);
    if (ground != nullptr) {
        ground->setMdlIndex(modelIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grGWScene::grGWScene(const char* taskName) : grGW(taskName) {
    m_stateEvent = 0;
    m_timerEvent = 0.0f;
    m_frameWork = nullptr;
    m_posGimmickWork = nullptr;
    m_ctrlStepWork = nullptr;
    m_eventWork = nullptr;
    m_first = 1;
    m_eventOn = 0;
    m_tblStep = nullptr;
}

grGWScene::~grGWScene() { }

void grGWScene::update(float deltaFrame) {
    grGW::update(deltaFrame);
    StageData* data = static_cast<StageData*>(getStageData());
    if (data == nullptr) {
        return;
    }
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
        case State_0:
            setMotion(0, false, true, &m_frameCount);
            setMotionFrame(m_frameCount, 0);
            setVisibility(false);
            setEnableCollisionStatus(false);
            clearStep();
            if (m_sceneId != 0) {
                m_first = 0;
            }
            m_stateEvent = 0;
            m_timerEvent = 0.0f;
            m_state = State_1;
            break;
        case State_1:
            if (*m_sceneIdWork == m_sceneId) {
                if (m_first == 1) {
                    setMotion(1, false, true, nullptr);
                    m_first = 0;
                    m_frameLast = 0.0f;
                } else {
                    setMotion(0, true, true, &m_frameCount);
                    setMotionFrame(m_frameCount, 0);
                    m_frameLast = m_frameCount;
                    m_motionRatio *= -1.0f;
                }
                selectStep();
                if (randf() < data->unk2C) {
                    m_eventOn = 1;
                }
                m_state = State_3;
            }
            break;
        case State_2:
            break;
        case State_3:
            if (*m_sceneIdWork != m_sceneId) {
                if (m_ctrlStepWork != nullptr) {
                    ++*m_ctrlStepWork;
                }
                setMotion(0, false, true, &m_frameCount);
                if (m_motionRatio < 0.0f) {
                    m_motionRatio *= -1.0f;
                }
                updateSubEnd(deltaFrame);
                m_eventOn = 0;
                m_state = State_4;
            } else {
                if (getMotionFrame(0) > m_frameLast) {
                    setMotion(1, false, false, nullptr);
                } else {
                    m_frameLast = getMotionFrame(0);
                }
                setVisibility(true);
                setEnableCollisionStatus(true);
                updateSub(deltaFrame);
            }
            break;
        case State_4:
            if (getMotionFrame(0) >= m_frameCount) {
                m_state = State_0;
            }
            break;
    }
}

void grGWScene::clearStep() { }

void grGWScene::selectStep() { }

void grGWScene::updateSub(float deltaFrame) {
    updateStep(deltaFrame);
    updateEvent(deltaFrame);
}

void grGWScene::updateSubEnd(float deltaFrame) { }

void grGWScene::updateStep(float deltaFrame) { }

void grGWScene::updateEvent(float deltaFrame) { }
