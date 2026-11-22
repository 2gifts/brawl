#include <gf/gf_scene.h>
#include <sq/sq_to_melee.h>
#include <sr/sr_common.h>
#include <types.h>
#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <sc/sc_sel_stage.h>
#include <string.h>

extern char lbl_1_data_6F5C[0x58];
extern scSelStage* lbl_1_bss_18B4;
sqToMelee* sqToMelee::create() {
    return new (Heaps::GlobalMode) sqToMelee("sqToMelee");
}

void sqToMelee::exit() { }

void sqToMelee::start() {
    unk8 = 0;
    unk18 = g_gfApplication->unk110Value != 0;
    g_GameGlobal->initSelCharData();
    scSelStage* scene = static_cast<scSelStage*>(gfSceneManager::getInstance()->searchScene(lbl_1_data_6F5C));
    lbl_1_bss_18B4 = scene;
    scene->unk3B0 = 0;
    memset(scene->unk3B8, 0, sizeof(scene->unk3B8));
    scene->unk440 = 0;
    scene->unk438 = 0;
    g_GameGlobal->m_tournamentData->unk509 = 0;
}

// TODO: clean up, give variables real names
namespace jj_22D3C {

struct Flags {
    unsigned char mode : 6;
    unsigned char unused : 2;
    unsigned char rule : 3;
    unsigned char rest : 5;
};
extern "C" {
void setNextScene__14gfSceneManagerFv(void*, const char*, int);
void setNextSequence__14gfSceneManagerFv(void*, const char*, int);
void fn_1_23270(void*);
void fn_80053704(int, int);
extern const char lbl_1_data_6F50[];
extern unsigned char lbl_1_bss_18B0;
extern char* lbl_1_bss_18B4;
extern char* g_gfApplication;

void fn_1_22D3C(char* self) {
    const char* str = lbl_1_data_6F50;
    gfSceneManager* mgr = gfSceneManager::getInstance();
    int cont = 1;
    while (cont) {
        cont = 0;
        switch (*(unsigned int*)(self + 8)) {
        default:
            *(int*)(self + 8) = 0;
        case 0:
            g_gfApplication[0x110] = 0;
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 0xc) = 3;
            *(int*)(self + 0x14) = 0;
            *(unsigned char*)(self + 0x10) = 13;
            break;
        case 1:
            setNextScene__14gfSceneManagerFv(mgr, str + 24, *(unsigned char*)(self + 0x10));
            *(int*)(self + 8) = 2;
            break;
        case 2:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 0xc);
            break;
        case 3:
            setNextScene__14gfSceneManagerFv(mgr, str + 40, *(int*)(self + 0x14));
            *(int*)(self + 8) = 4;
            break;
        case 4:
            switch (mgr->unk1) {
            case 0: *(int*)(self + 8) = 19; break;
            case 1: *(int*)(self + 8) = 5; break;
            case 2: *(int*)(self + 8) = 7; break;
            }
            cont = 1;
            break;
        case 5:
            setNextScene__14gfSceneManagerFv(mgr, str + 12, 0);
            *((unsigned char*)g_GameGlobal->m_selCharData + 4) = 3;
            *(unsigned char*)(lbl_1_bss_18B4 + 0x448) = 1;
            *(int*)(lbl_1_bss_18B4 + 0x444) = 0;
            *(int*)(self + 8) = 6;
            break;
        case 6:
            switch (mgr->unk1) {
            case 0:
                *(int*)(self + 8) = 3;
                *(int*)(self + 0x14) = 1;
                break;
            case 1:
            case 3: {
                int flag = mgr->unk1 == 3;
                lbl_1_bss_18B0 = flag;
                *((unsigned char*)g_GameGlobal->m_tournamentData + 0x50a) = flag;
                *(int*)(self + 8) = 3;
                *(int*)(self + 0x14) = 2;
                break;
            }
            }
            cont = 1;
            break;
        case 7:
            *(int*)(self + 8) = 8;
            cont = 1;
            *(int*)(self + 0x14) = 0;
            break;
        case 8:
            setNextScene__14gfSceneManagerFv(mgr, str + 52, *(int*)(self + 0x14));
            *(int*)(self + 8) = 9;
            break;
        case 9:
            switch (mgr->unk1) {
            case 0:
                switch (*((unsigned char*)g_GameGlobal->m_tournamentData + 5)) {
                case 0:
                    *(int*)(self + 8) = 10;
                    *(int*)(lbl_1_bss_18B4 + 0x444) = 1;
                    break;
                case 1: {
                    unsigned char* p = (unsigned char*)g_GameGlobal->m_selCharData;
                    int missing = 0;
                    if (p[0xb9] == 0) missing = 1;
                    else if (p[0x115] == 0) missing = 1;
                    else if (p[0x171] == 0) missing = 1;
                    else if (p[0x1cd] == 0) missing = 1;
                    if (missing == 0) {
                        *(int*)(self + 8) = 10;
                        *(int*)(lbl_1_bss_18B4 + 0x444) = 1;
                    } else {
                        *(int*)(self + 8) = 10;
                        *(int*)(lbl_1_bss_18B4 + 0x444) = 0;
                    }
                    break;
                }
                case 2:
                    *(int*)(self + 8) = 13;
                    *(int*)(lbl_1_bss_18B4 + 0x444) = 0;
                    break;
                }
                break;
            case 2:
                *(int*)(self + 8) = 3;
                *(int*)(self + 0x14) = 3;
                break;
            case 1:
                *(int*)(self + 8) = 3;
                *(int*)(self + 0x14) = 2;
                break;
            case 3:
                *(int*)(self + 8) = 19;
                break;
            }
            cont = 1;
            break;
        case 10:
            cont = 1;
            *(int*)(self + 8) = 11;
            break;
        case 11:
            setNextScene__14gfSceneManagerFv(mgr, str + 12, 0);
            *((unsigned char*)g_GameGlobal->m_selCharData + 4) = 3;
            *(unsigned char*)(lbl_1_bss_18B4 + 0x448) = 0;
            *(int*)(self + 8) = 12;
            break;
        case 12:
            switch (mgr->unk1) {
            case 0:
                *(short*)((char*)g_GameGlobal->m_selStageData + 0x22) = 1;
            case 1:
            case 3: {
                int flag = mgr->unk1 == 3;
                lbl_1_bss_18B0 = flag;
                *((unsigned char*)g_GameGlobal->m_tournamentData + 0x50a) = flag;
                *(int*)(self + 8) = 13;
                break;
            }
            }
            cont = 1;
            break;
        case 13:
            fn_1_23270(self);
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 0xc) = 14;
            *(unsigned char*)(self + 0x10) = 0;
            break;
        case 14:
            g_gfApplication[0x110] = *(unsigned char*)(self + 0x18);
            setNextScene__14gfSceneManagerFv(mgr, str + 64, 0);
            *(int*)(self + 8) = 15;
            break;
        case 15:
            cont = 1;
            g_gfApplication[0x110] = 0;
            *(int*)(self + 8) = 16;
            break;
        case 16: {
            char* p = (char*)g_GameGlobal->m_modeMelee;
            char* result = (char*)g_GameGlobal->m_resultInfo;
            ((Flags*)(p + 8))->mode = 32;
            *(short*)(p + 0x1a) = 40;
            *(int*)(p + 0x20) = 0;
            p[0x16] = 0;
            if (((Flags*)(p + 8))->rule == 1) {
                *(int*)(result + 8) = *((unsigned char*)g_GameGlobal->m_setRule + 4);
            }
            fn_80053704(0, 0);
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 0xc) = 17;
            *(unsigned char*)(self + 0x10) = 3;
            break;
        }
        case 17:
            setNextScene__14gfSceneManagerFv(mgr, str + 72, 0);
            *(int*)(self + 8) = 18;
            break;
        case 18:
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 0xc) = 8;
            *(int*)(self + 0x14) = 1;
            *(unsigned char*)(self + 0x10) = 13;
            break;
        case 19:
            g_gfApplication[0x110] = *(unsigned char*)(self + 0x18);
            setNextSequence__14gfSceneManagerFv(mgr, str + 84, 4);
            *(unsigned char*)(lbl_1_bss_18B4 + 0x448) = 1;
            *(int*)(lbl_1_bss_18B4 + 0x444) = 0;
            { char* p = lbl_1_bss_18B4;
            *(int*)(p + 0x35c) = -1;
            *(int*)(p + 0x360) = 0xf0; }
            *(int*)(self + 8) = 20;
            break;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_23270 {
extern "C" {
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
void gmSetRuleToGlobalModeMelee__17gmGlobalModeMeleeFv(void*);
void fn_80055760(u32, s32);
gmItSwitch* gmGetItemSwitchData_NormalStage__Fi(u32);
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
int getFighterColorFileNo__6muMenuFii(int, int);
void fn_8010F960(u16, void*, void*, int);
extern u8 lbl_1_bss_18B0;
}
struct SwitchData { u64 item; u32 pokemon; u32 assist; };
struct ModeData {
 u8 pad0[8]; u8 gameMode:6; u8 modeRest:2;
 u8 rule:3; u8 count:3; u8 rest:2;
 u8 padA[9]; u8 teams; u8 pad14[2]; u8 frequency; u8 pad17[3]; u16 stage;
 u8 pad1C[0x14]; SwitchData switches; u8 pad40[0x58];
 struct Player { u8 kind, state, id, no; s8 stocks; u8 file, color, controller, start; u8 pad9[2]; u8 team; u8 padC[0x16]; u16 value; u8 pad24[0x38]; } players[4];
};
struct SelectData { u8 pad0[0x33]; u8 teams; u8 pad34[0x84]; ModeData::Player players[4]; };
struct StageData { u8 pad[0x22]; u16 stage; u8 substage; };
extern "C" void fn_1_23270() {
 ModeData* mode=(ModeData*)g_GameGlobal->m_modeMelee;
 SelectData* select=(SelectData*)g_GameGlobal->m_selCharData;
 StageData* stageSelect=(StageData*)g_GameGlobal->m_selStageData;
 gmGlobalRecord::MenuData* record=g_GameGlobal->getGlobalRecordMenuDatap();
 gmInitGlobalModeMelee__FP17gmGlobalModeMelee(mode); gmSetRuleToGlobalModeMelee__17gmGlobalModeMeleeFv(mode);
 u16 stage=lbl_1_bss_18B0==0?stageSelect->stage:0x35;
 mode->stage=stage;
 fn_80055760(stage,stageSelect->substage);
 mode->gameMode=1;
 mode->teams=select->teams;
 mode->switches=*(SwitchData*)gmGetItemSwitchData_NormalStage__Fi(mode->stage);
 mode->switches.item &= *(u64*)&record->m_itemSwitch;
 mode->frequency=record->m_itemFrequency;
 mode->count=0;
 ModeData* output=mode;
 for(int i=0;i<4;++i,select=(SelectData*)((u8*)select+0x5c),output=(ModeData*)((u8*)output+0x5c)) {
  if(select->players[0].state!=3) {
   int character=select->players[0].kind;
   int costume=select->players[0].file;
   output->players[0].state=select->players[0].state;
   output->players[0].kind=character;
   output->players[0].team=select->players[0].team;
   output->players[0].color=costume;
   int kind=exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(character);
   output->players[0].file=kind==0x28?0:getFighterColorFileNo__6muMenuFii(kind,costume);
   output->players[0].id=i;
   output->players[0].start=mode->count;
   output->players[0].value=*(u16*)((u8*)select+0xde);
   if(mode->rule==1) output->players[0].stocks=g_GameGlobal->m_setRule->m_stockCount;
   mode->count++;
  }
 }
 fn_8010F960(mode->stage,(u8*)mode+0x1c,(u8*)mode+0x5c,0);
}
}

