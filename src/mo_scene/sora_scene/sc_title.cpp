#include <gf/gf_3d_scene.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/menu.h>
#include <mu/mu_boot_pad.h>
#include <mu/mu_fade.h>
#include <mu/mu_opening_movie.h>
#include <mu/mu_resource_loader.h>
#include <mu/mu_title.h>
#include <mu/mu_title_sunset.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <snd/snd_system.h>
#include <sr/sr_common.h>
#include <st/loader/st_loader_manager.h>
#include <types.h>
#include <ut/ut_relocate.h>

// Starts the read of a pack of the scene module into the archive (scResourceLoader of the scene module).
extern "C" void fn_1_3C68(scResourceLoader* loader, int heap, const char* path, gfArchive* archive, int size);
// Tells whether the pack is read.
extern "C" bool fn_1_3E50(scResourceLoader* loader);
// Gets the data of the file with that number from the archive (main module).
extern "C" void* fn_800B03F0(gfArchive* archive, int index);

// The title scene (module sora_scene): plays the opening movie, the sunset with the characters and the title, and waits
// for the start button (or for the time out, which goes to the demo).
class scTitle : public muMenu {
public:
    enum Step {
        Step_Load = 0,
        Step_LoadWait = 1,
        Step_Setup = 2,
        Step_MovieLoad = 3,
        Step_MovieStart = 4,
        Step_MovieWait = 5,
        Step_SunsetStart = 6,
        Step_SunsetWait = 7,
        Step_TitleShort = 8,
        Step_TitleShortWait = 9,
        Step_Movie = 10,
        Step_MovieEndWait = 11,
        Step_Title = 12,
        Step_TitleMovieWait = 13,
        Step_ResetLoaders = 14,
        Step_TitleWait = 15,
        Step_Decide = 16,
        Step_End = 17,
        Step_TimeOut = 18
    };

    u8 m_unk350[0xC];
    int m_unk35C;
    muTitleSunsetParam* m_sunsetParam;          // 0x360 "muOpeningParam" of the pack
    muTitleSunsetFade* m_sunsetFade;            // 0x364 "muOpeningFadeParam" of the pack
    u8 m_noOpening;                             // 0x368 the scene comes back from the menu: only the title
    u8 m_isDecided;                             // 0x369 a button was pressed (start or skip)
    u8 m_isSkip;                                // 0x36A the opening is skipped to the title
    u8 m_isStart;                               // 0x36B the start button was pressed
    int m_result;                               // 0x36C what the sequence gets (1: the title timed out)
    int m_step;                                 // 0x370
    int m_frame;                                // 0x374
    gfArchive* m_archive;                       // 0x378 the pack of the scene
    scResourceLoader* m_loader;                 // 0x37C reads the pack
    muFadeTask* m_fade;                         // 0x380
    muTitleSunsetTask* m_sunset;                // 0x384
    muTitleTask* m_title;                       // 0x388
    muOpeningMovieTask* m_movie;                // 0x38C
    muBootPad* m_bootPad;                       // 0x390

    scTitle(const char* name) : muMenu(name) {
        m_unk35C = 0;
        m_archive = NULL;
        m_loader = NULL;
        m_sunsetParam = NULL;
        m_sunsetFade = NULL;
        m_fade = NULL;
        m_sunset = NULL;
        m_title = NULL;
        m_movie = NULL;
    }
    static scTitle* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    virtual void init();
    void createTask();
    void releaseTask();
    void setupData();
};
static_assert(sizeof(scTitle) == 0x394, "Class is wrong size!");

// A player loader as the title scene sees it: bit 0x20 of the byte at 0x5D.
// TODO: clean up - name the flag in stLoaderPlayer.
struct scTitleLoaderFlags {
    u8 m_unk0[0x5D];
    u8 m_unk5D_b7 : 1;
    u8 m_unk5D_b6 : 1;
    u8 m_unk5D_b5 : 1;
    u8 m_unk5D_b4 : 5;
};
// Clears the flag of the loader of player i.
#define CLR_LOADER(i) \
    { \
        scTitleLoaderFlags* loader = reinterpret_cast<scTitleLoaderFlags*>(g_stLoaderManager->m_loaderPlayers[i]); \
        if (loader != NULL) { \
            loader->m_unk5D_b5 = 0; \
        } \
    }

scTitle* scTitle::create() {
    scTitle* scene = new (Heaps::GlobalMode) scTitle("scTitle");
    scene->init();
    return scene;
}

void scTitle::init() { }

void scTitle::start() {
    switch (gfSceneManager::getInstance()->m_memoryLayout) {
        case 0:
            m_noOpening = 0;
            m_isDecided = 0;
            break;
        case 1:
            m_noOpening = 1;
            m_isDecided = 1;
            break;
        default:
            m_noOpening = 0;
            m_isDecided = 0;
            break;
    }
    m_bootPad = new (Heaps::MenuInstance) muBootPad();
    m_step = Step_Load;
    g_sndSystem->stopBGM(0);
}

