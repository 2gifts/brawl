#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick_spring.h>
#include <gr/gr_yakumono.h>
#include <so/area/so_area_module_impl.h>
#include <types.h>
#include <yk/yakumono.h>

// The spring of the Subspace and Stage Builder stages (sora_melee gr_gimmick_adv_spring.o): the same record and layout
// as grGimmickSpring, with node animations instead of a shape animation.
class grGimmickAdvSpring : public grYakumono {
protected:
    enum State {
        State_Rest = 0x0,
        State_On = 0x1,
        State_Off = 0x2
    };

    grGimmickSpringData* m_springData; // 0x150
    int unk154;                        // 0x154
    u8 m_state;                        // 0x158, State
    char _159[3];
    float m_animFrame;                 // 0x15C
    u32 m_animOnLength;                // 0x160, frames of node animation 1
    u32 m_animOffLength;               // 0x164, frames of node animation 0
    soAreaData m_areaData;             // 0x168
    soSet<soAreaData> m_areaDataSet;   // 0x188
    ykAreaData m_ykData;               // 0x190

public:
    grGimmickAdvSpring(const char* taskName);
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual ~grGimmickAdvSpring();
    virtual void setInitializeFlag();
    virtual void setMotionOn();
    virtual void setMotionOff();
    virtual void getTopNode(Vec3f* pos);

    static grGimmickAdvSpring* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grGimmickAdvSpring) == 0x19C, "Class is wrong size!");
