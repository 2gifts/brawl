#include <st/stage.h>
#include <st/st_event_rule.h>
#include <types.h>

// TODO: notifyEventDead.

stEventRuleDeadReason::stEventRuleDeadReason(int reason) : stEventRule(1) {
    m_state = reason;
}

stEventRuleDeadReason::~stEventRuleDeadReason() { }

bool stEventRuleDeadReason::chechEventRule() {
    bool decide = false;
    if (m_state == 1 && g_Stage != NULL) {
        decide = true;
    }
    if (decide) {
        return g_Stage->isEventEnd(0, &unk8, &unkC);
    }
    return false;
}

#include <ft/ft_manager.h>
// TODO: clean up, give variables real names
namespace jj_25DEAC {

extern "C" void OSReport(const char* msg, ...);
extern "C" void setStockCount__7ftOwnerFi(void*, void*);
extern "C" u32 isFlag__17ftLogDataAccesserFUlUl(void*, u32, u32);
extern char lbl_27_data_544C8[];
extern char lbl_27_data_544DC[];

extern "C" int notifyEventDead__21stEventRuleDeadReasonFiUl(void* arg0, int arg1) {
    void* owner = g_ftManager->getOwner(arg1);
    switch (*(int*)((char*)arg0 + 0x10)) {
    case 0:
        if (isFlag__17ftLogDataAccesserFUlUl((char*)*(int*)owner + 0x9e8, 0x13, 0) == 1) {
            OSReport(lbl_27_data_544C8);
            setStockCount__7ftOwnerFi(owner, 0);
            return 0;
        }
        break;
    case 1:
        if (isFlag__17ftLogDataAccesserFUlUl((char*)*(int*)owner + 0x9e8, 0x12, 0) == 1) {
            OSReport(lbl_27_data_544DC);
            setStockCount__7ftOwnerFi(owner, 0);
            return 0;
        }
        break;
    }
    return 1;
}
}

