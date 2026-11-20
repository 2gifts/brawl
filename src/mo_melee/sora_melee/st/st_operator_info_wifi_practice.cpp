#include <st/operator/st_operator_info.h>
#define MU_WIFI_INTERFACE_TASK_INTERNAL
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/network.h>
#include <revolution/OS.h>
#include <types.h>

// The Wi-Fi practice info operator (sora_melee 0x258FBC..0x25A62C; processBegin, the constructor and the network error check
// are not decompiled yet).
class stOperatorInfoWifiPractice : public stOperatorInfo {
public:
    char _11b[0x31];
    bool m_isError;           // 0x14C
    bool m_isEnd;             // 0x14D HYPOTHESIS: getEndType answers 3 when set
    u8 m_endType;             // 0x14E
    bool m_isEnableOperation; // 0x14F
    bool m_isStopRequest;     // 0x150
    u32 m_errorLevel;         // 0x154
    char _158[0x10];
    s32 m_playerAreaNum;      // 0x168 HYPOTHESIS: the number of player areas walked by startOperator

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

// TODO: clean up - MATCH-ONLY: the original inlines the (empty) stOperatorInfo destructor into the derived one, which
// then calls ~stOperator directly; the header declares it out of line, so this unit redeclares it inline.
inline stOperatorInfo::~stOperatorInfo() { }

stOperatorInfoWifiPractice::~stOperatorInfoWifiPractice() { }

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

// TODO: clean up - placeholder fn_ name for the member at 0x25A3BC (sets up the Wi-Fi state of one player area).
extern "C" void fn_27_25A3BC(stOperatorInfoWifiPractice* self, int area, int);

// Tells the other machines this one is ready (AID info state 3), then sets up every player area that is not local.
void stOperatorInfoWifiPractice::startOperator() {
    muWifiInterfaceTask* task = g_muWifiInterfaceTask;
    u8 aid = Network::getMyAID();
    if (aid == 0xff) {
        aid = 4;
    }
    task->m_aidInfos[aid].unk55 = 3;
    task->sendAIDInfo(false, 0xfe);
    for (int i = 0; i < m_playerAreaNum; i++) {
        if (!task->isMyPlayerArea(i)) {
            fn_27_25A3BC(this, i, 1);
        }
    }
    m_isEnableOperation = true;
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
