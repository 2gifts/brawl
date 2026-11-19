#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_net_any_watch.h>
#include <sr/sr_common.h>
#include <types.h>

// HYPOTHESIS: cached net watch bet scene
extern gfScene* lbl_1_bss_1B90;
// HYPOTHESIS: cached value cleared on start
extern s32 lbl_1_bss_1B94;

sqNetAnyWatch* sqNetAnyWatch::create() {
    return new (Heaps::GlobalMode) sqNetAnyWatch("sqNetAnyWatch");
}

void sqNetAnyWatch::exit() { }

void sqNetAnyWatch::start() {
    unk8 = 0;
    unkC = 0;
    unk10 = g_gfApplication->unk110Value != 0;
    g_GameGlobal->initSelCharData();
    lbl_1_bss_1B90 = gfSceneManager::getInstance()->searchScene("scNetWatchBet");
    lbl_1_bss_1B94 = 0;
}

#include <mt/mt_prng.h>
// TODO: clean up, give variables real names
namespace jj_3C8E8 {

extern "C" {
extern mtRand lbl_805A0420;
extern int lbl_1_bss_1B88;
extern int lbl_1_bss_1B8C;
extern int lbl_1_data_9990[4];
extern int lbl_1_data_9960[8];
u32 fn_80051A6C(void*);
const double lbl_1_rodata_2490 = 4503599629467648.0;
const float lbl_1_rodata_2498 = 100.0f;
}

extern "C" void fn_1_3C8E8() {
    if (lbl_1_bss_1B88) {
        lbl_1_bss_1B8C = 0;
        return;
    }
    int weights[4] = {lbl_1_data_9990[0], lbl_1_data_9990[1], lbl_1_data_9990[2], lbl_1_data_9990[3]};
    int total = 0;
    for (int i = 0; i < 4; i++) {
        total += weights[i];
    }
    int value = (int)((float)total * lbl_805A0420.randf());
    int sum = 0;
    for (int i = 0; i < 4; i++) {
        if (weights[i] != 0) {
            sum += weights[i];
            if (value < sum) {
                lbl_1_bss_1B8C = i;
                return;
            }
        }
    }
}

extern "C" void fn_1_3CA24() {
    s32 temp_r0;
    s32 var_r31;
    s32 temp_r30;
    s32 temp_r29;
    s32 temp_r28;
    s32 temp_r27;
    s32 temp_r26;
    s32 temp_r25;
    s32 temp_r24;
    s32 var_r23;
    s32 var_r3;
    s32 var_r4;
    if ((s32)(lbl_1_rodata_2498 * lbl_805A0420.randf()) > 0x14) {
        lbl_1_bss_1B88 = 0;
        return;
    }
    temp_r24 = lbl_1_data_9960[0];
    var_r31 = lbl_1_data_9960[7];
    temp_r25 = lbl_1_data_9960[1];
    temp_r26 = lbl_1_data_9960[2];
    temp_r27 = lbl_1_data_9960[3];
    temp_r28 = lbl_1_data_9960[4];
    temp_r29 = lbl_1_data_9960[5];
    temp_r30 = lbl_1_data_9960[6];
    if (fn_80051A6C(lbl_1_data_9960) == 1U) var_r31 = 0;
    var_r23 = temp_r24;
    var_r23 += temp_r25;
    var_r23 += temp_r26;
    var_r23 += temp_r27;
    var_r23 += temp_r28;
    var_r23 += temp_r29;
    var_r23 += temp_r30;
    var_r23 += var_r31;
    temp_r0 = (s32)(lbl_805A0420.randf() * (f32)var_r23);
    s32 weights[8] = {temp_r24, temp_r25, temp_r26, temp_r27, temp_r28, temp_r29, temp_r30, var_r31};
    var_r3 = 0;
#pragma unroll
    for (var_r4 = 0; var_r4 < 8; ++var_r4) {
        if (weights[var_r4] != 0) {
            var_r3 += weights[var_r4];
            if (temp_r0 < var_r3) {
                lbl_1_bss_1B88 = var_r4;
                return;
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_3CC3C {
struct SceneState {
    s32 unk0;
    u16 unk4[50];
    s32 unk68;
    u16 unk6C;
    u32 unk70;
};

extern "C" {
extern void* g_GameGlobal;
extern SceneState lbl_1_bss_1B94;

void gmSetBGMIDUseEnable__FUl(u32 value);
s32 gmGetHideBGMKindFromBgmID__FUl(u32 value);
u32* getBGMAppFlag__10GameGlobalFv(void* ptr);
void fn_80052704(u16 idx, u32 arg);
void fn_80052AC0(u16 idx, u32 arg);

void fn_1_3CC3C(void) {
    s32 i;
    u32* bits;
    s32 idx;
    s32 n;

    switch (lbl_1_bss_1B94.unk0) {
    case 1:
        fn_80052704(lbl_1_bss_1B94.unk6C, 1);
        break;
    case 2:
    case 3:
    case 4:
        for (i = 0; i < lbl_1_bss_1B94.unk68; i++) {
            fn_80052AC0(lbl_1_bss_1B94.unk4[i], 1);
        }
        break;
    case 7:
        gmSetBGMIDUseEnable__FUl(lbl_1_bss_1B94.unk70);
        idx = gmGetHideBGMKindFromBgmID__FUl(lbl_1_bss_1B94.unk70);
        bits = getBGMAppFlag__10GameGlobalFv(g_GameGlobal);
        n = idx * 2 + 1;
        bits[n / 32] |= 1 << (n % 32);
        break;
    }
    lbl_1_bss_1B94.unk0 = 0;
}
}
}

