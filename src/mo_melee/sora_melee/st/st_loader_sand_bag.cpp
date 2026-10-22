#include <gm/gm_global_mode_melee.h>
#include <if/if_mngr.h>
#include <it/it_manager.h>
#include <st/loader/st_loader_sand_bag.h>
#include <types.h>

// HYPOTHESIS: stLoader::isEntity (sora_melee 0x23AC10, map) and IfPlayer::appear/disappear (main.dol, map); their
// names belong to other units, so they stay stand-ins here.
extern "C" bool fn_27_23AC10(stLoader* loader);
extern "C" void fn_800E08A0(IfPlayer* player, stLoaderSandBagAppearInfo* info, u8 unk);
extern "C" void fn_800E1384(IfPlayer* player);

static stLoaderSandBagMarker sMarkerA(0xFF, 0);
static stLoaderSandBagMarker sMarkerB(0xFF, 1);

stLoaderSandBag::stLoaderSandBag(u32 playerId)
    : stLoaderPlayer(playerId), m_item(NULL), unk64(false), m_itemKind(Item_HomeRun_Sandbag) { }

stLoaderSandBag::~stLoaderSandBag() { }

void stLoaderSandBag::processBegin() {
    if ((u8)_0x45[1] != 0 && itManager::getInstance() != NULL) {
        switch (m_state) {
            case 0:
                m_state = 1;
                break;
            case 1:
                m_state = 2;
                break;
            case 2:
                m_state = 3;
                break;
            case 3:
                if ((u8)_0x45[2] == 1) {
                    m_state = 5;
                }
                break;
            case 5:
                if ((u8)_0x45[2] == 1) {
                    m_state = 6;
                } else {
                    m_state = 0;
                }
                break;
        }
    }
}

void stLoaderSandBag::entryEntity() {
    if (fn_27_23AC10(this) == true) {
        return;
    }
    if (isLoad()) {
        itManager* itemManager = itManager::getInstance();
        if (itemManager != NULL) {
            m_item = itemManager->createItem(m_itemKind);
            if (m_item != NULL) {
                entryInfo(unk64);
                stLoader::entryEntity();
            }
        }
    }
}

void stLoaderSandBag::entryInfo(bool) {
    if (unk64) {
        gmPlayerInitData* initData = getWatchPlayerInitData();
        stLoaderSandBagAppearInfo info;
        info.m_state = initData->m_state;
        stLoaderSandBagInitDataView* view = reinterpret_cast<stLoaderSandBagInitDataView*>(initData);
        info.m_characterKind = view->m_characterKind;
        info.m_colorFileNo = initData->m_colorFileNo;
        if (view->unk1B_3) {
            info.unk10 = 2;
        } else if (view->m_isMetal) {
            info.unk10 = 1;
        } else {
            info.unk10 = 0;
        }
        if (initData->m_isStamina) {
            info.m_damage = (u16)initData->m_hitPointMax;
            info.m_isStamina = true;
        } else {
            info.m_damage = (u16)initData->m_startDamage;
            info.m_isStamina = false;
        }
        info.unk1C = 0;
        info.m_teamNo = (u8)initData->m_teamNo;
        info.unk0 = 1;
        if (g_IfMngr != NULL) {
            fn_800E08A0(g_IfMngr->m_ifPlayers[m_playerId], &info, reinterpret_cast<u8*>(g_IfMngr)[0xA8]);
        }
    }
}

void stLoaderSandBag::removeEntity() {
    if (itManager::getInstance() != NULL && m_item != NULL && isLoad() && fn_27_23AC10(this)) {
        if (unk64 == true && g_IfMngr != NULL) {
            fn_800E1384(g_IfMngr->m_ifPlayers[m_playerId]);
        }
        stLoader::removeEntity();
        m_item = NULL;
    }
}

void stLoaderSandBag::lockEntity() { }

void stLoaderSandBag::unlockEntity() { }

void stLoaderSandBag::entryEntitySuddenDeath() {
    entryEntity();
}

void stLoaderSandBag::entryEntityRebirth() {
    entryEntity();
}

bool stLoaderSandBag::isEntry() {
    return true;
}

void stLoaderSandBag::setSuddenDeath() { }

void stLoaderSandBag::removeInfo() { }
