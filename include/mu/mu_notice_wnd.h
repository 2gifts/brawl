#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <mu/mu_menu_controller.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

class MuObject;
class MuMsg;
namespace nw4r { namespace g3d { class ScnGroup; } }

// The window with a text and a few buttons that the menus open for a notice (for example "the save data is broken").
// HYPOTHESIS: the fields are the ones that the menus of the start of the game read and write; the rest is not known.
class muNoticeWndTask : public gfTask {
public:
    // HYPOTHESIS: pointer and controller types follow the notice task calls.
    MuObject* unk40;
    nw4r::g3d::ScnGroup* unk44;
    MuMsg* unk48;
    void* unk4C;
    u32 unk50;
    muMenuController unk54;
    int unkFC;
    u8 m_selected;              // 0x100 the button that was chosen
    u8 m_unk101[3];
    int m_step;                 // 0x104 the step of the window (3: it was answered)
    int unk108;
    int unk10C;
    int unk110;
    int unk114;
    char* unk118;
    int unk11C;
    u8 m_unk120;                // 0x120 set by the menus of the start of the game for every window they open
    u32 unk124;
    u32 unk128;
    virtual ~muNoticeWndTask();
    virtual void processDefault();
    void printText(u32 index, const char* text, int length, int kind);
    void setDoneButtonLabel(void* msgData, u32 index);
    static int mainStepAppearMain(muNoticeWndTask* task);
    static void mainStepLeaveInit(muNoticeWndTask* task);
    static int mainStepLeaveMain(muNoticeWndTask* task);
};

// muNoticeWndTask::create(kind, text file, text number, resource file, scene group, unk, default button, heap, ...)
// HYPOTHESIS: the kind is 0 for a window with one button, 1 for a window with a yes and a no button and 3 for a window
// without a button that has to be closed by the program.
extern "C" muNoticeWndTask* fn_800BEEB8(int kind, void* msgData, int msgId, nw4r::g3d::ResFile* resFile, void* scnRoot, int unk14,
                                       int defaultButton, int heap, int heap2, int unk0, int unk1, int unk2);
// muNoticeWndTask::create1(...): the same with a text that is given (the text and its size) instead of a number
extern "C" muNoticeWndTask* fn_800BF048(int kind, const char* text, int size, nw4r::g3d::ResFile* resFile, void* scnRoot, int unk14,
                                       int defaultButton, int heap, int heap2, int unk0, int unk1, int unk2);
// muNoticeWndTask::setStep(step), deleteWnd(), setYesButtonLabel(text file, text number) and setNoButtonLabel(...)
extern "C" void fn_800BF828(muNoticeWndTask* wnd, int step);
extern "C" void fn_800BF830(muNoticeWndTask* wnd);
extern "C" void fn_800BF97C(muNoticeWndTask* wnd, void* msgData, int msgId);
extern "C" void fn_800BFA04(muNoticeWndTask* wnd, void* msgData, int msgId);
