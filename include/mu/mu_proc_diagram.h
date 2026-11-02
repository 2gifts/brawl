#pragma once

#include <StaticAssert.h>
#include <mu/menu.h>  // MuCharKind
#include <mu/mu_msg.h>
#include <mu/mu_proc_menu.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The Chronicle diagrams of the Vault (sora_menu_main): records of each character and each name tag, shown as a table
// (one player), a comparison (two players) or rankings (three and four players).

// The records the diagrams show, in the order of the data table (map: MuDiagDataKind, muDiagGetDataChar ...).
// HYPOTHESIS: the names follow the getters of the map (muDiagGetTotalBeatChar ...); the last three kinds have no getter.
enum MuDiagDataKind {
    MuDiagData_TotalBeat = 0,
    MuDiagData_TotalFall = 1,
    MuDiagData_Suicide = 2,
    MuDiagData_HitRate = 3,
    MuDiagData_GiveDamage = 4,
    MuDiagData_TakeDamage = 5,
    MuDiagData_RecoverDamage = 6,
    MuDiagData_MaxDamage = 7,
    MuDiagData_RoundNum = 8,
    MuDiagData_ChampNum = 9,
    MuDiagData_KetsuNum = 10,
    MuDiagData_PlayTime = 11,
    MuDiagData_UseRate = 12,
    MuDiagData_JoinAverage = 13,
    MuDiagData_WalkDistance = 14,
    MuDiagData_JumpDistance = 15,
    MuDiagData_FallDistance = 16,
    MuDiagData_BlowDistance = 17,
    MuDiagData_GetCoin = 18,
    MuDiagData_MakeDropCoin = 19,
    MuDiagData_DropCoin = 20,
    MuDiagData_FallButtocks = 21,
    MuDiagData_SwimDistance = 22,
    MuDiagData_SwimFall = 23,
    MuDiagData_Unk24 = 24,
    MuDiagData_Unk25 = 25,
    MuDiagData_Unk26 = 26,
    MuDiagData_Num = 27,
};

// How muDiagTransformDataValue writes a value.
// HYPOTHESIS: the names come from the formats it uses ("%d", "%d:%02d", "%.2f" of the value / 100, centimetres as metres
// or feet).
enum MuDiagValueKind {
    MuDiagValue_Number = 0,
    MuDiagValue_MinSec = 1,
    MuDiagValue_Hundredths = 2,
    MuDiagValue_Distance = 3,
};

// One record of the diagrams: how to read it for a character and for a name tag, its unit (the message after the value)
// and its two messages (the title and the explanation).
// HYPOTHESIS: the struct name; the field names follow muDiagGetDataChar/Name/UnitKind/TitleMsgID/ExplMsgID of the map.
struct muDiagDataInfo {
    u32 (*m_getChar)(MuCharKind kind);
    u32 (*m_getName)(int nameNo);
    u8 m_unitKind;
    u8 m_valueKind;  // MuDiagValueKind
    u32 m_titleMsgId;
    u32 m_explMsgId;
};
static_assert(sizeof(muDiagDataInfo) == 0x14, "Class is wrong size!");

// The number of name tags of the records (GameGlobal::getGlobalRecordNameDatap)
enum { MuDiag_NameNum = 120 };

// The characters in the order the diagrams list them (map: muDiagGetMuCharKindRaw/RawNo).
MuCharKind muDiagGetMuCharKindRaw(int no);
int muDiagGetMuCharKindRawNo(MuCharKind kind);

u32 muDiagGetDataChar(MuDiagDataKind kind, MuCharKind chara);
u32 muDiagGetDataName(MuDiagDataKind kind, int nameNo);
u32 muDiagGetDataUnitKind(MuDiagDataKind kind);
u32 muDiagGetDataTitleMsgID(MuDiagDataKind kind);
u32 muDiagGetDataExplMsgID(MuDiagDataKind kind);
int muDiagTransformDataValue(char* buf, int* unitMsgId, MuDiagDataKind kind, u32 value, u32 max);

