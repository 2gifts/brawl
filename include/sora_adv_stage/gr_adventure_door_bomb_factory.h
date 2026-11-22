#pragma once

#include <StaticAssert.h>
#include <sora_adv_stage/gr_adventure_door.h>
#include <types.h>

// HYPOTHESIS: the type name is from the sora_adv_stage map
// (createGimmickFunc<30grAdventureDoorBombFactoryData, ...>); the layout past the door data is not known.
struct grAdventureDoorBombFactoryData;

// The door of the bomb factory of the adventure mode: a plain door with its own open animation.
class grAdventureDoorBombFactory : public grAdventureDoor {
public:
    u8 m_unk1A0[4];
    grAdventureDoorBombFactoryData* m_bombFactoryData; // 0x1A4

    grAdventureDoorBombFactory(const char* taskName);
    static grAdventureDoorBombFactory* create(int mdlIndex, u32 jumpData, const char* taskName);
    virtual ~grAdventureDoorBombFactory();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual void openTheDoor();
};
static_assert(sizeof(grAdventureDoorBombFactory) == 0x1A8, "Class is wrong size!");
