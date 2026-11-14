#include <ad/ad_ft_event.h>
#include <ft/ft_manager.h>
#include <gm/gm_global.h>
#include <gm/gm_global_record.h>
#include <if/if_adv_gauge.h>
#include <if/if_adv_mngr.h>
#include <if/if_mngr.h>
#include <if/if_player.h>
#include <snd/snd_system.h>
#include <so/damage/so_damage.h>
#include <sr/sr_common.h>
#include <st/operator/st_operator_info.h>
#include <st/stage.h>
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

namespace {

// gmCharacterKind -> fighter seal group (out of range: 35).
const u8 s_charaToSealGroup[44] = {
    0, 1, 2, 3, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 13,
    14, 14, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25, 25,
    25, 25, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34
};

inline int getSealGroup(int charaKind) {
    if (charaKind < 0 || charaKind > 43) {
        return 35;
    }
    return s_charaToSealGroup[charaKind];
}

inline bool isAdvModeNoStarWarp(gmAdvData* adv) {
    if (adv != NULL && (adv->m_mode == 1 || adv->m_mode == 2 || adv->m_mode == 3)) {
        return true;
    }
    return false;
}

} // namespace

void adFtEvent::notifyEventSetDamage(int entryId, float damage, u32 percent, bool p4, bool p5) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (m_operatorInfo != NULL) {
        m_operatorInfo->m_playerDamage[playerNo] = percent;
    }
    m_condition->m_playersInitData[playerNo].m_startDamage = percent;
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->m_players[playerNo]->updateDamageHP(percent, p5);
    }
    if (g_IfMngr != NULL) {
        g_IfMngr->m_ifPlayers[g_IfMngr->unk68[playerNo]]->updateDamageHP(percent, p5);
    }
    if (p4) {
        if (g_IfAdvMngr != NULL) {
            float rate = getDamageInfoRumbleRate(damage);
            IfPlayer* player = g_IfAdvMngr->m_players[playerNo];
            if (player->unk1C == player->unk18) {
                player->unk264 = rate;
                player->setDamageState(1);
            }
        }
        if (g_IfMngr != NULL) {
            float rate = getDamageInfoRumbleRate(damage);
            IfPlayer* player = g_IfMngr->m_ifPlayers[g_IfMngr->unk68[playerNo]];
            if (player->unk1C == player->unk18) {
                player->unk264 = rate;
                player->setDamageState(1);
            }
        }
    }
}

void adFtEvent::notifyEventRecover(int entryId, int hp) {
    int playerNo = g_ftManager->getPlayerNo(entryId);
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->m_players[playerNo]->updateDamageHP(hp, 1);
    }
    if (g_IfMngr != NULL) {
        g_IfMngr->m_ifPlayers[g_IfMngr->unk68[playerNo]]->updateDamageHP(hp, 1);
    }
}

void adFtEvent::notifyEventOutsideDeadArea(int entryId, soGroundUtil::DeadAreaCheckResult result, bool* out) {
    gmAdvData* adv = g_GameGlobal->m_advData;
    if (g_ftManager->getPlayerNo(entryId) != 1) {
        return;
    }
    if (adv->unk606_6) {
        return;
    }
    if (g_Stage->isCameraLocked()) {
        return;
    }
    ftManager* manager = g_ftManager;
    if (!manager->isFighterActivate(manager->getEntryId(0), -1)) {
        *out = true;
        return;
    }
    if (isAdvModeNoStarWarp(g_GameGlobal->m_advData)) {
        return;
    }
    if (g_Stage->helperStarWarp() == 1) {
        *out = true;
    }
}

void adFtEvent::notifyEventGetItem(int entryId, itKind kind, int itVariation, int genParamId, int instanceId) {
    switch (kind) {
    case 0x49: {
        gmAdvData* adv = g_GameGlobal->m_advData;
        u32 added;
        if (static_cast<s8>(adv->unk5FA) < 98) {
            adv->m_slots[static_cast<s8>(adv->unk5FA)].m_id = 1;
            adv->m_slots[static_cast<s8>(adv->unk5FA)].m_value = 0xFF;
            adv->unk5FA++;
            added = true;
        } else {
            added = false;
        }
        if (added == true) {
            if (g_IfAdvMngr != NULL) {
                g_IfAdvMngr->addStockSmb();
                if (!g_GameGlobal->m_advData->unk606_7) {
                    IfAdvMngr* mngr = g_IfAdvMngr;
                    mngr->unk48_40 = true;
                    if (mngr->m_gauge != NULL) {
                        if (mngr->unk48_40 == true) {
                            mngr->m_gauge->addPressStart(IfPlayer::getPlayerLocator());
                        } else {
                            mngr->m_gauge->removePressStart(IfPlayer::getPlayerLocator());
                        }
                    }
                }
            }
            gmAdvData* data = g_GameGlobal->m_advData;
            gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
            if (data->unk2B4 <= data->unk2B5) {
                data->unk2B5 = 0;
            }
            if (getSealGroup(static_cast<s8>(data->unk2B9[data->unk2B5])) == getSealGroup(melee->m_playersInitData[0].m_characterKind)) {
                data->unk2B5++;
            }
            g_sndSystem->playSE(static_cast<SndID>(0x2290), -1, 0, 0, -1);
        }
        break;
    }
    }
    if (g_Stage != NULL) {
        g_Stage->stAdventureEventGetItem(entryId, kind, itVariation, genParamId, instanceId);
    }
}

void adFtEvent::notifyEventChangeEnd(int entryId, int playerNo, int activeInstanceIndex, ftKind kind) {
    int faceKind = kind;
    switch (g_ftManager->getFighterGmKind(entryId)) {
    case 29:
    case 31:
    case 33:
        switch (faceKind) {
        case 0x1D: faceKind = 0x1D; break;
        case 0x1F: faceKind = 0x21; break;
        case 0x1E: faceKind = 0x1F; break;
        }
        break;
    case 3:
    case 4:
        switch (faceKind) {
        case 3: faceKind = 3; break;
        case 0x18: faceKind = 4; break;
        }
        break;
    case 14:
    case 15:
        switch (faceKind) {
        case 0xD: faceKind = 0xE; break;
        case 0xE: faceKind = 0xF; break;
        }
        break;
    case 12:
    case 44:
        switch (faceKind) {
        case 0xB: faceKind = 0xC; break;
        case 0x30: faceKind = 0x2C; break;
        }
        break;
    case 23:
    case 45:
        switch (faceKind) {
        case 0x15: faceKind = 0x17; break;
        case 0x31: faceKind = 0x2D; break;
        }
        break;
    default:
        return;
    }
    if (g_IfMngr != NULL) {
        int areaNo = getInfoAreaNo(playerNo);
        g_IfMngr->m_ifPlayers[g_IfMngr->unk68[areaNo]]->setStockFace(0, faceKind);
    }
    if (g_IfAdvMngr != NULL) {
        int areaNo = getInfoAreaNo(playerNo);
        g_IfAdvMngr->m_players[areaNo]->setStockFace(0, faceKind);
    }
}
