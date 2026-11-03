#pragma once

#include <StaticAssert.h>
#include <gf/gf_sys_recorder2.h>
#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_scn_mdl_expand.h>
#include <mu/selchar/mu_select_character_name_entry.h>
#include <mu/wifi/mu_wifi_data_load_task.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The window of the main menu that connects to Nintendo Wi-Fi Connection (mode 0, from the top screen) or disconnects
// from it (mode 1, from the Wi-Fi screen). It first asks Yes/No; when the first connection made a new Wi-Fi user it
// then asks for the name that friends see, with the keyboard of the name menu.
// Function names come from the sora_menu_main map; the steps are static functions that a table of the class calls.
// HYPOTHESIS: the names of the fields follow what the steps do with them.
class muWifiCnctWndTask : public gfTask {
public:
    // The models of the window: the model name, the first and the last object it makes, and its priority over the
    // priority of the window.
    struct Model {
        const char* m_name;
        u8 m_start;
        u8 m_end;
        u8 m_priority;
    };

    enum Obj {
        Obj_Wnd = 0,            // the window (its node animation brings it in and takes it out)
        Obj_Icon = 1,           // the picture in the window (at the bone pos00); an animation for each state
        Obj_KeyBase = 2,        // the keyboard of the name entry (mode 0 only)
        Obj_KeySet0 = 3,        // the five sets of keys (hiragana, katakana, letters, numbers, letters of Europe)
        Obj_KeyCursor = 8,
        Obj_KeyLine = 9,
        Obj_Num = 10,
    };

    enum Step {
        Step_CreateWifiInst = 0,    // makes the Wi-Fi interface (mode 0)
        Step_WifiDataLoad = 1,      // the Wi-Fi data load window (muWifiDataLoadTask)
        Step_Appear = 2,            // the window comes in and asks Yes/No
        Step_Confirm = 3,
        Step_Connect = 4,
        Step_Disconnect = 5,
        Step_Done = 6,              // shows the result (and the friend code after a first connection)
        Step_AppearNameEntry = 7,
        Step_NameEntry = 8,
        Step_LeaveNameEntry = 9,
        Step_Leave = 10,            // the window leaves
        Step_Post = 11,             // waits for the save of the friend data, gives the HOME Menu its memory back
        Step_End = 12,              // the window has closed
        Step_Num = 13,
    };

    enum Result {
        Result_Done = 0,            // connected (mode 0) or disconnected (mode 1)
        Result_Error = 1,           // m_errorIndex is the error of the Wi-Fi interface
        Result_BadName = 2,
        Result_Cancelled = 3,       // No, or the Wi-Fi data load window was left
    };

    struct StepFunc {
        void (*m_init)(muWifiCnctWndTask* task);
        int (*m_main)(muWifiCnctWndTask* task);
    };

    int m_mode;                         // 0x40 0: connect, 1: disconnect
    MuObject* m_objects[Obj_Num];       // 0x44
    nw4r::g3d::ScnMdlExpand* m_scnMdl;  // 0x6C the window with the picture at pos00
    nw4r::g3d::ScnGroup* m_scnGroup;    // 0x70 the group of the scene of the menu
    MuMsg* m_msg;                       // 0x74
    muMenuController m_controller;      // 0x78
    int m_unk120;                       // 0x120 cleared by the constructor (HYPOTHESIS: never used)
    bool m_yes;                         // 0x124 Yes is selected
    int m_result;                       // 0x128 a Result
    int m_errorIndex;                   // 0x12C the error of the Wi-Fi interface (muWifiInterfaceTask::getErrorMsgID)
    int m_errorCode;                    // 0x130 the error code that is shown
    bool m_isNewUser;                   // 0x134 the connection made a new Wi-Fi user (the friend code is new)
    int m_step;                         // 0x138 a Step
    int m_prevStep;                     // 0x13C the step whose init function ran last
    int m_timer;                        // 0x140 frames before the window closes by itself after a connection
    s32 m_seHandle;                     // 0x144 the sound of the connection while it runs
    int m_unk148;                       // 0x148 cleared by the constructor (HYPOTHESIS: never used)
    MuSelctChrNameEntry m_nameEntry;    // 0x14C
    nw4r::g3d::ResFile* m_wndRes;       // 0x1E0 the models of the Wi-Fi data load window
    muWifiDataLoadTask* m_dataLoadTask; // 0x1E4
    int m_priority;                     // 0x1E8
    gfSysRecorder2* m_recorder;         // 0x1EC saves the Wi-Fi data after a disconnection
    bool m_hasDelivery;                 // 0x1F0 the Done step lists the data that was received (the window stays)
    bool m_canEnd;                      // 0x1F1 the Post step has nothing left to wait for

