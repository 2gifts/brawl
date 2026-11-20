#include <st/loader/st_loader_player.h>
#include <gm/gm_global.h>
#include <gm/gm_global_mode_melee.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <if/if_mngr.h>
#include <if/if_adv_mngr.h>
#include <if/if_player.h>
#include <gm/gm_result_info.h>
#include <gm/gm_stage_data.h>
#include <revolution/OS.h>
#include <types.h>

// stLoaderPlayer pieces from the middle text range of the original unit (sora_melee 0x23B5F0..0x23E614); the rest of the
// range (entryEntity, entryInfo, entryEntityResult and the rebirth / sudden death entries) is not decompiled yet.

// TODO: clean up - placeholder fn_ names: the ftManager method at 0x10995C (takes the entry id) and the report format
// of the unit (its data is not split yet).
extern "C" void fn_27_10995C(ftManager* manager, int entryId, int);
extern "C" const char lbl_27_data_4D330[];

// TODO: clean up - placeholder fn_ name; the entity removal: records the current fighter number, appends the items the
// fighter collected to the player's collection of the stage data (adventure only), writes back the result kind and
// releases the entry.
extern "C" void fn_27_23E45C(stLoaderPlayer* loader) {
    if (loader->isEntity()) {
        u8 fighterNo = g_ftManager->getCurrentFighterNo(loader->m_entryId);
        loader->m_56 = fighterNo;
        loader->_0x54[1] = fighterNo;
        u32 flag = ((u8)loader->_0x5b[2] >> 6) & 1;
        if (flag == 1 && loader->getWatchPlayerInitData()->m_state == 0) {
            ftOwner* owner = g_ftManager->getOwner(loader->m_entryId);
            for (u8 i = 0; i < owner->getCollectionItemNum(); i++) {
                gmStageData::ItemCollection* collection;
                u32 variation = owner->getCollectionItemVariation(i);
                u32 kind = owner->getCollectionItemKind(i);
                // TODO: clean up - one item collection per player follows the first one in gmStageData.
                collection = &(&g_GameGlobal->m_stageData->m_itemCollection)[loader->m_playerId];
                if (collection->m_num < 0x100) {
                    collection->m_kinds[collection->m_num] = kind;
                    collection->m_variations[collection->m_num] = variation;
                    OSReport(lbl_27_data_4D330, collection->m_num, kind, variation);
                    collection->m_num++;
                }
            }
        }
        gmResultInfo* resultInfo = g_GameGlobal->m_resultInfo;
        gmPlayerResultInfo* playerResult = &resultInfo->m_playersResultInfo[loader->getWatchPlayerInitData()->m_playerNo];
        if (playerResult->m_characterKind != g_ftManager->getResultFighterGmKind(loader->m_entryId)) {
            playerResult->m_characterKind = g_ftManager->getResultFighterGmKind(loader->m_entryId);
        }
        fn_27_10995C(g_ftManager, loader->m_entryId, 0);
        loader->m_entryId = -1;
    }
    loader->stLoader::removeEntity();
}

void stLoaderPlayer::removeInfo() {
    if (m_mode != Mode_Result) {
        if (g_IfMngr != NULL) {
            u8 id = getWatchPlayerInitData()->m_playerId;
            g_IfMngr->m_ifPlayers[id]->disappear();
        }
        if (g_IfAdvMngr != NULL) {
            u8 id = getWatchPlayerInitData()->m_playerId;
            g_IfAdvMngr->m_players[id]->disappear();
        }
    }
}

// TODO: clean up - placeholder fn_ name; removes the info when the entity is entered.
extern "C" void fn_27_23E34C(stLoaderPlayer* loader) {
    if (loader->isEntity()) {
        loader->removeInfo();
    }
}

void stLoaderPlayer::removeEntity() {
    if (m_globalModeMelee->m_meleeInitData.m_gameMode == 6 && isEntity() && m_playerId < 2) {
        int value = g_ftManager->getOwner(m_entryId)->getLog().m_logDataAccesser.getInt(0x1f, 0);
        // TODO: clean up - raw offset into the adventure data (a per-player counter at 0x4918).
        *(int*)((char*)g_GameGlobal->m_advData + m_playerId * 4 + 0x4918) += value;
    }
    if (isEntity()) {
        removeInfo();
    }
    fn_27_23E45C(this);
}

// TODO: clean up - the flag byte at 0x1D of gmPlayerInitData is read through this overlay; name its bits in
// gm_global_mode_melee.h (left alone here because nearly every unit includes it).
struct gmPlayerInitDataFlags1D {
    bool m_0x1D_7 : 1;
    bool m_0x1D_6 : 1;
    bool m_0x1D_5 : 1;
    bool m_0x1D_4 : 1; // HYPOTHESIS: forces colour file 0xC
    bool m_0x1D_3 : 1; // HYPOTHESIS: forces colour file 0xD
    bool m_0x1D_2 : 1;
    bool m_0x1D_1 : 1;
    bool m_0x1D_0 : 1;
};

// TODO: clean up - placeholder fn_ name; checks whether the loaded fighter still fits the player init data (kind and
// colour file) and moves the loader state to 5 (reload) or 7.
extern "C" void fn_27_23BB10(stLoaderPlayer* loader) {
    u8 state = loader->m_state;
    if (state == 3 || (u8)(state + 0xf8) <= 1) {
        gmPlayerInitData* initData = loader->getWatchPlayerInitData();
        if (initData->m_state == 3 || initData->m_characterKind != loader->m_characterKind) {
            goto reload;
        }
        initData = loader->getWatchPlayerInitData();
        u8 color = initData->m_colorFileNo;
        if (reinterpret_cast<gmPlayerInitDataFlags1D*>(initData->_0x1D)->m_0x1D_4) {
            color = 0xc;
        } else if (reinterpret_cast<gmPlayerInitDataFlags1D*>(initData->_0x1D)->m_0x1D_3) {
            color = 0xd;
        }
        if (initData->m_isSpycloak) {
            color |= 0x80;
        }
        if ((u8)loader->_0x5b[0] != color || (u8)loader->_0x54[1] != loader->m_56 || (u8)loader->_0x45[2] == 1) {
        reload:
            loader->m_state = 5;
        } else {
            loader->m_state = 7;
        }
    }
}
