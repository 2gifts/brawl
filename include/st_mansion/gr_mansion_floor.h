#pragma once

#include <mt/mt_vector.h>

#include <st_mansion/gr_mansion.h>

// One of the two ghost floors (the platforms that move by themselves): a floor moves to the side or up and down for a while,
// sometimes falls when it is landed on and then comes back. The left one (type 4) and the right one (type 5) move to
// the sides in opposite directions.
class grMansionFloor : public grMansion {
protected:
    u8 unk158;              // 0x158 the step of the move to the side
    u8 unk159;              // 0x159 the step of the move up and down
    float unk15C;           // 0x15C frames until the next step of the side move
    float unk160;           // 0x160 frames until the next step of the up and down move
    float unk164;           // 0x164 frames the floor still sinks (after a landing)
    Vec3f* unk168;          // 0x168 where the floor is (the stage reads the same place)
    Vec3f unk16C;           // 0x16C where the floor started
    Vec3f* unk178;          // 0x178 the limits of the camera
    u8* unk17C;             // 0x17C the step of the floor in the stage
    u8 unk180;              // 0x180 4 = the left floor, 5 = the right floor
    float unk184;           // 0x184 the speed to the side
    float unk188;           // 0x188 the speed up and down
    float unk18C;
    u8 unk190;              // 0x190 1 = the floor was not moved yet
    u8 unk191;              // 0x191 1 = the floor falls the next time it moves
    float unk194;           // 0x194 frames since a fighter landed on the floor
    u8 unk198;              // 0x198 the animation that plays
    float unk19C;           // 0x19C the frame of the animation
    float unk1A0;           // 0x1A0 frames of the animation

public:
    static grMansionFloor* create(int modelIndex, const char* nodeName, const char* taskName);
    grMansionFloor(const char* taskName);
    virtual ~grMansionFloor();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateActive(float deltaFrame);
    virtual void updateMoveLeft(float deltaFrame);
    virtual void updateMoveRight(float deltaFrame);
    virtual void updateMoveUpDown(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 index, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* pos) { unk168 = pos; }
    virtual void setPosLimitWork(Vec3f* posLimit) { unk178 = posLimit; }
    virtual void setStateWork(u8* state) { unk17C = state; }
    virtual void setType(u8 type) { unk180 = type; }
};
static_assert(sizeof(grMansionFloor) == 0x1A4, "Class is wrong size!");
