#pragma once

#include <mt/mt_vector.h>
#include <types.h>

// A weapon/item/gimmick the CPU AI tracks as a thing to dodge or pick up.
// Partial layout verified through the search filters and tracking updates.
class aiWeapon {
public:
    u32 m_objId;       // 0x00: object id
    int m_kind;        // 0x04: weapon (wn) or item (it) kind, depending on m_type
    u16 m_flags;       // 0x08: 1 = active, 4 = m_floorBlk is valid
    s8 m_type;         // 0x0A: 0 = weapon, 1/4/5 = item
    u8 unkB;
    int m_floorBlk;    // 0x0C: floor block under the object
    char unk10[0x18];
    Vec3f m_pos;       // 0x28
    char unk34[0x20];
    s8 m_team;        // 0x54: search() can exclude this team
    s8 m_ownerPlayer; // 0x55: player number resolved through the founder task
    char unk56[0xE];

    aiWeapon() {
        m_flags = 0;
    }
    int getWnKind();
    int getItKind();
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
    char unk1918[8];

    aiWeaponMgr();
    ~aiWeaponMgr();
    void reset();
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
