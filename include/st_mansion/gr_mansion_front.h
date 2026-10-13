#pragma once

#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>

#include <st_mansion/gr_mansion.h>

// The front of the house: the wall that hides the inside. It opens when a fighter comes near (an area of the house gone) and
// closes again; when one of the areas is back it shows the effect of the repair.
class grMansionFront : public grMansion {
protected:
    Vec3f* unk158;      // 0x158 the two locators of the front (left and right)
    u8* unk15C;         // 0x15C the steps of the four areas
    u8* unk160;         // 0x160 the step of the event (0 = the event goes on)
    float unk164;       // 0x164 frames until the wall closes again (fighters near it keep it open)
    u8 unk168;          // 0x168 1 = a fighter is near the wall
    u8 unk169;          // 0x169 the step of the opening/closing animation
    u8 unk16A;          // 0x16A the animation that plays
    float unk16C;       // 0x16C the frame the animation goes to
    float unk170;       // 0x170 frames of the animation
    snd3DGenerator unk174;  // 0x174

public:
    static grMansionFront* create(int modelIndex, const char* nodeName, const char* taskName);
    grMansionFront(const char* taskName) : grMansion(taskName), unk174() {
        unk158 = NULL;
        unk15C = NULL;
        unk160 = NULL;
        unk164 = 5.0f;
        unk168 = 0;
        unk169 = 2;
        unk16A = 1;
        unk16C = 0.0f;
        unk170 = 0.0f;
    }
    virtual ~grMansionFront();
    virtual void update(float deltaFrame);
    virtual void updateArea(float deltaFrame);
    virtual void updateDestroy(float deltaFrame);
    virtual void setMotion(u32 index, bool loop, bool force, float* frameCount);
    virtual void setPosGimmickWork(Vec3f* positions) { unk158 = positions; }
    virtual void setStateWork(u8* state) { unk15C = state; }
    virtual void setStateAreaWork(u8* state) { unk160 = state; }
};
static_assert(sizeof(grMansionFront) == 0x17C, "Class is wrong size!");
