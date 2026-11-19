#include <cm/cm_camera_controller.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <if/if_utility.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_object.h>
#include <string.h>
#include <types.h>

class scReplay;

// The replay browser of sora_menu_replay.rel (its full class is in mu/mu_replay.h, which has its own view of the scene).
// Only the words that the scene reads when it ends are declared here.
class muReplayTask : public utility::StandardMenu {
public:
    struct Work {
        u8 m_unk000[0x1A0];
        int m_curMedia;                 // 0x1A0 the medium that was browsed
        u8 m_unk1A4[0x1CE4 - 0x1A4];
        char* m_collection;             // 0x1CE4 the collection; the name of the replay file is at 0x15D
    };

    Work* m_work;                       // 0x1AC
    static muReplayTask* create(scReplay* scene);
};

// The scene of the replay browser (module sora_scene): a menu scene that runs the replay browser of sora_menu_replay.rel.
// It hands the medium and the name of the file that the player decided on to the replay.
class scReplay : public muMenu {
public:
    // What start makes.
    struct Work {
        MenuRoot* m_root;               // 0x00 the scene root of the menu
        muReplayTask* m_task;           // 0x04
        muObjProcessTask* m_objProcess; // 0x08 updates the objects of the menu
    };

    u8 m_unk350[0x35C - 0x350];
    Work* m_work;                       // 0x35C
    int m_cursor;                       // 0x360 the medium the replay is on
    char m_name[0x40];                  // 0x364 the name of the replay file

    scReplay(const char* name) : muMenu(name), m_work(NULL), m_cursor(0) { m_name[0] = 0; }
    static scReplay* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    // Gives the object process of the menu the objects it updates.
    void setMotionObj(MuObject** objs, int num);
};
static_assert(sizeof(scReplay) == 0x3A4, "Class is wrong size!");

scReplay* scReplay::create() {
    return new (Heaps::GlobalMode) scReplay("scReplay");
}

void scReplay::setModule() {
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_collect_viewer.rel", false);
    setLoadModule("sora_menu_replay.rel", false);
}

void scReplay::start() {
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_work->m_root = MenuRoot::create("ReplayTask", 0x26, "/menu/defaultcamera/CharacterSelect.brres");
    m_work->m_objProcess = muObjProcessTask::create(Heaps::MenuInstance);
    m_work->m_task = muReplayTask::create(this);
    CameraController::getInstance()->changeInput(CameraController::MenuFixed);
}

s32 scReplay::process() {
    return 0;
}

s32 scReplay::exit() {
    muReplayTask* task = m_work->m_task;
    switch (gfSceneManager::getInstance()->unk1) {
    case 0:
        m_cursor = task->m_work->m_curMedia;
        strcpy(m_name, task->m_work->m_collection + 0x15D);
        break;
    case 1:
        break;
    }
    if (m_work == NULL) {
        return 0;
    }
    m_work->m_root->exit();
    m_work->m_task->exit();
    m_work->m_objProcess->exit();
    gfHeapManager::free(m_work);
    m_work = NULL;
    return 0;
}

void scReplay::setMotionObj(MuObject** objs, int num) {
    muObjProcessTask* objProcess = m_work->m_objProcess;
    objProcess->m_objs = objs;
    objProcess->m_count = num;
}
