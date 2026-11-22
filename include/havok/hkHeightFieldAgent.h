#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkBvAgent.h>
#include <havok/hkArray.h>

struct hkCollisionDispatcher;
struct hkContactMgr;
struct hkPenetrationTarget;

// Collision agent for a height field shape against another shape. Only the query functions are named so far: the
// constructor and the layout of the object are not recovered yet.
struct hkHeightFieldAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    // 0x0C one contact point per collision sphere of the convex body (HYPOTHESIS: the name).
    hkArray<u16> m_contactPointId;

    hkHeightFieldAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input, hkContactMgr* contactMgr);

    static void registerAgent(hkCollisionDispatcher* dispatcher);
    static hkCollisionAgent* createHeightFieldBAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                     hkCollisionInput& input, hkContactMgr* contactMgr);
    static hkCollisionAgent* createHeightFieldAAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                     hkCollisionInput& input, hkContactMgr* contactMgr);

    // The non-static query slots forward to the static functions with the this pointer dropped (tail calls).
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);

    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};

// HYPOTHESIS: symmetric variant of hkHeightFieldAgent. The class name is the map name
// (hkSymmetricAgentLinearCast<hkHeightFieldAgent>). Each wrapper swaps the first two pair arguments.
struct hkSymmetricAgentLinearCast_18hkHeightFieldAgent : hkHeightFieldAgent {
    hkSymmetricAgentLinearCast_18hkHeightFieldAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                                    hkContactMgr* contactMgr)
        : hkHeightFieldAgent(bodyA, bodyB, input, contactMgr) {}
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};

// HYPOTHESIS: hkSymmetricAgent<hkHeightFieldAgent> (map name); only its linear cast is written.
struct hkSymmetricAgent_18hkHeightFieldAgent_ : hkSymmetricAgentLinearCast_18hkHeightFieldAgent {
    hkSymmetricAgent_18hkHeightFieldAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                           hkContactMgr* contactMgr)
        : hkSymmetricAgentLinearCast_18hkHeightFieldAgent(bodyA, bodyB, input, contactMgr) {}
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
};
