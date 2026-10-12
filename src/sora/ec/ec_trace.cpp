#include <types.h>

struct ecTrace {
    u8 unk0[0x8];
    s16 unk8;
    u8 unkA[0x4];
    u8 unkE;
    u8 unkF[0x21];
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    u32 unk44;
    s16 unk48;
    s16 unk4A;
    u8 unk4C[0x24];
    s8 unk70;

    ecTrace(u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6);
};

extern "C" void fn_80065C98(ecTrace* p1);

ecTrace::ecTrace(u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6) {
    unk30 = -1;
    unk34 = -1;
    unk38 = -1;
    unk3C = -1;
    unk40 = 0;
    unk8 = p1 + 1;
    unk70 = (s8) p5;
    fn_80065C98(this);
    unkE |= 1;
    if (p6 != 0) {
        unkE |= 2;
    }
    unk44 = p2;
    unk48 = (s16) p3;
    unk4A = (s16) p4;
}
