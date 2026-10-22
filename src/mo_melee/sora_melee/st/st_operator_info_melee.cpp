#include <st/operator/st_operator_info_melee.h>
#include <types.h>

extern "C" {
extern u32 lbl_27_data_53050;
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
