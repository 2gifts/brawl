#pragma once

#include <StaticAssert.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_proc_main.h>
#include <nt/nt_etc_global.h>
#include <nt/nt_etc_nwc24.h>
#include <nt/nt_etc_xchg.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

// The Wi-Fi option of the Options screen (mu_proc_wifi_opt.o in the map of the debug build): two entries, the
// permission to watch the videos of others (nteGlobal::isPermitWatching) and the permission to receive the service of
// WiiConnect24 (nteGlobal::isPermitService); each is only shown when nteGlobal::isEnableButton allows it. A on an entry
// opens a list of the choices (up and down, A to take, B to leave it). Changing the service registers or deletes the
// download task of WiiConnect24 through an nteExchange while a notice window waits; the errors of WiiConnect24 come
// back through notify_Error and are shown in a notice window.
// HYPOTHESIS: the names of the fields, of the steps and of the tables follow what the functions do.
class muProcOptWifi : public muProcMenu, public IWiiConnect24_Error {
public:
    enum Step {
        Step_Select = 0,            // the cursor moves between the entries
        Step_List = 1,              // the list of the choices of an entry is open
        Step_Exchange = 2,          // the choice changed the service: start the job of WiiConnect24
        Step_WaitJob = 3,           // wait for the job (up to three seconds before the notice window opens)
        Step_OpenWnd = 4,
        Step_WaitWnd = 5,           // the notice window is open until the job is over (one second at least)
        Step_CloseWnd = 6,
        Step_Apply = 7,             // register or delete the download task
        Step_Error = 8,             // the window of an error is open
        Step_Leave = 9,
    };

    enum Entry {
        Entry_Watching = 0,
        Entry_Service = 1,
        EntryCount = 2,
        ChoiceCount = 3,            // the lines of the list of the choices
        Obj_Panel = 0xD2,           // the panel of the option (comes in and goes out)
        Obj_Entry0 = 0xD3,          // the two entries
        Obj_List = 0xD5,            // the list of the choices
        Mdl_Entries = 0x1D,         // the model the entries are attached to (its nodes are pos00 and pos01)
        Msg_List0 = 4,              // the five lines of m_msg on the list of the choices
        Msg_ListEnd = 9,
    };

    bool m_permit[EntryCount];      // 0x670 the permissions as they are now (watching, service)
    bool m_savedPermit[EntryCount]; // 0x672 the permissions before the list was opened
    u32 m_entryCount;               // 0x674 how many entries are shown
    MuMsg* m_msg;                   // 0x678 the texts of the screen (0, 1: titles; 2, 3: the entries; 4 ... 8: the list)
    MuMsg* m_errorMsg;              // 0x67C the text that goes before the error code
    nw4r::g3d::ResFile* m_resFile;  // 0x680 the resources of the notice windows
    muNoticeWndTask* m_wnd;         // 0x684
    nteExchange* m_exchange;        // 0x688
    bool m_needSave;                // 0x68C the system data must be saved after the job
    bool m_error;                   // 0x68D an error was told while the job ran
    int m_frame;                    // 0x690
    // The tables of the list of the choices, by entry and by the permission as it was when the list was opened.
    int m_listKind[EntryCount][2];  // 0x694 3: the list has a third line; 2: the second line (message 6) is hidden
    int m_listMin[EntryCount][2];   // 0x6A4 the first choice
    int m_listMax[EntryCount][2];   // 0x6B4 the last choice
    int m_listStart[EntryCount][2]; // 0x6C4 the choice the cursor starts on
    bool m_listPermit[EntryCount][2][ChoiceCount]; // 0x6D4 the permission each choice gives
    int m_listDLTask[2][ChoiceCount];              // 0x6E0 what each choice of the service does to the download task
                                                   //       (1: delete it, 2: delete all of them)
    int m_listMsg[EntryCount][2][Msg_ListEnd - Msg_List0];  // 0x6F8 the message of each line of the list
    bool m_listShown;               // 0x748 the list of the choices is attached to the scene
    int m_choice;                   // 0x74C
    int m_choiceMin;                // 0x750
    int m_choiceMax;                // 0x754

    muProcOptWifi(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData, void* errorMsgData, nw4r::g3d::ResFile* resFile);
    virtual ~muProcOptWifi() {
        delete m_msg;
        delete m_errorMsg;
    }
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
    virtual void notify_Error(int kind, int unk, int code);
    void addModelMessage();

    // HYPOTHESIS: not in the map (inlined); the names follow what they do.
    void removeModelMessage() {
        if (m_listShown) {
            m_group->Remove(m_objs[Obj_List]->m_sceneModel);
            m_msg->detachMuObject(m_objs[Obj_List]);
            m_listShown = false;
        }
    }
    void setHelpMsg() {
        m_panel->setMsg(0x96 + m_cursor * 2 + (m_cursor == Entry_Watching ? m_permit[Entry_Watching] == false
                                                                           : m_permit[Entry_Service] == false),
                        1, 0);
    }
    void dispPermit() {
        m_objs[Obj_Entry0 + Entry_Watching]->setFrameMatCol(m_permit[Entry_Watching]);
        u32 msgNo = 3;
        if (m_permit[Entry_Watching] == true) {
            msgNo = 2;
        }
        m_msg->printIndex(2, msgNo, NULL);
        m_objs[Obj_Entry0 + Entry_Service]->setFrameMatCol(m_permit[Entry_Service]);
        msgNo = 5;
        if (m_permit[Entry_Service] == true) {
            msgNo = 4;
        }
        m_msg->printIndex(3, msgNo, NULL);
        setHelpMsg();
    }
    // The permissions are taken (they become the ones to come back to).
    void savePermit() {
        m_savedPermit[Entry_Watching] = m_permit[Entry_Watching];
        m_savedPermit[Entry_Service] = m_permit[Entry_Service];
        dispPermit();
    }
    // The job failed: the permissions come back to the ones before the list.
    void restorePermit() {
        m_permit[Entry_Watching] = m_savedPermit[Entry_Watching];
        m_permit[Entry_Service] = m_savedPermit[Entry_Service];
        nteGlobal::setPermitWatching(m_permit[Entry_Watching]);
        nteGlobal::setPermitService(m_permit[Entry_Service]);
        dispPermit();
    }
};
static_assert(sizeof(muProcOptWifi) == 0x758, "Class is wrong size!");
