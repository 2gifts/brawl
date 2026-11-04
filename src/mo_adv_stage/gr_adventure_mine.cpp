#include <mt/mt_matrix.h>
#include <sora_adv_stage/gr_adventure_mine.h>

extern "C" const char lbl_40_data_6380[];

grAdventureMine* grAdventureMine::create(int mdlIndex, const char* taskName) {
    grAdventureMine* mine = new (Heaps::StageInstance) grAdventureMine(taskName);
    if (mine != NULL) {
        mine->setMdlIndex(mdlIndex);
    }
    return mine;
}

void grAdventureMine::processUpdate() {
    if (m_isUpdate) {
        grGimmick::processUpdate();
    }
}

grAdventureMine::~grAdventureMine() { }

void grAdventureMine::update(float deltaFrame) {
    grGimmick::updateCallback(0);
    m_isUpdate = false;
}

void grAdventureMine::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grGimmickBlock::startup(data, unk1 | 0x108, layerType);
    setCalcuCallbackRoot(8);
    setSoundEffect(0, 0x2279, Vec2f(0.0f, 0.0f));
    setEffect(0, 0x1030022, lbl_40_data_6380, Vec2f(0.0f, 0.0f), 1.0f);
    grGimmick::changeNodeAnim(0, 0);
}

void grAdventureMine::callBreak() {
    Matrix matrix(true);
    getMatrix(&matrix);
    Vec3f pos = matrix.getTranslation();
    startGimmickSE(0);
    startGimmickEffect(0);
}
