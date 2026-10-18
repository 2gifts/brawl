#include <mt/mt_prng.h>
#include <sora_adv_stage/gr_adventure_common.h>
#include <sora_adv_stage/gr_adventure_tikuwa.h>

grAdventureTikuwa* grAdventureTikuwa::create(int mdlIndex, const char* taskName) {
    grAdventureTikuwa* ground = new (Heaps::StageInstance) grAdventureTikuwa(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureTikuwa::~grAdventureTikuwa() { }

// HYPOTHESIS: frames the floor holds before it starts to shake, per difficulty level.
static u16 sTikuwaWaitFrames[16] = { 12, 11, 10, 8, 8, 8, 6, 6, 6, 4, 4, 4, 3, 2, 1, 0 };

void grAdventureTikuwa::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    m_data = static_cast<grAdventureTikuwaData*>(m_gimmickData);
    m_data->m_waitFrames = sTikuwaWaitFrames[advGetDifficultyLevel()];
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
    Vec3f pos;
    Vec3f rot;
    pos = Vec3f(m_data->m_pos.m_x, m_data->m_pos.m_y, 0.0f);
    rot = Vec3f(0.0f, 0.0f, m_data->m_rotZ);
    setPos(&pos);
    setRot(&rot);
    grGimmick::updateCallback(0);
    createIsValidTrigger(&m_data->m_trigger);
}

void grAdventureTikuwa::update(float deltaFrame) {
    switch (m_state) {
    case 0:
        m_hasUpdatedG3dCalcWorld = true;
        break;
    case 1:
        if (m_isLanding == 0) {
            m_state = 0;
            m_timer = 0.0f;
        }
        if (m_timer >= (float)m_data->m_waitFrames) {
            m_state = 2;
            m_timer = 0.0f;
        }
        m_isLanding = 0;
        break;
    case 2:
    case 3: {
        grGimmick::update(deltaFrame);
        Vec3f pos = getPos();
        if (m_state == 2) {
            if ((u32)m_timer % 10 < 5) {
                pos.m_y = m_data->m_pos.m_y + m_data->m_shakeRange * randf();
            } else {
                pos.m_y = m_data->m_pos.m_y - m_data->m_shakeRange * randf();
            }
            if (m_timer >= (float)m_data->m_shakeFrames) {
                pos.m_x = m_data->m_pos.m_x;
                m_state = 3;
                m_timer = 0.0f;
            }
        } else {
            m_speedY += m_data->m_fallAccel;
            pos.m_y += m_speedY;
            if (m_timer >= (float)m_data->m_fallFrames) {
                m_state = 4;
                m_timer = 0.0f;
                setValid(0);
            }
        }
        setPos(&pos);
        grGimmick::updateCallback(0);
        break;
    }
    case 4:
        if (m_data->m_respawnFrames != 0 && m_timer >= (float)m_data->m_respawnFrames - 120.0f) {
            m_state = 5;
            m_timer = 0.0f;
            m_blinkTimer = 0.0f;
            m_blinkFrames = 6;
            Vec3f pos;
            pos = Vec3f(m_data->m_pos.m_x, m_data->m_pos.m_y, 0.0f);
            setPos(&pos);
        }
        break;
    case 5:
        if (m_blinkTimer > (float)m_blinkFrames) {
            setVisibility(m_isVisible ? 0 : 1);
            m_blinkTimer = 0.0f;
        }
        if (m_timer > 60.0f) {
            m_blinkFrames = 3;
        }
        if (m_timer > 120.0f) {
            m_state = 0;
            m_timer = 0.0f;
            setValid(1);
        }
        m_blinkTimer += deltaFrame;
        break;
    }
    m_timer += deltaFrame;
}

void grAdventureTikuwa::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, reinterpret_cast<int*>(&sOwnerFlags))) {
        if (isFirstContact && m_state == 0) {
            m_speedY = m_data->m_fallSpeed;
            m_state = 1;
            m_timer = 0.0f;
        }
        m_isLanding = 1;
    }
}

void grAdventureTikuwa::setVisibility(u32 unk1) {
    Ground::setVisibility(unk1);
}

