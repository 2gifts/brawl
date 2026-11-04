#pragma once

#include <types.h>
#include <st/st_event_rule.h>

class stEventRuleGrowUp : public stEventRule {
public:
    stEventRuleGrowUp(int param);
    virtual ~stEventRuleGrowUp();

    // HYPOTHESIS: member types are guessed from the constructor stores.
    int m_param;
    u32 m_value;
    int m_count;
};
