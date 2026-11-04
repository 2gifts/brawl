#pragma once

#include <StaticAssert.h>
#include <sora_adv_stage/gr_adventure_move_attack_ground.h>
#include <types.h>

class grAdventureFallingRock : public grAdventureMoveAttackGround {
protected:
    float m_timer; // 0x168, HYPOTHESIS: countdown before the rock's attack is set
    u8 m_isStarted; // 0x16C, HYPOTHESIS: set once the motion path has started
    u8 m_isAttackSet; // 0x16D, HYPOTHESIS: the attack gimmick has been applied
    void* m_data; // 0x170

public:
    grAdventureFallingRock(const char* taskName) : grAdventureMoveAttackGround(taskName) {
        m_timer = 0.0f;
        m_isStarted = 0;
        m_isAttackSet = 0;
    }
    virtual ~grAdventureFallingRock();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);

    static grAdventureFallingRock* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureFallingRock) == 0x174, "Class is wrong size!");
