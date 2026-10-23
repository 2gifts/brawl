#include <gf/gf_archive.h>
#include <gm/gm_global.h>
#include <if/if_adv_mngr.h>
#include <sr/sr_common.h>
#include <mu/mu_resource_loader.h>
#include <st/loader/st_loader_info_adventure.h>
#include <st/loader/st_loader_sand_bag.h>
#include <types.h>

// HYPOTHESIS: IfAdvMngr members (main.dol), the scene module's scResourceLoader read/isLoaded (sora_scene) and
// stLoaderStageAdventureCommon::getAdvRadarRes (sora_melee 0x23B44C, map); their names belong to other units, so
// they stay stand-ins here.
extern "C" {
void fn_80106A5C(IfAdvMngr* mngr, gfArchive* archive);
bool fn_80106E40(IfAdvMngr* mngr);
void fn_80106A64(IfAdvMngr* mngr, void* res1, void* res2, void* res3, void* res4);
void fn_80106D10(IfAdvMngr* mngr);
void fn_80106E48(IfAdvMngr* mngr);
void fn_8010C38C(void* unk);
void fn_1_3C68(scResourceLoader* loader, int heap, const char* path, gfArchive* archive, int size);
bool fn_1_3E50(scResourceLoader* loader);
void fn_27_23B44C(stLoader* loader, void** res1, void** res2, void** res3, void** res4);
}

// HYPOTHESIS: the IfAdvMngr fields this loader touches; the shared header leaves the class empty.
struct stLoaderInfoAdventureIfView {
    char _0[0x48];
    u8 unk48_7 : 1;
    char _49[0x7];
    int unk50;
    char _54[0xB0];
    void* unk104;
};

// MATCH-ONLY: the Subspace save fields read as the original does; gmAdvData leaves this range unnamed.
static inline u8 getAdvMode(gmAdvData* advData) {
    return reinterpret_cast<u8*>(advData)[0x604];
}

static stLoaderSandBagMarker sMarkerA(0xFF, 0);
static stLoaderSandBagMarker sMarkerB(0xFF, 1);

stLoaderInfoAdventure::stLoaderInfoAdventure() : m_archive(NULL), m_loader(NULL) { }

stLoaderInfoAdventure::~stLoaderInfoAdventure() { }

void stLoaderInfoAdventure::processBegin() {
    if ((u8)_0x45[1] != 0) {
        IfAdvMngr* advMngr = g_IfAdvMngr;
        gmAdvData* advData = g_GameGlobal->m_advData;
        if (getAdvMode(advData) != 2) {
            reinterpret_cast<stLoaderInfoAdventureIfView*>(advMngr)->unk50 =
                *reinterpret_cast<int*>(reinterpret_cast<u8*>(advData) + 0x4910);
        }
        switch (m_state) {
            case 0:
                if ((u8)_0x45[2] == 1) {
                    break;
                }
                if (m_commonLoader != NULL && !m_commonLoader->isLoad()) {
                    break;
                }
                m_state = 1;
                break;
            case 1:
                if (advMngr != NULL) {
                    m_archive = new (Heaps::InfoResource) gfArchive();
                    m_loader = new (Heaps::InfoResource) scResourceLoader();
                    fn_1_3C68(m_loader, Heaps::InfoResource, "menu2/if_adv_mngr.pac", m_archive, 0);
                    fn_80106A5C(advMngr, m_archive);
                }
                m_state = 2;
                break;
            case 2:
                if (advMngr != NULL) {
                    if (!fn_1_3E50(m_loader)) {
                        break;
                    }
                    if (!fn_80106E40(advMngr)) {
                        break;
                    }
                    if (m_commonLoader != NULL) {
                        void* res1;
                        void* res2;
                        void* res3;
                        void* res4;
                        fn_27_23B44C(m_commonLoader, &res1, &res2, &res3, &res4);
                        fn_80106A64(g_IfAdvMngr, res1, res2, res3, res4);
                    } else {
                        fn_80106A64(g_IfAdvMngr, NULL, NULL, NULL, NULL);
                    }
                    m_state = 3;
                } else {
                    m_state = 3;
                }
                break;
            case 3:
                if ((u8)_0x45[2] == 1) {
                    m_state = 5;
                }
                break;
            case 5:
                removeEntity();
                if (advMngr != NULL) {
                    fn_80106D10(advMngr);
                    if (m_archive != NULL) {
                        delete m_archive;
                        m_archive = NULL;
                    }
                    if (m_loader != NULL) {
                        delete m_loader;
                        m_loader = NULL;
                    }
                }
                if ((u8)_0x45[2] == 1) {
                    m_state = 6;
                } else {
                    m_state = 0;
                }
                break;
        }
    }
}

void stLoaderInfoAdventure::entryEntity() {
    int count = 2;
    GameGlobal* global = g_GameGlobal;
    gmAdvData* advData = global->m_advData;
    if (getAdvMode(advData) == 3) {
        gmGlobalModeMelee* melee = global->m_modeMelee;
        if (melee->m_playersInitData[2].m_state != 3) {
            count++;
        }
        if (melee->m_playersInitData[3].m_state != 3) {
            count++;
        }
    }
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->setNumPlayer(count);
        fn_80106E48(g_IfAdvMngr);
        if (getAdvMode(advData) == 2) {
            fn_8010C38C(reinterpret_cast<stLoaderInfoAdventureIfView*>(g_IfAdvMngr)->unk104);
        }
        reinterpret_cast<stLoaderInfoAdventureIfView*>(g_IfAdvMngr)->unk48_7 = false;
        g_IfAdvMngr->startMelee();
    }
}

void stLoaderInfoAdventure::removeEntity() {
    if (g_IfAdvMngr != NULL) {
        g_IfAdvMngr->destroyModel();
    }
}
