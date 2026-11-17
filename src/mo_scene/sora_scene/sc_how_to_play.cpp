#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/mu_boot_pad.h>
#include <mu/mu_how_to_play.h>
#include <sr/sr_common.h>
#include <types.h>

// The "how to play" movie scene of the first boot.
class scHowToPlay : public gfScene {
    muHowToPlayTask* m_task;
    muBootPad* m_pad;
    s32 m_result;
    s32 m_step;

public:
    static scHowToPlay* create();
    scHowToPlay(const char* name) : gfScene(name), m_task(nullptr), m_pad(nullptr) { }
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};

scHowToPlay* scHowToPlay::create() {
    return new (Heaps::GlobalMode) scHowToPlay("scHowToPlay");
}

void scHowToPlay::start() {
    m_result = 2;
    m_step = 0;
}

s32 scHowToPlay::process() {
    if (m_pad) {
        m_pad->update();
    }
    switch (m_step) {
    case 0:
        m_task = muHowToPlayTask::create();
        m_task->initialize();
        m_pad = new (Heaps::MenuInstance) muBootPad;
        m_task->openMovie();
        m_step = 1;
        break;
    case 1:
        {
            bool loading = m_task->isLoadingMovie() == true;
            if (!loading) {
                m_step = 2;
            }
        }
        break;
    case 2:
        m_task->createData();
        g_gfApplication->unkD0.endKeepScreen();
        m_step = 3;
        break;
    case 3:
        m_task->m_state = 1;
        m_task->startMovie();
        m_task->addMovieModel();
        m_step = 4;
        break;
    case 4:
        if (m_pad->isPressBootSkipButton() == true) {
            m_task->skip();
            m_result = 1;
        }
        if (m_pad->isPressStartButton() == true) {
            m_task->skip();
            m_result = 0;
        }
        if (m_task->m_state == 3) {
            m_step = 5;
        }
        break;
    case 5:
        m_task->removeMovieModel();
        m_step = 6;
        break;
    case 6:
        m_task->release();
        m_task->exit();
        m_task = nullptr;
        delete m_pad;
        m_pad = nullptr;
        g_gfApplication->unkD0.startKeepScreen();
        {
            s32 result = m_result;
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = result;
            mgr->processStep = 2;
        }
        m_step++;
        break;
    }
    return 0;
}

s32 scHowToPlay::exit() {
    return 0;
}

void scHowToPlay::setModule() {
    setLoadModule("sora_menu_boot.rel", false);
    setLoadModule("sora_menu_title.rel", false);
}
