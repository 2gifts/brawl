#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_tour_rule.h>
#include <sr/sr_common.h>
#include <types.h>

// The scene of the rules of the tournament (module sora_scene): runs the rule menu of sora_menu_rule.rel and, when the menu
// is left, goes on with the scene the result asks for.
class scTourRule : public gfScene {
public:
    muTourRuleTask* m_task;             // 0x08

    scTourRule(const char* name) : gfScene(name) { }
    static scTourRule* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scTourRule) == 0xC, "Class is wrong size!");

scTourRule* scTourRule::create() {
    return new (Heaps::GlobalMode) scTourRule("scTourRule");
}

void scTourRule::start() {
    switch (gfSceneManager::getInstance()->m_memoryLayout) {
    case 0: {
        muTourRuleTask* task = new (Heaps::MenuInstance) muTourRuleTask(muTourRuleTask::Mode_New);
        task->init();
        m_task = task;
        break;
    }
    case 1: {
        muTourRuleTask* task = new (Heaps::MenuInstance) muTourRuleTask(muTourRuleTask::Mode_Edit);
        task->init();
        m_task = task;
        break;
    }
    case 2: {
        muTourRuleTask* task = new (Heaps::MenuInstance) muTourRuleTask(muTourRuleTask::Mode_Resume);
        task->init();
        m_task = task;
        break;
    }
    case 3: {
        muTourRuleTask* task = new (Heaps::MenuInstance) muTourRuleTask(muTourRuleTask::Mode_Rules);
        task->init();
        m_task = task;
        break;
    }
    }
}

s32 scTourRule::process() {
    switch (m_task->m_result) {
    case muTourRuleTask::Result_Start:
        {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 2;
            mgr->processStep = 2;
        }
        break;
    case muTourRuleTask::Result_Resume:
        {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 1;
            mgr->processStep = 2;
        }
        break;
    case muTourRuleTask::Result_Back:
        {
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
        }
        break;
    }
    return 0;
}

s32 scTourRule::exit() {
    m_task->exit();
    m_task = NULL;
    return 0;
}

void scTourRule::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_tour.rel", false);
}
