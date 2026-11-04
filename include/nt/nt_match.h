#pragma once

#include <StaticAssert.h>
#include <types.h>

// Local shadow of BrawlHeaders' nt/nt_match.h: adds NtMatching (main.dol 0x80147434 ..., names from the sora map).

class DWCAllocType {
public:
    static void* ntAlloc(int, u32, u32);
    static void ntFree(int, void*);
};
// TODO size assertion

class NtMatching {
public:
    // HYPOTHESIS: starts the save of the user data (the name of the Wi-Fi user) in a thread, when the network state
    // allows it.
    static void thSaveUserDataNow();
};
