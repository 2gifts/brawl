#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <mu/menu.h>
#include <mu/mu_event.h>
#include <types.h>

// What the scene keeps of the fighters of the event (the event decides them).
// HYPOTHESIS: the names follow how the menu fills them.
struct scEventFighter {
    u8 m_kind;
    u8 m_costume;
};
struct scEventFighters {
    u32 m_unk00;
    u16 m_unk04;
    u8 m_unk06;
    u8 m_count : 3;             // 0x07 how many fighters
    u8 m_unk07 : 5;
    scEventFighter m_fighters[4];
};

class MenuRoot;
class scResourceLoader;

// The scene of the event menu (module sora_scene): loads the pack of the menu and runs the event task of
// sora_menu_event.rel. The task writes the choice of the player to the scene.
class scSelEvent : public muMenu {
public:
    // What start makes: the scene root of the pack and the task.
    struct Work {
        MenuRoot* m_root;                       // 0x00
        muEventTask* m_task;                    // 0x04
    };

    u8 m_unk350[0xC];
    Work* m_work;                               // 0x35C
    muEventSetting m_setting;                   // 0x360 the choice of the player
    scEventFighters m_fighters;                 // 0x384
    int m_firstPad;                             // 0x394 the pad of the first player when the menu is entered anew
    gfArchive* m_archive;                       // 0x398 the pack of the menu
    scResourceLoader* m_loader;                 // 0x39C reads the pack

    scSelEvent(const char* name) : muMenu(name), m_work(NULL), m_firstPad(0), m_archive(NULL), m_loader(NULL) { reset(); }
    static scSelEvent* create();
    void reset();
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
};
static_assert(sizeof(scSelEvent) == 0x3A0, "Class is wrong size!");
