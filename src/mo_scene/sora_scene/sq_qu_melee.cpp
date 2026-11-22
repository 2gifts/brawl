#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_qu_melee.h>
#include <sr/sr_common.h>
#include <types.h>


static u8 s_handicap;

sqQuMelee* sqQuMelee::create() {
    return new (Heaps::GlobalMode) sqQuMelee("sqQuMelee");
}

void sqQuMelee::start() {
    unk8 = 0;
    unk18 = g_gfApplication->unk110[0] != 0;
    g_GameGlobal->initSelCharData();
    s_handicap = g_GameGlobal->m_setRule->m_handicap;
    g_GameGlobal->m_setRule->m_handicap = 0;
}

void sqQuMelee::exit() {
    g_gfApplication->unk110[0] = unk18;
}

// TODO: clean up, give variables real names
namespace jj_2442C {
struct Flags {
    unsigned char mode : 6;
    unsigned char unused : 2;
    unsigned char rule : 3;
    unsigned char rest : 5;
};
extern "C" {
void setNextScene__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void setNextSequence__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void fn_1_3D2D8();
void fn_1_24860(void*, int);
void fn_80053704(int, int);
extern const char lbl_1_data_7170[];
extern unsigned char lbl_1_bss_18C0;
extern char* g_gfApplication;
void fn_1_2442C(char* self) {
    const char* str = lbl_1_data_7170;
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
            *(int*)(self + 12) = 3;
            *(int*)(self + 20) = 0;
            *(unsigned char*)(self + 16) = 2;
            cont = 1;
            break;
        case 1:
            setNextScene__14gfSceneManagerFv(mgr, str + 12, *(unsigned char*)(self + 16));
            *(int*)(self + 8) = 2;
            break;
        case 2:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 12);
            break;
        case 3:
            setNextScene__14gfSceneManagerFv(mgr, str + 28, *(int*)(self + 20));
            *(int*)(self + 8) = 4;
            break;
        case 4:
            switch (mgr->unk1) {
            case 0: *(int*)(self + 8) = 18; break;
            case 1: *(int*)(self + 8) = 5; *(int*)(self + 20) = 0; break;
            }
            cont = 1;
            break;
        case 5:
            setNextScene__14gfSceneManagerFv(mgr, str + 40, *(int*)(self + 20));
            *(int*)(self + 8) = 6;
            break;
        case 6:
            switch (mgr->unk1) {
            case 0: *(int*)(self + 8) = 3; *(int*)(self + 20) = 1; break;
            case 1: *(int*)(self + 8) = 7; break;
            }
            cont = 1;
            break;
        case 7:
            ((char*)g_GameGlobal->m_selCharData)[3] = ((char*)g_GameGlobal->m_tournamentData)[2];
            ((char*)g_GameGlobal->m_selCharData)[4] = 1;
            ((char*)g_GameGlobal->m_selCharData)[8] = 0;
            *(int*)(self + 8) = 1;
            *(int*)(self + 12) = 8;
            *(unsigned char*)(self + 16) = 13;
            cont = 1;
            break;
        case 8:
            fn_1_3D2D8();
            setNextScene__14gfSceneManagerFv(mgr, str + 52, 0);
            *(int*)(self + 8) = 9;
            break;
        case 9:
            switch (mgr->unk1) {
            case 0:
                *(int*)(self + 8) = 5;
                *(int*)(self + 20) = ((unsigned char*)g_GameGlobal->m_selCharData)[5] + 2;
                break;
            case 1:
                *(int*)(self + 8) = 10;
                ((char*)g_GameGlobal->m_tournamentData)[2] = ((char*)g_GameGlobal->m_selCharData)[3];
                break;
            }
            cont = 1;
            break;
        case 10:
            setNextScene__14gfSceneManagerFv(mgr, str + 72, 0);
            ((char*)g_GameGlobal->m_selCharData)[4] = 1;
            *(int*)(self + 8) = 11;
            break;
        case 11:
            switch (mgr->unk1) {
            case 0:
                ((char*)g_GameGlobal->m_selCharData)[3] = ((char*)g_GameGlobal->m_tournamentData)[2];
                ((char*)g_GameGlobal->m_selCharData)[4] = 1;
                ((char*)g_GameGlobal->m_selCharData)[8] = 0;
                *(int*)(self + 8) = 8;
                break;
            case 1:
            case 3: {
                int val = 0;
                if (mgr->unk1 == 3) val = 0x35;
                *(int*)(self + 8) = 12;
                fn_1_24860(self, val);
                break;
            }
            case 2: break;
            }
            cont = 1;
            break;
        case 12:
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 12) = 13;
            *(unsigned char*)(self + 16) = 0;
            break;
        case 13:
            g_gfApplication[0x110] = self[24];
            setNextScene__14gfSceneManagerFv(mgr, str + 88, 0);
            *(int*)(self + 8) = 14;
            break;
        case 14:
            *(int*)(self + 8) = 15;
            cont = 1;
            g_gfApplication[0x110] = 0;
            break;
        case 15: {
            char* p = (char*)g_GameGlobal->m_modeMelee;
            char* result = (char*)g_GameGlobal->m_resultInfo;
            ((Flags*)(p + 8))->mode = 32;
            *(short*)(p + 0x1a) = 40;
            *(int*)(p + 0x20) = 0;
            p[0x16] = 0;
            if (((Flags*)(p + 8))->rule == 1)
                *(int*)(result + 8) = ((unsigned char*)g_GameGlobal->m_setRule)[4];
            fn_80053704(0, 0);
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 12) = 16;
            *(unsigned char*)(self + 16) = 3;
            break;
        }
        case 16:
            setNextScene__14gfSceneManagerFv(mgr, str + 96, 0);
            *(int*)(self + 8) = 17;
            break;
        case 17:
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 12) = 5;
            *(int*)(self + 20) = 1;
            *(unsigned char*)(self + 16) = 2;
            break;
        case 18:
            ((char*)g_GameGlobal->m_setRule)[5] = lbl_1_bss_18C0;
            setNextSequence__14gfSceneManagerFv(mgr, str + 108, 5);
            *(int*)(self + 8) = 19;
            break;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_24860 {
extern "C" {
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
void gmSetRuleToGlobalModeMelee__17gmGlobalModeMeleeFv(void*);
void fn_80055760(u16,u8);
u8* gmGetItemSwitchData_NormalStage__Fi(u16);
void fn_8010F960(u16,void*,void*,int);
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
s8 getFighterColorFileNo__6muMenuFii(int,int);
}
struct Flags { u8 pad[8]; u8 mode:6, low:2; u8 rule:3, count:3, rest:2; };
extern "C" void fn_1_24860(int unused, s32 arg) {
 u8 *obj=(u8*)g_GameGlobal->m_modeMelee;
 u8 *src=(u8*)g_GameGlobal->m_selCharData;
 u8 *sel=(u8*)g_GameGlobal->m_selStageData;
 u8 *rec=(u8*)g_GameGlobal->getGlobalRecordMenuDatap();
 gmInitGlobalModeMelee__FP17gmGlobalModeMelee(obj); gmSetRuleToGlobalModeMelee__17gmGlobalModeMeleeFv(obj);
 ((Flags*)obj)->mode=0;
 ((Flags*)obj)->rule=((u8*)g_GameGlobal->m_setRule)[2];
 if(arg==0) arg=*(u16*)(sel+0x22);
 *(s16*)(obj+0x1a)=arg;
 fn_80055760((u16)arg,sel[0x24]);
 u8 *p=gmGetItemSwitchData_NormalStage__Fi(*(u16*)(obj+0x1a));
 *(u64*)(obj+0x30)=*(u64*)p;
 *(s32*)(obj+0x38)=*(s32*)(p+8);
 *(s32*)(obj+0x3c)=*(s32*)(p+12);
 *(u64*)(obj+0x30)&=*(u64*)(rec+8);
 obj[0x16]=rec[0];
 ((Flags*)obj)->count=0;
 u16 zero=0;
 s8 *dst=(s8*)obj;
 int i=0;
 do {
  if(src[0xb9]!=3) {
   int kind=src[0xb8];
   dst[0x98]=kind;
   dst[0x99]=src[0xb9];
   dst[0xa3]=src[0xc3];
   u8 tmp=src[0xbd];
   int m=exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
   s8 v;
   if(m==0x28) v=0; else v=getFighterColorFileNo__6muMenuFii(m,tmp);
   dst[0x9d]=v;
   dst[0x9e]=src[0xbd];
   dst[0xb0]=src[0xd0];
   dst[0x9f]=src[0xbf];
   dst[0x9a]=i;
   dst[0xa0]=((Flags*)obj)->count;
   dst[0x9c]=-1;
   *(u16*)(dst+0xba)=zero;
   if(((Flags*)obj)->rule==1) dst[0x9c]=((u8*)g_GameGlobal->m_setRule)[4];
   ((Flags*)obj)->count++;
  }
  i++;
  src+=0x5c;
  dst+=0x5c;
 } while(i<4);
 fn_8010F960(*(u16*)(obj+0x1a),obj+0x1c,obj+0x5c,0);
}
}

