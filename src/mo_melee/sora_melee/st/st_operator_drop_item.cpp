#include <mt/mt_prng.h>
#include <st/operator/st_operator_drop_item.h>
#include <types.h>

stOperatorDropItemMelee::~stOperatorDropItemMelee() { }

void stOperatorDropItemMelee::startOperator() {
    m_state = 1;
    float frames[3][2] = {{1200.0f, 1500.0f}, {500.0f, 800.0f}, {300.0f, 500.0f}};
    m_framesElapsedItemDrop = 0.0f;
    m_framesRequiredItemDrop =
        frames[m_frequency][0] + (frames[m_frequency][1] - frames[m_frequency][0]) * randf();
}

// TODO: clean up, give variables real names
namespace jj_247420 {
extern "C" {
void* __ct__10stOperatorFPCc(void* self, const char* name);
extern const float lbl_27_rodata_3FC0;
extern unsigned char lbl_27_data_4EC38[];
}

struct Sub {
    char pad[0x16];
    signed char val;
};

struct Obj {
    char pad0[0x3C];
    void* vtbl;
    unsigned char b40;
    char pad41[3];
    Sub* sub;
    unsigned char b48;
    char pad49[3];
    float f4C;
    float f50;
    float f54;
    float f58;
    float f5C;
    float f60;
    float f64;
    float f68;
    unsigned char b6C;
};

extern "C" Obj* __ct__23stOperatorDropItemMeleeFPCc(Obj* self, const char* name) {
    __ct__10stOperatorFPCc(self, name);
    self->b6C = 0;
    self->vtbl = lbl_27_data_4EC38;
    self->f60 = lbl_27_rodata_3FC0;
    self->f64 = lbl_27_rodata_3FC0;
    self->f68 = lbl_27_rodata_3FC0;
    self->b40 = 0;
    signed char v = self->sub->val;
    if (v != 0) {
        self->b6C = (v - 1) & 3;
    }
    self->b48 = 0;
    self->f54 = lbl_27_rodata_3FC0;
    self->f58 = lbl_27_rodata_3FC0;
    self->f5C = lbl_27_rodata_3FC0;
    self->f4C = lbl_27_rodata_3FC0;
    self->f50 = lbl_27_rodata_3FC0;
    return self;
}
}

