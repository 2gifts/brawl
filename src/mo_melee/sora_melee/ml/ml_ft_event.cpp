// MATCH-ONLY: the constructor re-extends the s16 manage id at every use.
#define SO_EVENT_OBSERVER_MANAGE_S16
#include <cm/cm_camera_controller.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <if/if_mngr.h>
#include <if/if_player.h>
#include <ml/ml_ft_event.h>
#include <nt/nt_etc_upload_eval.h>
#include <revolution/OS/OSError.h>
#include <so/damage/so_damage.h>
#include <st/operator/st_operator_info.h>
#include <st/operator/st_operator_rule.h>
#include <st/stage.h>
#include <types.h>

extern "C" u8 fn_27_111D70(ftOwner* owner);
extern "C" void fn_800DD0CC(IfMngr* mngr, int areaNo1, int areaNo2);
extern "C" void fn_800DD200(IfMngr* mngr);

mlFtEvent::~mlFtEvent() {
}

void mlFtEvent::notifyEventBeat(int entryId1, int entryId2) {
    if (!isInfoManager()) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    int playerNo1 = g_ftManager->getPlayerNo(entryId1);
    int playerNo2 = g_ftManager->getPlayerNo(entryId2);
    if (m_operatorRule != NULL) {
        m_operatorRule->notifyEventBeat(playerNo1, playerNo2);
    } else if (m_condition->m_meleeInitData.m_0x3_2 && g_IfMngr != NULL) {
        if (m_condition->m_meleeInitData.m_isTeams == true
            && static_cast<u8>(m_condition->m_playersInitData[playerNo1].m_teamNo) == static_cast<u8>(m_condition->m_playersInitData[playerNo2].m_teamNo)) {
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo1].m_playerId]->setScoreVary(-1);
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo2].m_playerId]->setScoreVary(-1);
        } else {
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo1].m_playerId]->setScoreVary(1);
            g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo2].m_playerId]->setScoreVary(-1);
        }
    }
    nteUpdateEvaluator* evaluator = nteUpdateEvaluator::getInstance();
    if (evaluator != NULL) {
        evaluator->notifyEventBeat(playerNo1, playerNo2);
    }
}

void mlFtEvent::notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    if (!isInfoManager()) {
        return;
    }
    u32 faceKind;
    switch (g_ftManager->getFighterGmKind(entryId)) {
    case 29:
    case 31:
    case 33:
        switch (kind) {
        case 0x1D: faceKind = 0x1D; break;
        case 0x1F: faceKind = 0x21; break;
        case 0x1E: faceKind = 0x1F; break;
        }
        break;
    case 3:
    case 4:
        switch (kind) {
        case 3: faceKind = 3; break;
        case 0x18: faceKind = 4; break;
        }
        break;
    case 14:
    case 15:
        switch (kind) {
        case 0xD: faceKind = 0xE; break;
        case 0xE: faceKind = 0xF; break;
        }
        break;
    case 12:
    case 44:
        switch (kind) {
        case 0xB: faceKind = 0xC; break;
        case 0x30: faceKind = 0x2C; break;
        }
        break;
    case 23:
    case 45:
        switch (kind) {
        case 0x15: faceKind = 0x17; break;
        case 0x31: faceKind = 0x2D; break;
        }
        break;
    default:
        return;
    }
    g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])]->setStockFace(
        0, faceKind);
}

void mlFtEvent::notifyEventOnDamage(int entryId, u32 hp, soDamage* damage) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (m_operatorInfo != NULL) {
        m_operatorInfo->m_playerDamage[playerNo] = hp;
    }
    if (!isInfoManager()) {
        return;
    }
    if (m_condition->m_meleeInitData.m_hideDamageGauge) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    if (hp != 0) {
        IfPlayer* damagePlayer = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
        damagePlayer->updateDamageHP(hp, 0);
        float rumbleRate = getDamageInfoRumbleRate(damage->m_damageAdd);
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
        if (player->unk1C == player->unk18) {
            player->unk264 = rumbleRate;
            player->setDamageState(1);
        }
    }
    nteUpdateEvaluator* evaluator = nteUpdateEvaluator::getInstance();
    if (evaluator != NULL) {
        evaluator->notifyEventOnDamage(playerNo, hp);
    }
}

void mlFtEvent::notifyEventRecover(int entryId, int hp) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (!isInfoManager()) {
        return;
    }
    if (m_condition->m_meleeInitData.m_hideDamageGauge) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    if (playerNo >= 0) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
        player->updateDamageHP(hp, 1);
    }
}

void mlFtEvent::notifyEventSetDamage(int entryId, float damage, u32 percent, bool p4, bool p5) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (m_operatorInfo != NULL) {
        m_operatorInfo->m_playerDamage[playerNo] = percent;
    }
    if (!isInfoManager()) {
        return;
    }
    if (m_condition->m_meleeInitData.m_hideDamageGauge) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    CameraController::getInstance()->clearDisablePlayerNo(playerNo);
    if (playerNo >= 0) {
        IfPlayer* player = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
        player->updateDamageHP(percent, p5);
        if (p4) {
            float rumbleRate = getDamageInfoRumbleRate(damage);
            IfPlayer* rumblePlayer = g_IfMngr->m_ifPlayers[static_cast<u8>(g_IfMngr->unk68[getInfoAreaNo(playerNo)])];
            if (rumblePlayer->unk1C == rumblePlayer->unk18) {
                rumblePlayer->unk264 = rumbleRate;
                rumblePlayer->setDamageState(1);
            }
        }
    } else {
        OSReport("mlFtEvent::notifyEventSetDamage() nEntryID(0x%08x) is not safe!!!\n", entryId);
    }
}

