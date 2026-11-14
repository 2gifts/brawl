#include <ad/ad_ft_event.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <so/damage/so_damage.h>
#include <sr/sr_common.h>
#include <st/operator/st_operator_info.h>
#include <types.h>

adFtEvent* adFtEvent::create() {
    return new (Heaps::StageInstance) adFtEvent();
}

adFtEvent::~adFtEvent() { }

void adFtEvent::destroy() {
    mlFtEvent::destroy();
}

void adFtEvent::notifyEventOnDamage(int entryId, u32 hp, soDamage* damage) {
    notifyEventSetDamage(entryId, damage->m_damageAdd, hp, true, false);
}

void adFtEvent::notifyEventBeat(int entryId1, int entryId2) {
    g_ftManager->getPlayerNo(entryId1);
    g_ftManager->getPlayerNo(entryId2);
}

void adFtEvent::notifyEventSuicide(int entryId) {
    g_ftManager->getPlayerNo(entryId);
}

void adFtEvent::notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) { }

void adFtEvent::notifyEventSetCursor(int entryId, u32 index) {
    if (unk34_80 && m_operatorInfo != NULL) {
        m_operatorInfo->setPlayerCursorStatus(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)), index);
    }
}

void adFtEvent::notifyEventSetNameCursor(int entryId, u32 index) {
    if (unk34_80 && m_operatorInfo != NULL) {
        int playerNo = g_ftManager->getPlayerNo(entryId);
        m_operatorInfo->setPlayerNameCursorStatus(m_condition->m_playersInitData[playerNo].m_playerId, index);
    }
}

void adFtEvent::notifyEventSetLoupe(int entryId, u32 index) {
    if (unk34_80 && m_operatorInfo != NULL) {
        m_operatorInfo->setPlayerLoupeStatus(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)), index);
    }
}
