#pragma once

#include <gr/collision/gr_collision_joint.h>
#include <nw4r/ut/ut_LinkList.h>
#include <snd/snd_3d_generator.h>
#include <st_gw/gr_gw_scene.h>

// The fire scene (a burning building): a fire man runs along the steps of the building, the flames and the divers (who jump
// from the left and the right) come and go; items that fall out of the stage are taken away.
class grGWSceneFire : public grGWScene {
    struct StepNode { // a candidate for the step that the fire man runs to
        nw4r::ut::LinkListNode m_node;
        u8 m_index;
    };
    float m_timerStep;                  // 0x194 frames until the next step is picked
    u8* m_stepManhoreWork;              // 0x198 the step that the fire is on (from the stage)
    u8* m_typeManhoreWork;              // 0x19C the type of the fire (from the stage)
    u32 _1a0;
    grCollisionJoint** m_joints;        // 0x1A4 the joints of the 20 steps
    u8 m_stateDiver;                    // 0x1A8 the step of the sequence of the diver
    char _1a9[3];
    float m_timerDiver;                 // 0x1AC
    Vec3f* m_posDiverWork;              // 0x1B0 the places of the divers (the third is where the diver is now)
    u8* m_typeDiverWork;                // 0x1B4 the type of the diver (from the stage)
    u8 m_sideDiver;                     // 0x1B8 1 = from the left, 2 = from the right
    char _1b9[3];
    float m_posItemX;                   // 0x1BC where the item that the diver goes for is
    int m_itemId;                       // 0x1C0 the instance id of that item
    Vec3f* m_posLimitWork;              // 0x1C4 the limits of the camera (from the stage)
    snd3DGenerator m_snd;               // 0x1C8
    nw4r::ut::LinkList<StepNode, 0> m_steps; // 0x1D0

public:
    static grGWSceneFire* create(int modelIndex, const char* nodeName, const char* taskName);
    grGWSceneFire(const char* taskName);
    virtual ~grGWSceneFire();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateSub(float deltaFrame);
    virtual void updateSubEnd(float deltaFrame);
    virtual void updateStep(float deltaFrame);
    virtual void updateEvent(float deltaFrame);
    virtual void selectStep();
    virtual void clearStep();
    virtual void updateJoint(float deltaFrame);
    virtual void updateDiver(float deltaFrame);
    virtual void selectStepManhore();
    virtual void selectStepManhoreDetails();
    virtual void clearStepManhore();
    virtual bool isRemoveItem(int itemId);
    virtual void setStepManhoreWork(u8* step);
    virtual void setTypeManhoreWork(u8* type);
    virtual void setPosDiverWork(Vec3f* pos);
    virtual void setTypeDiverWork(u8* type);
    virtual void setPosLimitWork(Vec3f* limit);
};
static_assert(sizeof(grGWSceneFire) == 0x1DC, "Class is wrong size!");
