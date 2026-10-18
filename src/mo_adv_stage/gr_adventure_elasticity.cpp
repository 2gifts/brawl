#include <mt/mt_common.h>
#include <sora_adv_stage/gr_adventure_common.h>
#include <sora_adv_stage/gr_adventure_elasticity.h>

grAdventureElasticity* grAdventureElasticity::create(int mdlIndex, const char* taskName) {
    grAdventureElasticity* ground = new (Heaps::StageInstance) grAdventureElasticity(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureElasticity::~grAdventureElasticity() { }

// Registers one of the two sound-effect works of the floor when the record names a sound.
inline void grAdventureElasticity::setSoundEffect(int index, int soundId, Vec2f offset) {
    if (soundId > 0) {
        m_soundEffects[index].m_id = soundId;
        m_soundEffects[index].m_repeatFrame = 0;
        m_soundEffects[index].m_nodeIndex = 0;
        m_soundEffects[index].m_endFrame = 0;
        m_soundEffects[index].m_offsetPos = offset;
    }
}

void grAdventureElasticity::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    m_data = static_cast<grAdventureElasticityData*>(m_gimmickData);
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
    createSoundWork(2, 1);
    setSoundEffect(0, m_data->m_soundId, Vec2f(0.0f, 0.0f));
    setSoundEffect(1, m_data->m_soundId2, Vec2f(0.0f, 0.0f));
}

void grAdventureElasticity::update(float deltaFrame) {
    if (m_isMoving) {
        grGimmick::update(deltaFrame);
        if (m_isLanding == 0) {
            if (m_offsetY > 0.0f) {
                m_offsetY -= m_data->m_stopThreshold;
            } else {
                m_offsetY += m_data->m_stopThreshold;
            }
            if ((float)fabs(m_offsetY) < m_data->m_stopThreshold) {
                m_isMoving = 0;
                m_offsetY = 0.0f;
            }
        } else {
            m_offsetY += m_speedY;
        }
        Vec3f pos = getPos();
        pos.m_y = m_data->m_pos.m_y + m_offsetY;
        setPos(&pos);
        if (m_wasLanding != m_isLanding) {
            if (m_isLanding == 1) {
                startGimmickSE(0);
                stopGimmickSE(1);
            } else {
                startGimmickSE(1);
                stopGimmickSE(0);
            }
        }
        grGimmick::updateCallback(0);
    } else {
        m_hasUpdatedG3dCalcWorld = true;
    }
    m_wasLanding = m_isLanding;
    m_isLanding = 0;
}

void grAdventureElasticity::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, reinterpret_cast<int*>(&sOwnerFlags))) {
        gfTask* task = gfTask::getTask(collStatus->m_taskId);
        s8 playerNo = advGetPlayerNumber(task->m_taskId);
        if (playerNo == 0 || m_data->m_isPlayerOneOnly == 0) {
            if (m_isLanding == 0) {
                if (isFirstContact) {
                    float speed = mtSqrtf(m_data->m_bounceSpeed * m_data->m_bounceSpeed - m_offsetY * (m_data->m_gravity * m_offsetY));
                    m_speedY = speed;
                    if (m_data->m_bounceSpeed < 0.0f) {
                        m_speedY = -(float)(double)speed;
                    }
                    m_isMoving = 1;
                }
                float accel = -m_offsetY * m_data->m_gravity;
                m_speedY += accel;
                if ((float)fabs(m_speedY) <= (float)fabs(accel)) {
                    m_speedY = 0.0f;
                }
                m_isLanding = 1;
            }
        }
    }
}

void grAdventureElasticity::setVisibility(u32 unk1) {
    Ground::setVisibility(unk1);
}

