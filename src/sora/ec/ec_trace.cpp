#include <types.h>

struct ecTrace {
    u8 unk0[0xF];
    u8 unkF;
    u8 unk10[0x68];
    f32 unk78;
    u32 unk7C;
    u32 unk80;

    void end(u32 p1, u32 p2);
};

extern f32 lbl_805A1998;

void ecTrace::end(u32 p1, u32 p2) {
    f32 ratio;

    if (unkF == 0) {
        if (p2 == 0) {
            unkF = 1;
            if (p1 == 0) {
                ratio = lbl_805A1998;
            } else {
                ratio = lbl_805A1998 / (f32) p1;
            }
            unk78 = ratio;
            return;
        }
        unk7C = p2;
        unk80 = p1;
        return;
    }
    if (unkF == 1) {
        if (p1 == 0) {
            ratio = lbl_805A1998;
        } else {
            ratio = lbl_805A1998 / (f32) p1;
        }
        unk78 = ratio;
    }
}
