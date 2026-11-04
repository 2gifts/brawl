#include <st/operator/st_operator_info_melee.h>
#include <ip/ip_switch.h>
#include <types.h>

// HYPOTHESIS: the static callback object registered with the key recorder.
struct stOperatorInfoMeleeCallBack : ipKeyRecorder::CallBack {
    virtual int process(u8 command, u16 value);
};

extern "C" {
extern u32 lbl_27_data_53050;
extern stOperatorInfoMeleeCallBack lbl_27_bss_59E0;
}

stOperatorInfoMelee::stOperatorInfoMelee() : stOperatorInfo("") {
    m_isEnableOperation = false;
    m_isStopRequest = false;
    m_11b_5 = false;
    m_11b_4 = false;
    m_11b_3 = false;
    m_11b_2 = false;
    m_11b_1 = true;
    m_11b_0 = false;
    m_11c_7 = false;
    m_138 = -1;
    m_13c = lbl_27_data_53050;
    m_140 = 0;
    m_144 = lbl_27_data_53050;
    m_148 = 0;
    m_14c = lbl_27_data_53050;
    m_150 = 0;
    if (ipSwitch::getInstance() && ipSwitch::getInstance()->m_keyRecorder) {
        ipSwitch::getInstance()->m_keyRecorder->addCallBack(&lbl_27_bss_59E0);
    }
}

void stOperatorInfoMelee::startOperator() {
    m_isEnableOperation = true;
    m_11c_7 = false;
    m_13c = lbl_27_data_53050;
    m_140 = 0;
}

bool stOperatorInfoMelee::isEnableOperation() {
    return m_isEnableOperation;
}

u32 stOperatorInfoMelee::getEndType() {
    if (m_11b_3) {
        return 0;
    }
    if (m_11b_5) {
        u32 type = 2;
        if (m_11b_4) {
            type = 3;
        }
        return type;
    }
    return 1;
}

u32 stOperatorInfoMelee::getErrorLevel() {
    return 0;
}

void stOperatorInfoMelee::stopOperatorRequest() {
    m_isStopRequest = true;
}
