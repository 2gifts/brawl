#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_friend_homerun.h>
#include <sr/sr_common.h>
#include <types.h>


sqNetFriendHomerun* sqNetFriendHomerun::create() {
    return new (Heaps::GlobalMode) sqNetFriendHomerun("sqNetFriendHomerun");
}

void sqNetFriendHomerun::start() {
    unk8 = 0;
    g_GameGlobal->initSelCharData();
}

void sqNetFriendHomerun::exit() { }

// TODO: clean up, give variables real names
namespace jj_39780 {
extern "C" {
extern const char lbl_1_data_9068[];
extern char* g_gfApplication;
extern int g_muWifiInterfaceTask;
void fn_1_3D2D8();
void fn_1_3D450();
void fn_1_39BB0(char*);
void fn_1_39DF0(char*);
void fn_1_39F9C(char*);
int gmGetMenuDecisionPad__Fv();
void gmAddOnlineFriendTimeCount__FUl(unsigned int);
void* getAssignedMeleeRule__19muWifiInterfaceTaskFv(int);
void fn_800CCEC4(int, int);
void fn_800CFE1C(int);
void setNextScene__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void setNextSequence__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void fn_1_39780(char* self) {
    const char* str = lbl_1_data_9068;
    char* p;
    gfSceneManager* mgr = gfSceneManager::getInstance();
    int cont = 1;
    while (cont) {
        cont = 0;
        switch (*(unsigned int*)(self + 8)) {
        default:
            *(int*)(self + 8) = 9;
            *(int*)(self + 0xC) = 1;
            *(unsigned char*)(self + 0x10) = 13;
            cont = 1;
            break;
        case 1:
            fn_1_3D2D8();
            g_gfApplication[0x110] = 0;
            setNextScene__14gfSceneManagerFv(mgr, str + 0x14, 0);
            *(unsigned char*)(*(char**)((char*)g_GameGlobal + 0x10) + 8) = 1;
            *(unsigned char*)(*(char**)((char*)g_GameGlobal + 0x10) + 4) = 14;
            p = *(char**)((char*)g_GameGlobal + 0x10);
            *(unsigned char*)p = gmGetMenuDecisionPad__Fv() + 1;
            *(int*)(self + 8) = 2;
            break;
        case 2:
            switch (*(int*)((char*)mgr + 0x284)) {
            case 0: *(int*)(self + 8) = 0xB; break;
            case 2: *(int*)(self + 8) = 0xC; break;
            case 1: *(int*)(self + 8) = 3; break;
            }
            cont = 1;
            break;
        case 3:
            if (g_muWifiInterfaceTask == 0) {
                *(int*)(self + 8) = 0xC;
                cont = 1;
            } else {
                fn_1_39BB0(self);
                *(int*)(self + 8) = 9;
                cont = 1;
                *(int*)(self + 0xC) = 4;
                *(unsigned char*)(self + 0x10) = 8;
            }
            break;
        case 4:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x28, 0);
            *(int*)(self + 8) = 5;
            break;
        case 5:
            switch (*(int*)((char*)mgr + 0x284)) {
            case 2:
            default: *(int*)(self + 8) = 6; break;
            case 3: *(int*)(self + 0xC) = 0xB; break;
            case 4: *(int*)(self + 0xC) = 0xC; break;
            case 5: *(int*)(self + 0xC) = 0xB; break;
            case 6: *(int*)(self + 0xC) = 0xC; break;
            }
            if (*(int*)(self + 8) != 6) {
                *(int*)(self + 8) = 9;
                *(unsigned char*)(self + 0x10) = 2;
            }
            cont = 1;
            break;
        case 6:
            fn_1_39DF0(self);
            {
                unsigned char v = *((unsigned char*)getAssignedMeleeRule__19muWifiInterfaceTaskFv(g_muWifiInterfaceTask) + 0xA);
                *(unsigned char*)(*(char**)((char*)g_GameGlobal + 0x10) + 1) = v;
            }
            *(int*)(self + 8) = 9;
            *(int*)(self + 0xC) = 7;
            *(unsigned char*)(self + 0x10) = 10;
            cont = 1;
            break;
        case 7:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x28, 0);
            *(int*)(self + 8) = 8;
            break;
        case 8:
            gmAddOnlineFriendTimeCount__FUl(*(unsigned int*)(*(char**)((char*)g_GameGlobal + 0x18) + 0x1384) / 60);
            switch (*(int*)((char*)mgr + 0x284)) {
            default:
                fn_1_39DF0(self);
                *(int*)(self + 8) = 7;
                break;
            case 1:
                if (*(unsigned char*)(*(char**)((char*)g_GameGlobal + 0x18) + 0x1378) == 9) {
                    *(int*)(self + 0xC) = 1;
                    *(unsigned char*)(self + 0x10) = 13;
                    *(int*)(self + 8) = 9;
                } else {
                    int n = *(int*)(self + 0x14) + 1;
                    *(int*)(self + 0x14) = n;
                    if (*(unsigned char*)(self + 0x11) == 0 && *(unsigned char*)(self + 0x12) == 0 && *(int*)(self + 0x18) > n) goto retry;
                    if (*(unsigned char*)(self + 0x11) != 0 || *(unsigned char*)(self + 0x12) != 0 || *(int*)(self + 0x1C) >= 1) goto quit;
                    *(int*)(self + 0x14) = 0;
                    *(int*)(self + 0x1C) = *(int*)(self + 0x1C) + 1;
                    goto retry;
                quit:
                    *(int*)(self + 0xC) = 1;
                    *(unsigned char*)(self + 0x10) = 13;
                    *(int*)(self + 8) = 9;
                    break;
                retry:
                    *(int*)(self + 8) = 7;
                    fn_1_39F9C(self);
                    fn_800CFE1C(g_muWifiInterfaceTask);
                }
                break;
            case 3:
                *(int*)(self + 0xC) = 0xB;
                *(unsigned char*)(self + 0x10) = 2;
                *(int*)(self + 8) = 9;
                break;
            case 4:
                *(int*)(self + 0xC) = 0xC;
                *(unsigned char*)(self + 0x10) = 2;
                *(int*)(self + 8) = 9;
                break;
            case 5:
                *(int*)(self + 0xC) = 0xB;
                *(unsigned char*)(self + 0x10) = 2;
                *(int*)(self + 8) = 9;
                break;
            case 6:
                *(int*)(self + 0xC) = 0xC;
                *(unsigned char*)(self + 0x10) = 2;
                *(int*)(self + 8) = 9;
                break;
            }
            cont = 1;
            break;
        case 9:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x30, *(unsigned char*)(self + 0x10));
            *(int*)(self + 8) = 10;
            break;
        case 10:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 0xC);
            break;
        case 11:
            fn_800CCEC4(g_muWifiInterfaceTask, 0);
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x40, 0x1A);
            *(int*)(self + 8) = 13;
            break;
        case 12:
            fn_1_3D450();
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x40, 0x23);
            *(int*)(self + 8) = 13;
            break;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_39DF0 {
struct GGData {
    char pad0[8];
    u8 f8 : 6;
    u8 f8b : 2;
    u8 f9 : 3;
    u8 f9b : 5;
    u8 fa0 : 1;
    u8 fa1 : 1;
    u8 fa2 : 1;
    u8 fa3 : 1;
    u8 fa4 : 4;
    u8 fb;
    u8 fc : 1;
    u8 fc1 : 7;
    char padd[0x13 - 0xd];
    u8 f13;
    char pad14[2];
    u8 f16;
    u8 pad17;
    u8 f18;
    u8 pad19;
    u16 f1a;
    char f1c[4];
    int f20;
    char pad24[0xc];
    int f30;
    int f34;
    int f38;
    int f3c;
    char pad40[0x1c];
    char f5c[4];
    u8 f60;
    u8 f61;
    u8 pad62;
    u8 f63;
    u8 f64;
    char pad65[3];
    int f68;
    int f6c;
    int f70;
    int f74;
    int f78;
    int f7c;
};

struct Self {
    char pad0[0x11];
    u8 f11;
    bool f12;
    u8 pad13;
    int f14;
    int f18;
    int f1c;
};

struct Info {
    char pad[0x20];
    u8 f20;
    u8 f21;
};

extern "C" {
extern GGData** g_GameGlobal;
extern int g_muWifiInterfaceTask;
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
int getAssignedPlayerKind__19muWifiInterfaceTaskFi(int, int);
int getAssignedHomerunRule__19muWifiInterfaceTaskFv(int);
Info* getAssignedMeleeRule__19muWifiInterfaceTaskFv(int);
void fn_8010F960(u16, void*, void*, int);
void fn_1_32F8();
void fn_1_3D2D8();
void fn_1_3A1A8(Self*);
void fn_1_39F9C(Self*);
void fn_1_39DF0(Self* self);
}

void fn_1_39DF0(Self* self) {
    GGData* gg = g_GameGlobal[2];
    int task = g_muWifiInterfaceTask;
    gmInitGlobalModeMelee__FP17gmGlobalModeMelee(gg);
    self->f14 = 0;
    self->f18 = 0;
    self->f1c = 0;
    for (int i = 0; i < 4; i++) {
        if (getAssignedPlayerKind__19muWifiInterfaceTaskFi(task, i) != 3) {
            self->f18++;
        }
    }
    self->f11 = 0;
    if (self->f12 = getAssignedHomerunRule__19muWifiInterfaceTaskFv(task) == 0) {
        fn_1_3A1A8(self);
    } else {
        fn_1_39F9C(self);
    }
    gg->f8 = 0x12;
    gg->f1a = 0x34;
    gg->f13 = self->f12 != 0;
    gg->f9 = 1;
    gg->f20 = 600;
    gg->fc = 0;
    gg->f18 = 0;
    gg->f63 = self->f11;
    gg->f64 = self->f12;
    gg->f68 = self->f14;
    gg->f6c = self->f1c;
    gg->fa3 = 1;
    gg->f70 = 0;
    gg->f74 = 0;
    gg->f78 = 0;
    gg->f7c = 0;
    gg->f34 = 0;
    gg->f30 = 0;
    gg->f38 = 0;
    gg->f3c = 0;
    gg->f16 = 0;
    Info* info = getAssignedMeleeRule__19muWifiInterfaceTaskFv(task);
    gg->f60 = info->f20;
    gg->f61 = info->f21;
    fn_8010F960(gg->f1a, gg->f1c, gg->f5c, 0);
    fn_1_32F8();
    fn_1_3D2D8();
}
}

