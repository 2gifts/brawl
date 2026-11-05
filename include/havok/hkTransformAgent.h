#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>

#include <havok/hkShape.h>
#include <havok/hkShapeContainer.h>
#include <havok/hkQuaternion.h>
#include <havok/hkTransform.h>

struct hkCdBody;
struct hkCollisionInput;

// Shape that places a child shape with a fixed transform. No code of the shape itself is in the image (only the
// agent), so the layout comes from the agent's accesses. HYPOTHESIS: the child container right after hkShape
// (child pointer at 0x10, as in hkConvexTranslateShape one word later), the rotation as a quaternion at 0x20 (the
// agent's processCollision rotates the swept rotations by it) and the full transform at 0x30.
struct hkTransformShape : hkShape {
    hkSingleShapeContainer m_childShape; // 0x0C
    hkQuaternion m_rotation;             // 0x20
    hkTransform m_transform;             // 0x30
};

// Transform agent: wraps the agent of the child shape (0x0C) and forwards every call to it with a child body whose
// motion is the body's transform composed with the shape's transform. Object size 0x10.
struct hkCollisionDispatcher;

struct hkTransformAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkCollisionAgent* m_childAgent; // 0x0C

    // Registers the transform agent pairs (shape type 0x19) with the dispatcher.
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Creates the agent of the child shape for a body whose motion is A's motion state with the composed transform.
    hkTransformAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, hkContactMgr* mgr);

    // Create functions registered with the dispatcher (A: transform shape first; B: the symmetric agent, shapes swapped).
    static hkTransformAgent* createTransformAAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                                   hkContactMgr* contactMgr);
    static hkTransformAgent* createTransformBAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                                   hkContactMgr* contactMgr);

    virtual ~hkTransformAgent();

    virtual void cleanup();
    // Overrides of the hkCollisionAgent slots with the base parameter lists.
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float t0, float t1, void* arg);
    virtual void removePoint(u16 key);
    virtual void commitPotential(u16 key);
    virtual void createZombie(u16 key);

    virtual void processCollision(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void linearCast(void* unk0, void* unk1, void* unk2, void* unk3, void* unk4);
    static void staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, void* target,
                                 void* startTarget);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                       void* target);
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                      void* target);
    virtual void updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2);
};

// HYPOTHESIS: the symmetric (shape-swapped) variant hkSymmetricAgent<hkTransformAgent> (map name
// hkSymmetricAgent_16hkTransformAgent_).
struct hkSymmetricAgent_16hkTransformAgent_ : hkTransformAgent {
    hkSymmetricAgent_16hkTransformAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                         hkContactMgr* mgr)
        : hkTransformAgent(bodyB, bodyA, input, mgr) {}
    virtual ~hkSymmetricAgent_16hkTransformAgent_();
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d);
};

// HYPOTHESIS: hkSymmetricAgentLinearCast<hkTransformAgent>. The wrappers swap the two shapes and
// replace the collector by a flipping collector on the stack (same scheme as hkSphereBoxAgent).
struct hkSymmetricAgentLinearCast_16hkTransformAgent_ : hkSymmetricAgent_16hkTransformAgent_ {
    hkSymmetricAgentLinearCast_16hkTransformAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                   const hkCollisionInput& input, hkContactMgr* mgr)
        : hkSymmetricAgent_16hkTransformAgent_(bodyA, bodyB, input, mgr) {}
    virtual ~hkSymmetricAgentLinearCast_16hkTransformAgent_();

    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void updateShapeCollectionFilter(void* unk0, void* unk1, void* unk2);

    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticLinearCast(void* unk0, void* unk1, void* unk2, void* unk3, void* unk4);
    virtual void processCollision(void* a, void* b, void* c, void* d);
};
