#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>
#include <havok/hkVector4.h>

struct hkCollisionDispatcher;

struct hkCdBody;
struct hkCollisionInput;
struct hkContactMgr;
struct hkPenetrationTarget;

// Collision agent that streams a bounding volume tree against another shape (0x40 bytes).
// Layout from the constructor and the destructor: the hkCollisionAgent 0x08 word, one word copied
// from the third constructor argument, eight zeroed floats, a hkArray<unsigned int> at 0x30 whose
// storage is the single inline entry at 0x3C.
struct hkBvTreeStreamAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    int unkC;                      // 0x0C the collision input's dispatcher (copied by the constructor)
    hkReal unk10[8];               // 0x10..0x2F zeroed by the constructor (HYPOTHESIS: two vectors)
    hkInplaceArray<unsigned int, 1> m_list; // 0x30 HYPOTHESIS: list managed by hkAgent1nMachine (see cleanup)

    hkBvTreeStreamAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                        hkContactMgr* contactMgr);
    virtual ~hkBvTreeStreamAgent();

    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& c, void* d); // 0x1C not written yet
    virtual void cleanup();
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c); // 0x24
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float t0, float t1, void* arg);

    static hkBvTreeStreamAgent* createBvTreeShapeAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                       const hkCollisionInput& input, hkContactMgr* contactMgr);
    static hkBvTreeStreamAgent* createShapeBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                   const hkCollisionInput& input, hkContactMgr* contactMgr);
    static void registerAgent(void* dispatcher);
    static void registerConvexListAgent(void* dispatcher);
    static void registerMultiRayAgent(void* dispatcher);

    // Forwarders to the hkBvTreeAgent static query functions (tail calls in the original).
    void getPenetrations(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* d);
    void getClosestPoints(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* d);
    void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* d, void* e);
};

// HYPOTHESIS: agent variant created by createBvTreeShapeAgent. Its vtable (lbl_804868E8) is not
// identified yet; the class adds no members and only replaces the vtable after the base constructor.
struct hkBvTreeStreamAgentVariant : hkBvTreeStreamAgent {
    hkBvTreeStreamAgentVariant(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                               hkContactMgr* contactMgr)
        : hkBvTreeStreamAgent(bodyA, bodyB, input, contactMgr) {}
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};
