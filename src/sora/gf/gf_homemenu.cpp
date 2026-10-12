#include <types.h>
#include <gf/gf_homemenu.h>

extern "C" void fn_80228138(s32 p1);

struct gfHomeMenuFields {
    char unk0[0x9C];
    s32 unk9C;
    char unkA0[0x10];
    union {
        struct {
            u32 unkB0Hi : 4;
            u32 unkB0Mode : 4;
            u32 unkB0Lo : 24;
        } unkB0;
        u8 unkB0Bytes[4];
    };
};

void gfHomeMenu::kill() {
    gfHomeMenuFields* self = reinterpret_cast<gfHomeMenuFields*>(this);

    fn_80228138(0);
    fn_80228138(1);
    fn_80228138(2);
    fn_80228138(3);
    self->unkB0.unkB0Mode = 2;
}
