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
