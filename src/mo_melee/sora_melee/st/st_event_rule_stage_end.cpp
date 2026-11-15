#include <st/stage.h>
#include <st/st_event_rule.h>
#include <types.h>

stEventRuleStageEnd::stEventRuleStageEnd() : stEventRule(1) { }

stEventRuleStageEnd::~stEventRuleStageEnd() { }

// The stage decides the event.
bool stEventRuleStageEnd::chechEventRule() {
    if (g_Stage != NULL) {
        return g_Stage->isEventEnd(0, &unk8, &unkC);
    }
    return false;
}