s32 scTitle::process() {
    m_bootPad->update();
    switch (m_step) {
        case Step_Load:
            createTask();
            m_archive = new (Heaps::MenuResource) gfArchive();
            m_loader = new (Heaps::MenuResource) scResourceLoader();
            fn_1_3C68(m_loader, Heaps::MenuResource, "/menu2/sc_title.pac", m_archive, 0);
            m_step = Step_LoadWait;
            break;
        case Step_LoadWait:
            {
                // TODO: clean up - the negated result is kept in a word (cntlzw / srwi.), a plain test branches on it.
                u32 notLoaded = !fn_1_3E50(m_loader);
                if (notLoaded) {
                    break;
                }
            }
            m_step = Step_Setup;
            break;
        case Step_Setup:
            muMenu::loadMenuSound();
            setupData();
            if (m_fade != NULL) {
                m_fade->m_state = 1;
                m_fade->addModel();
                m_fade->setObjectFadeColor(1.0f, 1.0f, 1.0f, 1.0f);
            }
            g_gfApplication->unkD0.endKeepScreen();
            if (gfSceneManager::getInstance()->m_memoryLayout != 1) {
                m_movie->openMovie();
                m_step = Step_MovieLoad;
            } else {
                m_step = Step_Title;
            }
            break;
        case Step_MovieLoad:
            if (!m_movie->isLoadingMovie()) {
                m_step = Step_MovieStart;
            }
            break;
        case Step_MovieStart:
            m_movie->m_state = 1;
            m_movie->setupMovie();
            m_movie->startMovie();
            m_step = Step_MovieWait;
            m_frame = 0;
            break;
        case Step_MovieWait:
            if (m_frame >= 0x10) {
                m_step = Step_SunsetStart;
            }
            break;
        case Step_SunsetStart:
            m_sunset->m_state = 1;
            m_step = Step_SunsetWait;
            break;
        case Step_SunsetWait:
            if (m_sunset->m_state == 11) {
                m_step = Step_TitleShort;
            }
            break;
        case Step_TitleShort:
            m_title->m_state = 1;
            m_title->addModelShort();
            m_title->m_short = 1;
            m_step = Step_TitleShortWait;
            break;
        case Step_TitleShortWait:
            if (m_movie->m_frame >= 0xBB) {
                m_title->m_state = 0x12;
                m_step = Step_Movie;
            }
            break;
        case Step_Movie:
            m_sunset->m_state = 12;
            m_sunset->removeNagashiModel();
            m_sunset->removeSunsetModel();
            m_title->m_state = 0x13;
            m_title->resetAnimetionParam();
            m_title->removeModelNormal();
            m_title->removeModelShort();
            m_movie->addMovieModel();
            m_step = Step_MovieEndWait;
            break;
        case Step_MovieEndWait:
            if (m_movie->m_frame >= 0xD1A) {
                m_step = Step_Title;
            }
            break;
        case Step_Title:
            if (m_movie != NULL) {
                m_movie->removeMovieModel();
            }
            m_title->m_state = 1;
            m_title->addModelNormal();
            m_title->m_short = 0;
            m_step = Step_TitleMovieWait;
            break;
        case Step_TitleMovieWait: {
            u32 isEnd;
            if (m_movie == NULL) {
                isEnd = true;
            } else {
                isEnd = m_movie->m_state == 3;
            }
            if (isEnd == true) {
                m_step = Step_ResetLoaders;
            }
            break;
        }
        case Step_ResetLoaders:
            // TODO: clean up - written out for the four players (a loop reloads the address of g_stLoaderManager each time).
            CLR_LOADER(0);
            CLR_LOADER(1);
            CLR_LOADER(2);
            CLR_LOADER(3);
            m_step = Step_TitleWait;
            break;
        case Step_TitleWait:
            if (m_title->m_state == 0x12) {
                if (m_title->m_timeOut == 1) {
                    m_result = 1;
                } else {
                    m_result = 0;
                }
                m_step = Step_TimeOut;
            }
            break;
        case Step_Decide:
            if (m_fade != NULL) {
                m_fade->removeModel();
            }
            if (m_sunset != NULL) {
                m_sunset->m_state = 12;
                m_sunset->removeNagashiModel();
                m_sunset->removeSunsetModel();
            }
            if (m_title != NULL) {
                m_title->m_state = 0x13;
                m_title->removeModelNormal();
            }
            releaseTask();
            m_step = Step_End;
            break;
        case Step_End: {
            g_gfApplication->unkD0.startKeepScreen();
            int result = m_result;
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = result;
            mgr->processStep = 2;
            break;
        }
        case Step_TimeOut: {
            if (m_fade != NULL) {
                m_fade->removeModel();
            }
            if (m_sunset != NULL) {
                m_sunset->m_state = 12;
                m_sunset->removeNagashiModel();
                m_sunset->removeSunsetModel();
            }
            if (m_title != NULL) {
                m_title->m_state = 0x13;
                m_title->removeModelNormal();
            }
            releaseTask();
            g_gfApplication->unkD0.startKeepScreen();
            int result = m_result;
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = result;
            mgr->processStep = 2;
            m_step++;
            break;
        }
    }
    m_frame++;
    g_gfSceneRoot->layerUpdateFrame(0);
    g_gfSceneRoot->layerUpdateFrame(8);
    switch (m_step) {
        case Step_MovieWait:
        case Step_SunsetWait:
        case Step_TitleShortWait:
        case Step_MovieEndWait:
            if (!m_isDecided) {
                if (m_bootPad->isPressStartButton() == true) {
                    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    g_sndSystem->playSE(static_cast<SndID>(0x13), -1, 0, 0, -1);
                    m_isStart = 1;
                    m_isDecided = 1;
                } else if (m_bootPad->isPressBootSkipButton() == true) {
                    if (m_fade != NULL) {
                        m_fade->setObjectFadeColor(1.0f, 1.0f, 1.0f, 0.0f);
                    }
                    if (m_bootPad->isOnEdge(0x200) == true) {
                        g_sndSystem->playSE(static_cast<SndID>(2), -1, 0, 0, -1);
                    } else {
                        g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
                    }
                    m_isSkip = 1;
                    m_isDecided = 1;
                }
            }
            if (m_isStart == 1) {
                m_isStart = 0;
                m_step = Step_Decide;
            }
            if (m_isSkip == 1) {
                m_sunset->skip();
                m_title->skip();
                m_movie->skip();
                m_step = Step_Title;
                m_isSkip = 0;
            }
            break;
        case Step_TitleMovieWait:
        case Step_TitleWait:
            if (m_bootPad->isPressBootSkipButton() == true) {
                m_title->requestTimeOut();
            }
            break;
    }
    return 0;
}

