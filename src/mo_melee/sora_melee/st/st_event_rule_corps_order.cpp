#include <st/st_event_rule.h>
#include <types.h>

stEventRuleCorpsOrder::stEventRuleCorpsOrder() : stEventRule(1) {
    m_nextIndex = 0;
    m_outOfOrder = false;
}

stEventRuleCorpsOrder::~stEventRuleCorpsOrder() { }

// The corps members have to fall in index order.
void stEventRuleCorpsOrder::notifyEventDeadCorpsMember(int index) {
    if (m_nextIndex == index) {
        m_nextIndex++;
    } else {
        m_outOfOrder = true;
    }
}

bool stEventRuleCorpsOrder::chechEventRule() {
    if (m_outOfOrder) {
        unk8 = 8;
        unkC = 3;
        return true;
    }
    return false;
}
