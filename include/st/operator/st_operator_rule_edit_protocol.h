#pragma once

#include <st/operator/st_operator_rule.h>
#include <types.h>

class stOperatorRuleEditProtocol : public stOperatorRule {
public:
    stOperatorRuleEditProtocol();
    virtual void processBegin();
    virtual void renderXlu();
    virtual ~stOperatorRuleEditProtocol();
    virtual bool isEnd();
    virtual void startOperator();
    void chengeCameraType(int type);
    bool getPadProxy(int type);

    u32 unkCC;
    u32 unkD0;
    u32 unkD4;
    u32 unkD8;
    u32 unkDC;
    u32 unkE0;
    u32 unkE4;
    u32 unkE8;
    u32 unkEC;
    float unkF0;
    float unkF4;
    void* unkF8;
    void* unkFC;
    void* unk100;
    void* unk104;
    bool unk108;
    u8 unk109[0x23];
    u8 unk12C[0xD0];
    u32 unk1FC;
};
