#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_tour_panel_ctrl.h>
#include <types.h>


muTourPanelCtrlTask::muTourPanelCtrlTask(MuObject* option, void* msgData) {
    m_option = option;
    unk2C_b1 = false;
    m_msg = MuMsg::create(1, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x200, 1);
    m_msg->setMsgData(msgData);
    m_msg->attachScnMdlSimple(0, m_option->m_sceneModel, 0, 1.0f / 16.0f);
    m_msg->setAlignMode(0, MuMsg::Align_Center);
    m_msg->setFontColor(0, 0x2F, 0x2E, 0x2E, 0xFF);
    m_msg->setEdgeEnable(0, false);
}

muTourPanelCtrlTask::~muTourPanelCtrlTask() {
    delete m_msg;
}

void muTourPanelCtrlTask::processAnim() {
}

// Shows the text of the help (when asked to), and remembers its number.
void muTourPanelCtrlTask::setMsg(int msgNo, int print, void* msgData) {
    if (print != 0) {
        m_msg->printIndex(0, msgNo);
    }
    m_msgNo = msgNo;
}

void muTourPanelCtrlTask::changeProcess(u32 panel, int msgNo, int bg, float titleFrame) {
    m_msgNo = msgNo;
    m_timer = 0;
}

void muTourPanelCtrlTask::changeProcess1(int screen, int msgNo) {
}

void muTourPanelCtrlTask::frameCancel() {
}