// A page of the diagrams: the table of one player, the comparison of two, the rankings of three or four. muProcDiagram
// makes the four pages and runs the one of the number of players chosen.
// HYPOTHESIS: the roles of the virtual functions follow their names in the map and what muProcDiagram::init does with
// them (setConf gets the scene group, the message of the screen and the first message window the page may use;
// getUseNumOfMsgWnd tells how many windows it uses).
class muProcDiagramPageBase : public muProcMenu {
public:
    muProcDiagramPageBase(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcMenu(objs, mdls) { m_itemCount = 0; }
    virtual ~muProcDiagramPageBase() { }
    virtual void playSE_CursorMove() = 0;
    virtual void setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd);
    virtual bool isPageChanging();
    virtual bool controlProc();
    virtual void begin(int side);
    virtual void main();
    virtual void post(int side);
    virtual void finish();
    virtual int getUseNumOfMsgWnd();
};

// The first page: the KOs of each character against each other character (the rows KO'd the columns), the rows in the
// order of their total. Up and Down scroll the rows, Left and Right the columns.
// HYPOTHESIS: what the page shows follows makeRankTbl and updateDispLine (gmGlobalRecord::CharaNumData::m_beatCount).
class muProcDiagram1p : public muProcDiagramPageBase {
public:
    // What the page does (the same states as muProcDiagram2p).
    enum State {
        State_Hidden = 0,
        State_Shown = 1,
        State_Come = 2,
        State_Leave = 3,
        State_ComeSide = 4,
        State_LeaveSide = 5,
        State_Num = 6,
    };

    enum {
        Obj_RowArrow = 140,     // its visibility shows the arrows of the rows
        Obj_ColumnArrow = 141,  // its visibility shows the arrows of the columns
        Obj_Row = 142,          // the six rows (142 ... 147): the icon and rank of the character and its total
        Obj_Column = 148,       // the five columns (148 ... 152): the icon of the character and its values
        Mdl_Row = 14,           // the two models put in the scene group while the page is shown
        Mdl_Column = 15,
        RowNum = 6,             // the rows shown
        ColumnNum = 5,          // the columns shown
        CharaNum = 35,          // the characters the page can list (MuCharKind 0 ... 34)
        Msg_Rank = 0,           // the message windows from m_msgWnd on: the rank of each row,
        Msg_Total = 6,          // the total of each row,
        Msg_Value = 12,         // then the values of the rows, ColumnNum for each row
        MsgWndNum = 42,
    };

    // A row: a character and the KOs it made of all the characters listed.
    struct Row {
        MuCharKind m_chara;  // 0x0
        u32 m_total;         // 0x4
        u8 m_no;             // 0x8 the place of the character in m_charas (orders the rows with the same total)
    };

    // The characters that can be used and the rows of the table.
    struct Table {
        MuCharKind m_charas[CharaNum];  // 0x00 in the order of MuCharKind
        u8 m_charaNum;                  // 0x8C
        Row m_rows[CharaNum];           // 0x90 the most KOs first
    };

    int m_msgWnd;    // 0x66C the first message window of the page
    int m_state;     // 0x670 State
    MuMsg* m_msg;    // 0x674
    Table* m_table;  // 0x678
    u8 m_column;     // 0x67C the first column shown
    u8 m_row;        // 0x67D the first row shown

    static const u8 sStateAnimIdx[State_Num];  // the entry of sStateAnims of each state
    static MuAnimIdxData sStateAnims[State_Num];
    static MuAnimIdxData sCursorAnims[3];      // the page comes, scrolled back, scrolled on

