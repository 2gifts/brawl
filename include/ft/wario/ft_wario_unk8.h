#pragma once

#include <StaticAssert.h>
#include <types.h>

// HYPOTHESIS: these two-word records come from a shared item/article header.
// The native Wario and bike Item initializers construct (0xff, 0) and (0xff, 1);
// their purpose is unknown. The existing out-of-line constructor stores both words.
struct ftWarioUnk8 {
    int unk0;
    int unk4;
    ftWarioUnk8(int unk0, int unk4);
};
static_assert(sizeof(ftWarioUnk8) == 8, "Native two-word record extent");
