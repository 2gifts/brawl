#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stdio.h>
#include <types.h>

// HYPOTHESIS: a pair that the figure menus keep two of in every unit (an id that is 0xFF while it is not used, and a
// state); the pairs are made when the unit starts.
struct muFigPair {
    int m_id;
    int m_state;

    muFigPair(int id, int state) : m_id(id), m_state(state) { }
};

static muFigPair s_pair0(0xFF, 0);
static muFigPair s_pair1(0xFF, 1);

// The trophy screen that shows one trophy and its stand (module sora_menu_main). The class is only described by the members
// the decompiled functions touch.
// Function names come from the sora_menu_main map.
// HYPOTHESIS: the list of the trophies of the screen: the entries are 0x60 bytes, the name of the file is at 8.
struct tyFigDisp2Entry {
    u8 m_unk0[8];
    const char* m_name; // 0x08
    u8 m_unkC[0x60 - 0xC];
};

struct tyFigDisp2List {
    tyFigDisp2Entry* m_entries; // 0x00
};

class tyFigDisp2Task : public gfTask {
public:
    u8 m_unk40[0x44 - 0x40];
    u8 m_bgNo;                              // 0x44
    u8 m_unk45[0xB0 - 0x45];
    nw4r::g3d::ResFile m_resFile;           // 0xB0
    u8 m_unkB4[0xD4 - 0xB4];
    MuMsg* m_msg;                           // 0xD4
    tyFigDisp2List* m_list;                 // 0xD8
    u8 m_unkDC[0x244 - 0xDC];
    int m_figSlot;                          // 0x244
    int m_figIndex;                         // 0x248
    u8 m_unk24C[0x290 - 0x24C];
    gfFileIOHandle m_handle;                // 0x290
    u8 m_unk294[0x298 - 0x294];
    muNoticeWndTask* m_wnd;                 // 0x298
    nw4r::g3d::ScnGroup* m_scnGroup;        // 0x29C
    void* m_figBuf[4];                      // 0x2A0
    void* m_bgBuf[2];                       // 0x2B0
    int m_bgSlot;                           // 0x2B8
    u8 m_unk2BC[0x300 - 0x2BC];

    tyFigDisp2Task();
    static tyFigDisp2Task* create(int a, int b);
    void initProc(int a, int b);

    void mainStepNoFigureInit();
    void mainStepExitConfirmInit();
    void mainStepLoadFigureInit();
    void mainStepLoadBGInit();
};

void tyFigDisp2Task::mainStepNoFigureInit() {
    m_msg->printIndex(6, 5, NULL);
}

void tyFigDisp2Task::mainStepExitConfirmInit() {
    m_wnd = fn_800BEEB8(1, m_msg->getBinary(), 6, &m_resFile, m_scnGroup, 0x17, 0, 0x2A, 0x2B, 0, 1, 0xF0);
}

void tyFigDisp2Task::mainStepLoadFigureInit() {
    char path[0x40];
    int slot = m_figSlot;
    sprintf(path, "/toy/fig/%s.brres", m_list->m_entries[m_figIndex].m_name);
    m_handle.readRequest(path, m_figBuf[slot], 0, 0);
}

void tyFigDisp2Task::mainStepLoadBGInit() {
    char path[0x40];
    sprintf(path, "/menu/collection/FigureDisp2_bg%d.brres", m_bgNo);
    m_handle.readRequest(path, m_bgBuf[m_bgSlot], 0, 0);
    m_bgSlot++;
    if (m_bgSlot >= 2) {
        m_bgSlot = 0;
    }
}

static tyFigDisp2Task* s_task;

tyFigDisp2Task* tyFigDisp2Task::create(int a, int b) {
    s_task = new ((Heaps::HeapType)2) tyFigDisp2Task();
    s_task->initProc(a, b);
    return s_task;
}
