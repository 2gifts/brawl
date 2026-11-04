#include <gm/gm_global.h>
#include <ip/ip_padrec.h>
#include <ip/ip_switch.h>
#include <st/operator/st_operator_controller.h>
#include <st/st_global_utility.h>
#include <types.h>


stOperatorController::stOperatorController(const char* name)
    : stOperator("stOperatorController") { }

stOperatorController::~stOperatorController() { }

void stOperatorController::processBegin() { }

void stOperatorController::stopController() {
    ipSwitch::getInstance()->stopPadRec();
    ipSwitch::getInstance()->stopPadPlay();
}

void stOperatorController::setMeleeProcessCounter(u16 count) {
    ipSwitch::getInstance()->m_keyRecorder->setMeleeInitialProcessCount(count);
}

u16 stOperatorController::getMeleeProcessCounter() {
    return ipSwitch::getInstance()->m_keyRecorder->getMeleeInitialProcessCount();
}

bool stOperatorController::isPlaying() {
    bool isPlaying = true;
    if (ipSwitch::getInstance()->m_keyRecorder->m_mode != ipKeyRecorder::Playing &&
        !ipSwitch::getInstance()->m_flags.unk7)
    {
        isPlaying = false;
    }
    return isPlaying;
}

void stOperatorController::startController() {
    stSetBottonConfig(g_GameGlobal->m_modeMelee);
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    bool isHomerunVersus = false;
    if (melee->m_meleeInitData.m_stageKind == 0x34 &&
        !melee->m_meleeInitData.m_isHomerunSingle &&
        !melee->m_meleeInitData.m_isHomerunCombi) {
        isHomerunVersus = true;
    }
    u8 chunk = 0;
    if (isHomerunVersus) {
        chunk = melee->m_meleeInitData.m_homerunTeamA +
            melee->m_meleeInitData.m_homerunTeamB * 2;
    }
    if (!ipSwitch::getInstance()->m_flags.unk7) {
        bool canRecord;
        switch (melee->m_meleeInitData.m_gameMode) {
        case Game_Mode_Training:
        case Game_Mode_Net_Friend_Practice:
        case Game_Mode_Net_Kumite_Practice:
        case Game_Mode_Net_Homerun_Practice:
        case 0x15:
        case 0x16:
        case Game_Mode_Net_Okiraku_Practice:
        case 0x1B:
        case Game_Mode_Net_Team_Practice:
        case 0x1E:
        case Game_Mode_Button:
        case Game_Mode_Result:
        case Game_Mode_Demo:
            canRecord = false;
            break;
        default:
            canRecord = true;
            break;
        }
        if (canRecord) {
            ipKeyRecorder* recorder = ipSwitch::getInstance()->m_keyRecorder;
            if (chunk == 0) {
                recorder->clear(melee->m_meleeInitData.m_gameMode == Game_Mode_Target ? 5 : 3);
                recorder->setBaseInfo(melee);
                if (melee->m_meleeInitData.m_stageKind == 0x35) {
                    recorder->setStageData(g_GameGlobal->m_stageEditData, 0x470);
                }
                if (melee->m_meleeInitData.m_gameMode == Game_Mode_Adventure) {
                    recorder->setAdvData(g_GameGlobal->m_advData);
                }
            }
        }
    }
}

void stOperatorController::setControllerSource() {
    gmGlobalModeMelee* melee = g_GameGlobal->m_modeMelee;
    bool isHomerunVersus = false;
    if (melee->m_meleeInitData.m_stageKind == 0x34 &&
        !melee->m_meleeInitData.m_isHomerunSingle &&
        !melee->m_meleeInitData.m_isHomerunCombi) {
        isHomerunVersus = true;
    }
    u8 chunk = 0;
    if (isHomerunVersus) {
        chunk = melee->m_meleeInitData.m_homerunTeamA +
            melee->m_meleeInitData.m_homerunTeamB * 2;
    }
    if (ipSwitch::getInstance()->m_flags.unk7) {
        ipSwitch::getInstance()->startPadPlay(chunk);
        ipSwitch* input = ipSwitch::getInstance();
        input->m_flags.unk7 = 0;
    } else {
        bool canRecord;
        switch (melee->m_meleeInitData.m_gameMode) {
        case Game_Mode_Training:
        case Game_Mode_Net_Friend_Practice:
        case Game_Mode_Net_Kumite_Practice:
        case Game_Mode_Net_Homerun_Practice:
        case 0x15:
        case 0x16:
        case Game_Mode_Net_Okiraku_Practice:
        case 0x1B:
        case Game_Mode_Net_Team_Practice:
        case 0x1E:
        case Game_Mode_Button:
        case Game_Mode_Result:
        case Game_Mode_Demo:
            canRecord = false;
            break;
        default:
            canRecord = true;
            break;
        }
        if (canRecord) {
            ipSwitch::getInstance();
            ipSwitch::getInstance()->startPadRec(chunk);
        }
    }
}
