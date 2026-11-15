#include <ft/ft_entry.h>
#include <ft/ft_entry_manager.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_slot.h>
#include <ft/ft_slot_manager.h>
#include <ft/fighter.h>
#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <types.h>

// TODO: the larger ftEntry and ftEntryManager members.

extern "C" {
// HYPOTHESIS: ftEntryManager::setSwap (unnamed in sora_melee).
void fn_27_11967C(ftEntryManager* manager, int unk, int entryId1, int entryId2, bool unk2, int unk3);
// HYPOTHESIS: ftInput::setContNo (unnamed in sora_melee).
void fn_27_124830(ftInput* input, int contNo);
// HYPOTHESIS: ftInput::standbyAdvFollow (unnamed in sora_melee).
void fn_27_1241D8(ftInput* input, int unk);
// HYPOTHESIS: Fighter::setZoom (unnamed in sora_melee).
void fn_27_13231C(Fighter* fighter, float unk1, int unk2, int unk3);
// HYPOTHESIS: ftEntry::process (unnamed in sora_melee).
void fn_27_1186D4(ftEntry* entry);
// HYPOTHESIS: the index of the area instance `areaId` of an object (-1 if it has none).
int fn_27_8CC68(StageObject* object, int areaId);
// HYPOTHESIS: Fighter::addDamage (unnamed in sora_melee).
void fn_27_13C1F0(Fighter* fighter, float damage);
}

// The head word of gfTaskScheduler (its process type is private there).
struct ftEntrySchedulerView {
    s32 unk0 : 8;
    s32 m_processType : 8;
};

static inline ftEntry* getEntry(int entryId) {
    return g_ftEntryManager->getEntity(entryId);
}

static inline Fighter* getActiveFighter(ftEntry* entry) {
    return entry->m_instances[(u8)entry->m_activeInstanceIndex].m_fighter;
}

void ftEntry::standby(int unk) {
    for (int i = 0; i < m_entryCount; i++) {
        Fighter* fighter = m_instances[i].m_fighter;
        if (fighter != NULL) {
            fighter->standby(unk);
        }
    }
    fn_27_1241D8(m_input, 0);
    unkF = 1;
    unkC = 0;
}

// The sub fighter (instance 1) starts at the owner's start position with the owner's damage.
void ftEntry::startSubFighter() {
    if (_0xe & 2) {
        Vec3f pos = m_owner->getStartPos();
        float lr = m_owner->getStartLr();
        Fighter* sub = m_instances[1].m_fighter;
        sub->start(&pos, lr, m_owner->getDamage(), 1, g_ftManager->m_mode == 1);
    }
}

void ftEntry::standbyAdvFollow() {
    for (int i = 0; i < m_entryCount; i++) {
        Fighter* fighter = m_instances[i].m_fighter;
        if (fighter != NULL) {
            fighter->standbyAdvFollow();
        }
    }
    fn_27_1241D8(m_input, 0);
}

// HYPOTHESIS: the word at +0x28 of the fighter is its task id.
int ftEntry::isExistFighter(int kind) {
    for (int i = 0; (u8)i < m_entryCount; i++) {
        Fighter* fighter = m_instances[(u8)i].m_fighter;
        if (fighter != NULL && (u32)kind == *reinterpret_cast<u32*>(reinterpret_cast<u8*>(fighter) + 0x28)) {
            return i;
        }
    }
    return 0xFF;
}

// The character kind of the active instance: the forms of the characters that change (HYPOTHESIS: the trainer's Pokemon,
// Zelda/Sheik, Samus/Zero Suit) report the form's own kind.
int ftEntry::getCurrentInstanceGmKind() {
    if (_0xe & 1) {
        switch (m_instances[(u8)m_activeInstanceIndex].m_kind) {
        case 0x1d:
            return 0x1d;
        case 0x1e:
            return 0x1f;
        case 0x1f:
            return 0x21;
        }
    } else {
        switch (m_characterKind) {
        case 0xe:
        case 0xf:
            switch (m_instances[(u8)m_activeInstanceIndex].m_kind) {
            case 0xd:
                return 0xe;
            case 0xe:
                return 0xf;
            }
            break;
        case 3:
        case 4:
            switch (m_instances[(u8)m_activeInstanceIndex].m_kind) {
            case 3:
                return 3;
            case 0x18:
                return 4;
            }
            break;
        }
    }
    return m_characterKind;
}

void ftEntry::entryEnd() {
    for (u8 i = 0; i < m_entryCount; i++) {
        g_ftSlotManager->m_slots[m_slotIndex].remove(i, 0x2000, 0);
    }
}

void ftEntry::resultEnd() {
    for (u8 i = 0; i < m_entryCount; i++) {
        g_ftSlotManager->m_slots[m_slotIndex].remove(i, 0x4000, 0);
    }
}

void ftEntry::exitFinal(int unk) {
    if ((u32)unk == 1) {
        g_ftSlotManager->m_slots[m_slotIndex].remove(0xFF, 0x1000, 0);
        _0x10 = 0;
    } else {
        _0x10 = 2;
    }
}