    muProcDiagram1p(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcDiagramPageBase(objs, mdls) { }
    virtual ~muProcDiagram1p();
    virtual void playSE_CursorMove();
    virtual void setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd);
    virtual bool isPageChanging();
    virtual bool controlProc();
    virtual void begin(int side);
    virtual void main();
    virtual void post(int side);
    virtual void finish();
    virtual int getUseNumOfMsgWnd();
    void makeRankTbl();
    void initMsg();
    void updateDisp(bool arrows);
    void updateDispLine(int line, int row, Row* data);
    int getRowArrowVisible();
    int getColumnArrowVisible();
    void changeState(int state);
};
static_assert(sizeof(muProcDiagram1p) == 0x680, "Class is wrong size!");

// The second page: for each record (Up and Down choose it), the five characters with the highest value.
// HYPOTHESIS: the class is named for the page number of the map (muProcDiagram2p); what it shows follows setConf and
// updateDisp.
class muProcDiagram2p : public muProcDiagramPageBase {
public:
    // What the page does.
    // HYPOTHESIS: the names follow the animations each state starts (sStateAnims) and the state main moves to after it.
    enum State {
        State_Hidden = 0,
        State_Shown = 1,
        State_Come = 2,
        State_Leave = 3,
        State_ComeSide = 4,
        State_LeaveSide = 5,
        State_Num = 6,
    };

    enum {
        Obj_Page = 153,         // the messages are attached to it; its visibility shows the arrows
        Obj_CharIcon = 154,     // the five icons of the characters (154 ... 158)
        Mdl_Page = 16,          // put in the scene group while the page is shown
        RankNum = 5,            // the characters shown for each record
        KindNum = 24,           // the records the page goes through
        Msg_Title = 0,          // the message windows from m_msgWnd on: the title of the record, its explanation,
        Msg_Expl = 1,           // then the values of the five characters
        Msg_Value = 2,
        MsgWndNum = 7,
    };

    // A character and its value of a record.
    struct Rank {
        MuCharKind m_chara;
        u32 m_value;
    };

    int m_msgWnd;               // 0x66C the first message window of the page
    int m_state;                // 0x670 State
    MuMsg* m_msg;               // 0x674
    int m_index;                // 0x678 the place of the record shown in sKindOrder
    MuDiagDataKind m_kind;      // 0x67C the record shown
    Rank (*m_ranks)[RankNum];   // 0x680 RankNum characters for each of the KindNum records, best first

    static const u8 sStateAnimIdx[State_Num];       // the entry of sStateAnims of each state
    static MuAnimIdxData sStateAnims[State_Num];
    static MuAnimIdxData sCursorAnims[3];           // the page comes, the previous record, the next record
    static MuDiagDataKind sKindOrder[KindNum];

    muProcDiagram2p(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcDiagramPageBase(objs, mdls) { }
    virtual ~muProcDiagram2p();
    virtual void playSE_CursorMove();
    virtual void setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd);
    virtual bool isPageChanging();
    virtual bool controlProc();
    virtual void begin(int side);
    virtual void main();
    virtual void post(int side);
    virtual void finish();
    virtual int getUseNumOfMsgWnd();
    int getArrowVisible();
    void changeState(int state);
    void updateDisp(bool arrows);
};
static_assert(sizeof(muProcDiagram2p) == 0x684, "Class is wrong size!");

// What the rankings of three and four players share.
class muProcDiagram34pBase : public muProcDiagramPageBase {
public:
    muProcDiagram34pBase(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcDiagramPageBase(objs, mdls) { }
    virtual ~muProcDiagram34pBase() { }
};

class muProcDiagram3p : public muProcDiagram34pBase {
public:
    u32 m_unk66C[15];
    muProcDiagram3p(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcDiagram34pBase(objs, mdls) { }
    virtual ~muProcDiagram3p();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcDiagram3p) == 0x6A8, "Class is wrong size!");

class muProcDiagram4p : public muProcDiagram34pBase {
public:
    u32 m_unk66C[21];
    muProcDiagram4p(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcDiagram34pBase(objs, mdls) { }
    virtual ~muProcDiagram4p();
    virtual void playSE_CursorMove();
};
static_assert(sizeof(muProcDiagram4p) == 0x6C0, "Class is wrong size!");

// The Chronicle diagrams screen of the Vault: a bar at the top names the page shown and its neighbours, the arrows of the
// bar show the buttons that turn the page, and a panel under it holds the page.
// HYPOTHESIS: the field roles follow init and proc; the size is not known (nothing carved makes the screen yet).
class muProcDiagram : public muProcMenu {
public:
    enum Page {
        Page_1p = 0,
        Page_2p = 1,
        Page_3p = 2,
        Page_4p = 3,
        Page_Num = 4,
    };

