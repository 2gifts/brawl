#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkMemory.h>

struct hkBroadPhaseHandlePair;
struct hkCollidableCollidableFilter;

// Receives the pairs of one combination of broad-phase handle types (slots 0x10 and 0x14).
struct hkBroadPhaseListener : hkReferencedObject {
    virtual void addCollisionPair(hkBroadPhaseHandlePair& pair) = 0;    // 0x10
    virtual void removeCollisionPair(hkBroadPhaseHandlePair& pair) = 0; // 0x14
};

// Listener for the type combinations nobody handles.
struct hkNullBroadPhaseListener : hkBroadPhaseListener {
    HK_DECLARE_REF_ALLOCATOR(0x1f)

    virtual ~hkNullBroadPhaseListener();
    virtual void addCollisionPair(hkBroadPhaseHandlePair& pair);
    virtual void removeCollisionPair(hkBroadPhaseHandlePair& pair);
};

enum {
    HK_MAX_BROAD_PHASE_TYPES = 8,
};

// Turns the pairs the broad phase reports into agents (map names hkTypedBroadPhaseDispatcher::addPairs,
// removePairs, removeDuplicates; the world keeps one at 0x58).
// HYPOTHESIS: the parameter types beyond the call shape. The filter argument is the world's collision filter seen
// through its first interface (the callers pass filter + 8, null-checked).
struct hkTypedBroadPhaseDispatcher {
    // Allocated and freed as class 0x25 (0x104 bytes).
    static void operator delete(void* p) {
        hkMemory::getInstance().deallocateChunk(p, sizeof(hkTypedBroadPhaseDispatcher), 0x25);
    }

    hkTypedBroadPhaseDispatcher();
    ~hkTypedBroadPhaseDispatcher();
    void addPairs(hkBroadPhaseHandlePair* pairs, int numPairs, const hkCollidableCollidableFilter* filter);
    void removePairs(hkBroadPhaseHandlePair* pairs, int numPairs);
    // drops the pairs that are in both lists
    static void removeDuplicates(hkArray<hkBroadPhaseHandlePair>& newPairs, hkArray<hkBroadPhaseHandlePair>& delPairs);

    hkBroadPhaseListener* m_broadPhaseListeners[HK_MAX_BROAD_PHASE_TYPES][HK_MAX_BROAD_PHASE_TYPES]; // 0x000
    hkBroadPhaseListener* m_nullBroadPhaseListener;                                                  // 0x100
};
