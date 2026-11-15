#include <st/st_event_rule.h>
#include <types.h>

// TODO: notifyEventYoshiEgg (adds the egg count its caller hands over in r4; the base slot declares no argument).

stEventRuleYoshiEgg::stEventRuleYoshiEgg() : stEventRule(1) {
    m_count = 0;
}

stEventRuleYoshiEgg::~stEventRuleYoshiEgg() { }

bool stEventRuleYoshiEgg::chechEventRule() {
    if (m_count >= 2) {
        unk8 = 7;
        unkC = 4;
        return true;
    }
    return false;
}
