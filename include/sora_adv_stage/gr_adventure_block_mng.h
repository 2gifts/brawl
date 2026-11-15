#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <so/so_array.h>
#include <types.h>
#include <yk/yakumono.h>

class grGimmickBlock;

// HYPOTHESIS: the hit sphere record of one block (soCollisionHitData with its flag word, which the YK_STAGE_FULL shadow
// of the hit header does not describe).
struct grAdventureBlockHitData {
    Vec3f m_startOffsetPos;
    Vec3f m_endOffsetPos;
    float m_size;
    unsigned int m_nodeIndex : 9;
    unsigned int m_part : 2;
    unsigned int m_height : 2;
    unsigned int m_0x1c_3 : 2;
    unsigned int m_shapeType : 1;
    unsigned int m_0x1c_1 : 16;
};
static_assert(sizeof(grAdventureBlockHitData) == 0x20, "Class is wrong size!");

// HYPOTHESIS: the gimmick record of a block group. Offsets are the bytes createBlock, startup and the clipping code read.
struct grAdventureBlockMngData {
    Vec3f m_hitStartOffset; // 0x00
    Vec3f m_hitEndOffset;   // 0x0C
    float m_hitSize;        // 0x18
    s8 m_hitShape;          // 0x1C
    u8 m_collFlags;         // 0x1D, bit 0: keep the block collision joint as it is
    u8 unk1E[0xB];
    u8 m_blockMdlIndex;     // 0x29
    u8 m_noSave;            // 0x2A, HYPOTHESIS: 1 when the broken blocks are not kept in the initialize info
    u8 m_situationODD;      // 0x2B
    u8 unk2C;
    u8 m_collIndex;         // 0x2D, 0xFF: the blocks have no collision of their own
};

// HYPOTHESIS: the record setInitializeInfo/getInitializeInfo exchange (kind 0x30 = a block group, one bit per block).
struct grAdventureBlockInitializeInfo {
    int m_kind;
    u32 unk4;
    u64 m_brokenMask;
};

// The breakable block group of the Subspace stages (sora_adv_stage gr_adventure_block_mng.o): it creates one
// grGimmickBlock per model node and one hit sphere per block on a shared ykNormal hit object.
class grAdventureBlockMng : public grYakumono {
protected:
    u32 m_blockNum;                     // 0x150
    grGimmickBlock** m_blocks;          // 0x154
    grAdventureBlockMngData* m_data;    // 0x158
    u8 m_blockKind;                     // 0x15C, HYPOTHESIS: 0 creates plain grGimmickBlocks
    grAdventureBlockHitData* m_hitData; // 0x160
    soSet<grAdventureBlockHitData>* m_hitSets; // 0x164
    ykDataGroup* m_dataGroups;          // 0x168
    ykData* m_ykData;                   // 0x16C
public:
    u8 m_isDamaged;                     // 0x170, set by onDamage, cleared (with the attacker info) by update

    grAdventureBlockMng(u8 blockKind, const char* taskName);
    virtual ~grAdventureBlockMng();
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void setInitializeInfo(int initializeInfo);
    virtual bool getInitializeInfo(int initializeInfo);
    virtual void invalidatedByCameraClipping();
    virtual void dbDispInvalidatedByCameraClippingSphere();
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void requestBuild(u8 index);
    virtual void requestBuild(grGimmickBlock* block);
    virtual void requestBreak(u8 index);
    virtual void createInvalidCameraClippingSphere();

    static grAdventureBlockMng* create(int mdlIndex, u8 blockKind, const char* taskName);

    bool createBlock(gfArchive* archive);
    void messageBreak(grGimmickBlock* block);
};
static_assert(sizeof(grAdventureBlockMng) == 0x174, "Class is wrong size!");
