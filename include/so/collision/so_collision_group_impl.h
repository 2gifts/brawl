#pragma once

// Local layout of soCollisionGroup shared by so_collision.cpp and so_collision_group.cpp (the two halves of the original
// collision units): so/collision/so_collision_group.h keeps the opaque size that the array units need.

#include <so/collision/so_collision.h>
#include <so/collision/so_collision_group.h>
#include <so/collision/so_collision_manager.h>
#include <so/collision/so_collision_log.h>
#include <nw4r/ut/ut_list.h>
#include <types.h>

// One collision group of a soCollision: the collision primitives of the group, the log of the hits it took this frame
// (its own list or, while it references a shared log group of the collision manager, that group's lists) and the past
// logs.
// HYPOTHESIS: field names; the lists' roles follow the soCollision methods that walk them. so/collision/so_collision_group.h
// only gives the size, so the layout and the group's (unnamed) methods live in this view of the class.
// TODO: clean up: move the layout and these methods into soCollisionGroup (its array units need the opaque copies).
class soCollisionGroupImpl {
public:
    nw4r::ut::List m_pastLogList;     // 0x00
    nw4r::ut::List m_checkList;       // 0x0C
    int m_id;                         // 0x18: unique id, renewed whenever the log group reference is dropped
    nw4r::ut::List m_collisionList;   // 0x1C: newCollision (link at 0x28)
    nw4r::ut::List m_logList;         // 0x28: the group's own log
    nw4r::ut::List* m_refLogList;     // 0x34: m_logList, or the referenced log group's log
    nw4r::ut::List* m_refPastLogList; // 0x38
    nw4r::ut::List* m_refCheckList;   // 0x3C
    nw4r::ut::List m_absoluteList;    // 0x40 (link at 0x10)
    float m_posX;                     // 0x4C
    float m_unk50;                    // 0x50
    float m_posY;                     // 0x54
    float m_bbox[6];                  // 0x58: bounding box of the active primitives (min xyz, max xyz)
    s16 m_logGroup;                   // 0x70
    u8 m_logGroupKey;                 // 0x72
    u8 unk73;
    u8 m_situation;                   // 0x74

    void removeLog(nw4r::ut::List* list, bool free, u32 all);
    void addLog(soCollisionLog* log, bool check);
    void refLogGroup(int logGroup);
    void unRefLogGroup();
    bool isRefLogGroup();
    void useLogGroup();
    void unUseLogGroup();
    bool isUseLogGroup();

    // Empties this frame's log (inlined into the soCollision methods).
    void initLog() {
        removeLog(m_refLogList, true, false);
        if (isRefLogGroup() == true) {
            removeLog(&m_logList, true, false);
        }
    }
};

// The lazily created collision manager (the global, not the class's static member that soCollisionManager::get uses).
// TODO: clean up: one singleton accessor in so/collision/so_collision_manager.h.
inline soCollisionManager* getCollisionManager() {
    if (!g_soCollisionManager) {
        g_soCollisionManager = new (Heaps::System) soCollisionManager;
    }
    return g_soCollisionManager;
}

// A shared log group of the collision manager (0x34 bytes; the manager keeps an array of them at +0xA4).
// HYPOTHESIS: layout from the reference and use counting below.
struct soCollisionLogGroup {
    nw4r::ut::List m_logList;     // 0x00
    nw4r::ut::List m_pastLogList; // 0x0C
    nw4r::ut::List m_checkList;   // 0x18
    int m_groupId;                // 0x24: the group id of the first referencer
    u32 m_index : 8;              // 0x28
    u32 m_refCount : 8;
    u32 m_useCount : 15;
    u32 m_isLeaveFree : 1;        // release the logs when the last reference goes
    u8 unk2C[8];
};


// TODO: clean up: raw offset of the log group array until soCollisionManager declares it.
inline soCollisionLogGroup* getLogGroups() {
    return *(soCollisionLogGroup**)((u8*)getCollisionManager() + 0xA4);
}

inline soCollisionGroupImpl& getGroup(soArray<soCollisionGroup>* groups, u32 index) {
    return (soCollisionGroupImpl&)groups->at(index);
}

// soCollisionManager log helpers (so_collision_manager.cpp).
// HYPOTHESIS: names; the map leaves them unnamed.
// TODO: clean up: make these soCollisionManager members (so/collision/so_collision_manager.h).
void soCollisionManagerAddCheckList(soCollisionManager* manager, soCollision* collision, int kind);
void soCollisionManagerClearCheckList(soCollisionManager* manager);
soCollisionLog* soCollisionManagerAllocateLog(soCollisionManager* manager);
void soCollisionManagerFreeLog(soCollisionManager* manager, void* log);
u32 soCollisionManagerGetLogIndex(soCollisionManager* manager, soCollisionLog* log);
soCollisionLog* soCollisionManagerGetLog(soCollisionManager* manager, int index);
u8 soCollisionManagerRefLogGroup(soCollisionManager* manager, int logGroup, int* groupId);
void soCollisionManagerReleaseLogGroup(soCollisionManager* manager, int logGroup);
void soCollisionManagerUnRefLogGroup(soCollisionManager* manager, int logGroup);
