#pragma once

#include <havok/hkMemory.h>

// Per-thread caching front end for hkMemory: size-class free lists plus a chained stack area.
struct hkThreadMemory {
    virtual void setStackArea(void* area, int size);        // 0x08
    virtual void releaseCachedMemory();                     // 0x0C
    virtual ~hkThreadMemory();                              // 0x10
    virtual void* onStackOverflow(int nbytes);              // 0x14
    virtual void onStackUnderflow(void* p);                 // 0x18 (p: the freed position, unused here)

    hkMemory* m_memory;       // 0x04
    int m_referenceCount;     // 0x08
    int unkC;                 // 0x0C
    char* m_stackTop;         // 0x10
    char* m_stackBlock;       // 0x14
    char* m_stackBase;        // 0x18
    char* m_stackEnd;         // 0x1C
    int m_stackSize;          // 0x20
    int m_maxFreeListSize;    // 0x24
    void* m_freeList[17];     // 0x28
    int m_numFree[17];        // 0x6C
    int m_rowToSize[17];      // 0xB0
    signed char m_sizeToRow[0x201]; // 0xF4
    int m_largeSizeToRow[8];  // 0x2F8

    static hkThreadMemory* s_instance;

    hkThreadMemory(hkMemory* memory, int maxFreeListSize);

    // Bytes the stack area can still hand out, less 16 for the rounding of a request.
    // HYPOTHESIS: the name and the reason for the 16 (hkSimulation::integrate compares its memory need with it).
    int getAvailableStackSize() const { return (int)(m_stackEnd - m_stackTop) - 16; }

    void addReference();
    void removeReference();
    // Stack allocation from the chained stack area (16 byte granularity).
    // BYTES: every function that inlines this has copy propagation off in the target (materialized hkBool tests,
    // reloads): the hkLocalArray users (hkAgentDispatchUtil::update, the hkWorldOperationUtil broadphase functions),
    // hk3AxisSweep::reQuerySingleObject, hkUnionFind::assignGroups. MWCC does that for a function holding a local with an
    // alignment attribute (TIPS, extsb/neg/or/srwi. form). HYPOTHESIS: which local carries it in the original is unknown
    // (size, p and next all give the same bytes); aligned(4) is the natural alignment, so the frame does not change.
    void* allocateStack(int nbytes) {
        int size __attribute__((aligned(4))) = (nbytes + 0x10) & ~0xF;
        char* p = m_stackTop;
        char* next = p + size;
        if (next <= m_stackEnd) {
            m_stackTop = next;
            return p;
        }
        return onStackOverflow(size);
    }
    void deallocateStack(void* p) {
        m_stackTop = (char*)p;
        if (p == m_stackBase) {
            onStackUnderflow(p);
        }
    }

    void onAllocate();
    void onDeallocate();
    void* allocateChunk(int nbytes, int cl);
    void deallocateChunk(void* p, int nbytes, int cl);
    static void replaceInstance(hkThreadMemory* m);
    static void init();
    static void quit();

    static void operator delete(void* p) {
        g_hkFree(p);
    }
};
