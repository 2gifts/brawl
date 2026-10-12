#include <types.h>

struct ecMgr {
    void endEffect(u32 arg0);
};

extern ecMgr* g_ecMgr;

struct ecTrace {
    // HYPOTHESIS: unk0 and unk4 are heap arrays that carry a 0x10-byte header before the data
    u8* unk0;
    u8* unk4;
    u8 unk8[0x24];
    u32 unk2C;

    ~ecTrace();
};

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
