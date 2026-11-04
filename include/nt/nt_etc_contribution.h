#pragma once

#include <gf/gf_thread.h>
#include <types.h>

class nteDWC;
class nteSBS;

class nteContribution : public gfThread {
public:
    // HYPOTHESIS: three unidentified callback interfaces occupy this region.
    u8 unk340[0xC];
    bool unk34C;
    s32 unk350;
    s32 unk354;
    void* unk358;
    u32 unk35C;
    Heaps::HeapType unk360;
    nteDWC* unk364;
    nteSBS* unk368;
    bool unk36C;
    s32 unk370;
    bool unk374;
    s32 unk378;
    bool unk37C;
    s32 unk380;
    bool unk384;
    s32 unk388;

    // HYPOTHESIS: parameter types follow upload's pointer and word tests.
    bool upload(s32 kind, void* data, u32 size);
    // HYPOTHESIS: callback arguments are word-sized notification and result codes.
    void notifySBSEndProc(s32 notification, s32 result);
    bool isAbleToContribute();
};
