#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/mu_adv_staffroll.h>
#include <mu/mu_fade.h>
#include <mu/mu_title_sunset.h>
#include <mu/mu_word_of_song.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_relocate.h>

// The ending of the adventure mode (module sora_scene): the words of the song, the sunset with the characters and the
// staff roll (sora_adv_menu_ending.rel), which the player can skip once it allows it.
class scAdvEnding : public gfScene {
public:
    enum Step {
        Step_Load = 0,
        Step_LoadWait = 1,
        Step_MovieLoad = 2,
        Step_WordStart = 3,
        Step_WordWait = 4,
        Step_Word = 5,
        Step_SunsetStart = 6,
        Step_SunsetWait = 7,
        Step_SunsetEnd = 8,
        Step_StaffrollStart = 9,
        Step_StaffrollWait = 10,
        Step_FadeEnd = 11,
        Step_End = 12
    };

    muFadeTask* m_fade;                         // 0x08
    muWordOfSongTask* m_wordOfSong;             // 0x0C
    muTitleSunsetTask* m_sunset;                // 0x10
    muAdvStaffrollTask* m_staffroll;            // 0x14
    gfFileIOHandle m_paramHandle;               // 0x18 mu_title_param.dat
    muTitleSunsetParam* m_sunsetParam;          // 0x1C "muEndingParam" of the file
    muTitleSunsetFade* m_sunsetFade;            // 0x20 "muEndingFadeParam" of the file
    u8 m_isDecided;                             // 0x24 the player pressed a button that skips
    u8 m_isSkip;                                // 0x25 the ending is skipped to the staff roll
    int m_step;                                 // 0x28
    gfFileIOHandle m_sunsetHandle;              // 0x2C sunset.brres
    nw4r::g3d::ResFile m_sunsetResFile;         // 0x30

    scAdvEnding(const char* name) : gfScene(name) { }
    static scAdvEnding* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    void createTask();
    void setupData();
    void releaseTask();
    bool isLoading() {
        if (!m_paramHandle.isReady()) {
            return true;
        }
        if (m_fade->isLoading() == true) {
            return true;
        }
        if (m_wordOfSong->isLoading() == true) {
            return true;
        }
        if (m_sunsetHandle.isReady() == false) {
            return true;
        }
        return m_staffroll->isLoadingFiles() == 1;
    }
};
static_assert(sizeof(scAdvEnding) == 0x34, "Class is wrong size!");

scAdvEnding* scAdvEnding::create() {
    return new (Heaps::GlobalMode) scAdvEnding("scAdvEnding");
}

void scAdvEnding::start() {
    m_fade = NULL;
    m_wordOfSong = NULL;
    m_sunset = NULL;
    m_staffroll = NULL;
    m_sunsetParam = NULL;
    m_step = Step_Load;
    g_sndSystem->stopBGM(0);
}