int ftEntry::getDragoonCount(bool partner) {
    if (partner == true && m_heartSwapEntryId != -1) {
        return getEntry(m_heartSwapEntryId)->m_owner->getDragoonCount();
    }
    return m_owner->getDragoonCount();
}

int ftEntry::getDragoonVariation(int index, bool partner) {
    if (partner == true && m_heartSwapEntryId != -1) {
        return getEntry(m_heartSwapEntryId)->m_owner->getDragoonVariation(index);
    }
    return m_owner->getDragoonVariation(index);
}

void ftEntry::toKnockOut(soDamageAttackerInfo* attackerInfo) {
    if (unkF == 6) {
        getActiveFighter(this)->toKnockOut(attackerInfo);
    }
}

void ftEntry::setContNo(int contNo) {
    if (m_heartSwapEntryId != -1 && g_ftEntryManager->m_isProcessHeartSwap == true) {
        ftEntryManager* manager = g_ftEntryManager;
        fn_27_11967C(manager, 0, *reinterpret_cast<int*>(reinterpret_cast<u8*>(manager) + 0x44),
                     *reinterpret_cast<int*>(reinterpret_cast<u8*>(manager) + 0x48), false, -1);
    }
    fn_27_124830(m_input, contNo);
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x70) = contNo;
}

void ftEntry::setSlow(bool setStatus, int strength, int duration, bool isTimer) {
    if (unkF == 6) {
        getActiveFighter(this)->setSlow(setStatus, strength, duration, isTimer);
        if (_0xe & 2) {
            m_instances[1].m_fighter->setSlow(setStatus, strength, duration, isTimer);
        }
    }
}

void ftEntry::setWarp(Vec3f* pos, float lr, u32 flags) {
    if (unkF == 6) {
        getActiveFighter(this)->warp(pos, lr, flags);
        if (_0xe & 2) {
            m_instances[1].m_fighter->warp(pos, lr, flags);
        }
    }
}

// HYPOTHESIS: the pending scaling request (kind at +0x20, -1 when none; type at +0x24), applied outside the passes 2..5.
void ftEntry::setScaling(int kind, int type) {
    if (unkF == 6) {
        int* request = reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x20);
        request[0] = kind;
        request[1] = type;
        gfTaskScheduler* scheduler = gfTaskScheduler::getInstance();
        if (scheduler != NULL) {
            int processType = reinterpret_cast<ftEntrySchedulerView*>(scheduler)->m_processType;
            if ((processType <= 1 || processType >= 6) && request[0] != -1) {
                if (request[0] == 0) {
                    if (_0xe & 2) {
                        m_instances[0].m_fighter->restoreScaling();
                        m_instances[1].m_fighter->restoreScaling();
                    } else {
                        getActiveFighter(this)->restoreScaling();
                    }
                } else if (_0xe & 2) {
                    m_instances[0].m_fighter->startScaling((Fighter::Scaling::Kind)request[0], (Fighter::Scaling::Type)request[1]);
                    m_instances[1].m_fighter->startScaling((Fighter::Scaling::Kind)request[0], (Fighter::Scaling::Type)request[1]);
                } else {
                    getActiveFighter(this)->startScaling((Fighter::Scaling::Kind)request[0], (Fighter::Scaling::Type)request[1]);
                }
                request[0] = -1;
            }
        }
    }
}

void ftEntry::setCurry() {
    if (unkF == 6) {
        getActiveFighter(this)->setCurry(true, -1);
    }
}

void ftEntry::setSuperStar() {
    if (unkF == 6) {
        getActiveFighter(this)->setSuperStar(true, -1, true);
    }
}

void ftEntry::setZoom(float unk1, int unk2, int unk3) {
    if (unkF == 6) {
        fn_27_13231C(getActiveFighter(this), unk1, unk2, unk3);
    }
}

int ftEntry::getTeam(bool useTeamPoint, bool partner) {
    if (partner == true && m_heartSwapEntryId != -1) {
        ftEntry* entry = getEntry(m_heartSwapEntryId);
        if (useTeamPoint == true) {
            return entry->m_playerNo;
        }
        return entry->m_owner->getTeam();
    }
    if (useTeamPoint == true) {
        return m_playerNo;
    }
    return m_owner->getTeam();
}

int ftEntry::getTeam2nd(bool partner) {
    if (partner == true && m_heartSwapEntryId != -1) {
        return getEntry(m_heartSwapEntryId)->m_playerNo;
    }
    return m_playerNo;
}

// The trainer (instance 3) leaves with the rest of the match unless only one player is left.
void ftEntry::disappearTrainer() {
    if (_0xe & 1) {
        m_instances[3].m_fighter->disappear(g_ftManager->m_mode == 1);
    }
}

