#pragma once

#include <mt/mt_vector.h>
#include <types.h>

struct AiClBlock;
class StageObject;

// A weapon/item/gimmick the CPU AI tracks as a thing to dodge or pick up.
// Partial layout verified through the search filters and tracking updates.
class aiWeapon {
public:
    u32 m_objId;       // 0x00: object id
    int m_kind;        // 0x04: weapon (wn) or item (it) kind, depending on m_type
    u16 m_flags;       // 0x08: 1 = active, 2 = predict from per-frame motion, 4 = floor query cached
    s8 m_type;         // 0x0A: 0 = weapon, 1/4/5 = item
    s8 m_sampleFrames; // 0x0B: countdown to the next 16-frame position sample
    AiClBlock* m_floorBlk; // 0x0C: cached floor segment; pickup/chase callers dereference it
    Vec3f m_oldSamplePos; // 0x10: previous completed position sample
    Vec3f m_samplePos;    // 0x1C: latest completed position sample
    Vec3f m_pos;       // 0x28
    Vec3f m_velocity;     // 0x34: position change during the last update
    Vec3f m_acceleration; // 0x40: change in per-frame velocity
    float m_speed;       // 0x4C: length of m_velocity
    float m_radius;      // 0x50: extent used by the prediction queries
    s8 m_team;        // 0x54: search() can exclude this team
    s8 m_ownerPlayer; // 0x55: player number resolved through the founder task
    s8 m_aiPriority;  // 0x56: Specialized_Param_Int_AI_Pri (1 for non-pickups)
    u8 unk57;
    u32 m_updates;        // 0x58: updates since this record was allocated
    int m_constraintParentId; // 0x5C: link index 3's parent task, or -1
    u32 m_itemTrait;  // 0x60: Work_Param_Int_Trait, zero for non-items

    aiWeapon() {
        m_flags = 0;
    }
    int getWnKind();
    int getItKind();
    // HYPOTHESIS: source parameter order/types; register use and the registry
    // caller establish type, position, extent, team and incoming flag bits.
    void update(int type, Vec3f* pos, float radius, s8 team, u16 flags);
    // Returns predicted frames until contact, or -1. The core mutates its
    // position argument; the public entry point makes a private copy first.
    int predict(Vec3f* pos, float radius, int frames);
    // MATCH-ONLY: the public wrapper calls the native core out of line.
    int predict_core(Vec3f* pos, float radius, int frames) __attribute__((never_inline));
    AiClBlock* getFloorBlk();
};
static_assert(sizeof(aiWeapon) == 0x64, "Class is wrong size!");

// HYPOTHESIS: descriptive record name; mark_wpn_update verifies the fields and
// 16-byte stride. The last word is not touched by that routine.
struct aiWeaponMark {
    u32 m_objId;
    int m_frames;
    aiWeapon* m_weapon;
    u32 unkC;
};
static_assert(sizeof(aiWeaponMark) == 0x10, "Class is wrong size!");

class aiWeaponMgr {
public:
    aiWeapon m_weapons[64];  // 0x0000
    u8 m_searchedBits[8];    // 0x1900: weapons found by search() that enum_searched has not returned yet
    int m_searchedCount;     // 0x1908
    int m_enumCount;         // 0x190C
    int m_enumCursor;        // 0x1910
    int m_count;             // 0x1914: number of active weapons
    int m_previousCount;    // 0x1918: active count at the start of this refresh
    // Set when update_core finds an eligible pickup with positive AI priority.
    bool m_hasPickupItem;   // 0x191C
    char unk191D[3];

    aiWeaponMgr();
    ~aiWeaponMgr();
    void reset();
    void update();
    // MATCH-ONLY: preserve the native out-of-line classifier call boundary.
    void update_core(StageObject* object, s8 type, int kind, u8* stale,
                     u32 objId, int constraintParentId) __attribute__((never_inline));
    void update_core2(Vec3f* pos, float radius, s8 team, s8 ownerPlayer,
                      s8 type, s8 aiPriority, int kind, u8* stale, u32 objId,
                      u32 flags, int constraintParentId, u32 itemFlags);
    // Each search replaces the pending result bitset and resets enumeration.
    // mode == 0 excludes types 4/5 and the selected team; nonzero mode selects
    // type 4 with flag 8. The gameplay name of that filter is not yet known.
    // MATCH-ONLY: mark_wpn_update calls this search out of line.
    int search(Vec3f* pos, float radius, int mode, s8 excludedTeam) __attribute__((never_inline));
    int search1(s8 ownerPlayer, int wnKind);
    int search_gimmic(Vec3f* pos, float radius);
    int search_item(int itKind);
    void mark_wpn_update(aiWeaponMark* marks, int count, u8* seen, int frames,
                         Vec3f* pos, float radius, s8 excludedTeam);
    // MATCH-ONLY: native callers consume the result set through this entry point.
    aiWeapon* enum_searched() __attribute__((never_inline));
    aiWeapon* near_searched(Vec2f* pos);
    aiWeapon* getWeapon(u32 objId);
};
static_assert(sizeof(aiWeaponMgr) == 0x1920, "Class is wrong size!");
