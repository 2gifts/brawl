#include <types.h>

extern "C" void OSReport(const char* msg, ...);
extern "C" void fn_800260B4(void* block);
extern const char lbl_804224D8[];

struct HeapBlock {
    s32 unk0;
    s32 unk4;
    s32 unk8;
};

struct HeapInfo {
    s32 unk0;
    HeapBlock* unk4;
    s32 unk8;
    s32 unkC;
};

extern HeapInfo g_HeapInfos[];

class gfHeapManager {
public:
    static void dumpAll();
};

void gfHeapManager::dumpAll() {
    const char* strs = lbl_804224D8; // MATCH-ONLY: keeps the string table base in a saved register
    HeapInfo* info;
    HeapBlock* block;
    s32 total = 0;
    s32 i;

    OSReport(strs + 0x1EC);
    OSReport(strs + 0x228);
    OSReport(strs + 0x1EC);

    info = g_HeapInfos;
    for (i = 0; i < 0x47; i++) {
        block = info->unk4;
        if (block != NULL) {
            fn_800260B4(block);
            total += block->unk8 - block->unk4;
        }
        info++;
    }

    OSReport(strs + 0x268, total);
    OSReport(strs + 0x1EC);
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0000[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0008[0x8] __attribute__((aligned(4))) = { 0 };
