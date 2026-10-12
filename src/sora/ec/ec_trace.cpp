#include <types.h>

struct ecMgr {
    void endEffect(u32 arg0);
};

extern ecMgr* g_ecMgr;
extern f32 lbl_805A1998;
extern "C" void fn_80065C98(struct ecTrace* p1);

struct ecTrace {
    // HYPOTHESIS: unk0 and unk4 are heap arrays that carry a 0x10-byte header before the data
    u8* unk0;
    u8* unk4;
    s16 unk8;
    u8 unkA[0x4];
    u8 unkE;
    u8 unkF;
    u8 unk10[0x1C];
    u32 unk2C;
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
    u8 unk71[0x7];
    f32 unk78;
    u32 unk7C;
    u32 unk80;

    ecTrace(u32 p1, u32 p2, u32 p3, u32 p4, u32 p5, u32 p6);
    ~ecTrace();
    void end(u32 p1, u32 p2);
};

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

ecTrace::~ecTrace() {
    if (unk0 != NULL) {
        // MATCH-ONLY: the original tests the pointer a second time before the header adjust
        if (unk0 != NULL) {
            delete[] (unk0 - 0x10);
        }
        unk0 = NULL;
    }
    if (unk4 != NULL) {
        // MATCH-ONLY: the original tests the pointer a second time before the header adjust
        if (unk4 != NULL) {
            delete[] (unk4 - 0x10);
        }
        unk4 = NULL;
    }
    g_ecMgr->endEffect(unk2C);
}

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
