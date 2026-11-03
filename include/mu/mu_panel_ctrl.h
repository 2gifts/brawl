#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <mu/mu_msg.h>
#include <types.h>

// The balloon of the scene that shows the text of the help of the entry the cursor is on. It is a task of the scene, so the
// functions that do nothing here are the ones that the other balloons of the scene use.
// HYPOTHESIS: the class has more fields (the size is a guess from the constructor).
class muPanelCtrlTask : public gfTask {
public:
    int m_msgNo;            // 0x40 the text that is shown (or that is shown when the panel has come in)
    int m_timer;            // 0x44 the frames since the last change (cleared by the constructor)
    MuMsg* m_msg;           // 0x48

    muPanelCtrlTask();
    virtual void processAnim();
    virtual ~muPanelCtrlTask();
    // Shows the text msgNo of msgData (when print is set) and remembers it.
    virtual void setMsg(int msgNo, int print, void* msgData);
    virtual void clearMsg() { m_msg->printf(0, ""); }
    virtual void setMsgData(void* data) { m_msg->setMsgData(data); }
    virtual void* getMsgData() { return m_msg->getBinary(); }
    // The menu goes to the screen `screen`, whose help is the text msgNo.
    virtual void changeProcess1(int screen, int msgNo);
    // The panel `panel` comes in with the title titleFrame and the background bg (see muMainPanelCtrlTask).
    virtual void changeProcess(u32 panel, int msgNo, int bg, float titleFrame);
    // Puts every animation at its end at once.
    virtual void frameCancel();
    virtual void startupSpecialMelee();
};
static_assert(sizeof(muPanelCtrlTask) == 0x4C, "Class is wrong size!");
