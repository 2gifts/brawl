#include <types.h>

#include <mu/mu_panel_ctrl.h>

muPanelCtrlTask::muPanelCtrlTask() : gfTask("muPanelCtrl", Category_Menu, 15, 8, true) {
    m_timer = 0;
}

muPanelCtrlTask::~muPanelCtrlTask() {
}

void muPanelCtrlTask::processAnim() {
}

void muPanelCtrlTask::setMsg(int msgNo, int print, void* msgData) {
}

void muPanelCtrlTask::changeProcess(u32 panel, int msgNo, int bg, float titleFrame) {
}

void muPanelCtrlTask::changeProcess1(int screen, int msgNo) {
}

void muPanelCtrlTask::frameCancel() {
}

void muPanelCtrlTask::startupSpecialMelee() {
}
