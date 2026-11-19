#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_hide_lib.h>
#include <memory.h>
#include <mu/mu_soft_list.h>
#include <sr/sr_common.h>
#include <types.h>

// The scene of the list of the unlocked games (module sora_scene): runs the list menu and goes back when it is over.
class scSoftList : public gfScene {
public:
    muSoftListTask* m_task;             // 0x08

    scSoftList(const char* name) : gfScene(name) { }
    static scSoftList* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scSoftList) == 0xC, "Class is wrong size!");

scSoftList* scSoftList::create() {
    return new (Heaps::GlobalMode) scSoftList("scSoftList");
}

void scSoftList::start() {
    gmCheckSoftList();
    muSoftListTask* task = new (Heaps::MenuInstance) muSoftListTask();
    task->init();
    m_task = task;
}

s32 scSoftList::process() {
    if (m_task->isEnd()) {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
    }
    return 0;
}

s32 scSoftList::exit() {
    m_task->exit();
    m_task = NULL;
    return 0;
}

void scSoftList::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_sel_stage.rel", false);
    setLoadModule("sora_menu_main.rel", false);
}
