#pragma once

#include <st/operator/st_operator_rule.h>
#include <types.h>

class ifVsResultTask;

class stOperatorRuleVsResult : public stOperatorRule {
public:
    ifVsResultTask* unkCC;

    stOperatorRuleVsResult();
    virtual ~stOperatorRuleVsResult();
    virtual bool isEnd();
    virtual void startOperator();
};
