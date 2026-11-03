// Local BrawlHeaders shadow: adds NtMatching::watchResetNodeMissMatch.
#pragma once

#include <StaticAssert.h>
#include <types.h>

class DWCAllocType {
public:
    static void* ntAlloc(int, u32, u32);
    static void ntFree(int, void*);
};
// TODO size assertion

// HYPOTHESIS: the matching manager NtTr resets (only the method it calls is declared).
class NtMatching {
public:
    void watchResetNodeMissMatch();
};

