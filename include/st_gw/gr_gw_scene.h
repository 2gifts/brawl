#pragma once

#include <st_gw/gr_gw.h>

class stDataMultiContainer;

class grGWScene : public grGW {
protected:
    u8 m_stateEvent;
    float m_timerEvent;
    float* m_frameWork;
    Vec3f* m_posGimmickWork;
    u8* m_ctrlStepWork;
    u8* m_eventWork;
    u8 m_first;
    u8 m_eventOn;
    stDataMultiContainer* m_tblStep;

public:
    static grGWScene* create(int modelIndex, const char* nodeName, const char* taskName);
    grGWScene(const char* taskName);
    virtual ~grGWScene();
    virtual void update(float deltaFrame);
    virtual void updateSub(float deltaFrame);
    virtual void updateSubEnd(float deltaFrame);
    virtual void updateStep(float deltaFrame);
    virtual void updateEvent(float deltaFrame);
    virtual void selectStep();
    virtual void clearStep();
    virtual void setFrameWork(float* frame);
    virtual void setEventWork(u8* event);
    virtual void setPosGimmickWork(Vec3f* positions);
    virtual void setCtrlStepWork(u8* control);
    virtual void setTblStepAcc(stDataMultiContainer* table);
};
static_assert(sizeof(grGWScene) == 0x194, "Class is wrong size!");
