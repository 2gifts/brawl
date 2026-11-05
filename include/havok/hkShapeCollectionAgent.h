#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>

struct hkCollisionDispatcher;

// One child of the collection with the agent that handles it (0x08 bytes).
struct hkShapeCollectionAgentKeyAgentPair {
    u32 m_key;                 // 0x00 shape key of the child
    hkCollisionAgent* m_agent; // 0x04
};

// Collision agent for a shape collection against another shape (0x38 bytes): one child agent per child shape of
// the collection (body A).
struct hkShapeCollectionAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)
    static void* operator new(unsigned long, void* where) { return where; } // placement new for the factories

    hkArray<hkShapeCollectionAgentKeyAgentPair> m_agents; // 0x0C
    u8 unk18[0x38 - 0x18];                                 // 0x18..0x37 members not identified yet

    // HYPOTHESIS: the first two parameters are the two shapes in some order; the factories pass them
    // in opposite orders.
    hkShapeCollectionAgent(void* a, void* b, void* c, int unk8Value);

    static hkShapeCollectionAgent* createListAAgent(void* a, void* b, void* c, int d);
    static hkShapeCollectionAgent* createListBAgent(void* a, void* b, void* c, int d);
    static void registerAgent(void* dispatcher);

    virtual void processCollision(void* unk0, void* unk1, void* unk2, void* unk3);
};


// HYPOTHESIS: agent variant created by createListBAgent. Its vtable (lbl_80486F38) is not identified
// yet; the class adds no members and only replaces the vtable after the base constructor.
struct hkShapeCollectionAgentVariant : hkShapeCollectionAgent {
    hkShapeCollectionAgentVariant(void* a, void* b, void* c, int unk8Value)
        : hkShapeCollectionAgent(a, b, c, unk8Value) {}
};
