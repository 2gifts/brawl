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