s32 scAdvEnding::process() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    switch (m_step) {
        case Step_Load:
            createTask();
            m_step = Step_LoadWait;
            break;
        case Step_LoadWait:
            if (!isLoading()) {
                m_staffroll->openMovie();
                m_step = Step_MovieLoad;
            }
            break;
        case Step_MovieLoad:
            if (!m_staffroll->isLoadingMovie()) {
                setupData();
                g_gfApplication->unkD0.endKeepScreen();
                m_step = Step_WordStart;
                if (gfSceneManager::getInstance()->m_memoryLayout <= 0) {
                    m_step = Step_WordStart;
                }
                if (gfSceneManager::getInstance()->m_memoryLayout >= 1) {
                    m_fade->m_state = 1;
                    m_fade->addModel();
                    m_fade->setFade(CFadeColor(1.0f, 0.0f, 0.0f, 0.2f), CFadeColor(1.0f, 0.0f, 0.0f, 0.2f), 10.0f);
                    m_step = Step_StaffrollStart;
                }
            }
            break;
        case Step_WordStart:
            m_wordOfSong->m_state = muWordOfSongTask::State_Start;
            m_staffroll->m_state = 1;
            m_fade->addModel();
            m_wordOfSong->addModel();
            m_step = Step_WordWait;
            break;
        case Step_WordWait:
            if (m_wordOfSong->m_state == muWordOfSongTask::State_FadeOut) {
                m_step = Step_Word;
            }
            break;
        case Step_Word:
            m_step = Step_SunsetStart;
            break;
        case Step_SunsetStart:
            m_sunset->m_state = 1;
            m_step = Step_SunsetWait;
            break;
        case Step_SunsetWait:
            if (m_sunset->m_state == 11) {
                m_step = Step_SunsetEnd;
            }
            break;
        case Step_SunsetEnd:
            m_wordOfSong->end();
            m_wordOfSong->removeModel();
            m_sunset->removeSunsetModel();
            m_step = Step_StaffrollStart;
            break;
        case Step_StaffrollStart:
            m_staffroll->m_state = 3;
            m_step = Step_StaffrollWait;
            break;
        case Step_StaffrollWait:
            if (m_staffroll->m_state == 0x18) {
                m_step = Step_FadeEnd;
            }
            break;
        case Step_FadeEnd:
            m_fade->removeModel();
            m_step = Step_End;
            break;
        case Step_End: {
            releaseTask();
            g_gfApplication->unkD0.startKeepScreen();
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
            m_step++;
            break;
        }
    }
    g_gfSceneRoot->layerUpdateFrame(0);
    g_gfSceneRoot->layerUpdateFrame(8);
    switch (m_step) {
        case Step_WordWait:
        case Step_SunsetWait:
        case Step_StaffrollWait:
            if (!m_isDecided && m_staffroll->isEnableSkip() == true) {
                u32 buttons = status.m_buttonsPressedThisFrame.bits;
                if ((buttons & 0x1000) || (buttons & 0x100000)) {
                    m_isSkip = 1;
                    m_isDecided = 1;
                }
            }
            if (m_isSkip == 1) {
                m_wordOfSong->skip();
                m_sunset->m_state = 11;
                m_sunset->removeNagashiModel();
                m_sunset->removeSunsetModel();
                m_staffroll->m_state = 4;
                m_staffroll->skip();
                m_step = Step_StaffrollWait;
                m_isDecided = 1;
                m_isSkip = 0;
            }
            break;
    }
    return 0;
}

s32 scAdvEnding::exit() {
    return 0;
}

void scAdvEnding::setModule() {
    setLoadModule("sora_menu_title_sunset.rel", false);
    setLoadModule("sora_adv_menu_ending.rel", false);
}

void scAdvEnding::createTask() {
    m_paramHandle.readRequest("/menu/titleloop/mu_title_param.dat", Heaps::MenuResource, 0, 0);
    m_fade = muFadeTask::create();
    m_fade->initialize();
    m_fade->createData();
    m_wordOfSong = muWordOfSongTask::create();
    m_wordOfSong->initialize();
    m_wordOfSong->createDataRequest();
    m_sunset = muTitleSunsetTask::create();
    m_sunset->initialize();
    m_sunset->m_skipFirst = 1;
    m_sunset->createCharaList();
    m_sunsetHandle.readRequest("/menu/titleloop/sunset.brres", Heaps::MenuResource, 0, 0);
    m_staffroll = muAdvStaffrollTask::create();
    m_staffroll->initialize();
    m_staffroll->requestFiles();
    m_isDecided = 0;
    m_isSkip = 0;
}

void scAdvEnding::setupData() {
    u8* param = static_cast<u8*>(m_paramHandle.getBuffer());
    utRelocate relocate(param, m_paramHandle.getSize());
    m_sunsetParam = static_cast<muTitleSunsetParam*>(relocate.getPublicAddress("muEndingParam"));
    m_sunsetFade = static_cast<muTitleSunsetFade*>(relocate.getPublicAddress("muEndingFadeParam"));
    m_fade->setData();
    m_wordOfSong->createData();
    m_wordOfSong->m_fade = m_fade;
    m_sunsetResFile = m_sunsetHandle.getBuffer();
    nw4r::g3d::ResFile::Init(&m_sunsetResFile);
    nw4r::g3d::ResFile resFile = m_sunsetResFile;
    m_sunset->createData(&resFile);
    m_sunset->m_fade = m_fade;
    m_sunset->setSunsetParam(m_sunsetParam);
    m_sunset->setFadeParam(m_sunsetFade);
    m_staffroll->createData();
}

void scAdvEnding::releaseTask() {
    gfHeapManager::free(m_paramHandle.getBuffer());
    m_paramHandle.release();
    m_fade->release();
    m_fade->exit();
    m_fade = NULL;
    m_wordOfSong->release();
    m_wordOfSong->exit();
    m_wordOfSong = NULL;
    m_sunset->release();
    m_sunset->exit();
    m_sunset = NULL;
    gfHeapManager::free(m_sunsetHandle.getBuffer());
    m_sunsetHandle.release();
    m_sunsetResFile.Release();
    m_staffroll->release();
    m_staffroll->exit();
    m_staffroll = NULL;
}
