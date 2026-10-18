#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <types.h>

// GSTG record of the adventure stage data (field names from the BrawlCrate GSTG node). HYPOTHESIS: the struct name.
struct grAdventureStaticModelData {
    Vec3f m_pos;             // 0x0
    Vec3f m_rot;             // 0xC
    u8 m_modelDataIndex;     // 0x18
    u8 m_collisionDataIndex; // 0x19
    u8 unk1A;
    u8 unk1B;
};
static_assert(sizeof(grAdventureStaticModelData) == 0x1C, "Class is wrong size!");

// A static model placed in a Subspace Emissary stage: its position and rotation come from the GSTG record.
class grAdventureStaticModel : public grGimmick {
    grAdventureStaticModelData* m_data; // 0x14C

public:
    grAdventureStaticModel(const char* taskName) : grGimmick(taskName) {
        m_data = NULL;
    }
    virtual ~grAdventureStaticModel();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);

    static grAdventureStaticModel* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grAdventureStaticModel) == 0x150, "Class is wrong size!");
