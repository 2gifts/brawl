#pragma once

#include <StaticAssert.h>
#include <gf/gf_thread.h>
#include <types.h>

class gfCollectionIO;
class nteND;

// HYPOTHESIS: the two callback interfaces at 0x340 and 0x344 remain unidentified.
class nteCheckData : public gfThread {
public:
    u8 unk340[8];
    bool unk348;
    s32 unk34C;
    Heaps::HeapType unk350;
    s32 unk354;
    void* unk358;
    bool unk35C;
    s32 unk360;
    s64 unk368;
    gfCollectionIO* unk370;
    bool unk374;
    nteND* unk378;
    bool unk37C;
    bool unk37D;
    bool unk37E;

    // HYPOTHESIS: argument types follow the callback caller's registers.
    void showError(s32 error, s32 kind);
    void notifyDoneNDProc(s32 proc, s32 result, u32 size);
    bool _reflectSetting(void* data, u32 size);
    bool _storeDelivData(void* data, u32 size);
};
static_assert(sizeof(nteCheckData) == 0x380, "Class is the wrong size!");
