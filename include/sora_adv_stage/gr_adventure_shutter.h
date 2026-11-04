#pragma once

#include <sora_adv_stage/gr_adventure_move_ground.h>

class grAdventureShutter : public grAdventureMoveGround {
protected:
    grGimmickMotionPathData* unk154;
    u8 unk158[8];

public:
    grAdventureShutter(const char* taskName) : grAdventureMoveGround(taskName), unk154(NULL) { }
    virtual ~grAdventureShutter();
    static grAdventureShutter* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureShutter) == 0x160, "Class is wrong size!");
