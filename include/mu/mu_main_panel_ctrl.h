#pragma once

#include <StaticAssert.h>
#include <mu/mu_object.h>
#include <mu/mu_panel_ctrl.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// The panel of the main menu: the background (its colour tells the group of the screen), the panel with the title of the
// screen, and the balloon with the text of the help. The screens of the menu tell it where they are (changeProcess1) and it
// plays the animations from one screen to the next.
// HYPOTHESIS: the names of the fields and of the animations follow what the functions do.
class muMainPanelCtrlTask : public muPanelCtrlTask {
public:
    enum Obj {
        Obj_Bg = 0,         // MenMainBg_TopN
        Obj_Panel = 1,      // MenMainPanel_TopN
        Obj_Title = 2,      // MenMainTitle_TopN, which also carries the text of the help
        ObjCount = 3,
    };

    // What processAnim does when the animations are far enough (m_timer frames).
    enum State {
        State_None = 0,
        State_Show = 1,         // after 10 frames: the text and the title
        State_ShowNow = 2,      // after 1 frame: the text and the title
        State_Clear = 3,        // after 5 frames: the text is cleared (the panel goes out)
        State_ShowText = 4,     // after 5 frames: the text
    };

    // The screens of the menu (1 ... 34): the panel, the frame of the title and the colour of the background.
    // HYPOTHESIS: the names of the fields follow where changeProcess1 passes them.
    struct Screen {
        u8 m_panel;
        u8 m_titleFrame;
        u8 m_bg;
    };
    enum {
        ScreenCount = 34,
        PanelCount = 5,     // the panels 1 ... 4 (0: none)
    };
    static const Screen sScreens[ScreenCount];
    // The animation of the title when the menu goes from panel p to a screen without it, and back.
    static const u8 sTitleLeaveAnims[PanelCount];
    static const u8 sTitleComeAnims[PanelCount];

    float m_titleFrame;             // 0x4C the frame of the texture of the title
    u32 m_panel;                    // 0x50 the panel that is in (0: none)
    int m_bg;                       // 0x54 the colour of the background
    int m_state;                    // 0x58 State
    nw4r::g3d::ResFile m_resFile;   // 0x5C
    muObjProcessTask* m_objProcess; // 0x60
    MuObject* m_objs[ObjCount];     // 0x64
    int m_bgAnim;                   // 0x70 the animation of Obj_Bg
    int m_panelAnim;                // 0x74 the animation of Obj_Panel
    int m_titleAnim;                // 0x78 the animation of Obj_Title
    int m_screen;                   // 0x7C the screen the menu is on (the argument of changeProcess1)
    void* m_msgData;                // 0x80 the message file of the text

    muMainPanelCtrlTask(void* resFile, void* msgData);
    virtual void processAnim();
    virtual ~muMainPanelCtrlTask();
    virtual void setMsg(int msgNo, int print, void* msgData);
    virtual void changeProcess1(int screen, int msgNo);
    virtual void changeProcess(u32 panel, int msgNo, int bg, float titleFrame);
    virtual void frameCancel();
    virtual void startupSpecialMelee();
};
static_assert(sizeof(muMainPanelCtrlTask) == 0x84, "Class is wrong size!");
