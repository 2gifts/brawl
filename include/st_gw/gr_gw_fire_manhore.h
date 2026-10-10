#pragma once

#include <snd/snd_3d_generator.h>
#include <st_gw/gr_gw.h>

// A fire of the fire scene (one of four kinds). It is shown while the stage says that its kind of fire burns, on the step
// that the stage names; a sound plays when it starts to burn on a new step.
class grGWFireManhore : public grGW {
protected:
    u8 m_type;                  // 0x174 the kind of fire that this ground is (6 - 9)
    char _175[3];
    Vec3f* m_posWork;           // 0x178 the places of the steps (from the stage)
    u8* m_stepWork;             // 0x17C the step that burns (0xFF = none)
    u8* m_ctrlStepWork;         // 0x180
    u8* m_typeWork;             // 0x184 the kind of fire that burns (from the stage)
    u8* m_eventWork;            // 0x188 the event of the scene (from the stage)
    u8 m_stepLast;              // 0x18C the step of the last update
    char _18d[3];
    snd3DGenerator m_snd;       // 0x190

public:
    static grGWFireManhore* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    inline grGWFireManhore(const char* taskName);
    virtual ~grGWFireManhore();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateVisible(float deltaFrame);
    virtual void setPosWork(Vec3f* pos);
    virtual void setStepWork(u8* step);
    virtual void setCtrlStepWork(u8* control);
    virtual void setEventWork(u8* event);
    virtual void setTypeWork(u8* type);
    virtual void setType(u8 type);
};
static_assert(sizeof(grGWFireManhore) == 0x198, "Class is wrong size!");
