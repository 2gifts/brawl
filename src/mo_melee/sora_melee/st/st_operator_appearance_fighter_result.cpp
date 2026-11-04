#include <st/operator/st_operator_appearance_fighter_result.h>
#include <types.h>
#include <st/operator/st_operator_rule_vsresult.h>

stOperatorAppearanceFighterResult::stOperatorAppearanceFighterResult() {
    unk1B5 = false;
    m_state = 0;
    unk1B8 = NULL;
}

stOperatorAppearanceFighterResult::~stOperatorAppearanceFighterResult() { }

bool stOperatorAppearanceFighterResult::isEnd() {
    return unk1B5;
}

void stOperatorAppearanceFighterResult::startAppearance() {
    stOperatorRuleVsResult* rule = static_cast<stOperatorRuleVsResult*>(unk1B8);
    m_state = 6;
    if (rule != NULL) {
        rule->startOperator();
    }
}
