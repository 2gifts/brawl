#include <types.h>

struct ecMgr {
    void endEffect(u32 arg0);
};

extern ecMgr* g_ecMgr;
extern f32 lbl_805A1998;

struct ecTrace {
    // HYPOTHESIS: unk0 and unk4 are heap arrays that carry a 0x10-byte header before the data
    u8* unk0;
    u8* unk4;
    u8 unk8[7];
    u8 unkF;
    u8 unk10[0x1C];
    u32 unk2C;
    u8 unk30[0x48];
    f32 unk78;
    u32 unk7C;
    u32 unk80;

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
