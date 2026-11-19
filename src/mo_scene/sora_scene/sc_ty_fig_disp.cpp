#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <gm/gm_common.h>
#include <sr/sr_common.h>
#include <types.h>

class scTyFigDisp;

// The root task of the figure display of sora_menu_main.rel (HYPOTHESIS: the names follow the symbol map).
class tyFigDisp1RootTask : public gfTask {
public:
    static tyFigDisp1RootTask* create();
    void setScnAnm(int index, void* anm);
    void addObj(int layer, void* obj);
    void removeObj(int layer, void* obj);
};

// The task that shows the figures (sora_menu_main.rel).
class tyFigDisp1Task : public gfTask {
public:
    static tyFigDisp1Task* create(scTyFigDisp* scene, int index, int pad, int unk);
};

// The scene of the figure display (module sora_scene): runs the tasks of the figure display of sora_menu_main.rel.
class scTyFigDisp : public gfScene {
public:
    // What start makes.
    struct Work {
        tyFigDisp1RootTask* m_root;     // 0x00
        tyFigDisp1Task* m_task;         // 0x04
        CameraController* m_camera;     // 0x08
    };

    int m_unk8;                         // 0x08 the first direction of the pad (see getPadDirection)
    Work* m_work;                       // 0x0C

    scTyFigDisp(const char* name) : gfScene(name), m_unk8(0), m_work(NULL) { }
    static scTyFigDisp* create();
    int getPadDirection();
    void setScnAnmIn(void* anm);
    void setScnAnmOut(void* anm);
    void addObjIn(void* obj);
    void addObjOut(void* obj);
    void removeObjIn(void* obj);
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scTyFigDisp) == 0x10, "Class is wrong size!");

scTyFigDisp* scTyFigDisp::create() {
    return new (Heaps::GlobalMode) scTyFigDisp("scTyFigDisp");
}

void scTyFigDisp::setModule() {
    setLoadModule("sora_menu_main.rel", false);
    setLoadModule("sora_menu_name.rel", false);
    setLoadModule("sora_menu_rule.rel", false);
    setLoadModule("sora_menu_sel_char_access.rel", false);
    setLoadModule("sora_menu_sel_stage.rel", false);
}

void scTyFigDisp::start() {
    m_work = static_cast<Work*>(gfHeapManager::alloc(Heaps::MenuInstance, sizeof(Work)));
    m_unk8 = getPadDirection();
    m_work->m_camera = CameraController::getInstance();
    m_work->m_camera->changeInput(CameraController::FigDisp1);
    m_work->m_root = tyFigDisp1RootTask::create();
    m_work->m_task = tyFigDisp1Task::create(this, m_unk8, gmGetMenuDecisionPad(), 0);
}

// The direction of the pad that decides (the stick or the cross: 2 left, 3 right, 4 up, 5 down; 1 for a shoulder button),
// the figure display starts with it.
int scTyFigDisp::getPadDirection() {
    int direction = 0;
    int pad = gmGetMenuDecisionPad();
    if (pad < 0 || pad >= 8) {
        pad = 0;
    }
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(pad, &status);
    u32 buttons = status.m_buttonsCurrentFrame2.bits;
    if ((buttons & 0x8) || status.m_stickY > 0x3C) {
        direction = 4;
    }
    if ((buttons & 0x4) || status.m_stickY < -0x3C) {
        direction = 5;
    }
    if ((buttons & 0x1) || status.m_stickX < -0x3C) {
        direction = 2;
    }
    if ((buttons & 0x2) || status.m_stickX > 0x3C) {
        direction = 3;
    }
    if ((buttons & 0x20) || (buttons & 0x40) || (buttons & 0x40000)) {
        direction = 1;
    }
    return direction;
}

s32 scTyFigDisp::process() {
    return 0;
}

s32 scTyFigDisp::exit() {
    if (m_work == NULL) {
        return 0;
    }
    g_gfApplication->unkD0.startKeepScreen();
    m_work->m_task->exit();
    m_work->m_root->exit();
    gfHeapManager::free(m_work);
    m_work = NULL;
    return 0;
}

void scTyFigDisp::setScnAnmIn(void* anm) {
    m_work->m_root->setScnAnm(1, anm);
}

void scTyFigDisp::setScnAnmOut(void* anm) {
    m_work->m_root->setScnAnm(0, anm);
}

void scTyFigDisp::addObjIn(void* obj) {
    m_work->m_root->addObj(0, obj);
}

void scTyFigDisp::addObjOut(void* obj) {
    m_work->m_root->addObj(1, obj);
}

void scTyFigDisp::removeObjIn(void* obj) {
    m_work->m_root->removeObj(0, obj);
}
