#include <sora_adv_stage/gr_adventure_fallingrock.h>
#include <types.h>

grAdventureFallingRock* grAdventureFallingRock::create(int mdlIndex, const char* taskName) {
    grAdventureFallingRock* ground = new (Heaps::StageInstance) grAdventureFallingRock(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

void grAdventureFallingRock::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureMoveAttackGround::startup(data, unk1, layerType);
    m_data = m_gimmickData;
}

grAdventureFallingRock::~grAdventureFallingRock() { }
