#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <it/it_figure_manager.h>
#include <it/it_manager.h>
#include <mu/mu_clear_get.h>
#include <sr/sr_common.h>
#include <ty/ty_fig_listmng.h>
#include <types.h>

// TODO: clean up - the scene passes a second argument (1) that muClearGetTask::setBreakBoxId(u32) of the shared header does
// not take (the box is queued for breaking); give the member its real signature and call it as a member.
extern "C" u32 setBreakBoxId__14muClearGetTaskFUl(muClearGetTask* task, u32 id, int unk);

// A static pair of the module that its initializer function fills in (the same shape as the one of scPrizeInfo).
// HYPOTHESIS: the names; the pairs are not read in this unit.
struct scClearGetterEntry {
    int m_id;
    int m_shown;
    scClearGetterEntry(int id, int shown) : m_id(id), m_shown(shown) { }
};
static scClearGetterEntry s_entryHidden(0xFF, 0);
static scClearGetterEntry s_entryShown(0xFF, 1);

// The scene of the prize boxes ("Challenger" trophies you can break with the hammer, module sora_scene): runs the clear
// getter task of main.dol. The boxes that were opened by the game are broken for the player right away when the scene is
// entered anew.
class scClearGetter : public gfScene {
public:
    muClearGetTask* m_task;             // 0x08
    u8 m_isStarted;                     // 0x0C the panel is in
    tyFigListDataManager* m_ownFigMng;  // 0x10 the list of the trophies when the scene made it itself
    int m_memoryLayout;                 // 0x14 how the scene was entered (1: the boxes that the game opened are broken)

    scClearGetter(const char* name) : gfScene(name) { }
    static scClearGetter* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scClearGetter) == 0x18, "Class is wrong size!");

scClearGetter* scClearGetter::create() {
    return new (Heaps::GlobalMode) scClearGetter("scClearGetter");
}

void scClearGetter::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_sel_stage.rel", false);
    setLoadModule("sora_menu_main.rel", false);
}

void scClearGetter::start() {
    m_ownFigMng = NULL;
    m_memoryLayout = gfSceneManager::getInstance()->m_memoryLayout;
    itFigureManager* figureManager = static_cast<itFigureManager*>(itManager::getInstance()->m_figureManager);
    tyFigListDataManager* figMng;
    muClearGetTask* task;
    int pad;
    if (figureManager != NULL) {
        figMng = figureManager->m_figList;
    } else {
        figMng = new (Heaps::MenuInstance) tyFigListDataManager(true);
        m_ownFigMng = figMng;
    }
    if (m_memoryLayout != 1) {
        pad = gmGetMenuDecisionPad();
        task = new (Heaps::MenuInstance) muClearGetTask(0, figMng, pad);
        task->init();
        m_task = task;
    } else {
        pad = gmGetMenuDecisionPad();
        task = new (Heaps::MenuInstance) muClearGetTask(1, figMng, pad);
        task->init();
        m_task = task;
    }
    if (m_memoryLayout == 1) {
        for (int i = 0; i < muClearGetTask::BoxNum; i++) {
            if (!isClearGetterOpen(i)) {
                u32* flags = reinterpret_cast<u32*>(g_GameGlobal->getClearGetAppFlag());
                u32 isSet = (flags[i / 32] & (1 << (i % 32))) != 0;
                if (isSet == 1) {
                    setBreakBoxId__14muClearGetTaskFUl(m_task, i, 1);
                }
            }
        }
    }
    m_isStarted = 0;
}

s32 scClearGetter::process() {
    if (m_isStarted == 0 && m_task->m_state >= muClearGetTask::State_Intro) {
        m_isStarted = 1;
    }
    if (m_task->isEnd()) {
        gfSceneManager* mgr = gfSceneManager::getInstance();
        mgr->unk1 = 0;
        mgr->processStep = 2;
    }
    return 0;
}

s32 scClearGetter::exit() {
    m_task->exit();
    tyFigListDataManager* figMng = m_ownFigMng;
    m_task = NULL;
    if (figMng != NULL) {
        delete figMng;
    }
    return 0;
}
