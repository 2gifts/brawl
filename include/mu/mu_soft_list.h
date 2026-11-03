#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <mu/mu_menuroot.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <mu/mu_scn_mdl_expand.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// One title of the Masterpieces list: a record of the list in the relocated file (the public symbol muSoftDataList).
// HYPOTHESIS: the names of the fields follow how muSoftListDataManager and muSoftListTask read them.
struct muSoftListData {
    int m_id;               // 0x00 1 ... ; a negative id ends the list
    int m_category;         // 0x04 the page of the list the title is on
    const char* m_title;    // 0x08
    u32 m_subTitleMsg;      // 0x0C the message of the subtitle
    u32 m_subTitleMsg2;     // 0x10 the second part of the subtitle (m_subTitleLines 1 and 2)
    int m_subTitleLines;    // 0x14 0: one message; 1: both messages on one line; 2: one message on each of two lines
    u32 m_unk18;
};
static_assert(sizeof(muSoftListData) == 0x1C, "Class is wrong size!");

// The titles of the Masterpieces list that can be played, sorted by page, and which of them are new. The flags come from
// the save data (gmGlobalRecord::m_softFlags) and the new flags go back to it when the manager is destroyed.
class muSoftListDataManager {
public:
    enum {
        SoftMax = 0x214,        // the flags of the save data have room for this many titles
        CategoryCount = 11,
        CategorySoftMax = 100,
    };

    // The names of the consoles: CategoryCount in Japanese, then CategoryCount in the other languages. One flat table:
    // every reader indexes it with CategoryCount + category (addi 0xB), not with a [2][11] row.
    static const char* sCategoryNames[2 * CategoryCount];

    muSoftListData* m_list;                                         // 0x0
    int m_count;                                                    // 0x4 the number of titles in the list
    int m_categoryStart[CategoryCount];                             // 0x8 the first title of each page in the list
    int m_categoryCount[CategoryCount];                             // 0x34 the titles of each page that can be played
    int m_categoryLines[CategoryCount];                             // 0x60 the lines they take
    muSoftListData* m_categorySofts[CategoryCount][CategorySoftMax]; // 0x8C the titles of each page that can be played
    u8 m_unlocked[SoftMax];                                         // 0x11BC by id - 1
    u8 m_new[SoftMax];                                              // 0x13D0 by id - 1

    muSoftListDataManager();
    ~muSoftListDataManager();
    void setData(u8* file, u32 size);
    muSoftListData* getSoftListData(int category, int index);
    bool isSubTitleTwoLine(int category, int index);
    u8 isNew(int category, int index);
    void resetNewFlag(int category, int index);
};
static_assert(sizeof(muSoftListDataManager) == 0x15E4, "Class is wrong size!");

// The Masterpieces list: the titles of one console (a category) as a scrolling list of lines, L/R (Y/X, or the
// directions of the menu controller) change the console and B leaves. The titles that are no longer new when the list
// closes are written back to the save data.
class muSoftListTask : public gfTask {
public:
    enum Step {
        Step_Load = 0,      // waits for the menu root and the archive
        Step_Appear = 1,    // the panel comes in
        Step_Main = 2,
        Step_End = 3,
    };
    enum {
        LineCount = 11,     // the lines of the list that are on the screen
        LineHeight = 290,   // the scroll of one line
    };
    // The objects of the list, in the order muObjProcessTask updates them.
    enum Obj {
        Obj_Bg = 0,         // "MenSoftBg_TopN"
        Obj_Panel = 1,      // "MenSoftPanel_TopN"
        Obj_Icon = 2,       // "MenSoftIconE_TopN", the name of the console
        Obj_Bar = 3,        // "MenSoftBar_TopN", the scroll bar
        Obj_Mask = 4,       // "MenSoftMask_TopN"
        Obj_Text = 5,       // "MenSoftText_TopN", one for each line of the list
        Obj_New = Obj_Text + LineCount,     // "MenSoftNew_TopN", the new mark of each line
        Obj_Num = Obj_New + LineCount,
    };

    // A title whose new flag was shown, so that it is cleared when the list closes.
    struct Seen {
        int m_category;
        u32 m_index;
    };

    MenuRoot* m_menuRoot;                                   // 0x40
    int m_step;                                             // 0x44
    int m_frame;                                            // 0x48 the frames of the current step
    muMenuController m_controller;                          // 0x4C
    u8 m_language;                                          // 0xF4 of the menu data of the save data
    MuObject* m_objs[Obj_Num];                              // 0xF8 see Obj
    nw4r::g3d::ScnMdlExpand* m_expands[1 + LineCount];      // 0x164 [0] the panel with the icon, the bar and the lines,
                                                            //       [1 + n] line n with its new mark
    muObjProcessTask* m_objProcessTask;                     // 0x194 updates m_objs
    MuMsg* m_msg;                                           // 0x198 0 the console, 1 the date, 2 + n the title of
                                                            //       line n, 13 + n its subtitle
    nw4r::g3d::ResFile m_resFile;                           // 0x19C
    muSoftListDataManager m_dataManager;                    // 0x1A0
    int m_category;                                         // 0x1784 the console that is shown
    u32 m_line;                                             // 0x1788 the line the list is scrolled to
    u32 m_topIndex;                                         // 0x178C the title on the first line
    bool m_isSecondLine;                                    // 0x1790 the first line shows the second line of a
                                                            //        subtitle of two lines
    u32 m_lineNum;                                          // 0x1794 the lines of the console
    u32 m_softNum;                                          // 0x1798 the titles of the console
    u32 m_scroll;                                           // 0x179C LineHeight for each line
    Seen m_seen[muSoftListDataManager::SoftMax];            // 0x17A0
    u32 m_seenNum;                                          // 0x2840
    gfArchive* m_archive;                                   // 0x2844 "/menu/soft_list/Soft.pac"
    bool m_isArchiveLoaded;                                 // 0x2848
    bool m_isMenuRootLoaded;                                // 0x2849

    muSoftListTask();
    virtual void processAnim();
    virtual ~muSoftListTask();
    bool isEnd();
    void printSoftData();
    bool scrollText(gfPadStatus* pad);
    void initMdlData();
};
static_assert(sizeof(muSoftListTask) == 0x284C, "Class is wrong size!");
