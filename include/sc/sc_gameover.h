#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_game_over.h>
#include <mu/mu_menuroot.h>
#include <types.h>

class scResourceLoader;

// The scene that shows the game over screen of the classic and all-star modes (module sora_scene): reads
// /menu2/sc_gameover.pac and runs the game over task of sora_menu_game_over.rel.
// HYPOTHESIS: the class hierarchy gfScene <- muMenu <- scGameover follows from the run time type information of the module.
class scGameover : public muMenu {
public:
    // What start makes: the scene root of the pack and the task.
    struct Work {
        MenuRoot* m_root;                       // 0x00
        muGameOverTask* m_task;                 // 0x04
    };

    u8 m_unk350[0xC];
    Work* m_work;                               // 0x35C
    muGameOverParam m_param;                    // 0x360 what the mode gives to the task
    bool m_isStarted;                           // 0x378 the task runs
    gfArchive* m_archive;                       // 0x37C the pack of the screen
    scResourceLoader* m_loader;                 // 0x380 reads the pack

    scGameover(const char* name) : muMenu(name), m_work(NULL), m_isStarted(false), m_archive(NULL), m_loader(NULL) {
        muGameOverParam param;
        memset(&param, 0, sizeof(param));
        m_param = param;
    }
    static scGameover* create();
    nw4r::g3d::ScnGroup* getScnRoot();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scGameover) == 0x384, "Class is wrong size!");
