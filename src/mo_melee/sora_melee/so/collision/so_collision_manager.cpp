#include <so/collision/so_collision_group_impl.h>
#include <types.h>

// The collision manager's log storage: a pool of soCollisionLog entries (a free list at 0x98, the entries at 0x94) and
// the shared log groups (an array at 0xA4 with a free list at 0xA8); the check lists of the collision kinds start at 0x40.
// HYPOTHESIS: the helper names (the map leaves them unnamed) and the field offsets, which soCollisionManager does not
// declare yet.
// TODO: clean up: declare the fields and these helpers in so/collision/so_collision_manager.h.
struct soCollisionManagerLogView {
    u8 unk0[0x40];
    nw4r::ut::List m_checkLists[7];   // 0x40
    soCollisionLog* m_logs;           // 0x94
    nw4r::ut::List m_freeLogList;     // 0x98
    soCollisionLogGroup* m_logGroups; // 0xA4
    nw4r::ut::List m_freeLogGroupList; // 0xA8
};

static inline soCollisionManagerLogView* view(soCollisionManager* manager) {
    return (soCollisionManagerLogView*)manager;
}

void soCollisionManagerAddCheckList(soCollisionManager* manager, soCollision* collision, int kind) {
    nw4r::ut::List_Append(&view(manager)->m_checkLists[kind], collision);
}

void soCollisionManagerClearCheckList(soCollisionManager* manager) {
    int i = 0;
    nw4r::ut::List* list = view(manager)->m_checkLists;
    for (; i < 7; i++, list++) {
        nw4r::ut::List_Init(list, 0x18);
    }
}

soCollisionLog* soCollisionManagerAllocateLog(soCollisionManager* manager) {
    soCollisionLog* log = (soCollisionLog*)nw4r::ut::List_GetPrev(&view(manager)->m_freeLogList, NULL);
    nw4r::ut::List_Remove(&view(manager)->m_freeLogList, log);
    return log;
}

void soCollisionManagerFreeLog(soCollisionManager* manager, void* log) {
    nw4r::ut::List_Append(&view(manager)->m_freeLogList, log);
}

u32 soCollisionManagerGetLogIndex(soCollisionManager* manager, soCollisionLog* log) {
    return (u32)((u8*)log - (u8*)view(manager)->m_logs) / sizeof(soCollisionLog);
}

soCollisionLog* soCollisionManagerGetLog(soCollisionManager* manager, int index) {
    return &view(manager)->m_logs[index];
}

int soCollisionManager::allocateLogGroup() {
    soCollisionLogGroup* group = (soCollisionLogGroup*)nw4r::ut::List_GetPrev(&view(this)->m_freeLogGroupList, NULL);
    nw4r::ut::List_Remove(&view(this)->m_freeLogGroupList, group);
    return group->m_index;
}

void soCollisionManager::freeLogGroup(int logGroup) {
    nw4r::ut::List_Append(&view(this)->m_freeLogGroupList, &view(this)->m_logGroups[logGroup]);
}

u8 soCollisionManagerRefLogGroup(soCollisionManager* manager, int logGroup, int* groupId) {
    soCollisionLogGroup* group = &view(manager)->m_logGroups[logGroup];
    u8 key = group->m_refCount;
    if (key == 0) {
        group->m_groupId = *groupId;
        group->m_useCount = 0;
    } else {
        *groupId = group->m_groupId;
    }
    group->m_refCount = group->m_refCount + 1;
    return key;
}

#pragma opt_loop_invariants off
void soCollisionManagerReleaseLogGroup(soCollisionManager* manager, int logGroup) {
    soCollisionLogGroup* group = &view(manager)->m_logGroups[logGroup];
    while (group->m_logList.numObjects != 0) {
        void* log = nw4r::ut::List_GetNext(&group->m_logList, NULL);
        nw4r::ut::List_Remove(&group->m_logList, log);
        soCollisionManagerFreeLog(manager, log);
    }
    while (group->m_pastLogList.numObjects != 0) {
        void* log = nw4r::ut::List_GetNext(&group->m_pastLogList, NULL);
        nw4r::ut::List_Remove(&group->m_pastLogList, log);
        soCollisionManagerFreeLog(manager, log);
    }
    while (group->m_checkList.numObjects != 0) {
        void* log = nw4r::ut::List_GetNext(&group->m_checkList, NULL);
        nw4r::ut::List_Remove(&group->m_checkList, log);
        soCollisionManagerFreeLog(manager, log);
    }
    group->m_isLeaveFree = false;
}
#pragma opt_loop_invariants reset

void soCollisionManagerUnRefLogGroup(soCollisionManager* manager, int logGroup) {
    view(manager)->m_logGroups[logGroup].m_refCount = view(manager)->m_logGroups[logGroup].m_refCount - 1;
    if (view(manager)->m_logGroups[logGroup].m_refCount == 0 && view(manager)->m_logGroups[logGroup].m_isLeaveFree) {
        soCollisionManagerReleaseLogGroup(manager, logGroup);
    }
}

void soCollisionManager::leaveFreeRefLogGroup(int logGroup) {
    soCollisionLogGroup* group = &view(this)->m_logGroups[logGroup];
    if (group->m_refCount == 0) {
        soCollisionManagerReleaseLogGroup(this, logGroup);
    } else {
        group->m_isLeaveFree = true;
    }
}
