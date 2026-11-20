#define SO_COLLISION_CTOR_DTOR
#include <so/collision/so_collision_group_impl.h>
#include <cl/cl_collision.h>
#include <types.h>

// TODO: clean up: the clManager instance still has its placeholder name (st_loader_manager.cpp refers to it by it).
extern clManager* lbl_27_bss_8;

static inline clManager* getClManager() {
    if (lbl_27_bss_8 == NULL) {
        lbl_27_bss_8 = new (Heaps::System) clManager;
    }
    return lbl_27_bss_8;
}

// The log entry as this group sees it: an entry of a referenced log group lives in the manager.
static inline soCollisionLog* getLogEntry(soCollisionGroupImpl& group, soCollisionLog* log) {
    if (group.isRefLogGroup() == true) {
        return fn_27_5012C(getCollisionManager(), (u16)log->m_30); // TODO: clean up: m_30 is the entry's index in the manager
    }
    return log;
}

soCollision::soCollision(int taskId, gfTask::Category taskCategory, int kind, int unk4,
                         soArray<soCollisionGroup>* collisionGroupArray, bool unk6) {
    m_kind = unk4;
    m_indirectTeam = kind;
    m_collisionGroupArray = collisionGroupArray;
    m_taskId = taskId;
    ((u8*)this)[0x10] = taskCategory; // TODO: clean up: the category is stored as a byte, not through the bitfield
    m_isNoTeam = false;
    _20[0] = unk6; // TODO: clean up: name the flag at 0x14
}

// Returns every primitive to the clManager and empties the logs (inlined into the destructor and clean).
static inline void cleanCollision(soCollision* collision) {
    for (int i = 0; i < collision->m_collisionGroupArray->size(); i++) {
        nw4r::ut::List* list = &getGroup(collision->m_collisionGroupArray, i).m_collisionList;
        while (list->numObjects != 0) {
            newCollision* primitive = (newCollision*)nw4r::ut::List_GetNext(list, NULL);
            nw4r::ut::List_Remove(list, primitive);
            getClManager()->freeCollision(primitive->getType(), primitive);
        }
    }
    for (int i = 0; i < collision->m_collisionGroupArray->size(); i++) {
        getGroup(collision->m_collisionGroupArray, i).initLog();
    }
    for (int i = 0; i < collision->m_collisionGroupArray->size(); i++) {
        soCollisionGroupImpl& group = getGroup(collision->m_collisionGroupArray, i);
        group.removeLog(group.m_refPastLogList, true, false);
        group.removeLog(group.m_refCheckList, true, false);
    }
}

soCollision::~soCollision() {
    cleanCollision(this);
}

void soCollision::clean() {
    cleanCollision(this);
}

newCollision* soCollision::getCollision(u32 index, u32 groupIndex) {
    return (newCollision*)nw4r::ut::List_GetNth(&getGroup(m_collisionGroupArray, groupIndex).m_collisionList, index);
}

// Clears the hit flag of every collision primitive of the group.
// Makes the group's primitive at index the given type (true when it had to be replaced); missing primitives up to the
// index are appended.
bool soCollision::allocateCollision(u32 index, bool type, u32 groupIndex) {
    nw4r::ut::List* list = &getGroup(m_collisionGroupArray, groupIndex).m_collisionList;
    int count = list->numObjects;
    if ((int)index < count) {
        int currentType = ((newCollision*)nw4r::ut::List_GetNth(&getGroup(m_collisionGroupArray, groupIndex).m_collisionList, index))
                              ->getType();
        if (currentType == type) {
            return false;
        }
        void* next = nw4r::ut::List_GetNext(list, nw4r::ut::List_GetNth(list, index));
        newCollision* collision =
            (newCollision*)nw4r::ut::List_GetNth(&getGroup(m_collisionGroupArray, groupIndex).m_collisionList, index);
        nw4r::ut::List_Remove(&getGroup(m_collisionGroupArray, groupIndex).m_collisionList, collision);
        getClManager()->freeCollision(currentType, collision);
        nw4r::ut::List_Insert(list, next, getClManager()->allocateCollision(type));
        return true;
    }
    for (index++; count < (int)index; count++) {
        nw4r::ut::List_Append(list, getClManager()->allocateCollision(type));
    }
    return false;
}

