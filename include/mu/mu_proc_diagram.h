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
u8 muDiagGetDataUnitKind(MuDiagDataKind kind);
u32 muDiagGetDataTitleMsgID(MuDiagDataKind kind);
u32 muDiagGetDataExplMsgID(MuDiagDataKind kind);
void muDiagTransformDataValue(char* buf, u32* unitMsgId, MuDiagDataKind kind, u32 value, u32 max);

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
    virtual void begin();
    virtual void main();
    virtual void post();
    virtual void finish();
    virtual int getUseNumOfMsgWnd();
};

class muProcDiagram1p : public muProcDiagramPageBase {
public:
    u32 m_unk66C[5];
    muProcDiagram1p(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcDiagramPageBase(objs, mdls) { }
    virtual ~muProcDiagram1p();
    virtual void playSE_CursorMove();
    virtual void setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd);
    virtual bool isPageChanging();
    virtual bool controlProc();
    virtual void begin();
    virtual void main();
    virtual void post();
    virtual void finish();
    virtual int getUseNumOfMsgWnd();
};
static_assert(sizeof(muProcDiagram1p) == 0x680, "Class is wrong size!");

class muProcDiagram2p : public muProcDiagramPageBase {
public:
    u32 m_unk66C[6];
    muProcDiagram2p(MuObject** objs, nw4r::g3d::G3dObj** mdls) : muProcDiagramPageBase(objs, mdls) { }
    virtual ~muProcDiagram2p();
    virtual void playSE_CursorMove();
    virtual void setConf(nw4r::g3d::ScnGroup* group, MuMsg* msg, int msgWnd);
    virtual bool isPageChanging();
    virtual bool controlProc();
    virtual void begin();
    virtual void main();
    virtual void post();
    virtual void finish();
    virtual int getUseNumOfMsgWnd();
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

// The Chronicle diagrams screen of the Vault.
// HYPOTHESIS: the field roles follow init (m_msg made there, m_msgData given to it); the size is not known (nothing carved
// makes the screen yet).
class muProcDiagram : public muProcMenu {
public:
    enum Page {
        Page_1p = 0,
        Page_2p = 1,
        Page_3p = 2,
        Page_4p = 3,
        Page_Num = 4,
    };

    // HYPOTHESIS: the roles of these objects follow exit (the messages are attached to Obj_MsgWnd; the other two are put
    // in the scene group by init).
    enum {
        Obj_Unk111 = 111,
        Obj_MsgWnd = 136,
        Mdl_Unk13 = 13,
    };

    u32 m_unk66C;
    u32 m_unk670;
    u32 m_unk674;
    u32 m_unk678;
    u32 m_unk67C;
    muProcDiagramPageBase* m_pages[Page_Num];   // 0x680
    MuMsg* m_msg;                               // 0x690
    void* m_msgData;                            // 0x694

    muProcDiagram(MuObject** objs, nw4r::g3d::G3dObj** mdls, void* msgData);
    virtual ~muProcDiagram();
    virtual void init(nw4r::g3d::ScnGroup* group, int from);
    virtual int proc();
    virtual void exit();
    virtual void playSE_CursorMove();
};
