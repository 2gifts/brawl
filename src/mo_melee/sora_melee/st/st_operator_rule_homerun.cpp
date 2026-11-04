#include <st/operator/st_operator_rule_homerun.h>
#include <types.h>
#include <string.h>

stOperatorRuleTimeHomerun::stOperatorRuleTimeHomerun() {
    unkD0 = 0;
    unkD4 = 0;
    unkD8 = 0;
    unkDC = 0;
    memset(unkDE, 0xFFFF, sizeof(unkDE));
    memset(unkEE, 0xFFFF, sizeof(unkEE));
    memset(unkFE, 0xFFFF, sizeof(unkFE));
    memset(unk10E, 0xFFFF, sizeof(unk10E));
    unk120 = 0x12E;
    unk124 = 0;
    unk128 = 0.0f;
}

bool stOperatorRuleTimeHomerun::isSuddenDeath() {
    return false;
}
