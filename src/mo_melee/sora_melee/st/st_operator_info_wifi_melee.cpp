#include <st/operator/st_operator_info.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <revolution/OS.h>
#include <types.h>

// The Wi-Fi melee info operator (sora_melee 0x2585D8..0x258FBC; processBegin, the destructor, startOperator and the
// network error check are not decompiled yet).
class stOperatorInfoWifiMelee : public stOperatorInfo {
public:
    bool m_isEnableOperation; // 0x11B
    bool m_isStopRequest;     // 0x11C
    bool m_isError;           // 0x11D
    bool m_isEnd;             // 0x11E HYPOTHESIS: getEndType answers 3 when set
    u8 m_endType;             // 0x11F
    u32 m_errorLevel;         // 0x120

    virtual void processBegin();
    virtual ~stOperatorInfoWifiMelee();
    virtual void startOperator();
    virtual void stopOperatorRequest();
    virtual bool isEnableOperation();
    virtual u32 getEndType();
    virtual u32 getErrorLevel();
    virtual bool isError();
};

// TODO: clean up - the report format of the unit, referenced by its label until the unit's data is split.
extern "C" const char lbl_27_data_5356C[];

// The end type: on a network error it follows the error level (1: 4, 2: 5, 3: 6; anything else is reported and gives 5).
u32 stOperatorInfoWifiMelee::getEndType() {
    if (m_isError) {
        muWifiInterfaceTask* task = g_muWifiInterfaceTask;
        switch (task->getNetworkErrorLevel()) {
        case 1:
            return 4;
        default:
            OSReport(lbl_27_data_5356C, task->getNetworkErrorLevel());
        case 2:
            return 5;
        case 3:
            return 6;
        }
    }
    if (m_isEnd) {
        return 3;
    }
    return m_endType;
}

bool stOperatorInfoWifiMelee::isError() {
    return m_isError;
}

u32 stOperatorInfoWifiMelee::getErrorLevel() {
    return m_errorLevel;
}

bool stOperatorInfoWifiMelee::isEnableOperation() {
    return m_isEnableOperation;
}

void stOperatorInfoWifiMelee::stopOperatorRequest() {
    m_isStopRequest = true;
}
