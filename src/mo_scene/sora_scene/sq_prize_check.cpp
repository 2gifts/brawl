#include <gf/gf_scene.h>
#include <sq/sq_prize_check.h>
#include <sr/sr_common.h>
#include <types.h>

sqPrizeCheck* sqPrizeCheck::create() {
    return new (Heaps::GlobalMode) sqPrizeCheck("sqPrizeCheck");
}

void sqPrizeCheck::exit() { }

bool sqPrizeCheck::checkModeMeleeClear() {
    bool result = false;
    switch (unk14) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 6:
    case 7:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 21:
        result = true;
        break;
    }
    return result;
}

// TODO: clean up, give variables real names
namespace jj_3A52C {
typedef int s32;
typedef unsigned int u32;

extern "C" {
    void* getInstance__14gfSceneManagerFv();
    void gmCheckChallengerToVsMelee__Fv(void);
    void gmCheckHideCondition__Fv(void);
    extern void* lbl_1_bss_1B74;
    extern unsigned char lbl_1_bss_1B84;
}

extern "C" void fn_1_3A52C(s32* self) {
    s32 temp_r4;

    self[6] = *(s32*)((char*)getInstance__14gfSceneManagerFv() + 0x31C);
    lbl_1_bss_1B84 = 0;
    if (lbl_1_bss_1B74 == 0) {
        lbl_1_bss_1B74 = getInstance__14gfSceneManagerFv();
    }
    temp_r4 = self[6];
    switch (temp_r4) {
    case 23:
        self[2] = 6;
        gmCheckHideCondition__Fv();
        return;
    case 1:
    case 3:
    case 4:
    case 5:
    case 7:
    case 9:
    case 12:
    case 15:
    case 17:
    case 19:
    case 21:
    case 22:
        self[5] = temp_r4;
        gmCheckHideCondition__Fv();
        self[2] = 6;
        return;
    case 18:
    case 20:
        self[5] = temp_r4;
        gmCheckHideCondition__Fv();
        self[2] = 0xA;
        return;
    case 0:
    case 2:
        self[5] = temp_r4;
        gmCheckHideCondition__Fv();
        gmCheckChallengerToVsMelee__Fv();
        self[2] = 0;
        return;
    default:
        self[5] = temp_r4;
        gmCheckHideCondition__Fv();
        self[2] = 0;
        return;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_3AEB8 {
struct Entry { int type; int value; int extra; };
struct Partial { int type; int value; int extra; };
struct State { char pad[0x370]; int count; Entry entries[16]; };
extern "C" State* lbl_1_bss_1B78;
struct Table14 { int v[14]; };
struct Table11 { int v[11]; };
extern "C" Table14 lbl_1_rodata_2410;
extern "C" Table11 lbl_1_rodata_2448;
static inline bool push(State* s, const Partial& e) {
    if (s->count >= 16) return false;
    s->entries[s->count++] = *(const Entry*)&e;
    return true;
}
extern "C" int fn_1_3AEB8(int id) {
    int result;
    if (id >= 0 && id <= 13) {
        Table14 t = lbl_1_rodata_2410;
        Partial e;
        e.type = 3;
        e.value = t.v[id];
        result = push(lbl_1_bss_1B78, e);
    } else if (id >= 0x75 && id <= 0x7f) {
        Table11 t = lbl_1_rodata_2448;
        Partial e;
        e.type = 4;
        e.value = t.v[id - 0x75];
        result = push(lbl_1_bss_1B78, e);
    } else {
        Partial e;
        e.type = 4;
        e.value = id;
        result = push(lbl_1_bss_1B78, e);
    }
    return result;
}
}

