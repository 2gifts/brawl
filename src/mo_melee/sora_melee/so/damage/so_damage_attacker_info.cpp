#include <so/damage/so_damage_attacker_info.h>
#include <so/so_external_value_accesser.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <types.h>

void soDamageAttackerInfo::clear() {
    m_directTaskId = -1;
    m_directTaskCategory = gfTask::Category_None;
    m_directSoKind = (soKind)-1;
    m_directSoSubKind = -1;
    m_indirectTaskId = -1;
    m_indirectSoKind = (soKind)-1;
    m_indirectTaskCategory = gfTask::Category_None;
    m_indirectSoSubKind = -1;
    m_indirectEntryId = -1;
    m_clearFrame = -1;
}

// HYPOTHESIS: the attacker is recorded directly; a fighter is also its own indirect attacker, and an article or item that a
// fighter owns records that fighter as the indirect attacker.
// HYPOTHESIS: an inline setter takes the task id and category of the direct attacker (both are read before either is
// stored).
static inline void setDirectTask(soDamageAttackerInfo* info, u8 taskCategory, u32 taskId) {
    info->m_directTaskCategory = taskCategory;
    info->m_directTaskId = taskId;
}

static inline void setDirectInfo(soDamageAttackerInfo* info, StageObject* attacker) {
    setDirectTask(info, attacker->m_taskCategory, attacker->m_taskId);
    info->m_directSoKind = attacker->soGetKind();
    info->m_directSoSubKind = attacker->soGetSubKind();
}

static inline void setIndirectInfo(soDamageAttackerInfo* info, Fighter* fighter) {
    info->m_indirectTaskId = fighter->m_taskId;
    info->m_indirectTaskCategory = fighter->m_taskCategory;
    info->m_indirectSoKind = fighter->soGetKind();
    info->m_indirectSoSubKind = fighter->soGetSubKind();
    info->m_indirectEntryId = fighter->m_entryId;
}

void soDamageAttackerInfo::setIndirect(u32 defenderTaskId, StageObject* attacker, StageObject* indirectAttacker) {
    if (defenderTaskId == soExternalValueAccesser::getTeamOwnerId(attacker)) {
        return;
    }
    if (indirectAttacker->soGetKind() == StageObject_Fighter) {
        setDirectInfo(this, attacker);
        setIndirectInfo(this, &dynamic_cast<Fighter&>(*indirectAttacker));
    } else {
        u32 ownerId = soExternalValueAccesser::getTeamOwnerId(attacker);
        if (ownerId != attacker->m_taskId) {
            int entryId = g_ftManager->getEntryIdFromTaskId(ownerId, NULL);
            if (entryId != -1) {
                setDirectInfo(this, attacker);
                setIndirectInfo(this, g_ftManager->getFighter(entryId, -1));
            }
        }
    }
    if (m_indirectSoKind != -1) {
        m_clearFrame = -1;
    }
}

void soDamageAttackerInfo::set(u32 defenderTaskId, StageObject* attacker) {
    if (defenderTaskId == soExternalValueAccesser::getTeamOwnerId(attacker)) {
        return;
    }
    if (attacker->soGetKind() == StageObject_Fighter) {
        setDirectInfo(this, attacker);
        setIndirectInfo(this, &dynamic_cast<Fighter&>(*attacker));
    } else {
        u32 ownerId = soExternalValueAccesser::getTeamOwnerId(attacker);
        if (ownerId != attacker->m_taskId) {
            int entryId = g_ftManager->getEntryIdFromTaskId(ownerId, NULL);
            if (entryId != -1) {
                setDirectInfo(this, attacker);
                setIndirectInfo(this, g_ftManager->getFighter(entryId, -1));
            }
        }
    }
    if (m_indirectSoKind != -1) {
        m_clearFrame = -1;
    }
}

void soDamageAttackerInfo::update() {
    if (m_clearFrame > 0) {
        if (--m_clearFrame == 0) {
            clear();
        }
    }
}

// TODO: clean up: MATCH-ONLY helper; the original reads every member before storing any of them, which only an inlined
// call with the members as arguments reproduces.
static inline void copyMembers(soDamageAttackerInfo* dst, int clearFrame, int indirectEntryId, int indirectSoSubKind,
                               soKind indirectSoKind, u8 indirectTaskCategory, int indirectTaskId, int directTaskId,
                               u8 directTaskCategory, int directSoSubKind, soKind directSoKind) {
    dst->m_directSoKind = directSoKind;
    dst->m_directSoSubKind = directSoSubKind;
    dst->m_directTaskCategory = directTaskCategory;
    dst->m_directTaskId = directTaskId;
    dst->m_indirectTaskId = indirectTaskId;
    dst->m_indirectTaskCategory = indirectTaskCategory;
    dst->m_indirectSoKind = indirectSoKind;
    dst->m_indirectSoSubKind = indirectSoSubKind;
    dst->m_indirectEntryId = indirectEntryId;
    dst->m_clearFrame = clearFrame;
}

void soDamageAttackerInfo::copy(soDamageAttackerInfo* src) {
    copyMembers(this, src->m_clearFrame, src->m_indirectEntryId, src->m_indirectSoSubKind, src->m_indirectSoKind,
                src->m_indirectTaskCategory, src->m_indirectTaskId, src->m_directTaskId, src->m_directTaskCategory,
                src->m_directSoSubKind, src->m_directSoKind);
}
