#include <sora_adv_stage/gr_adventure_move_ground.h>
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
