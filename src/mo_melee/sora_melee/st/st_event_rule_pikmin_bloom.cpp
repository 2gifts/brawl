#include <st/st_event_rule.h>
#include <types.h>

stEventRulePikminBloom::stEventRulePikminBloom() : stEventRule(1) {
    m_bloomed = false;
}

stEventRulePikminBloom::~stEventRulePikminBloom() { }

void stEventRulePikminBloom::notifyEventPikminBloom() {
    m_bloomed = true;
}

bool stEventRulePikminBloom::chechEventRule() {
    if (m_bloomed) {
        unk8 = 7;
        unkC = 4;
        return true;
    }
    return false;
}
