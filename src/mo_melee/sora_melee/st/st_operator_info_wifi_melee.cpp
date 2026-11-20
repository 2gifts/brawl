#include <st/operator/st_operator_info.h>
#define MU_WIFI_INTERFACE_TASK_INTERNAL
#include <mu/wifi/mu_wifi_interface.h>
#include <nt/network.h>
#include <ip/ip_switch.h>
#include <st/operator/st_operator_rule_melee.h>
#include <revolution/OS.h>
#include <types.h>

// The Wi-Fi melee info operator (sora_melee 0x2585D8..0x258FBC; processBegin and the constructor are not decompiled
// yet).
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

    bool checkNetworkError();
};

// TODO: clean up - the report formats of the unit, referenced by their labels until the unit's data is split; the melee
// rule operator instance and the Wi-Fi task method at 0x800CCEC4 by placeholder names.
extern "C" {
extern const char lbl_27_data_5356C[];
extern const char lbl_27_data_534F0[];
extern const char lbl_27_data_53530[];
extern stOperatorRuleMelee* lbl_27_bss_59A8;
void fn_800CCEC4(muWifiInterfaceTask* task, int);
}

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

// The network error check of a running match: an error level from the Wi-Fi task, or (team or free-for-all) fewer than two
// players or teams left, ends the match with error level 2.
bool stOperatorInfoWifiMelee::checkNetworkError() {
    if (m_errorLevel != 0) {
        return true;
    }
    muWifiInterfaceTask* task = g_muWifiInterfaceTask;
    m_errorLevel = task->getNetworkErrorLevel();
    if (m_errorLevel == 0 && m_isEnableOperation && m_124) {
        bool lost = false;
        int numPlayers = task->unkFD5;
        if (numPlayers <= 1) {
            OSReport(lbl_27_data_534F0, numPlayers);
            lost = true;
        }
        gmGlobalModeMelee* melee = m_condition;
        // TODO: clean up - the rule operator byte at 0xD5 (_147[2]) is read by hand; name it in st_operator_rule_melee.h.
        if (melee->m_meleeInitData.m_corpsKind == 0 && melee->m_meleeInitData.m_isTeams == true && lbl_27_bss_59A8 != NULL &&
            *reinterpret_cast<u8*>(&lbl_27_bss_59A8->_147[2])) {
            int numTeams;
            lbl_27_bss_59A8->getExistTeam(&numTeams);
            if (numTeams <= 1) {
                OSReport(lbl_27_data_53530, numTeams);
                lost = true;
            }
        }
        if (lost) {
            fn_800CCEC4(task, 0);
            ipSwitch::getInstance()->setSource(0);
            m_errorLevel = 2;
            task->setAppNetworkError(0xe, 2);
        }
    }
    return m_errorLevel != 0;
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
