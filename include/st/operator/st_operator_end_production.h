#pragma once

#include <st/operator/st_operator.h>

class stOperatorEndProduction : public stOperator {
public:
    u32 unk48;
    u32 unk4C;
    u8 unk50[0x30];
    float unk80;
    float unk84;
    float unk88;
    float unk8C;
    float unk90;
    float unk94;
    float unk98;
    float unk9C;
    float unkA0;
    bool unkA4;

    stOperatorEndProduction();
    virtual void processBegin();
    virtual ~stOperatorEndProduction();
};

static_assert(sizeof(stOperatorEndProduction) == 0xA8, "Class is wrong size!");
