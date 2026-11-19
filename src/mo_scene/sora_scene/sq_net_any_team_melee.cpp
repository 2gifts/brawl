#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_any_team_melee.h>
#include <sr/sr_common.h>
#include <types.h>


sqNetAnyTeamMelee* sqNetAnyTeamMelee::create() {
    return new (Heaps::GlobalMode) sqNetAnyTeamMelee("sqNetAnyTeamMelee");
}

void sqNetAnyTeamMelee::start() {
    unk8 = 0;
    unkC = 0;
    unk10 = g_gfApplication->unk110[0] != 0;
    g_GameGlobal->initSelCharData();
}

void sqNetAnyTeamMelee::exit() { }

// TODO: clean up, give variables real names
namespace jj_3B754 {
struct Flags { unsigned char mode:6; unsigned char unused:2; unsigned char rule:3; unsigned char rest:5; };
extern "C" {
extern char lbl_1_data_97F0[];
extern char* g_gfApplication;
extern int g_muWifiInterfaceTask;
void gmRuleBackupForNet__Fv();
int gmGetMenuDecisionPad__Fv();
void gmRuleResetForNet__Fv();
void gmAddOnlineAnybodyTimeCount__FUl(unsigned int);
void fn_80053704(int, int);
void fn_800CCEC4(int, int);
void dropInstance__18nteUpdateEvaluatorFv();
void fn_1_3D2D8();
void fn_1_3D450();
void fn_1_3BC64(void*);
void fn_1_3BEB4(void*);
void setNextScene__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void setNextSequence__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void fn_1_3B754(char* self) {
    const char* str = lbl_1_data_97F0;
    char* p;
    gfSceneManager* mgr = gfSceneManager::getInstance();
    int cont = 1;
    while (cont) {
        cont = 0;
        switch (*(unsigned int*)(self + 8)) {
        case 0: {
            g_gfApplication[0x110] = 0;
            gmRuleBackupForNet__Fv();
            char* o = (char*)g_GameGlobal->m_setRule;
            o[2] = 0; o[3] = 2; o[5] = 0; o[6] = 10;
            o[8] = 0; o[9] = 0; o[10] = 0; o[11] = 0; o[12] = 1;
            *(int*)(self + 8) = 14;
            *(int*)(self + 12) = 1;
            self[17] = 13;
            cont = 1;
            break;
        }
        case 1: {
            fn_1_3D2D8();
            setNextScene__14gfSceneManagerFv(mgr, str + 0x14, 0);
            ((char*)g_GameGlobal->m_selCharData)[8] = 0;
            ((char*)g_GameGlobal->m_selCharData)[4] = 17;
            p = (char*)g_GameGlobal->m_selCharData;
            *(unsigned char*)p = gmGetMenuDecisionPad__Fv() + 1;
            *(int*)(self + 8) = 2;
            break;
        }
        case 2:
            switch (mgr->unk1) {
            default: case 0: *(int*)(self + 12) = 16; break;
            case 2: *(int*)(self + 12) = 17; break;
            case 1: *(int*)(self + 12) = 3; break;
            }
            cont = 1;
            *(int*)(self + 8) = 14;
            if (*(int*)(self + 12) == 3) self[17] = 13;
            else self[17] = 2;
            break;
        case 3:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x28, 1);
            ((char*)g_GameGlobal->m_selCharData)[4] = 17;
            *(int*)(self + 8) = 4;
            break;
        case 4:
            switch (mgr->unk1) {
            default: case 0: *(int*)(self + 8) = 16; break;
            case 2: *(int*)(self + 8) = 17; break;
            case 1: case 3: *(int*)(self + 8) = 5; break;
            }
            cont = 1;
            break;
        case 5:
            fn_1_3BC64(self);
            *(int*)(self + 8) = 14;
            cont = 1;
            self[17] = 8;
            *(int*)(self + 12) = 6;
            break;
        case 6:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x38, 0);
            *(int*)(self + 8) = 7;
            break;
        case 7:
            switch (mgr->unk1) {
            default: case 2: *(int*)(self + 8) = 8; break;
            case 3: *(int*)(self + 12) = 16; break;
            case 4: *(int*)(self + 12) = 17; break;
            case 5: *(int*)(self + 12) = 16; break;
            case 6: *(int*)(self + 12) = 17; break;
            }
            cont = 1;
            if (*(int*)(self + 8) != 8) {
                *(int*)(self + 8) = 14;
                self[17] = 2;
            }
            break;
        case 8:
            if (g_muWifiInterfaceTask == 0) {
                *(int*)(self + 8) = 17;
                cont = 1;
            } else {
                fn_1_3BEB4(self);
                *(int*)(self + 8) = 14;
                cont = 1;
                *(int*)(self + 12) = 9;
                self[17] = 0;
            }
            break;
        case 9:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x38, 0);
            *(int*)(self + 8) = 10;
            break;
        case 10:
            switch (mgr->unk1) {
            default: case 2: *(int*)(self + 8) = 11; break;
            case 3: *(int*)(self + 12) = 16; break;
            case 4: *(int*)(self + 12) = 17; break;
            case 5: *(int*)(self + 12) = 16; break;
            case 6: *(int*)(self + 12) = 17; break;
            }
            cont = 1;
            if (*(int*)(self + 8) != 11) {
                *(int*)(self + 8) = 14;
                self[17] = 2;
            }
            break;
        case 11: {
            char* p = (char*)g_GameGlobal->m_modeMelee;
            char* q = (char*)g_GameGlobal->m_resultInfo;
            ((Flags*)(p + 8))->mode = 32;
            *(short*)(p + 0x1A) = 0x28;
            *(int*)(p + 0x20) = 0;
            p[0x16] = 0;
            if (((Flags*)(p + 8))->rule == 1)
                *(int*)(q + 8) = ((unsigned char*)g_GameGlobal->m_setRule)[4];
            fn_80053704(0, 1);
            gmAddOnlineAnybodyTimeCount__FUl(*(unsigned int*)(q + 0x1384) / 60);
            *(int*)(self + 8) = 14;
            cont = 1;
            *(int*)(self + 12) = 12;
            self[17] = 3;
            break;
        }
        case 12:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x40, 0);
            *(int*)(self + 8) = 13;
            break;
        case 13:
            dropInstance__18nteUpdateEvaluatorFv();
            switch (mgr->unk1) {
            default: case 2:
                *(int*)(self + 12) = 16;
                *(int*)(self + 8) = 14;
                self[17] = 2;
                break;
            case 1:
                *(int*)(self + 12) = 1;
                *(int*)(self + 8) = 14;
                self[17] = 13;
                break;
            }
            cont = 1;
            break;
        case 16:
            fn_800CCEC4(g_muWifiInterfaceTask, 0);
            dropInstance__18nteUpdateEvaluatorFv();
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x4C, 0x1D);
            g_gfApplication[0x110] = self[16];
            *(int*)(self + 8) = 0x12;
            gmRuleResetForNet__Fv();
            break;
        case 17:
            fn_1_3D450();
            dropInstance__18nteUpdateEvaluatorFv();
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x4C, 0x23);
            g_gfApplication[0x110] = self[16];
            *(int*)(self + 8) = 0x12;
            gmRuleResetForNet__Fv();
            break;
        case 14:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x58, (unsigned char)self[17]);
            *(int*)(self + 8) = 15;
            break;
        case 15:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 12);
            break;
        default:
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x68, 0);
            *(int*)(self + 8) = 0;
            break;
        }
    }
}
}
}

