#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_intro.h>
#include <mu/mu_menuroot.h>
#include <types.h>

// Who fights in the intro (the scene keeps it for the intro task, which copies it when it starts).
struct scIntroParam {
    typedef muIntroEntry Entry;

    int m_introNo;                  // 0x00 the number of the intro (the map animation "ItrSimpleMap0000_TopN__%d")
    int m_mode;                     // 0x04 0 and 1 are fights, a larger value is the number of a mini intro (mode - 2)
    int m_playerNum;                // 0x08
    Entry m_players[3];             // 0x0C
    int m_enemyNum;                 // 0x24
    Entry m_enemies[2];             // 0x28

    scIntroParam() : m_introNo(0), m_mode(0), m_playerNum(1), m_enemyNum(0) {
        memset(m_players, 0, sizeof(m_players));
        memset(m_enemies, 0, sizeof(m_enemies));
    }
    // One player against nobody.
    void reset() {
        memset(this, 0, sizeof(*this));
        m_introNo = 0;
        m_mode = 0;
        m_playerNum = 1;
        m_players[0].m_charKind = 0;
        m_players[0].m_type = 0;
        m_enemyNum = 0;
    }
};
static_assert(sizeof(scIntroParam) == 0x38, "Class is wrong size!");

// The scene of the intro (module sora_scene): reads the camera of the intro map and runs the intro task of
// sora_menu_intro.rel. The intro shows who fights: the players (up to 3) and the enemies (up to 2), each with a
// character and a kind of entry.
// HYPOTHESIS: the class hierarchy gfScene <- muMenu <- scIntro follows from the run time type information of the module.
class scIntro : public muMenu {
public:
    // What start makes: the scene root of the menu and the intro task.
    struct Work {
        MenuRoot* m_root;           // 0x00
        muIntroTask* m_task;        // 0x04
        int m_unk8;                 // 0x08
    };

    u8 m_unk350[0xC];
    scIntroParam m_param;           // 0x35C
    int m_rumbleController;         // 0x394
    Work* m_work;                   // 0x398
    u8 m_isLoaded;                  // 0x39C the camera is read and the intro task runs
    u8 m_rumbleSetting;             // 0x39D
    gfFileIOHandle m_handle;        // 0x3A0 the camera of the intro map
    int m_unk3A4;                   // 0x3A4
    u8 m_unk3A8;                    // 0x3A8

    scIntro();
    static scIntro* create();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scIntro) == 0x3AC, "Class is wrong size!");
