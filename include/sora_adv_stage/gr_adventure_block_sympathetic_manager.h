#pragma once

#include <StaticAssert.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_hit_circle_subject.h>
#include <st/st_trigger_observe.h>
#include <types.h>

class grGimmickBlockSympathetic;

// HYPOTHESIS: the adventure sympathetic-block group (sora_adv_stage gr_adventure_block_pos_sympathetic.o).
class grAdventureBlockSympatheticManager : public grGimmick, public grHitCircleSubject {
public:
    struct Node {
        Node* m_prev;
        Node* m_next;
        grGimmickBlockSympathetic* m_block;

        Node() : m_prev(0), m_next(0) {}
    };

    // HYPOTHESIS: the gimmick data record. Offsets are the bytes createBlock, startup and setInitializeFlag read.
    struct Data {
        u8 unk0[0x29];
        u8 unk29;
        u8 unk2A[3];
        u8 unk2D;
        u8 unk2E[0x12];
        u8 unk40;
        u8 unk41[3];
        float unk44;
        stTriggerData unk48;
    };

    grAdventureBlockSympatheticManager(const char* taskName)
        : grGimmick(taskName), m_head(0), m_tail(0), m_cursor(0), m_count(0) {}

    virtual ~grAdventureBlockSympatheticManager();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setInitializeFlag();
    virtual void invalidatedByCameraClipping();
    virtual void notifyHitCircle(void* circle);

    static grAdventureBlockSympatheticManager* create(int mdlIndex, const char* taskName);

    void createBlock(gfArchive* archive);
    void createInvalidCameraClippingSphere();

    Node* m_head;
    Node* m_tail;
    Node* m_cursor;
    u32 m_count;
    Data* m_data;
};
static_assert(sizeof(grAdventureBlockSympatheticManager) == 0x168, "Class is wrong size!");
