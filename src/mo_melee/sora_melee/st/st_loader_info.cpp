#include <gf/gf_memory_pool.h>
#include <if/if_mngr.h>
#include <sr/sr_common.h>
#include <st/loader/st_loader_info.h>
#include <st/loader/st_loader_sand_bag.h>
#include <types.h>

// HYPOTHESIS: IfMngr::loadResourceBody, freeResource, createModel, startMelee and destroyModel (main.dol, map); their
// names belong to the IfMngr unit, so they stay stand-ins here.
extern "C" {
void fn_800DB944(IfMngr* mngr, int type, bool isOnline, int unk, u32 sub);
void fn_800DBD3C(IfMngr* mngr);
void fn_800DBF08(IfMngr* mngr, gmGlobalModeMelee* melee);
void fn_800DC57C(IfMngr* mngr, gmGlobalModeMelee* melee);
void fn_800DC2E0(IfMngr* mngr);
}

// MATCH-ONLY: the native code reads the character kind as an unsigned byte, not through the enum bitfield.
static inline u8 getCharacterKind(gmPlayerInitData* initData) {
    return reinterpret_cast<stLoaderSandBagInitDataView*>(initData)->m_characterKind;
}

static stLoaderSandBagMarker sMarkerA(0xFF, 0);
static stLoaderSandBagMarker sMarkerB(0xFF, 1);

stLoaderInfo::stLoaderInfo()
    : stLoader("StLoaderInfo"), m_resourceType(0), m_isOnline(false), m_resourceSub(5), m_reqResourceType(0),
      m_reqIsOnline(false), m_reqResourceSub(5), m_commonLoader(NULL) { }

stLoaderInfo::~stLoaderInfo() { }

void stLoaderInfo::processBegin() {
    if ((u8)_0x45[1] != 0) {
        switch (m_state) {
            case 0:
                m_state = 1;
                break;
            case 1:
                m_resourceType = m_reqResourceType;
                m_isOnline = m_reqIsOnline;
                m_resourceSub = m_reqResourceSub;
                fn_800DB944(g_IfMngr, m_resourceType, m_isOnline, 1, m_resourceSub);
                m_state = 2;
                break;
            case 2:
                if (!g_IfMngr->isLoadResourceFinished()) {
                    break;
                }
                m_state = 3;
            case 3:
                if (m_resourceType != m_reqResourceType || m_isOnline != m_reqIsOnline ||
                    m_resourceSub != m_reqResourceSub || (u8)_0x45[2] == 1) {
                    m_state = 5;
                }
                break;
            case 5:
                removeEntity();
                fn_800DBD3C(g_IfMngr);
                if ((u8)_0x45[2] == 1) {
                    m_state = 6;
                } else {
                    m_state = 0;
                }
                break;
        }
    }
}

void stLoaderInfo::entryEntity() {
    if (getCharacterKind(&m_globalModeMelee->m_playersInitData[1]) == Character_Boss_MasterHand) {
        m_globalModeMelee->m_playersInitData[1].m_state = 1;
    }
    if (getCharacterKind(&m_globalModeMelee->m_playersInitData[2]) == Character_Boss_CrazyHand) {
        m_globalModeMelee->m_playersInitData[2].m_state = 1;
    }
    if (g_HeapInfos[Heaps::InfoInstance].m_name != NULL) {
        fn_800DBF08(g_IfMngr, m_globalModeMelee);
        fn_800DC57C(g_IfMngr, m_globalModeMelee);
        if (getCharacterKind(&m_globalModeMelee->m_playersInitData[1]) == Character_Boss_MasterHand) {
            m_globalModeMelee->m_playersInitData[1].m_state = 3;
        }
        if (getCharacterKind(&m_globalModeMelee->m_playersInitData[2]) == Character_Boss_CrazyHand) {
            m_globalModeMelee->m_playersInitData[2].m_state = 3;
        }
    }
}

void stLoaderInfo::removeEntity() {
    if (g_HeapInfos[Heaps::InfoInstance].m_name != NULL) {
        fn_800DC2E0(g_IfMngr);
    }
}

void stLoaderInfo::setResouceType(int type, u32 sub) {
    m_reqResourceType = type;
    m_reqResourceSub = sub & 7;
}

void stLoaderInfo::setOnline(bool isOnline) {
    m_reqIsOnline = isOnline;
}
