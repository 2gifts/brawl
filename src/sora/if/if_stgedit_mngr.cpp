#include <types.h>
#include <if/if_stgedit_mngr.h>
#include <if/if_stgedit.h>
#include <gf/gf_3d_scene.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0350[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0358[0x8] __attribute__((aligned(4))) = { 0 };

void IfStgeditMngr::setVisible(bool isVisible) {
    unk10->setVisible(isVisible);
}

void IfStgeditMngr::setVisibleZoomHelp(bool isVisible) {
    unk10->setVisibleZoomHelp(isVisible);
}

void IfStgeditMngr::destroyModel() {
    if (unk10 != NULL) {
        delete unk10;
        unk10 = NULL;
    }
    if (unk4 != NULL) {
        unk4->Destroy();
        unk4 = NULL;
    }
    g_gfSceneRoot->removeInfoCamResAnm();
}
