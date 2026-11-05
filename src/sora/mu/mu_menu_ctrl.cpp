#include <gf/gf_pad_system.h>
#include <mu/mu_menu_controller.h>
#include <types.h>
#include <gm/gm_global.h>
#include <mt/mt_vector.h>

extern const u32 lbl_80407BF8[4];

void muMenuController::initRepeatCounter(int buttons) {
    for (int direction = 0; direction < 4; direction++) {
        if (buttons & lbl_80407BF8[direction]) {
            unk2C[0][direction].unk0 = 1;
            unk2C[0][direction].unk4 = 0;
            unk2C[0][direction].unk5 = 0;
        }
    }
}

bool muMenuController::setAButtonPushedControllerData() {
    gfPadStatus status;
    for (int controller = 0; controller < 8; controller++) {
        g_gfPadSystem->getSysPadStatus(controller, &status);
        if (status.m_error == gfPadError::NONE &&
            (status.m_buttonsPressedThisFrame.bits & 0x1100)) {
            gmSetRule* rule = g_GameGlobal->m_setRule;
            rule->m_ctrlNamePlayerNo = controller;
            rule->unk20 = status.m_controllerType;
            return true;
        }
    }
    return false;
}

Vec2f muMenuController::exchangeDir8ToVec(int direction) {
    Vec2f vector;
    if (direction == 8) {
        vector.m_y = 0.0f;
        vector.m_x = 0.0f;
        return vector;
    } else {
        float angle = 0.7853981852531433f * direction;
        vector.m_x = sin(angle);
        vector.m_y = cos(angle);
    }
    return vector;
}

void muMenuController::setControllerFlag(u32 mask) {
    unk4 = mask;
    unk2C[0][0].unk0 = 0;
    unk2C[0][0].unk4 = 0;
    unk2C[0][0].unk5 = 0;
    unk2C[1][0].unk0 = 0;
    unk2C[1][0].unk4 = 0;
    unk2C[1][0].unk5 = 0;
    unk2C[2][0].unk0 = 0;
    unk2C[2][0].unk4 = 0;
    unk2C[2][0].unk5 = 0;
    unk2C[0][1].unk0 = 0;
    unk2C[0][1].unk4 = 0;
    unk2C[0][1].unk5 = 0;
    unk2C[1][1].unk0 = 0;
    unk2C[1][1].unk4 = 0;
    unk2C[1][1].unk5 = 0;
    unk2C[2][1].unk0 = 0;
    unk2C[2][1].unk4 = 0;
    unk2C[2][1].unk5 = 0;
    unk2C[0][2].unk0 = 0;
    unk2C[0][2].unk4 = 0;
    unk2C[0][2].unk5 = 0;
    unk2C[1][2].unk0 = 0;
    unk2C[1][2].unk4 = 0;
    unk2C[1][2].unk5 = 0;
    unk2C[2][2].unk0 = 0;
    unk2C[2][2].unk4 = 0;
    unk2C[2][2].unk5 = 0;
    unk2C[0][3].unk0 = 0;
    unk2C[0][3].unk4 = 0;
    unk2C[0][3].unk5 = 0;
    unk2C[1][3].unk0 = 0;
    unk2C[1][3].unk4 = 0;
    unk2C[1][3].unk5 = 0;
    unk2C[2][3].unk0 = 0;
    unk2C[2][3].unk4 = 0;
    unk2C[2][3].unk5 = 0;
    unk98.unk0 = 0;
    unk98.unk4 = 0;
    unk98.unk5 = 0;
}

void muMenuController::setControllerID(u32 controller) {
    if (controller != 0xFF) {
        if (controller == 0xF0) {
            unk4 = 0xFF;
        } else {
            unk4 = 1 << controller;
        }
    } else {
        unk4 = 0;
    }
    for (int direction = 0; direction < 4; direction++) {
        for (int group = 0; group < 3; group++) {
            unk2C[group][direction].unk0 = 0;
            unk2C[group][direction].unk4 = 0;
            unk2C[group][direction].unk5 = 0;
        }
    }
    unk98.unk0 = 0;
    unk98.unk4 = 0;
    unk98.unk5 = 0;
}

// muMenuController::getPadStatus (the map name); other files call it by this stand-in name.
extern "C" gfPadStatus fn_800B9F90(muMenuController* self, int filterDirections) {
    gfPadStatus pads[8];
    gfPadStatus merged;
    gfPadStatus* destination;
    int count;
    u8 bit;
    int controller;
    u16 padMask = g_gfPadSystem->m_gamePadMask;
    destination = pads;
    count = 0;
    bit = 1;
    for (controller = 0; controller < 8; controller++, bit <<= 1) {
        if ((self->unk4 & bit) && (padMask & bit)) {
            g_gfPadSystem->getSysPadStatus(controller, destination);
            if (filterDirections) {
                if (destination->m_controllerType == gfPadType::NUNCHUK ||
                    (destination->m_controllerType == gfPadType::WIIMOTE &&
                     (destination->m_buttonsCurrentFrame2.bits & GF_PAD_BUTTON_BIT_L))) {
                    destination->m_buttonsCurrentFrame2.bits &= ~0xF;
                }
            }
            destination++;
            count++;
        }
    }
    gfPadSystem::merge(pads, count, &merged);
    return merged;
}

int muMenuController::exchangeButton2Dir8(u32 buttons) {
    int direction;
    if (buttons & GF_PAD_BUTTON_BIT_DPAD_UP) {
        if (buttons & GF_PAD_BUTTON_BIT_DPAD_RIGHT) {
            direction = 1;
        } else if (buttons & GF_PAD_BUTTON_BIT_DPAD_LEFT) {
            direction = 7;
        } else {
            direction = 0;
        }
    } else if (buttons & GF_PAD_BUTTON_BIT_DPAD_DOWN) {
        if (buttons & GF_PAD_BUTTON_BIT_DPAD_RIGHT) {
            direction = 3;
        } else if (buttons & GF_PAD_BUTTON_BIT_DPAD_LEFT) {
            direction = 5;
        } else {
            direction = 4;
        }
    } else if (buttons & GF_PAD_BUTTON_BIT_DPAD_RIGHT) {
        direction = 2;
    } else if (buttons & GF_PAD_BUTTON_BIT_DPAD_LEFT) {
        direction = 6;
    } else {
        direction = 8;
    }
    return direction;
}

int muMenuController::getButtonPushedControllerID(u32 buttons) {
    gfPadStatus status;
    for (int controller = 0; controller < 8; controller++) {
        g_gfPadSystem->getSysPadStatus(controller, &status);
        if (status.m_error == gfPadError::NONE) {
            if ((status.m_buttonsPressedThisFrame.bits & buttons) ||
                ((buttons & GF_PAD_BUTTON_BIT_DPAD_LEFT) && status.m_stickX < -60) ||
                ((buttons & GF_PAD_BUTTON_BIT_DPAD_RIGHT) && status.m_stickX > 60) ||
                ((buttons & GF_PAD_BUTTON_BIT_DPAD_UP) && status.m_stickY < -60) ||
                ((buttons & GF_PAD_BUTTON_BIT_DPAD_DOWN) && status.m_stickY > 60)) {
                return controller;
            }
        }
    }
    return -1;
}
