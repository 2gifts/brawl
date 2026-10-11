#pragma once

#include <gr/gr_gimmick_spring.h>
#include <snd/snd_3d_generator.h>
#include <st/st_trigger.h>
#include <st/st_trigger_observe.h>

// The trampoline of the fire scene (the one the people jump on): it shoots the fighter up while the scene plays, and is
// folded away (hidden, without its area) while the event of the scene is on.
class grGWFireSpring : public grGimmickSpring {
protected:
    u8* m_sceneIdWork;          // 0x19C the scene that plays (from the stage)
    u8 m_sceneId;               // 0x1A0 the scene that the ground belongs to
    char _1a1[3];
    u8* m_eventWork;            // 0x1A4 the event of the scene (from the stage)
    u8 m_eventLast;             // 0x1A8 the event of the last update
    char _1a9[3];
    Vec3f* m_posWork;           // 0x1AC the place of the trampoline (from the stage)
    u8 m_triggerMade;           // 0x1B0
    u8 m_startFlag;             // 0x1B1
    char _1b2[2];
    Vec3f m_scale;              // 0x1B4 the scale of the node of the model (read once)
    snd3DGenerator m_snd;       // 0x1C0

public:
    static grGWFireSpring* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
    grGWFireSpring(const char* taskName);
    virtual ~grGWFireSpring();
    virtual void startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void processGameProc();
    virtual void update(float deltaFrame);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual void getTopNode(Vec3f* pos);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateScaleBase(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setSceneIDWork(u8* scene);
    virtual void setSceneID(u8 scene);
    virtual void setEventWork(u8* event);
    virtual void setPosWork(Vec3f* pos);
};
static_assert(sizeof(grGWFireSpring) == 0x1C8, "Class is wrong size!");