void soCollision::initCheck(u32 groupIndex) {
    nw4r::ut::List* list = &getGroup(m_collisionGroupArray, groupIndex).m_collisionList;
    for (newCollision* collision = NULL; (collision = (newCollision*)nw4r::ut::List_GetNext(list, collision)) != NULL;) {
        collision->m_isActive = false;
    }
}

void soCollision::initLog(u32 groupIndex) {
    getGroup(m_collisionGroupArray, groupIndex).initLog();
}

// Moves this frame's log entries into the past log (absolute hits are dropped) and, unless stopped, ages the past log: an
// entry whose remaining frames (0x1C) run out is returned to the collision manager.
// TODO: clean up: soCollisionLog reads its frame count signed (m_life); this reads it unsigned.
void soCollision::updatePastLog(bool isStop, u32 groupIndex) {
    soCollisionGroupImpl& group = getGroup(m_collisionGroupArray, groupIndex);
    nw4r::ut::List* logList = group.m_refLogList;
    nw4r::ut::List* pastLogList = group.m_refPastLogList;
    soCollisionLog* log = (soCollisionLog*)nw4r::ut::List_GetNext(logList, NULL);
    while (log != NULL) {
        soCollisionLog* next = (soCollisionLog*)nw4r::ut::List_GetNext(logList, log);
        if ((u8)log->_38 == group.m_logGroupKey) {
            nw4r::ut::List_Remove(logList, log);
            if (!log->m_isAbsolute) {
                nw4r::ut::List_Append(pastLogList, log);
            }
        }
        log = next;
    }
    if (group.isRefLogGroup() == true) {
        group.removeLog(&group.m_logList, true, false);
    }
    if (!isStop) {
        soCollisionLog* past = (soCollisionLog*)nw4r::ut::List_GetNext(pastLogList, NULL);
        while (past != NULL) {
            soCollisionLog* next = (soCollisionLog*)nw4r::ut::List_GetNext(pastLogList, past);
            if ((u8)past->_38 == group.m_logGroupKey) {
                u16& frames = *(u16*)&past->m_life;
                if (frames != 0 && --frames == 0) {
                    nw4r::ut::List_Remove(pastLogList, past);
                    fn_27_50108(getCollisionManager(), past);
                }
            }
            past = next;
        }
    }
}

void soCollision::initLogAll() {
    for (int i = 0; i < m_collisionGroupArray->size(); i++) {
        getGroup(m_collisionGroupArray, i).initLog();
    }
}

void soCollision::initPastLog(u32 groupIndex, bool keep) {
    soCollisionGroupImpl& group = getGroup(m_collisionGroupArray, groupIndex);
    group.removeLog(group.m_refPastLogList, true, keep);
    group.removeLog(group.m_refCheckList, true, keep);
}

void soCollision::initPastLogAll(u32 keep) {
    for (int i = 0; i < m_collisionGroupArray->size(); i++) {
        soCollisionGroupImpl& group = getGroup(m_collisionGroupArray, i);
        group.removeLog(group.m_refPastLogList, true, keep);
        group.removeLog(group.m_refCheckList, true, keep);
    }
}

void soCollision::invalidLog(soCollisionLog* collisionLog, u32 groupIndex) {
    if (getGroup(m_collisionGroupArray, groupIndex).isRefLogGroup() == true) {
        collisionLog = fn_27_5012C(getCollisionManager(), (u16)collisionLog->m_30);
    }
    nw4r::ut::List_Remove(getGroup(m_collisionGroupArray, groupIndex).m_refLogList, collisionLog);
    fn_27_50108(getCollisionManager(), collisionLog);
}

