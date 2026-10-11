#pragma once

#include <gr/gr_yakumono.h>

// The base of the grounds of Flat Zone 2. A ground belongs to one scene (the scene id): it is only active while the stage
// plays that scene, and its animation plays from the start when the scene begins.
class grGW : public grYakumono {
protected:
    u8 m_state;                 // 0x150 the step of the sequence of the ground
    float m_timer;              // 0x154 frames until the next step
    u8* m_sceneIdWork;          // 0x158 the scene that plays (from the stage)
    u8 m_sceneId;               // 0x15C the scene that the ground belongs to (4 = all)
    u8 m_motion;                // 0x15D the animation that plays (1 at the start: none)
    float m_frameLast;          // 0x160 the frame of the animation of the last update
    float m_frameCount;         // 0x164 how many frames the animation has
    Vec3f m_scale;              // 0x168 the scale of the node of the model (read once)

public:
    grGW(const char* taskName);
    virtual ~grGW();
    virtual void update(float deltaFrame);
    virtual void updateScaleBase(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 index, bool loop, bool force, float* frameCount);
    virtual void setSceneIDWork(u8* scene);
    virtual void setSceneID(u8 scene);

    static grGW* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grGW) == 0x174, "Class is wrong size!");
