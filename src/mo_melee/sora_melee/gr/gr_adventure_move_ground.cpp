#include <gr/gr_gimmick_motion_path.h>
#include <mt/mt_prng.h>
#include <sora_adv_stage/gr_adventure_move_ground.h>
#include <st/st_trigger.h>
#include <types.h>

grAdventureMoveGround* grAdventureMoveGround::create(int mdlIndex, const char* taskName) {
    grAdventureMoveGround* ground = new (Heaps::StageInstance) grAdventureMoveGround(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureMoveGround::grAdventureMoveGround(const char* taskName) : grYakumono(taskName) {
    unk150 = NULL;
    m_transparencySettings = 0xE;
}

grAdventureMoveGround::~grAdventureMoveGround() { }

void grAdventureMoveGround::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
}

void grAdventureMoveGround::changeMotionPathRandomSE(u32 range) {
    int id;
    int offset;
    if (range >= 1) {
        offset = (int)(randf() * range) % (int)range;
        for (int i = 0; i < 5; i++) {
            if (unk150->m_moveSE[i].m_id > 0) {
                id = offset + unk150->m_moveSE[i].m_id;
                grGimmickMotionPath* path = m_gimmickMotionPath;
                if (id > 0) {
                    path->m_soundEffects[i + 3].m_id = id;
                }
            }
        }
        if (unk150->m_endSE[0].m_id > 0) {
            id = offset + unk150->m_endSE[0].m_id;
            grGimmickMotionPath* path = m_gimmickMotionPath;
            if (id > 0) {
                path->m_soundEffects[8].m_id = id;
            }
        }
        if (unk150->m_endSE[1].m_id > 0) {
            id = offset + unk150->m_endSE[1].m_id;
            grGimmickMotionPath* path = m_gimmickMotionPath;
            if (id > 0) {
                path->m_soundEffects[9].m_id = id;
            }
        }
        if (unk150->m_endSE[2].m_id > 0) {
            id = offset + unk150->m_endSE[2].m_id;
            grGimmickMotionPath* path = m_gimmickMotionPath;
            if (id > 0) {
                path->m_soundEffects[10].m_id = id;
            }
        }
    }
}

void grAdventureMoveGround::setInitializeFlag() {
    if (!g_stTriggerMng->getTriggerFlag(&unk150->m_isValidTrigger, true)) {
        if (unk150->m_isValidTrigger.m_isValid) {
            setValid(0);
        }
    } else {
        if (!unk150->m_isValidTrigger.m_isValid) {
            setValid(0);
        }
    }
    if (g_stTriggerMng->getTriggerFlag(&unk150->m_motionTrigger, false) == true) {
        m_gimmickMotionPath->setFrame((float)m_gimmickMotionPath->m_frameCount);
        m_gimmickMotionPath->setFrameModelAnim();
    }
}

void grAdventureMoveGround::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask(collStatus, reinterpret_cast<int*>(&sOwnerFlags))) {
        if ((s8)getPlayerNumber(collStatus) == 0) {
            g_stTriggerMng->setTriggerFlag(&unk150->m_landingTrigger);
        }
    }
}
