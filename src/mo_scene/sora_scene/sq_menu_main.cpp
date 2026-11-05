#include <gf/gf_scene.h>
#include <sq/sq_menu_main.h>
#include <sr/sr_common.h>
#include <types.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>

sqMenuMain* sqMenuMain::create() {
    return new (Heaps::GlobalMode) sqMenuMain("sqMenuMain");
}

void sqMenuMain::start() {
    gfSceneManager::getInstance();
    unk8 = 9;
    unk10 = -1;
    unkC = -1;
}

void sqMenuMain::exit() { }

void sqMenuMain::setNext() {
    gfSceneManager* manager = gfSceneManager::getInstance();
    gmInitGlobalModeMelee(g_GameGlobal->m_modeMelee);
    int repeat = 1;
    int menuLayout;
    while (repeat) {
        repeat = 0;
        switch (unk8) {
        case 9:
            manager->setNextScene("scMemoryChange", 2);
            unk8 = 0;
            break;
        default: {
            unkC = manager->unk31C;
            if (unkC == 42) {
                unk8 = 1;
                unk10 = 35;
                manager->unk31C = 0;
                repeat = 1;
                break;
            }
            int layout;
            switch (unkC) {
            default:
            case 0: layout = 0; break;
            case 1: layout = 1; break;
            case 2: layout = 2; break;
            case 3: layout = 3; break;
            case 4: layout = 4; break;
            case 5: layout = 5; break;
            case 6: layout = 6; break;
            case 7: layout = 7; break;
            case 8: layout = 8; break;
            case 9: layout = 9; break;
            case 10: layout = 10; break;
            case 11: layout = 11; break;
            case 12: layout = 12; break;
            case 13: layout = 13; break;
            case 14: layout = 14; break;
            case 15: layout = 15; break;
            case 16: layout = 16; break;
            case 17: layout = 17; break;
            case 18: layout = 18; break;
            case 19: layout = 19; break;
            case 20: layout = 20; break;
            case 21: layout = 21; break;
            case 22: layout = 22; break;
            case 23: layout = 24; break;
            case 24: layout = 25; break;
            case 25: layout = 26; break;
            case 26: layout = 27; break;
            case 27: layout = 29; break;
            case 28: layout = 30; break;
            case 29: layout = 31; break;
            case 30: layout = 34; break;
            case 31: layout = 35; break;
            case 33: layout = 38; break;
            case 32: layout = 37; break;
            case 34: layout = 39; break;
            case 35: layout = 40; break;
            case 36: layout = 41; break;
            case 37: layout = 42; break;
            case 38: layout = 43; break;
            case 39: layout = 44; break;
            case 40: layout = 45; break;
            case 41: layout = 46; break;
            }
            manager->setNextScene("muMenuMain", layout);
            unk8 = 1;
            break;
        }
        case 1: {
            int selection;
            if (unk10 != -1) {
                selection = unk10;
            } else {
                selection = manager->unk1;
            }
            unk10 = -1;
            switch (selection) {
            case 0:
                manager->setNextSequence("sqTitle", 1);
                unk8 = 10;
                break;
            case 1:
                manager->setNextSequence("sqVsMelee", 0);
                unk8 = 10;
                break;
            case 2:
                manager->setNextSequence("sqToMelee", 0);
                unk8 = 10;
                break;
            case 3:
                manager->setNextSequence("sqSpMelee", 0);
                unk8 = 10;
                break;
            case 4:
                manager->setNextSequence("sqSingleSimple", 0);
                unk8 = 10;
                break;
            case 5:
                manager->setNextSequence("sqAdventure", 0);
                unk8 = 10;
                break;
            case 6:
                manager->setNextSequence("sqSingleAllstar", 0);
                unk8 = 10;
                break;
            case 7:
                manager->setNextSequence("sqSingleBoss", 0);
                unk8 = 10;
                break;
            case 8:
                manager->setNextSequence("sqEvent", 0);
                unk8 = 10;
                break;
            case 9:
                manager->setNextSequence("sqTargetBreak", 0);
                unk8 = 10;
                break;
            case 10:
                manager->setNextSequence("sqHomerun", 0);
                unk8 = 10;
                break;
            case 11:
            case 37:
            case 38:
            case 39:
            case 40:
            case 41: {
                int kind = 0;
                switch (manager->unk1) {
                case 11: kind = 0; break;
                case 37: kind = 1; break;
                case 38: kind = 2; break;
                case 39: kind = 3; break;
                case 40: kind = 4; break;
                case 41: kind = 5; break;
                }
                manager->setNextSequence("sqKumite", kind);
                unk8 = 10;
                break;
            }
            case 12:
                manager->setNextSequence("sqTraining", 0);
                unk8 = 10;
                break;
            case 13:
                manager->setNextSequence("sqEdit", 0);
                unk8 = 10;
                break;
            case 14:
                manager->setNextSequence("sqReplay", 0);
                unk8 = 10;
                break;
            case 15:
                manager->setNextScene("scSealList", 0);
                unk8 = 2;
                break;
            case 16:
                manager->setNextScene("scSealDisp", 0);
                unk8 = 3;
                break;
            case 17:
                manager->setNextScene("scTyFigList", 0);
                unk8 = 4;
                break;
            case 18:
                manager->setNextSequence("sqTyFigDisp", 0);
                unk8 = 10;
                break;
            case 19:
                manager->setNextSequence("sqCoinShooter", 0);
                unk8 = 10;
                break;
            case 20:
                manager->setNextScene("scClearGetter", 0);
                unk8 = 5;
                break;
            case 21:
                manager->setNextScene("scSnapShot", 0);
                unk8 = 6;
                break;
            case 22:
                manager->setNextSequence("sqButton", 0);
                unk8 = 10;
                break;
            case 35:
                manager->setNextScene("scVCTrial", 0);
                unk8 = 7;
                break;
            case 36:
                manager->setNextScene("scSoftList", 0);
                unk8 = 8;
                break;
            case 24:
            case 25:
            case 26:
            case 27:
                manager->setNextSequence("sqNetFriendList", manager->unk1);
                unk8 = 10;
                break;
            case 29:
                manager->setNextSequence("sqNetAnyWatch", 0);
                unk8 = 10;
                break;
            case 30:
                manager->setNextSequence("sqNetAnyOkiraku", 0);
                unk8 = 10;
                break;
            case 31:
                manager->setNextSequence("sqNetAnyTeamMelee", 0);
                unk8 = 10;
                break;
            case 34:
                manager->setNextSequence("sqQuMelee", 0);
                unk8 = 10;
                break;
            default:
                manager->setNextSequence("sqDebugDefault", 0);
                unk8 = 10;
                break;
            }
            break;
        }
        case 5:
            manager->setNextSequence("sqPrizeCheck", 18);
            unk8 = 10;
            break;
        case 2:
        case 3:
        case 4:
        case 6:
        case 7:
        case 8:
            switch (unk8) {
            case 2: menuLayout = 15; break;
            case 3: menuLayout = 16; break;
            case 4: menuLayout = 17; break;
            case 6: menuLayout = 21; break;
            case 7: menuLayout = 38; break;
            case 8: menuLayout = 39; break;
            }
            manager->setNextScene("muMenuMain", menuLayout);
            unk8 = 1;
            break;
        }
    }
}
