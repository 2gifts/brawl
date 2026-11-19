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

// TODO: clean up, give variables real names
namespace jj_24E7D8 {
struct cmAIController {
    unsigned char pad[0x94];
    unsigned char flags;
};

extern cmAIController* g_cmAIController;

struct lbl_27_data_4F588_t {
    unsigned char pad[0xB0];
};

extern lbl_27_data_4F588_t lbl_27_data_4F588;

extern "C" {
void __dl__FPv(void*);
void* __dt__18stOperatorRuleTimeFv(void*, int);

struct Class24E7D8 {
    unsigned char pad[0x3C];
    void* field_3C;
};

void* fn_27_24E7D8(void* a, int b) {
    if (a != 0) {
        ((Class24E7D8*)a)->field_3C = &lbl_27_data_4F588;
        if (g_cmAIController) {
            g_cmAIController->flags |= 0x10;
        }
        __dt__18stOperatorRuleTimeFv(a, 0);
        if (b > 0) {
            __dl__FPv(a);
        }
    }
    return a;
}
}
}

