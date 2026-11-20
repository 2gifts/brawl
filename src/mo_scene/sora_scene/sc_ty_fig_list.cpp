#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <revolution/OS/OSError.h>
#include <sr/sr_common.h>
#include <types.h>

// The task of the figure list (sora_menu_main.rel).
class tyFigListTask : public gfTask {
public:
    int m_step;                         // 0x40 8: the list is left
    u8 m_unk44[0x650 - 0x44];

    tyFigListTask();
};

// The scene of the figure list (module sora_scene): runs the figure list task of sora_menu_main.rel.
class scTyFigList : public gfScene {
public:
    int m_unk8;                         // 0x08
    tyFigListTask* m_task;              // 0x0C

    scTyFigList() : gfScene("scTyFigList") { }
    static scTyFigList* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    virtual void init();
};
static_assert(sizeof(scTyFigList) == 0x10, "Class is wrong size!");

scTyFigList* scTyFigList::create() {
    scTyFigList* scene = new (Heaps::GlobalMode) scTyFigList();
    scene->init();
    return scene;
}

void scTyFigList::init() { }

void scTyFigList::start() {
    m_unk8 = 0;
    tyFigListTask* task = new (Heaps::MenuInstance) tyFigListTask();
    task->init();
    m_task = task;
}

s32 scTyFigList::process() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    status.clamp();
    if (m_task->m_step == 8) {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
    }
    return 0;
}

s32 scTyFigList::exit() {
    OSReport("\n exit %s\n", m_sceneName);
    m_task->exit();
    m_task = NULL;
    return 0;
}

void scTyFigList::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_sel_stage.rel", false);
    setLoadModule("sora_menu_main.rel", false);
}
