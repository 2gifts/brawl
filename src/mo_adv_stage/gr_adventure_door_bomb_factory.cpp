#include <sora_adv_stage/gr_adventure_door_bomb_factory.h>
#include <types.h>

grAdventureDoorBombFactory::grAdventureDoorBombFactory(const char* taskName) : grAdventureDoor(taskName) {
    m_bombFactoryData = NULL;
}

grAdventureDoorBombFactory::~grAdventureDoorBombFactory() { }

grAdventureDoorBombFactory* grAdventureDoorBombFactory::create(int mdlIndex, u32 jumpData, const char* taskName) {
    grAdventureDoorBombFactory* door = new (Heaps::StageInstance) grAdventureDoorBombFactory(taskName);
    if (door != NULL) {
        door->setJumpData(jumpData);
        door->setMdlIndex(mdlIndex);
    }
    return door;
}

void grAdventureDoorBombFactory::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grAdventureDoor::startup(data, unk1, layerType);
    // HYPOTHESIS: the slot is getModelCount in our Ground vtable; the result is not used.
    getModelCount();
    m_bombFactoryData = static_cast<grAdventureDoorBombFactoryData*>(m_gimmickData);
}

void grAdventureDoorBombFactory::update(float deltaFrame) {
    grAdventureDoor::update(deltaFrame);
    grGimmick::updateCallback(1);
}

void grAdventureDoorBombFactory::openTheDoor() {
    grGimmick::changeNodeAnim(0, 1);
    changeTexSrtAnim(2, 1);
    EachDoorTypeEffect();
}
