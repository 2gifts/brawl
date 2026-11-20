#include <cm/cm_camera_controller.h>
#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_simple_ending.h>
#include <sr/sr_common.h>
#include <stdio.h>
#include <types.h>

// Starts the read of a pack into the archive (main module).
// TODO: clean up - name it as a gfArchive member.
extern "C" int fn_800151FC(gfArchive* archive, const char* path, int heap, int unk1, int unk2, int unk3);

// The ending of the classic mode (module sora_scene): plays the movie of the character (or the boss battle picture
// when the scene is entered with a layout of 100 and more) of sora_menu_simple_ending.rel.
class scEnding : public gfScene {
public:
    enum Step {
        Step_Load = 0,
        Step_LoadWait = 1,
        Step_Setup = 2,
        Step_MovieStart = 3,
        Step_MovieWait = 4,
        Step_MovieEnd = 5,
        Step_PicStart = 6,
        Step_PicWait = 7,
        Step_PicEnd = 8,
        Step_End = 9
    };

    char m_paths[110][0x30];                    // 0x0008 the packs of the endings (classic 1 - 47, all-star 50 - 97, boss 100 - 104)
    gfArchive* m_archive;                       // 0x14A8
    muEndMovieTask* m_movie;                    // 0x14AC
    muEndPicTask* m_pic;                        // 0x14B0
    int m_step;                                 // 0x14B4
    u32 m_clearTime;                            // 0x14B8 the frames of the boss battles

    scEnding();
    static scEnding* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    bool isLoading() {
        if (!m_archive->unk9) {
            return true;
        }
        if (gfSceneManager::getInstance()->m_memoryLayout < 100 && m_movie->isLoading() == true) {
            return true;
        }
        return false;
    }
};
static_assert(sizeof(scEnding) == 0x14BC, "Class is wrong size!");

scEnding* scEnding::create() {
    return new (Heaps::GlobalMode) scEnding();
}

scEnding::scEnding() : gfScene("scEnding") {
    memset(m_paths, 0, sizeof(m_paths));
    int i = 0;
    sprintf(m_paths[i++], "/menu/intro/ending/--------------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple01.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple02.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple03.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple04.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple05.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple06.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple07.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple08.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple09.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple10.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple11.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple12.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple13.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple42.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple42.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple16.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple17.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple18.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple19.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple20.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/--------------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple22.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple23.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple04.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple25.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple26.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple27.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple28.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple28.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple28.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple28.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple32.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple33.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple34.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple35.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/--------------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple37.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple38.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/--------------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/--------------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple41.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/--------------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/--------------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple44.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/--------------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple46.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingSimple47.pac");
    i = 50;
    sprintf(m_paths[i++], "/menu/intro/ending/-----------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll01.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll02.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll03.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll04.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll05.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll06.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll07.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll08.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll09.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll10.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll11.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll12.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll13.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll42.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll42.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll16.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll17.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll18.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll19.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll20.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/-----------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll22.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll23.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll04.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll25.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll26.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll27.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll28.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll28.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll28.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll28.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll32.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll33.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll34.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll35.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/-----------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll37.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll38.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/-----------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/-----------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll41.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/-----------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/-----------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll44.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/-----------.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll46.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingAll47.pac");
    i = 100;
    sprintf(m_paths[i++], "/menu/intro/ending/EndingBoss01.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingBoss02.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingBoss03.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingBoss04.pac");
    sprintf(m_paths[i++], "/menu/intro/ending/EndingBoss05.pac");
    m_archive = NULL;
    m_movie = NULL;
    m_pic = NULL;
}

void scEnding::start() {
    m_step = Step_Load;
}

s32 scEnding::process() {
    switch (m_step) {
        case Step_Load: {
            m_archive = new (Heaps::MenuInstance) gfArchive();
            if (gfSceneManager::getInstance()->m_memoryLayout < 100) {
                m_movie = muEndMovieTask::create();
                m_movie->initialize();
            }
            m_pic = muEndPicTask::create();
            m_pic->initialize();
            int kind = gfSceneManager::getInstance()->m_memoryLayout % 50;
            if (kind < 0) {
                kind = 0;
            }
            int movieNo;
            int index = muMenu::exchangeGmCharacterKind2Something(kind) + 1;
            movieNo = index;
            if (gfSceneManager::getInstance()->m_memoryLayout >= 50) {
                index += 50;
            }
            if (gfSceneManager::getInstance()->m_memoryLayout >= 100) {
                index = gfSceneManager::getInstance()->m_memoryLayout;
            }
            fn_800151FC(m_archive, m_paths[index], Heaps::MenuResource, 0, 0, 0xFF);
            if (gfSceneManager::getInstance()->m_memoryLayout < 100) {
                m_movie->load(movieNo);
            }
            m_step = Step_LoadWait;
            break;
        }
        case Step_LoadWait:
            if (!isLoading()) {
                m_step = Step_Setup;
            }
            break;
        case Step_Setup:
            if (gfSceneManager::getInstance()->m_memoryLayout < 100) {
                m_movie->createData(m_archive);
            }
            m_pic->createData(m_archive);
            CameraController::getInstance()->changeInput(CameraController::MenuFixed);
            g_gfApplication->unkD0.endKeepScreen();
            if (gfSceneManager::getInstance()->m_memoryLayout < 100) {
                m_step = Step_MovieStart;
            } else {
                m_pic->setBossBattleClearTime(m_clearTime);
                m_step = Step_PicStart;
            }
            break;
        case Step_MovieStart:
            m_movie->m_step = 1;
            m_step = Step_MovieWait;
            break;
        case Step_MovieWait:
            if (m_movie->m_step == 5) {
                m_step = Step_MovieEnd;
            }
            break;
        case Step_MovieEnd:
            m_step = Step_PicStart;
            break;
        case Step_PicStart:
            m_pic->m_step = 1;
            m_step = Step_PicWait;
            break;
        case Step_PicWait:
            if (m_pic->m_step == 8) {
                m_step = Step_PicEnd;
            }
            break;
        case Step_PicEnd:
            m_step = Step_End;
            break;
        case Step_End: {
            g_gfApplication->unkD0.startKeepScreen();
            delete m_archive;
            m_archive = NULL;
            if (m_movie != NULL) {
                m_movie->release();
                m_movie->exit();
                m_movie = NULL;
            }
            m_pic->release();
            m_pic->exit();
            m_pic = NULL;
            gfSceneManager* mgr = gfSceneManager::getInstance();
            mgr->unk1 = 0;
            mgr->processStep = 2;
            m_step++;
            break;
        }
    }
    return 0;
}

s32 scEnding::exit() {
    return 0;
}

void scEnding::setModule() {
    setLoadModule("sora_menu_simple_ending.rel", false);
}
