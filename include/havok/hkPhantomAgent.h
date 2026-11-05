#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkCdBody.h>
#include <havok/hkShapeType.h>

struct hkCollisionInput;
struct hkContactMgr;

// HYPOTHESIS: phantom callback shape (shape type HK_SHAPE_PHANTOM_CALLBACK). Only the two slots
// used by hkPhantomAgent are known: slot 0x28 registers a pair of root bodies, slot 0x2C removes
// them. Slots 0x08..0x24 are not identified yet (the first two vtable words are a header).
struct hkPhantomCallbackShape {
    virtual void unkSlot08() = 0;
    virtual void unkSlot0C() = 0;
    virtual void unkSlot10() = 0;
    virtual void unkSlot14() = 0;
    virtual void unkSlot18() = 0;
    virtual void unkSlot1C() = 0;
    virtual void unkSlot20() = 0;
    virtual void unkSlot24() = 0;
    virtual void addPairRoots(const hkCdBody* rootOwner, const hkCdBody* rootOther, const hkCollisionInput& input) = 0; // 0x28
    virtual void removePairRoots(const hkCdBody* rootOwner, const hkCdBody* rootOther) = 0; // 0x2C
};

// HYPOTHESIS: object whose slot 0x0C takes three pointers (the penetration target of the agent
// forwarding functions). Slot 0x08 is not identified yet.
struct hkPenetrationTarget {
    virtual void unkSlot08() = 0;
    virtual void forwardPenetrations(const hkCdBody& a, const hkCdBody& b, const void* c) = 0; // 0x0C
};

// Phantom callback collision agent. Constructed from the two collision bodies of a pair. The
// constructor stores the root (topmost m_parent) body of each side and the shape type of each
// body's own shape; cleanup() notifies the phantom callback shape of a side when that side's
// shape is a phantom callback (HK_SHAPE_PHANTOM_CALLBACK).
struct hkCollisionDispatcher;

struct hkPhantomAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    const hkCdBody* m_rootA;                   // 0x0C root of body A (walk over m_parent)
    const hkCdBody* m_rootB;                   // 0x10 root of body B
    hkPhantomCallbackShape* m_phantomA;        // 0x14 set by createPhantomAgent for side A
    hkPhantomCallbackShape* m_phantomB;        // 0x18 set by createPhantomAgent for side B
    int m_shapeTypeA;                          // 0x1C shape type of body A
    int m_shapeTypeB;                          // 0x20 shape type of body B

    hkPhantomAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkContactMgr* contactMgr);
    virtual ~hkPhantomAgent();

    static hkPhantomAgent* createPhantomAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                              hkContactMgr* contactMgr);
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    static void staticGetClosestPoints();
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, hkPenetrationTarget* target);
    static void staticLinearCast();

    virtual void cleanup();
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& c, void* d);
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* target);
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};
