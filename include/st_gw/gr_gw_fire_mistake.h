#pragma once

#include <st_gw/gr_gw.h>

// A mistake of the fire man (the sign that shows when a victim is dropped): the stage sets its state to 12 and the sign
// plays its animation (m_frameLast counts the frames of it down) and goes away.
class grGWFireMistake : public grGW {
protected:
    u8* m_stateWork;            // 0x174 the state of the mistake (from the stage: 11 = none, 12 = shown)
    u8* m_eventWork;            // 0x178 the event of the scene (from the stage)

public:
    static grGWFireMistake* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    inline grGWFireMistake(const char* taskName);
    virtual ~grGWFireMistake();
    virtual void update(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setStateWork(u8* state);
    virtual void setEventWork(u8* event);
};
static_assert(sizeof(grGWFireMistake) == 0x17C, "Class is wrong size!");
