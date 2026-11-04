#include <types.h>
#include <if/if_reward_list_task.h>
#include <if/if_utility.h>
#include <mu/mu_msg.h>
#include <ty/ty_seal_listmng.h>
#include <string.h>

// Function bodies below are taken from https://github.com/jj-link/brawl.

extern "C" void fn_800F851C(int* p) {
    *p = 0;
}

IfRewardListTask::Panel::Panel() {
}

IfRewardListTask::Panel::~Panel() {
}

void IfRewardListTask::detachDialogMsg() {
    if (unk340 != NULL) {
        for (int message = 0; message < 2; ++message) {
            unk340->detachMuObject(unk268);
        }
    }
}

// muMenuController::main (the map name); include/mu/mu_menu_controller.h calls it by this stand-in name.
extern "C" void fn_800B9984(muMenuController* controller);

void IfRewardListTask::processDefault() {
    if (!unk101) {
        if (!utility::Loader::isLoadFinish()) {
            return;
        }
        createModel();
        initMsg();
        unk101 = true;
    }
    fn_800B9984(&unk48);
    controllProc();
    if (unk100) {
        updateScroll();
    }
}

char lbl_8045BFB8[15] = "Seal_Sammy_Mat";
char lbl_8059E468[6] = "pos00";
char lbl_8059E470[8] = "";
extern const float lbl_805A26A0;

