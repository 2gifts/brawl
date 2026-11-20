#include <so/collision/so_collision_group_impl.h>
#include <types.h>

// TODO: clean up: the group id counter still has its placeholder name.
extern int lbl_27_bss_B0;

soCollisionGroup::soCollisionGroup() {
    soCollisionGroupImpl* group = (soCollisionGroupImpl*)this;
    group->m_refLogList = &group->m_logList;
    group->m_refPastLogList = &group->m_pastLogList;
    group->m_refCheckList = &group->m_checkList;
    group->m_posX = 0.0f;
    group->m_unk50 = 0.0f;
    group->m_posY = 0.0f;
    group->m_logGroup = -1;
    group->m_logGroupKey = 0;
    group->unk73 = 0;
    group->m_situation = 0;
    nw4r::ut::List_Init(&group->m_collisionList, 0x28);
    nw4r::ut::List_Init(group->m_refLogList, 0);
    nw4r::ut::List_Init(group->m_refPastLogList, 0);
    nw4r::ut::List_Init(group->m_refCheckList, 0);
    nw4r::ut::List_Init(&group->m_absoluteList, 0x10);
    group->m_id = lbl_27_bss_B0++;
}

soCollisionGroup::~soCollisionGroup() { }

// Removes the entries of a log list (all of them, or only those of this group's log group key) and optionally returns them
// to the collision manager.
void soCollisionGroupImpl::removeLog(nw4r::ut::List* list, bool free, bool all) {
    // TODO: clean up: soCollisionLog::_38 is the log group key of the entry.
    soCollisionLog* log = (soCollisionLog*)nw4r::ut::List_GetNext(list, NULL);
    if (!free) {
        while (log != NULL) {
            soCollisionLog* next = (soCollisionLog*)nw4r::ut::List_GetNext(list, log);
            if (all == true || (u8)log->_38 == m_logGroupKey) {
                nw4r::ut::List_Remove(list, log);
            }
            log = next;
        }
    } else {
        while (log != NULL) {
            soCollisionLog* next = (soCollisionLog*)nw4r::ut::List_GetNext(list, log);
            if (all == true || (u8)log->_38 == m_logGroupKey) {
                nw4r::ut::List_Remove(list, log);
                fn_27_50108(getCollisionManager(), log);
            }
            log = next;
        }
    }
}

// Makes the group log into a shared log group of the collision manager.
void soCollisionGroupImpl::refLogGroup(int logGroup) {
    removeLog(&m_logList, true, false);
    removeLog(&m_pastLogList, true, false);
    removeLog(&m_checkList, true, false);
    m_logGroupKey = fn_27_501A4(getCollisionManager(), logGroup, &m_id);
    soCollisionLogGroup* group = &getLogGroups()[logGroup];
    m_logGroup = logGroup;
    m_refLogList = &group->m_logList;
    m_refPastLogList = &group->m_pastLogList;
    m_refCheckList = &group->m_checkList;
}

void soCollisionGroupImpl::unRefLogGroup() {
    fn_27_502EC(getCollisionManager(), m_logGroup);
    m_refLogList = &m_logList;
    m_refPastLogList = &m_pastLogList;
    m_refCheckList = &m_checkList;
    m_id = lbl_27_bss_B0++;
    m_logGroup = -1;
    m_logGroupKey = 0;
}

bool soCollisionGroupImpl::isRefLogGroup() {
    return m_refLogList != &m_logList;
}

void soCollisionGroupImpl::useLogGroup() {
    soCollisionLogGroup* logGroup = &getLogGroups()[m_logGroup];
    logGroup->m_useCount = logGroup->m_useCount + 1;
}

void soCollisionGroupImpl::unUseLogGroup() {
    soCollisionLogGroup* logGroup = &getLogGroups()[m_logGroup];
    logGroup->m_useCount = logGroup->m_useCount - 1;
}

bool soCollisionGroupImpl::isUseLogGroup() {
    return getLogGroups()[m_logGroup].m_useCount != 0;
}
