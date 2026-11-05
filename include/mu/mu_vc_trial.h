#pragma once

#include <StaticAssert.h>
#include <if/if_utility.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <types.h>

// HYPOTHESIS: an owning pointer that deletes what it holds when it gets another object or goes away (the semantics of
// std::auto_ptr; MSL has no header for it in this tree). Its constructor and destructor are the weak functions that
// __construct_array is given for the arrays of models.
template <class T>
class muAutoPtr {
public:
    muAutoPtr() : m_ptr(NULL) { }
    ~muAutoPtr() { delete m_ptr; }
    void reset(T* ptr) {
        if (m_ptr != ptr) {
            delete m_ptr;
            m_ptr = ptr;
        }
    }
    T* get() const { return m_ptr; }
    T* operator->() const { return m_ptr; }

private:
    T* m_ptr;
};

// HYPOTHESIS: a pointer to a task that the holder ends (gfTask::exit) when it gets another one or goes away.
template <class T>
class muTaskHolder {
public:
    muTaskHolder() : m_task(NULL) { }
    ~muTaskHolder() { reset(NULL); }
    void reset(T* task) {
        if (m_task != NULL) {
            m_task->exit();
        }
        m_task = task;
    }
    T* get() const { return m_task; }
    T* operator->() const { return m_task; }

private:
    T* m_task;
};

// HYPOTHESIS: a pointer to a g3d object that the holder destroys (G3dObj::Destroy) when it gets another one or goes away.
template <class T>
class muG3dObjHolder {
public:
    muG3dObjHolder() : m_obj(NULL) { }
    ~muG3dObjHolder() { reset(NULL); }
    void reset(T* obj) {
        if (m_obj != NULL) {
            m_obj->Destroy();
        }
        m_obj = obj;
    }
    T* get() const { return m_obj; }
    T* operator->() const { return m_obj; }
    // HYPOTHESIS: scene-parent comparisons access the held object directly.
    operator T*() const { return m_obj; }

private:
    T* m_obj;
};

// The trial play of the Virtual Console games in the vault (Masterpieces): a page of six titles, the time each one can be
// played and the controller it needs; deciding asks for a confirmation and then the scene (sora_scene) starts the game.
// The name is from the RTTI; the function names are guesses from what they do (HYPOTHESIS), the module map has none.
class muVCTrialTask : public utility::StandardMenu {
public:
    enum {
        NumTitles = 16,             // the entries of sTitles
        NumShownTitles = 12,        // the titles that the vault can show, in the order of the pages
        NumPanels = 6,              // the titles on one page
        NumColumns = 3,             // the titles in one row of a page
        NumIcons = 3,
        NumNotices = 4,
    };

    // The console of a title: the text of the info window, and the controller the title needs.
    enum Console {
        Console_NES = 0,            // HYPOTHESIS: 0 and 1 run on a Wii Remote held sideways
        Console_SNES = 1,
        Console_N64 = 2,            // HYPOTHESIS: 2 and 3 need a Classic Controller (or a GameCube controller)
        Console_Unk3 = 3,
    };

    // What the scene passes: a flags byte for each title (from the save data) and the title to start with.
    struct Param {
        u8 m_titleFlags[NumTitles]; // 0x00 the Title::m_flags of each title
        int m_title;                // 0x10 the title the cursor starts on (-1 for none)
    };

    // One title of the vault.
    struct Title {
        int m_nameMsg;              // 0x00 the text of the name of the title
        int m_console;              // 0x04 a Console
        int m_playSec;              // 0x08 how long the trial lasts (seconds)
        float m_texFrame;           // 0x0C the picture of the title in the panel
        union {
            u8 m_flags;             // 0x10 Param::m_titleFlags
            struct {
                u8 m_isAvailable : 1;   // the title is in the vault
                u8 m_isNew : 1;         // HYPOTHESIS: shows the mark of the panel
                u8 m_unk10 : 6;
            };
        };
    };

