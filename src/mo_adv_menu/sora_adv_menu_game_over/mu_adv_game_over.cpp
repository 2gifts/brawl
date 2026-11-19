#include <mu/adv/mu_adv_game_over.h>
#include <mu/menu.h>
#include <sr/sr_common.h>
#include <ty/ty_fig_listmng.h>
#include <ty/ty_seal_listmng.h>
#include <types.h>

// Tells whether the pack of the scene module is read (scResourceLoader of the scene module).
extern "C" bool fn_1_3E50(scResourceLoader* loader);

extern "C" muAdvGameOverTask* fn_33_44(muAdvGameOverTask* task,
        const muAdvGameOverTaskParam* param);

muAdvGameOverTask* muAdvGameOverTask::create(const muAdvGameOverTaskParam* param) {
    muAdvGameOverTask* task = static_cast<muAdvGameOverTask*>(
            operator new(0x3F8, Heaps::MenuInstance));
    if (task != nullptr) {
        task = fn_33_44(task, param);
    }
    return task;
}

// The lists of the stickers and of the trophies are loaded.
bool muAdvGameOverTask::preLoad() {
    bool loaded = false;
    if (unk280->isLoadFinish()) {
        if (unk27C->isLoadFinish()) {
            loaded = true;
        }
    }
    return loaded;
}

bool muAdvGameOverTask::checkLoadFinish() const {
    bool loaded = false;
    if (fn_1_3E50(unk3F4)) {
        if (muMenu::isLoadFinishMenuSound()) {
            loaded = true;
        }
    }
    return loaded;
}

// What the screen was left with: the entry the cursor was on (the continue entry counts as 4 or 5 when it cannot be
// used).
s32 muAdvGameOverTask::getSelection() const {
    s32 selection = unk234[unk22C];
    if (selection == 1) {
        if (unk1B0 == 0) {
            selection = 5;
        } else {
            muAdvGameOverObj* obj = unk2BC;
            if (obj != NULL) {
                if (obj->unk1AC == 1) {
                    selection = 5;
                    if (obj->unk1B4 == 0) {
                        selection = 4;
                    }
                }
            }
        }
    }
    return selection;
}
