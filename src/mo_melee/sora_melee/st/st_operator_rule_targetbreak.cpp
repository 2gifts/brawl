#include <if/if_mngr.h>
#include <st/operator/st_operator_rule_targetbreak.h>
#include <st_tbreak/st_tbreak.h>
#include <types.h>

stOperatorRuleTargetBreak::stOperatorRuleTargetBreak() : stOperatorRuleMelee(0) { }

stOperatorRuleTargetBreak::~stOperatorRuleTargetBreak() { }

void stOperatorRuleTargetBreak::decisionExtraRule() {
    if (m_isComplete) {
        m_decisionKind = Decision_Complete;
        m_resultInfo->m_decisionKind = gmResultInfo::Decision_Complete;
    } else {
        m_decisionKind = Decision_Failure;
        m_resultInfo->m_decisionKind = gmResultInfo::Decision_Failure;
    }
}

bool stOperatorRuleTargetBreak::isSuddenDeath() {
    return false;
}

void stOperatorRuleTargetBreak::updateExtraInfo() {
    if (g_IfMngr) {
        int count = 0;
        if (g_Stage && g_Stage->m_stageKind == Stages::TargetBreak) {
            count = static_cast<stTargetBreak*>(g_Stage)->m_targetsLeft;
        }
        // HYPOTHESIS: dispCorps consumes character kind and costume byte pairs.
        struct {
            int unk0;
            u8 unk4;
        } unk[10];
        for (int i = 0; i < count; i++) {
            unk[i].unk0 = 0x2C;
            unk[i].unk4 = m_level;
        }
        g_IfMngr->dispCorps(unk, count);
    }
}

bool stOperatorRuleTargetBreak::notifyEventDead(int entryId, u32 playerIndex, int arg) {
    ++unk284;
    stOperatorRuleMelee::notifyEventDead(entryId, playerIndex, arg);
    return false;
}

// TODO: clean up, give variables real names
namespace jj_252658 {
struct Data {
    unsigned char pad0[0x8];
    unsigned char field8;
    unsigned char pad9[0x90];
    unsigned char field99;
    unsigned char pad9A[0x5B];
    unsigned char fieldF5;
    unsigned char padF6[0x5B];
    unsigned char field151;
    unsigned char pad152[0x5B];
    unsigned char field1AD;
};
struct Obj {
    unsigned char pad0[0x44];
    Data* data;
    unsigned char pad48[0x89];
    unsigned char d1, d2, d3;
    unsigned char padD4[0x19C];
    int counter;
    int arr[4];
    int field284;
    unsigned char field288;
};
extern "C" void fn_27_2498DC(Obj*);
inline void add(Obj* self, int value) {
    if (*((unsigned char*)self->data + 0x99 + value * 0x5c) != 3) {
        self->arr[self->counter] = value;
        self->counter = self->counter + 1;
    }
}
extern "C" void fn_27_252658(Obj* self) {
    fn_27_2498DC(self);
    self->counter = 0;
    self->arr[0] = 0;
    self->arr[1] = 0;
    self->arr[2] = 0;
    self->arr[3] = 0;
    for (int i = 0; i < 4; i++) add(self, i);
    self->field284 = 0;
    self->field288 = 0;
    self->d1 = 0;
    if (((self->data->field8 >> 2) & 0x3F) == 8u) self->d3 = 1;
}
}

// TODO: clean up, give variables real names
namespace jj_2528DC {
extern "C" char* g_Stage;

extern "C" {
int fn_27_2528DC(char* self) {
    int v = *(int*)(self + 0x270);
    if (v != 0 && v <= *(int*)(self + 0x284)) {
        return 1;
    }
    char* p = g_Stage;
    if (p != 0 && *(int*)(p + 0x44) == 0x38) {
        int r = (*(int*)(p + 0x358) == 0);
        *(u8*)(self + 0x288) = r;
        return r;
    }
    return 0;
}
}
}

// TODO: clean up, give variables real names
namespace jj_2529AC {
extern "C" void fn_27_24ADF4(void*);
extern char* g_Stage;
extern char* g_GameGlobal;

extern "C" void fn_27_2529AC(char* self) {
    fn_27_24ADF4(self);
    *(int*)(self + 0x25c) = 0;
    *(int*)(self + 0x260) = 0;
    *(int*)(self + 0x264) = 0;
    *(int*)(self + 0x268) = 0;
    *(int*)(self + 0x26c) = 0;
    char* obj = g_Stage;
    if (obj && *(int*)(obj + 0x44) == 0x38) {
        unsigned int mode = *(unsigned int*)(obj + 0x354);
        int time = *(int*)(self + 0xe0);
        *(unsigned int*)(self + 0x268) = mode;
        *(int*)(self + 0x26c) = *(int*)(obj + 0x350);
        *(int*)(self + 0x264) = time;
        *(int*)(self + 0x25c) = (int)*(float*)(obj + 0x390);
        if (mode == 10) {
            *(int*)(self + 0x260) = time / 60;
        }
    }
    if ((((unsigned int)*(unsigned char*)(*(char**)(self + 0x44) + 8) >> 2) & 0x3f) == 3) {
        char* data = *(char**)(g_GameGlobal + 0x18);
        int* cursor = (int*)self;
        int i = 0;
        while (i < *(int*)(self + 0x270)) {
            int index = cursor[0x274 / 4];
            ++cursor;
            ++i;
            char* dest = data + *(unsigned char*)(*(char**)(self + 0x44) + index * 0x5c + 0x9b) * 0x2ac;
            *(int*)(dest + 0x264) = *(int*)(self + 0x260);
            *(int*)(dest + 0x268) = *(int*)(self + 0x25c);
            *(int*)(dest + 0x26c) = *(int*)(self + 0x268);
            *(int*)(dest + 0x270) = *(int*)(self + 0x264);
        }
    }
}
}