bool ftEntry::isProcessTechnique() {
    if ((u8)((m_flags11 & 8) != 0) == 1) {
        return true;
    }
    if ((u8)_0x10 != 0) {
        return true;
    }
    if (unkF == 3 && *(reinterpret_cast<u8*>(this) + 0x170) == 1) {
        return true;
    }
    return false;
}

// HYPOTHESIS: the change target (position at +0x78, facing at +0x84, pose at +0x14).
void ftEntry::prepareChange(float lr, Vec3f* pos, int changePose) {
    *reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(this) + 0x78) = *pos;
    *reinterpret_cast<float*>(reinterpret_cast<u8*>(this) + 0x84) = lr;
    unkC = 7;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x14) = changePose;
    unk11_20 = true;
    if (unkF != 3) {
        unkF = 5;
    }
}

void ftEntry::setHeal(float heal) {
    if (unkF == 6) {
        fn_27_13C1F0(getActiveFighter(this), -heal);
        if (_0xe & 2) {
            fn_27_13C1F0(m_instances[1].m_fighter, -heal);
        }
    }
}

void ftEntry::setVisibilityTrainer(bool visible) {
    if (unkF == 6 && (_0xe & 1)) {
        m_instances[3].m_fighter->setVisibility(visible);
    }
}

// HYPOTHESIS: +0x38 is the swap counter (starts at 1), +0x40/+0x44/+0x48 the swap entry ids.
ftEntryManager::ftEntryManager(int entryCapacity) : m_entries(NULL), m_entryCount(entryCapacity) {
    *reinterpret_cast<int*>(_0x38) = 1;
    m_isProcessHeartSwap = false;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x40) = -1;
    m_entries = new (static_cast<Heaps::HeapType>(2)) ftEntry[entryCapacity];
    m_entryArrayVector.clear();
    g_ftEntryManager = this;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x44) = -1;
    *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x48) = -1;
}

ftEntryManager::~ftEntryManager() {
    delete[] m_entries;
    m_entries = NULL;
    g_ftEntryManager = NULL;
}

ftEntry* ftEntryManager::getEntity(u32 entryId) {
    return &m_entries[(u8)entryId];
}

// The instance of the entry whose fighter task has the id.
static inline bool findInstanceFromTaskId(ftEntry* entry, int taskId, int* outInstanceIndex) {
    Fighter* fighter;
    int i;
    for (i = 0; i < entry->m_entryCount; i++) {
        fighter = entry->m_instances[i].m_fighter;
        if (fighter != NULL && (u32)taskId == *reinterpret_cast<u32*>(reinterpret_cast<u8*>(fighter) + 0x28)) {
            if (outInstanceIndex != NULL) {
                *outInstanceIndex = i;
            }
            return true;
        }
    }
    return false;
}

static inline bool findInstanceFromAreaId(ftEntry* entry, int areaId, int* outInstanceIndex) {
    int count = entry->m_entryCount;
    for (int i = 0; i < count; i++) {
        Fighter* fighter = entry->m_instances[i].m_fighter;
        if (fighter != NULL && fn_27_8CC68(fighter, areaId) != -1) {
            if (outInstanceIndex != NULL) {
                *outInstanceIndex = i;
            }
            return true;
        }
    }
    return false;
}

int ftEntryManager::getEntryIdFromAreaId(int areaId, int* outInstanceIndex) {
    int count = m_entryArrayVector.size();
    for (int i = 0; i < count; i++) {
        if (findInstanceFromAreaId(static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i), areaId, outInstanceIndex) == true) {
            return static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i)->m_entryId;
        }
    }
    return -1;
}

int ftEntryManager::getEntryIdFromTaskId(int taskId, int* outInstanceIndex) {
    int count = m_entryArrayVector.size();
    for (int i = 0; i < count; i++) {
        if (findInstanceFromTaskId(static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i), taskId, outInstanceIndex) == true) {
            return static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i)->m_entryId;
        }
    }
    return -1;
}

int ftEntryManager::getEntryIdFromPlayerNo(int playerNo) {
    int count = m_entryArrayVector.size();
    for (int i = 0; i < count; i++) {
        if (playerNo == static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i)->m_playerNo) {
            return static_cast<const soArray<ftEntry*>&>(m_entryArrayVector).at(i)->m_entryId;
        }
    }
    return -1;
}

void ftEntryManager::process() {
    int count = m_entryArrayVector.size();
    for (int i = 0; i < count; i++) {
        fn_27_1186D4(m_entryArrayVector.at(i));
    }
}

bool ftEntryManager::isValid(int entryId) {
    if (entryId != -1 && (int)(u8)entryId < (int)m_entryCount && m_entries[(u8)entryId].m_entryId == entryId) {
        return true;
    }
    return false;
}

void ftEntryManager::endSwap(bool unk) {
    if (m_isProcessHeartSwap == true) {
        fn_27_11967C(this, 0, *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x44),
                     *reinterpret_cast<int*>(reinterpret_cast<u8*>(this) + 0x48), unk, -1);
    }
}
