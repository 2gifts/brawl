#include <types.h>
#include <st/st_event_rule_grow_up.h>

extern "C" u32 lbl_27_data_54290;

stEventRuleGrowUp::stEventRuleGrowUp(int param) : stEventRule(1), m_param(param), m_value(lbl_27_data_54290), m_count(0) {
    m_value = lbl_27_data_54290;
    m_count = 0;
    unk8 = 0;
}

stEventRuleGrowUp::~stEventRuleGrowUp() {
}
