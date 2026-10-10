#pragma once

#include <st/st_data_container.h>
#include <st_gw/gr_gw.h>

// One of the 20 steps of the fire scene (a platform of the building: it is shown while its entry in the table of the scene is
// 1, and its place goes to the stage).
class grGWStep : public grGW {
protected:
    Vec3f* m_posWork;                   // 0x174 the place of the step (to the stage)
    u8* m_ctrlStepWork;                 // 0x178 the control step of the scene (from the stage)
    u8* m_eventWork;                    // 0x17C the event of the scene (from the stage)
    stDataMultiContainer* m_tblStep;    // 0x180 the table of the steps
    u32 m_id;                           // 0x184 the number of the step

public:
    inline grGWStep(const char* taskName);
    static grGWStep* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    virtual ~grGWStep();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateVisible(float deltaFrame);
    virtual void setPosWork(Vec3f* pos);
    virtual void setCtrlStepWork(u8* control);
    virtual void setEventWork(u8* event);
    virtual void setTblStepAcc(stDataMultiContainer* table);
    virtual void setID(u32 id);
};
static_assert(sizeof(grGWStep) == 0x188, "Class is wrong size!");