// HYPOTHESIS: names (the map leaves both unnamed); they write the entry as the group sees it.
// TODO: clean up: declare these as soCollision members in so/collision/so_collision.h.
void soCollisionResetLogTaskId(soCollision* collision, soCollisionLog* collisionLog, u32 groupIndex) {
    soCollisionLog* log = getLogEntry(getGroup(collision->m_collisionGroupArray, groupIndex), collisionLog);
    log->m_taskId = -1;
}

void soCollisionSetLogLife(soCollision* collision, soCollisionLog* collisionLog, s16 life, u32 groupIndex) {
    soCollisionLog* log = getLogEntry(getGroup(collision->m_collisionGroupArray, groupIndex), collisionLog);
    log->m_life = life;
}

// TODO: clean up: still unnamed manager helper (allocates a log entry).
extern "C" soCollisionLog* fn_27_500B8(soCollisionManager* manager);

// HYPOTHESIS: the field at 0x4 is the team accessor of the owner (slot 0x10 gives the team number), not an int.
class soCollisionTeamView {
public:
    virtual void v8();
    virtual void vC();
    virtual s8 getTeamNo();
};

// Records a hit by another collision's group into this group's past log.
// NONMATCHING: register assignment of the values read from the other collision.
void soCollision::addPastLog(soCollision* collision, u32 index, u32 groupIndex) {
    soCollisionLog* log = fn_27_500B8(getCollisionManager());
    u8 situation = getGroup(collision->m_collisionGroupArray, index).m_situation;
    Vec3f zero(0.0f, 0.0f, 0.0f);
    int kind = collision->m_kind;
    s8 teamNo = (*(soCollisionTeamView**)&collision->m_indirectTeam)->getTeamNo();
    u8 taskCategory = ((u8*)collision)[0x10];
    int taskId = collision->m_taskId;
    *(int*)&log->_spacer[8] = getGroup(collision->m_collisionGroupArray, index).m_id; // TODO: clean up: name the group id field
    log->m_taskId = taskId;
    log->m_taskCategory = taskCategory;
    log->m_teamNo = teamNo;
    log->m_kind = kind;
    log->_35 = 0;
    log->m_life = 0;
    log->m_collsionIndex = 0;
    log->m_damageIndex = 0;
    log->m_30 = 0;
    log->_38 = 0;
    log->m_pos = zero;
    log->m_isAbsolute = false;
    log->_40 = 1;
    log->_41 = false;
    log->_42 = situation;
    nw4r::ut::List_Append(getGroup(m_collisionGroupArray, groupIndex).m_refPastLogList, log);
}

void* soCollision::getLogList(u32 groupIndex) {
    return &getGroup(m_collisionGroupArray, groupIndex).m_logList;
}

void soCollision::setPosX(float posX, u32 groupIndex) {
    getGroup(m_collisionGroupArray, groupIndex).m_posX = posX;
}

float soCollision::getPosX(u32 groupIndex) {
    return getGroup(m_collisionGroupArray, groupIndex).m_posX;
}

// The collision situation of each situation kind (0 ground, 7 the last kind, everything else air).
// HYPOTHESIS: indexed by SituationKind.
static soCollision::Situation s_situationTable[8] = {
    soCollision::Situation_Ground, soCollision::Situation_Air, soCollision::Situation_Air, soCollision::Situation_Air,
    soCollision::Situation_Air,    soCollision::Situation_Air, soCollision::Situation_Air, soCollision::Situation_ODD,
};

soCollision::Situation soCollision::getSituation(int situation) {
    return s_situationTable[situation];
}

void soCollision::updateGroup(float posX, float posY, u32 groupIndex) {
    soCollisionGroupImpl& group = getGroup(m_collisionGroupArray, groupIndex);
    group.m_posX = posX;
    group.m_posY = posY;
}

// HYPOTHESIS: the same with the situation the group's hits count for.
void soCollisionUpdateGroupSituation(soCollision* collision, float posX, float posY, u32 groupIndex, u8 situation) {
    soCollisionGroupImpl& group = getGroup(collision->m_collisionGroupArray, groupIndex);
    group.m_posX = posX;
    group.m_posY = posY;
    getGroup(collision->m_collisionGroupArray, groupIndex).m_situation = situation;
}

