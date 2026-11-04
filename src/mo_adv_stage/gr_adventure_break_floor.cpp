#include <sora_adv_stage/gr_adventure_break_floor.h>
#include <sora_adv_stage/gr_adventure_common.h>

grAdventureBreakFloor* grAdventureBreakFloor::create(int mdlIndex, const char* taskName) {
    grAdventureBreakFloor* ground = new (Heaps::StageInstance) grAdventureBreakFloor(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureBreakFloor::~grAdventureBreakFloor() { }

void grAdventureBreakFloor::setInitializeFlag() {
    if (g_stTriggerMng->getTriggerFlag(&m_data->m_validTrigger, true) == false) {
        if (m_data->m_validTrigger.m_isValid) {
            setValid(0);
        }
    } else if (!m_data->m_validTrigger.m_isValid) {
        setValid(0);
    }
}

void grAdventureBreakFloor::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask__6GroundFP12grCollStatusPi(this, collStatus, reinterpret_cast<int*>(&sOwnerFlags)) && isFirstContact) {
        g_stTriggerMng->setTriggerFlag(&m_data->m_breakTrigger);
        m_isLanded = 1;
        m_timer = 0.0f;
    }
}

void grAdventureBreakFloor::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    switch (m_isLanded) {
    case 1:
        if (m_timer >= (float)unk158) {
            m_isLanded = 2;
        }
        break;
    case 2:
        changeNodeAnim(0, 0);
        for (u32 index = 0; index < 5; index++) {
            stopGimmickSE(index);
        }
        for (int index = 0; index < 5; index++) {
            startGimmickSE(index);
        }
        for (u32 index = 0; index < 5; index++) {
            stopGimmickEffect(index);
        }
        for (int index = 0; index < 5; index++) {
            startGimmickEffect(index);
        }
        setEnableCollisionStatus(false);
        m_isLanded = 3;
        break;
    case 3: {
        u32 animLength = m_modelAnims[0]->m_anmObjChrRes->m_anmChrFile.ptr()->m_animLength;
        u32 frame = m_modelAnims[0]->m_anmObjChrRes->GetFrame();
        if (frame == animLength) {
            setVisibility(0);
        }
        if (m_data->unk1E0 != 0 && m_timer >= (float)m_data->unk1E0) {
            m_isLanded = 4;
        }
        break;
    }
    case 4:
        setVisibility(1);
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
        setEnableCollisionStatus(true);
        m_isLanded = 0;
        break;
    }
    m_timer += deltaFrame;
    grGimmick::updateCallback(0);
}