void mlFtEvent::notifyEventSuicide(int entryId) {
    if (!isInfoManager()) {
        return;
    }
    if (!unk34_20) {
        return;
    }
    if (m_operatorRule != NULL && m_operatorRule->isGameSet()) {
        return;
    }
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (g_IfMngr != NULL && m_condition->m_meleeInitData.m_0x3_2) {
        g_IfMngr->m_ifPlayers[m_condition->m_playersInitData[playerNo].m_playerId]->setScoreVary(
            m_condition->m_meleeInitData.m_suicideScoreMultiplier);
    }
    if (g_Stage == NULL) {
        return;
    }
    if (unk38 != entryId) {
        return;
    }
    g_Stage->endAppear();
}

bool mlFtEvent::isInfoManager() {
    return g_IfMngr != NULL;
}

mlFtEvent::mlFtEvent() : ftOutsideEventObserver(-1, 0) {
    if (m_sendID <= -1) {
        soEventManageModule* module = &g_ftManager->m_eventManageModule;
        addObserverSubS16(module->getManageId(), this, -1);
    }
    m_condition = g_GameGlobal->m_modeMelee;
    m_operatorInfo = NULL;
    m_operatorRule = NULL;
    unk38 = 0;
    unk34_80 = true;
    unk34_40 = false;
    unk34_20 = true;
    unk34_10 = false;
}

void mlFtEvent::destroy() {
    delete this;
}

int mlFtEvent::getInfoAreaNo(int playerNo) {
    return m_condition->m_playersInitData[playerNo].m_playerId;
}

float mlFtEvent::getDamageInfoRumbleRate(float damage) {
    float min = 3.0f;
    float max = 15.0f;
    float rate = 0.6f;
    if (damage >= min && damage <= max) {
        return rate * ((damage - min) / (max - min));
    }
    if (damage < min) {
        return 0.0f;
    }
    return 1.0f;
}

void mlFtEvent::notifyEventKnockout(int entryId) {
    if (m_operatorRule != NULL) {
        m_operatorRule->notifyEventKnockout(g_ftManager->getScoreEntryId(entryId));
    }
    int playerNo = g_ftManager->getPlayerNo(entryId);
    CameraController::getInstance()->setDisablePlayerNo(playerNo);
}

void mlFtEvent::setCondition(gmGlobalModeMelee* condition) {
    m_condition = condition;
}

void mlFtEvent::notifyEventAddDragoonParts(int entryId, int part) {
    if (isInfoManager()) {
        g_IfMngr->addPlayerDragoonParts(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)), part);
    }
}

void mlFtEvent::notifyEventRemoveDragoonParts(int entryId, int part) {
    if (isInfoManager()) {
        int playerNo = g_ftManager->getPlayerNo(entryId);
        g_IfMngr->deletePlayerDragoonParts(getInfoAreaNo(playerNo), part);
        if (fn_27_111D70(g_ftManager->getOwner(entryId)) == 2) {
            g_IfMngr->setPlayerDragoonPartsComplete(getInfoAreaNo(playerNo), 0);
        }
    }
}

void mlFtEvent::notifyEventCompDragoonParts(int entryId) {
    if (isInfoManager()) {
        g_IfMngr->setPlayerDragoonPartsComplete(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)), 1);
    }
}

void mlFtEvent::notifyEventResetDragoonParts(int entryId) {
    if (isInfoManager()) {
        g_IfMngr->resetPlayerDragoonParts(getInfoAreaNo(g_ftManager->getPlayerNo(entryId)));
    }
}

void mlFtEvent::notifyEventChangeStart(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) { }

void mlFtEvent::notifyEventRemoveEntry(int entryId) {
    if (isInfoManager() && unk34_80 && m_operatorInfo != NULL) {
        int areaNo = getInfoAreaNo(g_ftManager->getPlayerNo(entryId));
        m_operatorInfo->setPlayerCursorClear(areaNo);
        m_operatorInfo->setPlayerLoupeStatus(areaNo, 0);
    }
}

void mlFtEvent::notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId) { }

void mlFtEvent::notifyEventReEntryRequestFighter() { }

void mlFtEvent::notifyEventSucceedHit(int entryId, u32 consecutiveHits, float totalDamage) { }

void mlFtEvent::notifyEventFinalSlow(int entryId, float rate, int frames) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(rate, frames);
    }
}

void mlFtEvent::notifyEventFinalSlowCancel(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(1.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventFinalStop(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(0.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventFinalStopCancel(int entryId) {
    if (g_Stage != NULL) {
        g_Stage->setMotionRatio(1.0f, 0.0f);
    }
}

void mlFtEvent::notifyEventPikminMakeBloomAll(int entryId) {
    if (m_operatorRule != NULL) {
        m_operatorRule->notifyEventPikminBloom();
    }
}

void mlFtEvent::notifyEventHeartSwapStart(int entryId1, int entryId2) {
    if (isInfoManager()) {
        int areaNo1 = getInfoAreaNo(g_ftManager->getPlayerNo(entryId1));
        fn_800DD0CC(g_IfMngr, areaNo1, getInfoAreaNo(g_ftManager->getPlayerNo(entryId2)));
    }
}

void mlFtEvent::notifyEventHeartSwapEnd(int entryId1, int entryId2) {
    if (isInfoManager()) {
        fn_800DD200(g_IfMngr);
    }
}

bool mlFtEventResult::isInfoManager() {
    return false;
}
