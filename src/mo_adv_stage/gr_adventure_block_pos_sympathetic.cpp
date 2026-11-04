#include <mt/mt_vector.h>
#include <sora_adv_stage/gr_adventure_block_sympathetic_manager.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_gimmick_block_sympathetic.h>
#include <st/st_trigger.h>

grGimmickBlockSympathetic::~grGimmickBlockSympathetic() { }

grAdventureBlockSympatheticManager* grAdventureBlockSympatheticManager::create(int mdlIndex, const char* taskName) {
    grAdventureBlockSympatheticManager* gimmick = new (Heaps::StageInstance) grAdventureBlockSympatheticManager(taskName);
    gimmick->setMdlIndex(mdlIndex);
    return gimmick;
}

void grAdventureBlockSympatheticManager::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    m_data = static_cast<Data*>(m_gimmickData);
    createBlock(data);

    grGimmickBlockSympathetic* block;
    u32 count;
    u32 i = 0;
    nw4r::g3d::ResMdl mdl = m_sceneModels[0]->m_resMdl;
    Node* head = m_head;
    count = m_count;
    m_cursor = head;
    for (; i < count; i++) {
        block = m_cursor->m_block;
        nw4r::g3d::ResNode node = mdl.GetResNode(static_cast<int>(block->unk150));
        Vec3f pos;
        pos.m_x = node->m_translation.m_x;
        pos.m_y = node->m_translation.m_y;
        pos.m_z = node->m_translation.m_z;
        float radius = m_data->unk44;
        u8 kind = m_data->unk40;
        block->setPos(&pos);
        block->updateCallback(0);
        block->updateG3dProcCalcWorld();
        Vec2f xy;
        xy.m_x = pos.m_x;
        xy.m_y = pos.m_y;
        block->m_hitCircle = addHitCircle(&xy, kind, 1, radius);
        m_cursor = m_cursor->m_next;
    }
    if (m_data->unk48.m_triggerId != 0) {
        g_stTriggerMng->createTrigger(Gimmick::Area_None, &m_data->unk48);
    }
    createInvalidCameraClippingSphere();
}

void grAdventureBlockSympatheticManager::setInitializeFlag() {
    if (m_data->unk48.m_triggerId != 0 && g_stTriggerMng->getTriggerFlag(&m_data->unk48, true) == true) {
        u32 count = m_count;
        m_cursor = m_head;
        for (u32 i = 0; i < count; i++) {
            m_cursor->m_block->setValid(0);
            m_cursor = m_cursor->m_next;
        }
    }
}
