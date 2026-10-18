#include <sora_adv_stage/gr_adventure_gimmick_id.h>
#include <sora_adv_stage/gr_adventure_static.h>

grAdventureStaticModel* grAdventureStaticModel::create(int mdlIndex, const char* taskName) {
    grAdventureStaticModel* ground = new (Heaps::StageInstance) grAdventureStaticModel(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grAdventureStaticModel::~grAdventureStaticModel() { }

void grAdventureStaticModel::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmick::startup(data, unk1, layerType);
    m_data = static_cast<grAdventureStaticModelData*>(m_gimmickData);
    if (m_modelAnims != NULL) {
        m_modelAnims[0]->unbindNodeAnim(m_sceneModels[0]);
    }
    makeCalcuCallback(1, Heaps::StageInstance);
    setCalcuCallbackRoot(3);
    setPos(&m_data->m_pos);
    setRot(&m_data->m_rot);
    updateCallback(0);
}
