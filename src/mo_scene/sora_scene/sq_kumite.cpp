#include <gf/gf_scene.h>
#include <sq/sq_kumite.h>
#include <sr/sr_common.h>
#include <types.h>

sqKumite* sqKumite::create() {
    return new (Heaps::GlobalMode) sqKumite("sqKumite");
}

void sqKumite::exit() { }

// TODO: clean up, give variables real names
namespace jj_35220 {
extern "C" {
void* getInstance__14gfSceneManagerFv();
void initSelCharData__10GameGlobalFv(void* game);
extern void* g_GameGlobal;
extern void* g_gfApplication;
extern char lbl_1_bss_1B68;
}

extern "C" void fn_1_35220(char* self) {
    char* mgr = (char*)getInstance__14gfSceneManagerFv();
    *(int*)(self + 0x14) = *(int*)(mgr + 0x31c);
    lbl_1_bss_1B68 = 0;
    if (*(int*)(self + 0x14) == 0xff) {
        *(int*)(self + 8) = 0xa;
    } else {
        *(int*)(self + 8) = 0;
        *(unsigned char*)(self + 0x11) = (*(unsigned char*)((char*)g_gfApplication + 0x110) != 0);
        initSelCharData__10GameGlobalFv(g_GameGlobal);
        int val = *(int*)(self + 0x14);
        char* ptr = (char*)(*(void**)((char*)g_GameGlobal + 0x10));
        switch (val) {
            default: ptr[2] = 0; break;
            case 1: ptr[2] = 1; break;
            case 2: ptr[2] = 2; break;
            case 3: ptr[2] = 3; break;
            case 4: ptr[2] = 4; break;
            case 5: ptr[2] = 5; break;
        }
    }
    *(unsigned char*)(self + 0x18) = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_35E34 {
struct SceneData {
    unsigned char pad0[0x250];
    int value;
    int pad254;
    int total;
    unsigned char pad25c[0x4fc - 0x25c];
    int extra;
    unsigned char pad500[0x1378 - 0x500];
    unsigned char mode;
};
struct Flags { unsigned char pad[9]; unsigned char bits; };
struct Settings { unsigned char pad[2]; unsigned char kind; };
struct GameGlobal {
    unsigned char pad0[8];
    Flags* flags;
    unsigned char padc[4];
    Settings* settings;
    unsigned char pad14[4];
    SceneData* data;
};
extern "C" {
extern GameGlobal* g_GameGlobal;
int gmCountMeleeCoin__Fii(int, int);
void fn_80051440(int);
void fn_1_35E34() {
    int type = 2;
    SceneData* data = g_GameGlobal->data;
    int value = data->value;
    unsigned int special;
    unsigned int bits = (g_GameGlobal->flags->bits >> 2) & 7;
    if (bits == 2) {
        type = 3;
        value += data->extra;
    }
    special = 0;
    switch (g_GameGlobal->settings->kind) {
    case 4:
    case 5:
        break;
    case 0:
    case 1:
        if (data->mode == 5) special = 1;
        break;
    case 2:
    case 3:
        if (data->mode == 1) special = 1;
        break;
    }
    if (special == 1) {
        type = 0;
        if (bits == 2) type = 1;
    }
    int amount = gmCountMeleeCoin__Fii(type, value);
    if (amount < 1) amount = 1;
    data->total += amount;
    fn_80051440(data->total);
}
}
}

