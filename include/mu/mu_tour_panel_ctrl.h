#pragma once

#include <StaticAssert.h>
#include <mu/mu_object.h>
#include <mu/mu_panel_ctrl.h>
#include <types.h>

// The balloon of the screens of the tournament: it shows the text of the help in the model of the option.
// HYPOTHESIS: the names of the fields come from how the balloon of the rules uses them (it has the same base).
class muTourPanelCtrlTask : public muPanelCtrlTask {
public:
    MuObject* m_option;         // 0x4C the model that the text is put on

    muTourPanelCtrlTask(MuObject* option, void* msgData);
    virtual void processAnim();
    virtual ~muTourPanelCtrlTask();
    virtual void setMsg(int msgNo, int print, void* msgData);
    virtual void changeProcess1(int screen, int msgNo);
    virtual void changeProcess(u32 panel, int msgNo, int bg, float titleFrame);
    virtual void frameCancel();
};
static_assert(sizeof(muTourPanelCtrlTask) == 0x50, "Class is wrong size!");
