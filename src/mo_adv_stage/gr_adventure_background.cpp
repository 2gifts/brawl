#include <gf/gf_camera.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>
#include <sora_adv_stage/gr_adventure_background.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <st/stage.h>
#include <string.h>

// HYPOTHESIS: an unnamed sora_melee float (several stage units read it the same way).
extern "C" float lbl_27_data_547E0;

grAdventure2* grAdventure2::create(int mdlIndex, const char* taskName) {
    grAdventure2* ground = new (Heaps::StageInstance) grAdventure2(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventure2::~grAdventure2() {
    for (u32 i = 0; i < 4; i++) {
        if (m_backGrounds[i] != NULL) {
            delete m_backGrounds[i];
            m_backGrounds[i] = NULL;
        }
    }
}

void grAdventure2::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
}

bool grAdventure2::setNode() {
    m_backGrounds[0] = new (Heaps::StageInstance) grAdventureBackGround(0, "enkei1");
    m_backGrounds[1] = new (Heaps::StageInstance) grAdventureBackGround(1, "enkei2");
    m_backGrounds[2] = new (Heaps::StageInstance) grAdventureBackGround(2, "enkei3");
    m_backGrounds[3] = new (Heaps::StageInstance) grAdventureBackGround(3, "enkei4");
    u32 modelCount = getModelCount();
    for (u32 i = 0; i < 4; i++) {
        for (u32 j = 0; j < modelCount; j++) {
            if (m_backGrounds[i]->setBackGround(m_sceneModels[j])) {
                break;
            }
        }
    }
    return true;
}

void grAdventure2::renderPre() {
    Ground::renderPre();
    float deltaFrame = lbl_27_data_547E0;
    for (u32 i = 0; i < 4; i++) {
        if (m_backGrounds[i] != NULL) {
            m_backGrounds[i]->update(deltaFrame);
        }
    }
}

grAdventureFighterPos* grAdventureFighterPos::create(int mdlIndex, const char* taskName) {
    grAdventureFighterPos* ground = new (Heaps::StageInstance) grAdventureFighterPos(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureFighterPos::grAdventureFighterPos(const char* taskName) : grGimmick(taskName) {
    unk14C = 0;
    m_transparencySettings = 0;
}

grAdventureFighterPos::~grAdventureFighterPos() { }

void grAdventureFighterPos::update(float deltaFrame) { }

bool grAdventureFighterPos::getLocaterNodePosition(Vec3f* pos, const char* nodeName) {
    for (u32 i = 0; i < getModelCount(); i++) {
        nw4r::g3d::ScnMdl* sceneModel = m_sceneModels[i];
        if (sceneModel != NULL) {
            nw4r::g3d::ResMdl resMdl = static_cast<nw4r::g3d::ScnMdlSimple*>(sceneModel)->m_resMdl;
            if (resMdl.IsValid()) {
                nw4r::g3d::ResNode node = resMdl.GetResNode(nodeName);
                if (node.IsValid()) {
                    pos->m_x = node.ptr()->m_translation.m_x;
                    pos->m_y = node.ptr()->m_translation.m_y;
                    pos->m_z = node.ptr()->m_translation.m_z;
                    return true;
                }
            }
        }
    }
    return false;
}

grAdventureBackGround::grAdventureBackGround(int index, const char* nodeName) {
    m_index = index;
    m_isFound = false;
    m_node = nw4r::g3d::ResNode();
    unk10 = 0;
    m_nodePos.m_x = 0.0f;
    m_nodePos.m_y = 0.0f;
    m_nodePos.m_z = 0.0f;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    m_scale.m_x = 0.0f;
    m_scale.m_y = 0.0f;
    memset(m_nodeName, 0, sizeof(m_nodeName));
    strncpy(m_nodeName, nodeName, sizeof(m_nodeName) - 1);
}

grAdventureBackGround::~grAdventureBackGround() { }

void grAdventureBackGround::update(float deltaFrame) {
    if (m_isFound && m_node.IsValid()) {
        gfCameraManager* cameraManager = gfCameraManager::getManager();
        if (cameraManager != NULL) {
            Vec3f cameraPos;
            Vec3f pos;
            cameraPos = cameraManager->m_cameras[0].m_centerPos;
            pos.m_x = m_nodePos.m_x + (cameraPos.m_x * m_scale.m_x - m_offset.m_x);
            pos.m_y = m_nodePos.m_y + (cameraPos.m_y * m_scale.m_y - m_offset.m_y);
            pos.m_z = m_nodePos.m_z - m_offset.m_z;
            m_node.SetTranslate(pos.m_x, pos.m_y, pos.m_z);
        }
    }
}

bool grAdventureBackGround::setBackGround(nw4r::g3d::ScnMdl* sceneModel) {
    m_isFound = false;
    m_scale.m_x = 0.0f;
    m_scale.m_y = 0.0f;
    m_nodePos.m_x = 0.0f;
    m_nodePos.m_y = 0.0f;
    m_nodePos.m_z = 0.0f;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    nw4r::g3d::ResMdl resMdl = static_cast<nw4r::g3d::ScnMdlSimple*>(sceneModel)->m_resMdl;
    if (!resMdl.IsValid()) {
        return false;
    }
    nw4r::g3d::ResNode node = resMdl.GetResNode(m_nodeName);
    if (!node.IsValid()) {
        return false;
    }
    m_isFound = true;
    m_node = node;
    m_nodePos.m_x = node.ptr()->m_translation.m_x;
    m_nodePos.m_y = node.ptr()->m_translation.m_y;
    m_nodePos.m_z = node.ptr()->m_translation.m_z;
    stAdventure2* stage = dynamic_cast<stAdventure2*>(g_Stage);
    const float* params = stage->m_backGroundParams;
    if (params != NULL) {
        m_scale.m_x = params[m_index];
        params += m_index;
        m_scale.m_y = params[4];
        m_offset.m_x = params[8];
        m_offset.m_y = params[12];
        m_offset.m_z = params[16];
    }
    return true;
}
