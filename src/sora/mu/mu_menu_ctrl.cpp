#include <gf/gf_pad_system.h>
#include <mu/mu_menu_controller.h>
#include <types.h>

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
