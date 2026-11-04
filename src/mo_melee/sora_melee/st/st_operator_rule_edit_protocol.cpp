#include <cm/cm_controller_ai.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <snd/snd_system.h>
#include <st/operator/st_operator_rule_edit_protocol.h>
#include <st_stageedit/st_stageedit.h>
#include <types.h>

extern float lbl_27_data_4F778[6];

void stOperatorRuleEditProtocol::chengeCameraType(int type) {
    if (type == 1) {
        if (unkE0 == 0) {
            g_sndSystem->playSE(static_cast<SndID>(3), -1, 0, 0, -1);
            return;
        }
        g_sndSystem->playSE(static_cast<SndID>(0x1D94), -1, 0, 0, -1);
        unkE0 = 0;
    } else {
        if (unkE0 == 1) {
            g_sndSystem->playSE(static_cast<SndID>(3), -1, 0, 0, -1);
            return;
        }
        g_sndSystem->playSE(static_cast<SndID>(0x1D95), -1, 0, 0, -1);
        unkE0 = 1;
    }
    if (g_Stage) {
        if (unkE0 == 0) {
            static_cast<stEdit*>(g_Stage)->setCameraParamDistance(unkF0, unkF4);
            g_cmAIController->unk95_80 = 0;
        } else {
            static_cast<stEdit*>(g_Stage)->setCameraParamDistance(lbl_27_data_4F778[unkE8], 1000.0f);
            g_cmAIController->unk95_80 = 0;
        }
    }
}

bool stOperatorRuleEditProtocol::getPadProxy(int type) {
    gfPadStatus pad;
    for (int i = 0; i < 8; i++) {
        g_gfPadSystem->getSysPadStatus(i, &pad);
        if (pad.m_error == 0) {
            switch (type) {
            case 1:
                switch (pad.m_controllerType) {
                case gfPadType::GCC:
                case gfPadType::WII_CLASSIC:
                    if (pad.m_buttonsPressedThisFrame.bits & 0x20) return true;
                    break;
                case gfPadType::WIIMOTE:
                    if ((pad.m_buttonsCurrentFrame2.bits & 0x80000) &&
                        (pad.m_buttonsPressedThisFrame.bits & 8)) return true;
                    break;
                case gfPadType::NUNCHUK:
                    if (pad.m_buttonsPressedThisFrame.bits & 8) return true;
                    break;
                }
                break;
            case 2:
                switch (pad.m_controllerType) {
                case gfPadType::GCC:
                case gfPadType::WII_CLASSIC:
                    if (pad.m_buttonsPressedThisFrame.bits & 0x40) return true;
                    break;
                case gfPadType::WIIMOTE:
                    if ((pad.m_buttonsCurrentFrame2.bits & 0x80000) &&
                        (pad.m_buttonsPressedThisFrame.bits & 4)) return true;
                    break;
                case gfPadType::NUNCHUK:
                    if (pad.m_buttonsPressedThisFrame.bits & 4) return true;
                    break;
                }
                break;
            case 3:
                if (pad.m_buttonsPressedThisFrame.bits & 0x1000) {
                    g_GameGlobal->m_setRule->m_ctrlNamePlayerNo = i;
                    return true;
                }
                break;
            }
        }
    }
    return false;
}

stOperatorRuleEditProtocol::stOperatorRuleEditProtocol() {
    unkCC = 0;
    unkD0 = 0;
    unkD4 = 0;
    m_state = 0;
    unkD8 = 0;
    unkDC = 0;
    unkE0 = 0;
    unkE4 = 0;
    unkE8 = 0;
    unkEC = 0;
    unk1FC = 0;
    unkF8 = 0;
    unkFC = 0;
    unk100 = 0;
    unk104 = 0;
    unk108 = false;
    unkF0 = 100.0f;
    unkF4 = 1000.0f;
    memset(unk12C, 0, sizeof(unk12C));
}

stOperatorRuleEditProtocol::~stOperatorRuleEditProtocol() {
    if (unkF8) {
        gfHeapManager::free(unkF8);
        unkF8 = 0;
    }
    if (unkFC) {
        gfHeapManager::free(unkFC);
        unkFC = 0;
    }
    if (unk100) {
        gfHeapManager::free(unk100);
        unk100 = 0;
    }
    if (unk104) {
        gfHeapManager::free(unk104);
        unk104 = 0;
    }
}

bool stOperatorRuleEditProtocol::isEnd() {
    return m_state == 4;
}

void stOperatorRuleEditProtocol::startOperator() {
    m_state = 1;
}
