#include <st/operator/st_operator_info.h>
#define MU_WIFI_INTERFACE_TASK_INTERNAL
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/network.h>
#include <revolution/OS.h>
#include <types.h>

// The Wi-Fi melee info operator (sora_melee 0x2585D8..0x258FBC; processBegin, the constructor and the network error check
// are not decompiled yet).
class stOperatorInfoWifiMelee : public stOperatorInfo {
public:
    bool m_isEnableOperation; // 0x11B
    bool m_isStopRequest;     // 0x11C
    bool m_isError;           // 0x11D
    bool m_isEnd;             // 0x11E HYPOTHESIS: getEndType answers 3 when set
    u8 m_endType;             // 0x11F
    u32 m_errorLevel;         // 0x120
    bool m_124;               // 0x124
    bool m_125;               // 0x125 HYPOTHESIS: set for the modes where the machine announces it is ready

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

// TODO: clean up - MATCH-ONLY: the original inlines the (empty) stOperatorInfo destructor into the derived one, which
// then calls ~stOperator directly; the header declares it out of line, so this unit redeclares it inline.
inline stOperatorInfo::~stOperatorInfo() { }

stOperatorInfoWifiMelee::~stOperatorInfoWifiMelee() { }

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

// Tells the other machines this one is ready (AID info state 4) when m_125 is set.
void stOperatorInfoWifiMelee::startOperator() {
    if (m_125) {
        muWifiInterfaceTask* task = g_muWifiInterfaceTask;
        u8 aid = Network::getMyAID();
        if (aid == 0xff) {
            aid = 4;
        }
        task->m_aidInfos[aid].unk55 = 4;
        task->sendAIDInfo(false, 0xfe);
    }
    m_isEnableOperation = true;
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
