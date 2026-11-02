#pragma once

#include <mu/mu_msg.h>
#include <mu/mu_proc_main.h>
#include <mu/selchar/mu_select_character_name_entry.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

class muNameTask;

// The Names option of the Options screen: the name menu (muNameTask) lists the names; choosing one opens a window that
// asks whether to delete it (Left: Yes, Right: No). B in the name menu goes back.
// HYPOTHESIS: the names of the fields follow muProcOptBtn (the same layout, made by muMenuMain) and what the functions
// do; the window's question is the text 0x2B of m_msgFile.
class muProcName : public muProcMenu {
public:
    enum Step {
        Step_Name = 0,              // the name menu is open
        Step_Confirm = 1,           // the window asks whether to delete the chosen name
        Step_ConfirmLeave = 2,      // the window leaves; the name menu comes back
        Step_Done = 3,              // the name menu closed by itself; proc ends the screen
    };

    enum Obj {
        Obj_NameList0 = 0xAA,       // the first of the three objects of the name list in m_objs
        Obj_Confirm = 0xAD,         // the window that asks whether to delete the name (frame of the colour: 0 No, 1 Yes)
        Obj_NameRemote = 0xAE,      // HYPOTHESIS: the object of the Wii Remote of the name menu
        Mdl_Name0 = 0x15,           // the first of the two models of the name menu in m_mdls
    };

    u8 m_unk66C_0 : 1;              // 0x66C cleared by init (HYPOTHESIS: never set)
    u8 m_yes : 1;                   // 0x66C the window has Yes selected
    int m_unk670;                   // 0x670 cleared by init
    MuMsg* m_msg;                   // 0x674 the text of the window
    void* m_msgFile;                // 0x678 the texts of the window (a file of the menu archive)
    MuSelctChrNameEntry m_entry;    // 0x67C HYPOTHESIS: a keyboard of its own (the name menu builds the one it uses)
    u16 m_unk710;                   // 0x710 cleared by init
    u16 m_unk712;                   // 0x712 cleared by init
    u8 m_unk714[0x71C - 0x714];
    muNameTask* m_nameTask;         // 0x71C the name menu while it is open
    int m_nameNo;                   // 0x720 the name that was chosen (deleted on Yes)
    bool m_shown;                   // 0x724 the name menu has finished coming in
    nw4r::g3d::ResFile* m_remoteRes;    // 0x728 the models of the Wii Remote (muMenuMain::m_resFileA8C)
    nw4r::g3d::ResFile* m_wndRes;       // 0x72C the models of the windows (muMenuMain::m_resFileA9C)
    void* m_msgData;                // 0x730 the texts of the name menu (a file of the menu archive)

    virtual ~muProcName() { }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
    int selectNameMain();
    int confirmMain();
};