void soCollisionSetGroupUnk50(soCollision* collision, float value, u32 groupIndex) {
    getGroup(collision->m_collisionGroupArray, groupIndex).m_unk50 = value;
}

void soCollision::initAbsolute(u32 groupIndex) {
    nw4r::ut::List_Init(&getGroup(m_collisionGroupArray, groupIndex).m_absoluteList, 0x10);
    _17[1] = false; // TODO: clean up: name the "has absolute" flag at 0x12
}

// HYPOTHESIS: name; appends an absolute (always hitting) entry to the group.
void soCollisionAddAbsolute(soCollision* collision, u32 groupIndex, void* absolute) {
    nw4r::ut::List_Append(&getGroup(collision->m_collisionGroupArray, groupIndex).m_absoluteList, absolute);
    collision->_17[1] = true;
}

void soCollision::refLogGroup(u32 groupIndex, u32 logGroupIndex) {
    getGroup(m_collisionGroupArray, groupIndex).refLogGroup(logGroupIndex);
}

void soCollision::unRefLogGroup(u32 groupIndex) {
    getGroup(m_collisionGroupArray, groupIndex).initLog();
    getGroup(m_collisionGroupArray, groupIndex).unRefLogGroup();
}

bool soCollision::isRefLogGroup(u32 groupIndex) {
    return getGroup(m_collisionGroupArray, groupIndex).isRefLogGroup();
}

void soCollision::useLogGroup(u32 groupIndex) {
    getGroup(m_collisionGroupArray, groupIndex).useLogGroup();
}

void soCollision::unUseLogGroup(u32 groupIndex) {
    getGroup(m_collisionGroupArray, groupIndex).unUseLogGroup();
}

bool soCollision::isUseLogGroup(u32 groupIndex) {
    return getGroup(m_collisionGroupArray, groupIndex).isUseLogGroup();
}

// HYPOTHESIS: name. Whether the other collision's group already logged a hit by this collision's group (this frame or in
// its past log); a past entry of the other side's own kind that was taken in the air (situation 1) while this group is on
// the ground is dropped instead.
// TODO: clean up: name the log fields used here (0x8 the group id, 0x2A the situation).
bool soCollisionIsLogged(soCollision* collision, u32 index, soCollision* other, u32 otherGroupIndex) {
    u32 groupId = getGroup(collision->m_collisionGroupArray, index).m_id;
    nw4r::ut::List* logList = getGroup(other->m_collisionGroupArray, otherGroupIndex).m_refLogList;
    soCollisionLog* log = NULL;
    while ((log = (soCollisionLog*)nw4r::ut::List_GetNext(logList, log)) != NULL) {
        if (*(u32*)&log->_spacer[8] == groupId ||
            ((u32)collision->m_taskId == (u32)log->m_taskId && collision->m_kind != log->m_kind)) {
            return true;
        }
    }
    u8 situation = getGroup(collision->m_collisionGroupArray, index).m_situation;
    nw4r::ut::List* pastLogList = getGroup(other->m_collisionGroupArray, otherGroupIndex).m_refPastLogList;
    soCollisionLog* past = (soCollisionLog*)nw4r::ut::List_GetNext(pastLogList, NULL);
    while (past != NULL) {
        soCollisionLog* next = (soCollisionLog*)nw4r::ut::List_GetNext(pastLogList, past);
        if (*(u32*)&past->_spacer[8] == groupId ||
            ((u32)collision->m_taskId == (u32)past->m_taskId && collision->m_kind != past->m_kind)) {
            if (past->_42 == 1 && situation == 0 && collision->m_kind == past->m_kind) {
                nw4r::ut::List_Remove(pastLogList, past);
                fn_27_50108(getCollisionManager(), past);
            } else {
                return true;
            }
        }
        past = next;
    }
    return false;
}
