#pragma once
#include <StaticAssert.h>
#include <types.h>

// HYPOTHESIS: copy view; field meanings and overlapping byte semantics unknown.
class wnPikminPikminOwnerInfo {
public:
    union { struct { u32 unk0, unk4; }; u32 words[2]; } unk0;
    float unk8;
    union { u32 unkc; struct { u8 unkc, unkd, unke, unkf; } bytes; struct { u32 condition : 5; u32 rest : 27; } bits; } unkc;
#ifdef FT_PIKMIN_OWNER_INFO_DTOR
    ~wnPikminPikminOwnerInfo() { } // the fighter unit emits the destructor out of line
#endif
    // MATCH-ONLY: the original copies the word and then repeats its second byte.
    void operator=(const wnPikminPikminOwnerInfo& other) {
        unk0 = other.unk0;
        unk8 = other.unk8;
        unkc.unkc = other.unkc.unkc;
        unkc.bytes.unkd = other.unkc.bytes.unkd;
    }
};
static_assert(sizeof(wnPikminPikminOwnerInfo) == 16, "Class is wrong size!");
