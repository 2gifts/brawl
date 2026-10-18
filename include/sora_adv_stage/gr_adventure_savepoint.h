#pragma once

#define YK_DATA_INLINE_CTOR
#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <so/area/so_area_module_impl.h>
#include <types.h>
#include <yk/yakumono.h>

// GSAV record of the adventure stage data (BrawlCrate GSAV node; the area fields are named from their use here).
// HYPOTHESIS: the struct name and the unknown words.
struct grAdventureSavePointData {
    u32 unk0[6];        // 0x0
    Vec2f m_areaOffset; // 0x18
    Vec2f m_areaRange;  // 0x20
    Vec2f m_pos;        // 0x28
    u8 m_modelDataIndex; // 0x30
    u8 unk31[3];
};
static_assert(sizeof(grAdventureSavePointData) == 0x34, "Class is wrong size!");

// The save point of a Subspace Emissary stage: an area trigger that, when a player touches it, asks the adventure data
// for a save (gmAdvData byte 0x603 = 7) and shows the activation for 60 frames.
class grAdventureSavePoint : public grYakumono {
    grAdventureSavePointData* m_data;    // 0x150
    soAreaData m_areaData;               // 0x154
    soSet<soAreaData> m_areaDataSet;     // 0x174
    ykAreaData m_ykData;                 // 0x17C
    u8 m_isActivated;                    // 0x188, set by onGimmickEvent
    u8 m_wasActivated;                   // 0x189, previous frame
    u8 _18A[2];
    float m_timer;                       // 0x18C

public:
    grAdventureSavePoint(const char* taskName) : grYakumono(taskName) {
        m_isActivated = 0;
        m_wasActivated = 0;
        m_timer = 0.0f;
    }
    virtual ~grAdventureSavePoint();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);

    static grAdventureSavePoint* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureSavePoint) == 0x190, "Class is wrong size!");
