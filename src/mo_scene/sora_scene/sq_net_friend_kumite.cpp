#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_friend_kumite.h>
#include <sr/sr_common.h>
#include <types.h>


sqNetFriendKumite* sqNetFriendKumite::create() {
    return new (Heaps::GlobalMode) sqNetFriendKumite("sqNetFriendKumite");
}

void sqNetFriendKumite::start() {
    unk8 = 0;
    g_GameGlobal->initSelCharData();
}

void sqNetFriendKumite::exit() { }

#include <mu/wifi/mu_wifi_interface.h>
// TODO: clean up, give variables real names
namespace jj_38AB0 {


struct SceneSeq {
    char pad[8];
    int state;
    int sub;
    unsigned char flag;
};

extern "C" {
extern char lbl_1_data_8F38[];

void setNextScene__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void setNextSequence__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void fn_1_38E98(SceneSeq*);
void fn_1_390F8(SceneSeq*);
void fn_1_3964C();
void fn_1_3D2D8();
void fn_1_3D450();
int gmGetMenuDecisionPad__Fv(void*);
void gmAddOnlineFriendTimeCount__FUl(unsigned int);
void fn_800CCEC4(void*, int);
}

extern "C" void fn_1_38AB0(SceneSeq* self) {
    const char* base = lbl_1_data_8F38;
    char* p18;
    gfSceneManager* mgr = gfSceneManager::getInstance();
    int again = 1;

    while (again) {
        unsigned char c3 = 3U;
        unsigned char c3e = 0x3EU;
        again = 0;
        switch (self->state) {
        default:
            self->state = 0;
            again = 1;
            break;
        case 0:
            self->state = 1;
            again = 1;
            self->sub = 3;
            self->flag = 0xD;
            break;
        case 1:
            setNextScene__14gfSceneManagerFv(mgr, base + 0x14, self->flag);
            self->state = 2;
            break;
        case 2:
            again = 1;
            self->state = self->sub;
            break;
        case 3: {
            fn_1_3D2D8();
            setNextScene__14gfSceneManagerFv(mgr, base + 0x24, 0);
            *(unsigned char*)(*(char**)((char*)g_GameGlobal + 0x10) + 8) = 1;
            *(unsigned char*)(*(char**)((char*)g_GameGlobal + 0x10) + 4) = 0xD;
            p18 = *(char**)((char*)g_GameGlobal + 0x10);
            *p18 = gmGetMenuDecisionPad__Fv(g_GameGlobal) + 1;
            self->state = 4;
            break;
        }
        case 4:
            switch (*(int*)((char*)mgr + 0x284)) {
            case 0:
                self->state = 0xB;
                break;
            case 2:
                self->state = 0xC;
                break;
            case 1:
                self->state = 5;
                break;
            }
            again = 1;
            break;
        case 5:
            if ((s32)g_muWifiInterfaceTask == 0) {
                self->state = 0xC;
                again = 1;
            } else {
                fn_1_38E98(self);
                self->state = 1;
                again = 1;
                self->sub = 6;
                self->flag = 8;
            }
            break;
        case 6:
            setNextScene__14gfSceneManagerFv(mgr, base + 0x38, 0);
            self->state = 7;
            break;
        case 7:
            switch (*(int*)((char*)mgr + 0x284)) {
            case 2:
            default:
                self->state = 8;
                break;
            case 3:
                self->sub = 0xB;
                break;
            case 4:
                self->sub = 0xC;
                break;
            case 5:
                self->sub = 0xB;
                break;
            case 6:
                self->sub = 0xC;
                break;
            }
            if (self->state != 8) {
                self->state = 1;
                self->flag = 2;
            }
            again = 1;
            break;
        case 8:
            fn_1_390F8(self);
            self->state = 1;
            again = 1;
            self->sub = 9;
            self->flag = 9;
            break;
        case 9:
            setNextScene__14gfSceneManagerFv(mgr, base + 0x38, 0);
            self->state = 0xA;
            break;
        case 10: {
            gmAddOnlineFriendTimeCount__FUl(
                *(unsigned int*)(*(char**)((char*)g_GameGlobal + 0x18) + 0x1384) / 60);
            fn_1_3964C();
            unsigned char* d = *(unsigned char**)((char*)g_GameGlobal + 8);
            for (int i = 0; i < 7; ++i) {
                if (d[0x99 + i * 0x5C] == 1) {
                    d[0x99 + i * 0x5C] = c3;
                    d[0x98 + i * 0x5C] = c3e;
                }
            }
            switch (*(int*)((char*)mgr + 0x284)) {
            case 2:
            default:
                self->state = 0;
                break;
            case 3:
                self->sub = 0xB;
                break;
            case 4:
                self->sub = 0xC;
                break;
            case 5:
                self->sub = 0xB;
                break;
            case 6:
                self->sub = 0xC;
                break;
            }
            if (self->state != 0) {
                self->state = 1;
                self->flag = 2;
            }
            again = 1;
            break;
        }
        case 11:
            fn_800CCEC4((void*)g_muWifiInterfaceTask, 0);
            setNextSequence__14gfSceneManagerFv(mgr, base + 0x40, 0x19);
            self->state = 0xD;
            break;
        case 12:
            fn_1_3D450();
            setNextSequence__14gfSceneManagerFv(mgr, base + 0x40, 0x23);
            self->state = 0xD;
            break;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_390F8 {

struct FlagsA {
    u8 f8 : 6; u8 f8low : 2;
    u8 f9top : 3; u8 f9mid : 3; u8 f9x : 1; u8 f9last : 1;
    u8 fa0 : 1; u8 fa1 : 5; u8 fa6 : 1; u8 fa7 : 1;
    u8 fb0 : 5; u8 fb5 : 1; u8 fb6 : 2;
    u8 fc0 : 1; u8 fc1 : 6; u8 fc7 : 1;
};
struct Mode {
    u8 pad[8];
    FlagsA flags;
    u8 pad0d[6];
    u8 f13;
    u8 f14;
    u8 pad15;
    u8 f16;
    u8 pad17[3];
    u16 f1a;
    u8 f1c[4];
    int f20;
};
struct Corps { u8 a,b,c,d,e; u8 f0:1; u8 f1:1; u8 f2:6; };
struct FlagByte { u8 a:1; u8 b:1; u8 c:6; };
struct PlayerFlag { u8 a:2; u8 b:1; u8 c:5; };
struct HighFlag { u8 a:1; u8 b:7; };
struct Pair { u32 a, b; };
extern "C" {
extern int g_muWifiInterfaceTask;
extern char lbl_1_data_8FB8[], lbl_1_data_8FD8[];
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
int getAssignedKumiteType__19muWifiInterfaceTaskFv(int);
u8* takeItemSwitch__19muWifiInterfaceTaskFPSc(int, u8*);
int getAssignedDragoon__19muWifiInterfaceTaskFv(int);
u64 getComposedCharaAcceptFlag__19muWifiInterfaceTaskFv(int);
u8* getAssignedMeleeRule__19muWifiInterfaceTaskFv(int);
u8 getAssignedPlayerKind__19muWifiInterfaceTaskFi(int,int);
int fn_800CB8F0(int,int);
u8 fn_800CB4AC(int,int);
int fn_800CB568(int,int);
u8 getAssignedPlayerContNum__19muWifiInterfaceTaskFi(int,int);
void getLocalPlayersOnamae__19muWifiInterfaceTaskFiPwPUc(int,int,u16*,u8*);
void getAIDName__19muWifiInterfaceTaskFUlPw(int,u8,u16*);
u8 getPlayerArea__19muWifiInterfaceTaskFiPUc(int,int,u8*);
void OSReport(const char*, ...);
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
int getFighterColorFileNo__6muMenuFii(int,int);
void fn_8010F960(u16,void*,void*,int);
void fn_1_32F8();
void fn_1_3D2D8();
}
static inline void setMode(Mode* mode, int kind, int time, bool flag1, bool flag2) {
    mode->f14 = kind;
    mode->f20 = time;
    mode->flags.fa0 = flag1;
    mode->flags.fa6 = flag2;
    mode->flags.f9last = flag2;
    mode->flags.fb5 = 0;
    mode->flags.fc0 = 0;
    mode->flags.fc7 = 0;
}
extern "C" void fn_1_390F8() {
    int var0;
    u8 buf[2];
    Mode* mode = (Mode*)g_GameGlobal->m_modeMelee;
    Corps* corps = (Corps*)g_GameGlobal->m_corps;
    u8* cursor;
    HighFlag* p24;
    u8* p23;
    u16* p22;
    PlayerFlag* p21;
    int var20;
    gmInitGlobalModeMelee__FP17gmGlobalModeMelee(mode);
    int rule = getAssignedKumiteType__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask);
    var0 = 0;
    switch (rule) {
    case 0: setMode(mode,5,0,1,1); var0 = 10; var20 = 0x27ed; break;
    case 1: setMode(mode,6,0,1,1); var0 = 100; var20 = 0x27ee; break;
    case 2: setMode(mode,7,0x2a30,1,0); var20 = 0x26fc; break;
    case 3: setMode(mode,8,0xd2f0,1,0); var20 = 0x27ee; break;
    case 4: setMode(mode,9,0,0,0); var20 = 0x26fc; break;
    case 5: setMode(mode,10,0,0,0); var20 = 0x2709; break;
    }
    mode->flags.f8 = 0x10;
    mode->flags.f9top = 1;
    mode->f13 = 1;
    mode->flags.f9mid = 2;
    mode->f1a = 1;
    corps->a = 2;
    corps->b = 0;
    corps->c = 5;
    corps->d = var0;
    corps->e = 2;
    corps->f0 = 0;
    corps->f1 = 1;
    u8* base = (u8*)mode;
    u8* info = takeItemSwitch__19muWifiInterfaceTaskFPSc(g_muWifiInterfaceTask, &buf[1]);
    *(int*)(base+0x38) = *(int*)(info+8);
    *(int*)(base+0x3c) = *(int*)(info+12);
    *(u64*)(base+0x30) = *(u64*)info;
    ((FlagByte*)(base+0x62))->b = getAssignedDragoon__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask);
    ((FlagByte*)(base+0x62))->a = 1;
    mode->f16 = 2;
    u64 t = getComposedCharaAcceptFlag__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask);
    *(u32*)(base+0x44) = (u32)t;
    *(u32*)(base+0x40) = (u32)(t >> 32);
    info = getAssignedMeleeRule__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask);
    base[0x60] = info[0x20];
    base[0x61] = info[0x21];
    cursor = base;
    p24 = (HighFlag*)(base+0xb3);
    p23 = base+0xb0;
    p22 = (u16*)(base+0xa4);
    p21 = (PlayerFlag*)(base+0xb4);
    int task = g_muWifiInterfaceTask;
    for (int i=0;i<7;i++) {
        if (i<2) {
            u8 v=getAssignedPlayerKind__19muWifiInterfaceTaskFi(task,i);
            cursor[0x99]=v;
            if (v==5) {
                OSReport(lbl_1_data_8FB8,i,v);
                cursor[0x99]=3;
            } else if (v==3) {
                OSReport(lbl_1_data_8FB8,i,v);
            } else {
                OSReport(lbl_1_data_8FD8,i,cursor[0x99],fn_800CB8F0(task,i));
                cursor[0x98]=fn_800CB4AC(task,i);
                u8 color=fn_800CB568(task,i);
                int kind=exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(cursor[0x98]);
                int file;
                if (kind==0x28) file=0;
                else file=getFighterColorFileNo__6muMenuFii(kind,color);
                cursor[0x9d]=file;
                cursor[0x9e]=fn_800CB568(task,i);
                cursor[0x9a]=fn_800CB8F0(task,i);
                cursor[0xa0]=i;
                cursor[0x9f]=getAssignedPlayerContNum__19muWifiInterfaceTaskFi(task,i);
                cursor[0x9c]=1;
                cursor[0xa3]=0;
                p24->a = 0;
                getLocalPlayersOnamae__19muWifiInterfaceTaskFiPwPUc(task,fn_800CB8F0(task,i),p22,p23);
                if (*(u16*)(cursor+0xa4)==0)
                    getAIDName__19muWifiInterfaceTaskFUlPw(task,getPlayerArea__19muWifiInterfaceTaskFiPUc(task,fn_800CB8F0(task,i),buf),p22);
                if (*(u16*)(cursor+0xa4)==0) p21->b=0;
                else p21->b=1;
            }
        } else {
            cursor[0x99]=3;
            cursor[0x9c]=0;
            p24->a = 1;
            cursor[0xa0]=0;
        }
        cursor+=0x5c;
        p24=(HighFlag*)((u8*)p24+0x5c); p23+=0x5c; p22=(u16*)((u8*)p22+0x5c);
        p21=(PlayerFlag*)((u8*)p21+0x5c);
    }
    mode->flags.f9mid=2;
    fn_8010F960(mode->f1a,base+0x1c,base+0x5c,var20);
    fn_1_32F8();
    fn_1_3D2D8();
}
}

