#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_vc_trial.h>
#include <revolution/vi.h>
#include <sr/sr_common.h>
#include <types.h>

// Starts a title of the Virtual Console (main module): its identifier and how long the trial lasts.
extern "C" void fn_8003D4E8(const char* titleId, int playSec);

// The identifiers of the titles of the vault (the same order as the titles of the menu; the empty ones have no trial).
static const char* s_titleIds[16] = {"HBDE", "HA9E", "HA8E", "HBIE", "HBAE", "HBCE", "HBEE", "HBGE",
                                     "HBFE", "HBBE", "",     "",     "",     "HBLE", "HBKE", ""};
static const char** s_titleIdTable = s_titleIds;

// The scene of the Virtual Console trial (module sora_scene): a menu scene that runs the task of the trial menu of
// sora_menu_main.rel (HYPOTHESIS: the trial is picked from the list of games in the main menu).
class scVCTrial : public muMenu {
public:
    u8 m_unk350[0x35C - 0x350];
    muVCTrialTask* m_task;              // 0x35C

    scVCTrial() : muMenu("scVCTrial"), m_task(NULL) { }
    static scVCTrial* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scVCTrial) == 0x360, "Class is wrong size!");

scVCTrial* scVCTrial::create() {
    return new (Heaps::GlobalMode) scVCTrial();
}

void scVCTrial::setModule() {
    setLoadModule("sora_menu_main.rel", false);
}

s32 scVCTrial::exit() {
    if (m_task != NULL) {
        m_task->exit();
    }
    m_task = NULL;
    return 0;
}

// When the menu has a title picked, the screen goes black and the trial of the title starts.
s32 scVCTrial::process() {
    muVCTrialTask* task = m_task;
    if (task->m_isEnd == 1) {
        int title = task->m_selected;
        if (title == -1) {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
        } else {
            int playSec = task->getPlaySec(title);
            VISetBlack(1);
            VIFlush();
            VIWaitForRetrace();
            fn_8003D4E8(s_titleIdTable[title], playSec);
        }
    }
    return 0;
}
