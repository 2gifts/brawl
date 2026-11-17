#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/mu_boot_pad.h>
#include <mu/mu_controller_operator.h>
#include <mu/mu_esrb.h>
#include <mu/mu_now_loading.h>
#include <mu/mu_strap.h>
#include <revolution/sc.h>
#include <sr/sr_common.h>
#include <types.h>

// The strap scene of the boot (health and safety screens, controller notice, rating notice).
class scStrap : public gfScene {
    gfArchive* m_archive;
    muStrapTask* m_strapTask;
    muControllerOperatorTask* m_controllerTask;
    muESRBTask* m_esrbTask;
    muNowLoadingTask* m_nowLoadingTask;
    muBootPad* m_pad;
    u8 m_language;
    s32 m_0x24;
    s32 m_0x28;
    u8 m_0x2c;

public:
    static scStrap* create();
    scStrap(const char* name) : gfScene(name), m_archive(nullptr), m_strapTask(nullptr), m_controllerTask(nullptr), m_esrbTask(nullptr), m_nowLoadingTask(nullptr), m_pad(nullptr) { }
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    void initialize();
    void release();
};

scStrap* scStrap::create() {
    return new (Heaps::GlobalMode) scStrap("scStrap");
}

s32 scStrap::exit() {
    return 0;
}

void scStrap::setModule() {
    setLoadModule("sora_menu_title.rel", false);
}

void scStrap::release() {
    delete m_archive;
    m_archive = nullptr;
    if (m_strapTask) {
        m_strapTask->release();
        m_strapTask->exit();
        m_strapTask = nullptr;
    }
    if (m_controllerTask) {
        m_controllerTask->release();
        m_controllerTask->exit();
        m_controllerTask = nullptr;
    }
    if (m_esrbTask) {
        m_esrbTask->release();
        m_esrbTask->exit();
        m_esrbTask = nullptr;
    }
    if (m_nowLoadingTask) {
        m_nowLoadingTask->release();
        m_nowLoadingTask->exit();
        m_nowLoadingTask = nullptr;
    }
    delete m_pad;
    m_pad = nullptr;
}