// TODO: clean up, give variables real names
namespace jj_39F9C {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct Wifi;

struct Flags {
    u8 top : 2;
    u8 enabled : 1;
    u8 bottom : 5;
};

struct Player {
    u8 character;
    u8 state;
    u8 playerId;
    u8 playerNo;
    u8 stock;
    u8 colorFile;
    u8 color;
    u8 controller;
    u8 startPoint;
    u8 pad9[2];
    u8 team;
    u16 name[6];
    u8 nameIndex;
    u8 pad19[3];
    Flags flags;
    u8 pad1D[3 + 0x3C];
};

struct Mode {
    u8 pad0[9];
    u8 flagTop : 3;
    u8 count : 3;
    u8 flagLow : 2;
    u8 padA[0x5E];
    int selection;
    int other;
    u8 pad70[0x28];
    Player players[4];
};

struct Global {
    u8 pad0[8];
    Mode* mode;
};

struct Scene {
    u8 pad0[0x14];
    int selection;
    int pad18;
    int other;
};

extern "C" {
extern Global* g_GameGlobal;
extern Wifi* g_muWifiInterfaceTask;

int getAssignedPlayerKind__19muWifiInterfaceTaskFi(Wifi*, int);
int fn_800CB4AC(Wifi*, int);
u8 fn_800CB568(Wifi*, int);
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(unsigned int);
int getFighterColorFileNo__6muMenuFii(int, int);
int fn_800CB8F0(Wifi*, int);
int getAssignedPlayerContNum__19muWifiInterfaceTaskFi(Wifi*, int);
void getLocalPlayersOnamae__19muWifiInterfaceTaskFiPwPUc(Wifi*, int, u16*, u8*);
int getPlayerArea__19muWifiInterfaceTaskFiPUc(Wifi*, int, int*);
void getAIDName__19muWifiInterfaceTaskFUlPw(Wifi*, int, u16*);

void fn_1_39F9C(Scene* self) {
    Mode* mode = g_GameGlobal->mode;
    Mode* cursor;
    u8* nameIndex;
    u16* name;
    Flags* flags;
    Wifi* wifi = g_muWifiInterfaceTask;
    int entries[4];
    int* next = entries;
    int tmp;

    int i = 0;
    do {
        if (getAssignedPlayerKind__19muWifiInterfaceTaskFi(wifi, i) != 3) {
            *next++ = i;
        }
        i++;
    } while (i < 4);

    unsigned int selected = entries[self->selection];
    cursor = mode;
    nameIndex = &mode->players[0].nameIndex;
    name = mode->players[0].name;
    flags = &mode->players[0].flags;

    int i2 = 0;
    do {
        cursor->players[0].state = getAssignedPlayerKind__19muWifiInterfaceTaskFi(wifi, i2);
        if (cursor->players[0].state == 0) {
            cursor->players[0].character = fn_800CB4AC(wifi, i2);
            cursor->players[0].team = i2;
            next = (int*)(u32)fn_800CB568(wifi, i2);
            int kind = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(
                cursor->players[0].character);
            int file;

            if (kind == 0x28) {
                file = 0;
            } else {
                file = getFighterColorFileNo__6muMenuFii(kind, (int)next);
            }

            cursor->players[0].colorFile = file;
            cursor->players[0].color = fn_800CB568(wifi, i2);
            cursor->players[0].playerId = fn_800CB8F0(wifi, i2);
            cursor->players[0].startPoint = i2;
            cursor->players[0].controller = getAssignedPlayerContNum__19muWifiInterfaceTaskFi(wifi, i2);
            cursor->players[0].stock = -(i2 == selected);
            getLocalPlayersOnamae__19muWifiInterfaceTaskFiPwPUc(wifi, fn_800CB8F0(wifi, i2), name, nameIndex);

            if (cursor->players[0].name[0] == 0) {
                int index = getPlayerArea__19muWifiInterfaceTaskFiPUc(wifi, fn_800CB8F0(wifi, i2), &tmp);
                getAIDName__19muWifiInterfaceTaskFUlPw(wifi, (u8)index, name);
            }

            if (cursor->players[0].name[0] == 0) {
                flags->enabled = 0;
            } else {
                flags->enabled = 1;
            }
        } else {
            cursor->players[0].startPoint = 0;
        }

        i2++;
        cursor = (Mode*)((u8*)cursor + sizeof(Player));
        nameIndex += sizeof(Player);
        name = (u16*)((u8*)name + sizeof(Player));
        flags = (Flags*)((u8*)flags + sizeof(Player));
    } while (i2 < 4);

    mode->selection = self->selection;
    mode->other = self->other;
    mode->count = 2;
}
}
}