    // The states of main. HYPOTHESIS: the original's states are source line numbers (a coroutine macro); the names
    // follow what main does in each one.
    enum State {
        State_Start = 0,
        State_Appear = 541,         // waits until the menu has appeared
        State_CheckNand = 552,      // checks that the save data of the trial fits on the Wii system memory
        State_Select = 572,         // the player chooses a title
        State_NoticeOpen = 603,     // a notice was opened
        State_Notice = 604,         // waits for the answer of the notice, then opens the next one or starts the title
        State_Save = 660,           // waits until the save data with the new "played" flag is written
        State_SaveDone = 672,
        State_SaveDone2 = 673,
        State_FadeOut = 676,        // waits for the fade to end
        State_Leave = 685,          // the menu is left without a title: shows the loading screen
        State_Loading = 698,        // waits for the scene to load
        State_End = 709,
    };

    static muVCTrialTask* create(const Param* param);
    // The length of the trial of a title (seconds).
    int getPlaySec(int title);

    virtual ~muVCTrialTask();
    virtual bool preLoad() { return true; }
    virtual void loadResource();
    virtual bool checkLoadFinish() const { return true; }
    virtual void createModel();
    virtual void update();

    void initTitles();
    void dispInfo();
    void setPage(int cursor, int page);
    void main();
    void moveCursor();
    void decide();
    void openNotice(int msg);

    // The title that the cursor is on (the table of the titles is a static of mu_vc_trial.cpp).
    inline const Title& getCurTitle() const;
    int getCurTitleNo() const { return m_titles[m_page * NumPanels + m_cursor]; }

    int m_isEnd;                    // 0x1AC the scene leaves when it is set
    int m_cursor;                   // 0x1B0 the panel of the page that the cursor is on
    int m_unk1B4;
    int m_wait;                     // 0x1B8 the frames until the menu has appeared
    int m_state;                    // 0x1BC a State
    int m_page;                     // 0x1C0
    int m_numTitles;                // 0x1C4 the titles that are in the vault
    int m_titles[NumTitles];        // 0x1C8 the titles that are in the vault, in the order of the pages
    int m_selected;                 // 0x208 the title that is started (-1 for none); the scene reads it
    int m_initTitle;                // 0x20C Param::m_title
    int m_pageAnim;                 // 0x210 the last page arrow animation (1: back, 2: next)
    int m_notices[NumNotices];      // 0x214 the texts of the notices that are still to show (-1 ends the list)
    bool m_isNandFull;              // 0x224 the save data of the trial does not fit
    int m_nandWait;                 // 0x228 the frames until the Wii system memory is checked again
    int m_unk22C;
    int m_unk230;
    utility::AutoResFile m_wndRes;  // 0x234 the notice window
    utility::AutoResFile m_resFile; // 0x268 the models of the menu
    utility::AutoFile m_msgFile;    // 0x29C the texts of the menu
    muAutoPtr<MuObject> m_bg;       // 0x2B4
    muAutoPtr<MuObject> m_info;     // 0x2B8 the info window (title, console and time) and the page arrows
    muAutoPtr<MuObject> m_panels[NumPanels];        // 0x2BC
    muAutoPtr<MuObject> m_icons[NumIcons];          // 0x2D4 [0]: the controller the title needs
    muAutoPtr<MuObject> m_cursorObj;                // 0x2E0
    muAutoPtr<MuObject> m_loading;                  // 0x2E4 the loading screen
    muAutoPtr<MuMsg> m_msg;                         // 0x2E8
    muG3dObjHolder<nw4r::g3d::ScnMdlExpand> m_expand;   // 0x2EC puts the panels on the nodes of the page
    muTaskHolder<muNoticeWndTask> m_notice;         // 0x2F0
    muTaskHolder<utility::FadeOutTask> m_fade;      // 0x2F4

private:
    muVCTrialTask(const Param* param);
};
static_assert(sizeof(muVCTrialTask) == 0x2F8, "Class is wrong size!");
