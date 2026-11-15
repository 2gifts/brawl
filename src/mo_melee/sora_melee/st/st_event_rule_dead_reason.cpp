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
