#pragma once

#include <gr/gr_yakumono.h>
#include <so/collision/so_collision_hit_part.h>
#include <so/so_array.h>
#include <st/st_trigger.h>
#include <yk/yakumono.h>

class grAdventureMine;

// HYPOTHESIS: the full hit sphere record (soCollisionHitData) with its flag word, which the YK_STAGE_FULL shadow of the hit
// header does not describe.
struct grMineHitData {
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
static_assert(sizeof(grMineHitData) == 32, "Class is wrong size!");

// HYPOTHESIS: the gimmick record of the mine group. The first 0x5C bytes are the attack data of every mine, the rest are
// the hit sphere that follows the mine and the trigger of the group (offsets are the bytes startup reads).
// HYPOTHESIS: startup copies the attack data of the record with one word more than grGimmick::AttackData holds here.
struct grMineAttackData : grGimmick::AttackData {
    u32 unk58;
};

struct grGimmickMineData {
    grMineAttackData m_attackData;      // 0x00
    Vec3f m_hitStartOffset;             // 0x5C
    Vec3f m_hitEndOffset;               // 0x68
    float m_hitSize;                    // 0x74
    u8 m_hitShape;                      // 0x78
    u8 unk79[3];
    int m_teamId;                       // 0x7C, startup stores 0x12
    stTriggerData m_trigger;            // 0x80
    u8 m_mineMdlIndex;                  // 0x84
    u8 unk85;
    u16 m_mineParam;                    // 0x86
    float unk88;
};

// HYPOTHESIS: the base of the mine manager is a class of sora_adv_stage that is not split yet (its constructor is
// fn_40_1C0CC, taking a byte for 0x15C and the task name).
class grAdventureMineManagerBase : public grYakumono {
public:
    grAdventureMineManagerBase(u8 unk15C, const char* taskName);

protected:
    u32 unk150;
    u32 unk154;
    grGimmickMineData* unk158;
    u8 unk15C;
    u32 unk160;
    u32 unk164;
    u32 unk168;
    u32 unk16C;
public:
    u8 unk170;
};

class grAdventureMineManager : public grAdventureMineManagerBase {
public:
    grAdventureMine* unk174[13];
    u8 unk1A8;
    grGimmickMineData* unk1AC;
    grMineHitData m_hits[13];             // 0x1B0
    soSet<grMineHitData> m_hitSets[13]; // 0x350
    ykDataGroup m_dataGroups[13];        // 0x3B8
    ykData m_ykData;                     // 0x454

    grAdventureMineManager(const char* taskName) : grAdventureMineManagerBase(0, taskName), unk1A8(0), unk1AC(NULL) { }

    static grAdventureMineManager* create(int mdlIndex, const char* taskName);

    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onInflict(soCollisionLog* collisionLog, u32 unk2, float power);
    virtual void dbDispInvalidatedByCameraClippingSphere();
    void chSympatheticDetonation();
    void chWarning();
    void frowExplosion(const int& index, float deltaFrame);
    void frowExploding(const int& index, float deltaFrame);
    void frowStandby(const int& index, float deltaFrame);
    void frowEnding(const int& index, float deltaFrame);
};

static_assert(sizeof(grAdventureMineManager) == 0x45C, "Class is wrong size!");
