#pragma once

#include <StaticAssert.h>
#include <mu/mu_menu_controller.h>
#include <types.h>

namespace nw4r { namespace g3d { class ResFile; class ScnGroup; } }

class MuObject;
class MuMsg;

class IfWifiIntrMenu {
public:
    nw4r::g3d::ScnGroup* m_group;
    MuObject* m_object;
    MuMsg* m_msg;
    // HYPOTHESIS: 1 for a single line, 2 when the menu offers a second choice.
    s32 m_mode;
    u32 unk10;
    // HYPOTHESIS: the highlighted choice while the menu is open.
    s32 m_cursor;
    s32 m_padNo;
    // HYPOTHESIS: 1 while the interrupt menu is on screen.
    u8 m_active;
    // HYPOTHESIS: frames left before the menu accepts a button.
    s32 m_timer;
    muMenuController m_controller;

    IfWifiIntrMenu();
    virtual ~IfWifiIntrMenu();
    // HYPOTHESIS: the second argument is the message data passed to MuMsg::setMsgData.
    void createModel(nw4r::g3d::ResFile* res, void* msgData);
    void startMelee(nw4r::g3d::ScnGroup* group);
    void startMenu(int padNo, int kind);
};
static_assert(sizeof(IfWifiIntrMenu) == 0xD0, "Class is the wrong size!");
