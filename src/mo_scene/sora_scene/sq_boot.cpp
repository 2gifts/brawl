#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <sq/sq_boot.h>
#include <sr/sr_common.h>
#include <types.h>

sqBoot* sqBoot::create() {
    return new (Heaps::GlobalMode) sqBoot("sqBoot");
}

void sqBoot::start() {
    unk14 = gfSceneManager::getInstance()->unk31C;
    if ((g_gfApplication->m_e4 >> 24) == 1) {
        unk14 = 2;
    }
    unk8 = 0;
}

// The sequence of the boot: the strap scene, the memory check, the main menu (or the prize check on the first boots).
void sqBoot::setNext() {
    gfSceneManager* mgr = gfSceneManager::getInstance();
    bool again = true;
    while (again) {
        again = false;
        switch (unk8) {
        default:
        case 0:
            unk8 = 5;
            again = true;
            break;
        case 1:
            mgr->setNextScene("scMemoryChange", unk10);
            unk8 = 2;
            break;
        case 2:
            again = true;
            unk8 = unkC;
            break;
        case 5:
            mgr->setNextScene("scStrap", 0);
            unk8 = 6;
            break;
        case 6:
            unk8 = 1;
            again = true;
            unkC = 3;
            unk10 = 2;
            break;
        case 3:
            mgr->setNextScene("scBoot", 0);
            unk8 = 4;
            break;
        case 4:
            if (unk14 == 2) {
                unk8 = 8;
            } else {
                unk8 = 7;
            }
            again = true;
            break;
        case 8:
            g_GameGlobal->m_appData->_0x0[0x28] |= 0x40;
            mgr->setNextSequence("sqMenuMain", 0x2A);
            break;
        case 7: {
            u32* bootCount = g_GameGlobal->getGlobalRecordBootCountDatap();
            if (*bootCount < 99999999) {
                *bootCount = *bootCount + 1;
            }
            g_GameGlobal->m_appData->_0x0[0x28] |= 0x40;
            mgr->setNextSequence("sqPrizeCheck", 0x14);
            break;
        }
        }
    }
}

void sqBoot::exit() { }
