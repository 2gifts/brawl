#include <st/operator/st_operator_info.h>
#include <mu/wifi/mu_wifi_interface.h>
#include <revolution/OS.h>
#include <types.h>

// The Wi-Fi practice info operator (sora_melee 0x258FBC..0x25A62C; processBegin, the destructor, startOperator and the
// network error check are not decompiled yet).
class stOperatorInfoWifiPractice : public stOperatorInfo {
public:
    char _11b[0x31];
    bool m_isError;           // 0x14C
    bool m_isEnd;             // 0x14D HYPOTHESIS: getEndType answers 3 when set
    u8 m_endType;             // 0x14E
    bool m_isEnableOperation; // 0x14F
    bool m_isStopRequest;     // 0x150
    u32 m_errorLevel;         // 0x154

    virtual void processBegin();
    virtual ~stOperatorInfoWifiPractice();
    virtual void startOperator();
    virtual void stopOperatorRequest();
    virtual bool isEnableOperation();
    virtual u32 getEndType();
    virtual u32 getErrorLevel();
    virtual bool isError();
};

// TODO: clean up - the report format of the unit, referenced by its label until the unit's data is split.
extern "C" const char lbl_27_data_53814[];

// The end type: on a network error it follows the error level (1: 4, 0 and 2: 5, 3: 6; anything else is reported and
// gives 5).
u32 stOperatorInfoWifiPractice::getEndType() {
    if (m_isError) {
        muWifiInterfaceTask* task = g_muWifiInterfaceTask;
        switch (task->getNetworkErrorLevel()) {
        case 1:
            return 4;
        default:
            OSReport(lbl_27_data_53814, task->getNetworkErrorLevel());
        case 0:
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

bool stOperatorInfoWifiPractice::isError() {
    return m_isError;
}

u32 stOperatorInfoWifiPractice::getErrorLevel() {
    return m_errorLevel;
}

bool stOperatorInfoWifiPractice::isEnableOperation() {
    return m_isEnableOperation;
}

void stOperatorInfoWifiPractice::stopOperatorRequest() {
    m_isStopRequest = true;
}