// TODO: clean up, give variables real names
namespace jj_3964C {
struct SceneState {
    unsigned char pad0[0x25];
    unsigned char f25;
    unsigned char pad26[0x2B - 0x26];
    unsigned char f2B;
    unsigned char pad2C[0x250 - 0x2C];
    int f250;
    unsigned char pad254[0x258 - 0x254];
    int f258;
    unsigned char pad25C[0x2D1 - 0x25C];
    unsigned char f2D1;
    unsigned char pad2D2[0x2D7 - 0x2D2];
    unsigned char f2D7;
    unsigned char pad2D8[0x4FC - 0x2D8];
    int f4FC;
    unsigned char pad500[0x1378 - 0x500];
    unsigned char f1378;
};

struct GameGlobalState {
    unsigned char pad0[0x18];
    SceneState* scene;
};

extern "C" {
extern GameGlobalState* g_GameGlobal;

int gmCountMeleeCoin__Fii(int, int);
void fn_80051440(int);

void fn_1_3964C(void) {
    SceneState* s = g_GameGlobal->scene;
    int value = 0;
    int count = 0;

    if (((s->f2B >> 4) & 1) && s->f25 == 0) {
        value = s->f250;
        count = 1;
    }
    if (((s->f2D7 >> 4) & 1) && s->f2D1 == 0) {
        count += 1;
        value += s->f4FC;
    }

    if (count > 0) {
        int type = 2;
        if (count >= 2) {
            type = 3;
        }
        if (s->f1378 == 5) {
            type = 0;
            if (count >= 2) {
                type = 1;
            }
        }
        int amount = gmCountMeleeCoin__Fii(type, value);
        if (amount < 1) {
            amount = 1;
        }
        s->f258 += amount;
        fn_80051440(s->f258);
    }
}
}
}

