#include <gr/gr_pirates_sling.h>
#include <gr/collision/gr_collision.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>

// MATCH-ONLY: keep the stage's original constant pool and node name string.
extern const float g_piratesSlingMotionConstants[]; // Begins with 0.0f, 60.0f, 5.0f
extern const char g_piratesSlingJointNode[];

grPiratesSling* grPiratesSling::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesSling* ground = new (Heaps::StageInstance) grPiratesSling(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesSling::grPiratesSling(const char* taskName) : grPirates(taskName), m_sndGenerator() {
    m_state = 0;
    m_timer = g_piratesSlingMotionConstants[0];
    m_mtxWork = NULL;
    m_stateWork = NULL;
    m_firstActivation = 1;
    m_motionId = 2;
    m_motionEndFrame = g_piratesSlingMotionConstants[0];
    m_collisionJoint = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
    m_yakumonoInitialized = 0;
    m_attackEnabled = 0;
    unk178 = 0;
    m_soundHandle = -1;
    unk188 = 0;
    m_dangerZoneId = -1;
}

grPiratesSling::~grPiratesSling() {
    if (unk178 != 0) {
        delete reinterpret_cast<void*>(unk178);
    }
    unk178 = 0;
}

void grPiratesSling::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateJoint(deltaFrame);
        updateCollision(deltaFrame);
        updateActive(deltaFrame);
        updateSE(deltaFrame);
        updateAI(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Makes the hit object once, as soon as the ground exists.
void grPiratesSling::updateYakumono(float deltaFrame) {
    if (m_yakumonoInitialized != 1) {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoInitialized = 1;
        }
    }
}

// Finds the collision joint this object owns the first time it exists.
void grPiratesSling::updateJoint(float deltaFrame) {
    if (m_collisionJoint == NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            u32 node;
            if (getNodeIndex(&node, 0, g_piratesSlingJointNode)) {
                u16 jointNum = collision->m_jointLen;
                for (u32 i = 0; i != jointNum; i++) {
                    grCollisionJoint* joint = collision->getJoint(i);
                    if (joint != NULL && joint->m_ground == this && joint->_0x4E == 0 && node == joint->m_nodeIndex) {
                        m_collisionJoint = joint;
                        joint->m_0x56_7 = true;
                        break;
                    }
                }
            }
        }
    }
}