s32 scTitle::exit() {
    delete m_bootPad;
    m_bootPad = NULL;
    return 0;
}

void scTitle::setModule() {
    setLoadModule("sora_menu_title.rel", false);
    setLoadModule("sora_menu_title_sunset.rel", false);
}

void scTitle::createTask() {
    if (!m_noOpening) {
        m_fade = muFadeTask::create();
        m_fade->initialize();
    }
    if (!m_noOpening) {
        m_sunset = muTitleSunsetTask::create();
        m_sunset->initialize();
        m_sunset->m_skipFirst = 0;
        m_sunset->createCharaList();
    }
    m_title = muTitleTask::create();
    m_title->initialize();
    if (!m_noOpening) {
        m_movie = muOpeningMovieTask::create();
        m_movie->initialize();
    }
    m_isSkip = 0;
    m_isStart = 0;
    m_result = 0;
    m_frame = 0;
}

void scTitle::releaseTask() {
    if (m_fade != NULL) {
        m_fade->release();
        m_fade->exit();
        m_fade = NULL;
    }
    if (m_sunset != NULL) {
        m_sunset->release();
        m_sunset->exit();
        m_sunset = NULL;
    }
    if (m_title != NULL) {
        m_title->release();
        m_title->exit();
        m_title = NULL;
    }
    if (m_movie != NULL) {
        m_movie->release();
        m_movie->exit();
        m_movie = NULL;
    }
    if (m_archive != NULL) {
        delete m_archive;
        m_archive = NULL;
    }
    if (m_loader != NULL) {
        delete m_loader;
        m_loader = NULL;
    }
}

void scTitle::setupData() {
    int size;
    void* data = m_archive->getData(0, static_cast<ARCNodeType>(1), 0x28, &size, 0xFFFE);
    utRelocate relocate(static_cast<u8*>(data), size);
    m_sunsetParam = static_cast<muTitleSunsetParam*>(relocate.getPublicAddress("muOpeningParam"));
    m_sunsetFade = static_cast<muTitleSunsetFade*>(relocate.getPublicAddress("muOpeningFadeParam"));
    if (m_fade != NULL) {
        nw4r::g3d::ResFile resFile(fn_800B03F0(m_archive, 0x23));
        m_fade->setData(&resFile);
    }
    if (m_sunset != NULL) {
        nw4r::g3d::ResFile resFile(fn_800B03F0(m_archive, 0x32));
        m_sunset->createData(&resFile);
        m_sunset->m_fade = m_fade;
        m_sunset->setSunsetParam(m_sunsetParam);
        m_sunset->setFadeParam(m_sunsetFade);
    }
    if (m_title != NULL) {
        m_title->createData(m_archive);
    }
    if (m_movie != NULL) {
        m_movie->createData(m_archive);
    }
}
