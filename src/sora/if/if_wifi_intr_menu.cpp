#include <gf/gf_pad_system.h>
#include <if/if_wifi_intr_menu.h>
#include <mu/mu_msg.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// muMenuController::init (the map name); mu_menu_lifecycle.cpp calls it by this stand-in name.
extern "C" void fn_800B96E8(muMenuController* controller, int pad);

// HYPOTHESIS: the one pause model this menu builds. start and end select the object slots.
struct IfWifiIntrModel {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
};

IfWifiIntrModel lbl_805A26E8 = {"InfPause0020_TopN", 0, 0, 0xC0};

// HYPOTHESIS: visibility frame for each controller type (GameCube, classic, remote, nunchuk).
const u8 lbl_805A2700[8] = {0, 3, 2, 1, 0, 0, 0, 0};

IfWifiIntrMenu::IfWifiIntrMenu() {
    m_group = NULL;
    m_object = NULL;
    m_msg = NULL;
    m_mode = 0;
    m_cursor = 0;
    m_padNo = 0;
    m_active = 0;
    m_timer = 0;
}

IfWifiIntrMenu::~IfWifiIntrMenu() {
    m_msg->detachMuObject(m_object);
    for (u32 i = 0; i < 1; i++) {
        if (m_object) {
            delete m_object;
            m_object = NULL;
        }
    }
    if (m_msg) {
        delete m_msg;
        m_msg = NULL;
    }
}

void IfWifiIntrMenu::startMenu(int padNo, int kind) {
    MuObject* object = m_object;
    float frame = object->m_modelAnim->m_anmObjVisRes->GetFrame();
    object->changeAnimN(kind != 0, 2, false, false);
    object->setFrameVisible(frame);
    m_mode = (kind != 0) + 1;
    m_msg->printIndex(0, 0x1D, NULL);
    if (kind != 0) {
        m_msg->printIndex(1, 0x1E, NULL);
    } else {
        m_msg->printf(1, "");
    }
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(padNo, &status);
    m_object->setFrameVisible(lbl_805A2700[status.m_controllerType]);
    m_cursor = 0;
    m_object->changeAnimN(0, 0x10, true, false);
    m_group->Insert(m_group->sceneItemsCount, m_object->m_sceneModel);
    m_padNo = padNo;
    m_active = 1;
    m_timer = 0xF;
    fn_800B96E8(&m_controller, padNo & 0xFF);
}

void IfWifiIntrMenu::endMenu() {
    m_group->Remove(m_object->m_sceneModel);
    m_active = 0;
}
