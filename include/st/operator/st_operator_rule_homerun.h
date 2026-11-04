#pragma once

#include <st/operator/st_operator_rule_time.h>
#include <types.h>

class stOperatorRuleTimeHomerun : public stOperatorRuleTime {
public:
    // HYPOTHESIS: word fields and halfword arrays inferred from constructor stores and fills.
    u32 unkD0;
    u32 unkD4;
    u32 unkD8;
    u8 unkDC;
    u8 unkDD;
    u16 unkDE[8];
    u16 unkEE[8];
    u16 unkFE[8];
    u16 unk10E[8];
    u32 unk120;
    u8 unk124;
    float unk128;

    stOperatorRuleTimeHomerun();
    virtual void processBegin();
    virtual ~stOperatorRuleTimeHomerun();
    virtual void startOperator();
    virtual bool isSuddenDeath();
};
