#pragma once

// Supersedes include/Brawl/Include/sc/sc_adv_name.h (shadows it via -I include): the layout of the members after 0x0C and
// the size (0x20) follow the scene's functions.

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_scene.h>
#include <mu/mu_adv_difficulty.h>
#include <mu/mu_adv_name.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// What the adventure mode gives to the name scene and gets back from it.
// HYPOTHESIS: the names follow how the scene fills the fields.
struct scAdvNameData {
    int m_name1;                        // 0x00 the name of the first player (number + 1, 0: a new one)
    int m_name2;                        // 0x04 the name of the second player (-1: there is no second player)
    int m_pad1;                         // 0x08 the pad of the first player
    int m_pad2;                         // 0x0C the pad of the second player (-1: there is none)
    u8 m_difficulty;                    // 0x10
    u8 m_is2P;                          // 0x11 both players are in the game
};

// The scene of the name selection of the adventure mode (module sora_scene): runs the name task of
// sora_adv_menu_name.rel and, when the player starts a new game, the difficulty task.
class scAdvName : public gfScene {
public:
    muAdvNameTask* m_task;              // 0x08
    muAdvDifficultyTask* m_diffTask;    // 0x0C
    scAdvNameData* m_data;              // 0x10
    gfFileIOHandle m_handle;            // 0x14 the camera of the menu
    nw4r::g3d::ResFile m_resFile;       // 0x18
    int m_step;                         // 0x1C

    scAdvName(const char* name) : gfScene(name) {
        m_task = NULL;
        m_diffTask = NULL;
        m_data = NULL;
    }
    static scAdvName* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    bool selectDifficulty();
};
static_assert(sizeof(scAdvName) == 0x20, "Class is the wrong size!");