    static const StepFunc sStepFuncs[Step_Num];
    static const Model sWndModels[2];
    static const Model sKeyModels[8];

    // HYPOTHESIS: the parameter types follow what muProcTop passes (three model files of muMenuMain, a buffer of the
    // menu archive, the scene group and the priority).
    static muWifiCnctWndTask* create(int mode, nw4r::g3d::ResFile* resFile, void* msgData, nw4r::g3d::ScnGroup* group,
                                     int priority, nw4r::g3d::ResFile* keyRes, nw4r::g3d::ResFile* wndRes);
    muWifiCnctWndTask();
    virtual void processDefault();
    virtual ~muWifiCnctWndTask();
    virtual void exit();
    void initProc(int mode, nw4r::g3d::ResFile* resFile, void* msgData, nw4r::g3d::ScnGroup* group, int priority,
                  nw4r::g3d::ResFile* keyRes, nw4r::g3d::ResFile* wndRes);
    void createObjResFile(const Model* models, int count, nw4r::g3d::ResFile* resFile, bool suffix, int priority);
    void initMsg(void* msgData);
    void dispWndMsg(bool disp, int msgNo);
    // BYTES: the original calls it with bl from mainStepDisconnectInit, which -inline auto would fold it into.
    void saveWifiData() __attribute__((never_inline));

    static void mainStepCreateWifiInstInit(muWifiCnctWndTask* task);
    static int mainStepCreateWifiInstMain(muWifiCnctWndTask* task);
    static void mainStepWifiDataLoadInit(muWifiCnctWndTask* task);
    static int mainStepWifiDataLoadMain(muWifiCnctWndTask* task);
    static void mainStepAppearInit(muWifiCnctWndTask* task);
    static int mainStepAppearMain(muWifiCnctWndTask* task);
    static void mainStepConfirmInit(muWifiCnctWndTask* task);
    static int mainStepConfirmMain(muWifiCnctWndTask* task);
    static void mainStepConnectInit(muWifiCnctWndTask* task);
    static int mainStepConnectMain(muWifiCnctWndTask* task);
    static void mainStepDisconnectInit(muWifiCnctWndTask* task);
    static int mainStepDisconnectMain(muWifiCnctWndTask* task);
    static void mainStepDoneInit(muWifiCnctWndTask* task);
    static int mainStepDoneMain(muWifiCnctWndTask* task);
    static void mainStepAppearNameEntryInit(muWifiCnctWndTask* task);
    static int mainStepAppearNameEntryMain(muWifiCnctWndTask* task);
    static void mainStepNameEntryInit(muWifiCnctWndTask* task);
    static int mainStepNameEntryMain(muWifiCnctWndTask* task);
    static void mainStepLeaveNameEntryInit(muWifiCnctWndTask* task);
    static int mainStepLeaveNameEntryMain(muWifiCnctWndTask* task);
    static void mainStepLeaveInit(muWifiCnctWndTask* task);
    static int mainStepLeaveMain(muWifiCnctWndTask* task);
    static void mainStepPostInit(muWifiCnctWndTask* task);
    static int mainStepPostMain(muWifiCnctWndTask* task);
};
static_assert(sizeof(muWifiCnctWndTask) == 0x1F4, "Class is wrong size!");
