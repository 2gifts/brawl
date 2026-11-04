#pragma once

#include <st/operator/st_operator_rule.h>
#include <types.h>

class stOperatorRuleAdventure : public stOperatorRule {
public:
    union {
        char _CC[0x2C];
        struct {
            float unkCC;
            float unkD0;
            bool unkD4;
            bool unkD5;
            bool unkD6;
            bool unkD7;
            bool unkD8;
            u8 unkD9[3];
            int unkDC;
            int unkE0;
            int unkE4;
            int unkE8;
            int unkEC;
            int unkF0;
            s8 unkF4;
            u8 unkF5[3];
        };
    };
    void* unkF8;

    stOperatorRuleAdventure();
    void initEndProduction(int);
    virtual void processBegin();
    virtual ~stOperatorRuleAdventure();
    virtual bool isEnd();
    virtual void startOperator();
    virtual void setOpAppearanceHelper(void* helper);
};