    // HYPOTHESIS: the roles of these objects follow init, setHelpArrowVisible and setPanelState.
    enum {
        Obj_Panel = 111,        // its texture shows the page
        Obj_Bar = 136,          // the page names are written on it; its visibility shows the arrows
        Obj_Unk137 = 137,
        Obj_ArrowPrev = 138,    // attached to the node "pos01" of the bar model
        Obj_ArrowNext = 139,    // attached to the node "pos02"
        Mdl_Bar = 13,           // the model the arrows are attached to
    };

    // The message windows of the bar
    enum {
        Msg_PrevPage = 0,
        Msg_Page = 1,
        Msg_NextPage = 2,
        Msg_PageWndNum = 3,     // the pages get the windows after these
    };

    // What the panel does (setPanelState).
    enum PanelState {
        Panel_Hidden = 0,
        Panel_Come = 1,
        Panel_Shown = 2,
        Panel_Leave = 3,
    };

    // What the help bar does (setHelpState).
    // HYPOTHESIS: the names follow the animations each state starts and the state helpMain moves to after it.
    enum HelpState {
        Help_Hidden = 0,
        Help_Come = 1,
        Help_Shown = 2,
        Help_Leave = 3,
        Help_TurnNext = 4,
        Help_TurnPrev = 5,
        Help_Unk6 = 6,
        Help_Unk7 = 7,
        Help_Unk8 = 8,
    };

    u8 m_unk66C_0 : 1;                          // 0x66C cleared by init
    int m_page;                                 // 0x670 Page
    int m_panelState;                           // 0x674 PanelState
    int m_helpState;                            // 0x678 HelpState
    u32 m_unk67C;                               // 0x67C cleared by init
    muProcDiagramPageBase* m_pages[Page_Num];   // 0x680
    MuMsg* m_msg;                               // 0x690
    void* m_msgData;                            // 0x694

    // The title of each page and the short name the bar shows for it next to the page shown.
    // HYPOTHESIS: the roles follow init and movePageMain (the title goes to the middle window, the names to the sides).
    struct PageMsg {
        u32 m_title;
        u32 m_name;
    };

    // HYPOTHESIS: the tables are static members. The functions that read them name each one separately instead of
    // reaching it from the base of the literals, as for objects with external linkage, and each is placed after the
    // literals of the first function that reads it.
    static const u8 sBarMsgBone[Msg_PageWndNum][2];   // the message windows of the bar and the bones they are written on
    static const u8 sPageFrame[Page_Num];             // the frame of the texture of the panel for each page
    // The frame of the texture of the page buttons for each pad type.
    // HYPOTHESIS: the index is gfPadType (the frames show the buttons of that controller).
    static const u8 sPadTypeFrame[4];
    // The buttons that turn the page for each pad type: the next page, the previous page.
    // HYPOTHESIS: the masks are gfPadButtons bits of the shoulder buttons of each controller.
    static const u32 sPageButtons[4][2];
    static const PageMsg sPageMsg[Page_Num];

    muProcDiagram(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData);
    virtual ~muProcDiagram();
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
    void setHelpArrowVisible(bool visible);
    void setPanelState(int state);
    void helpMain();
    void setHelpState(int state);
    void movePageMain();
};
