#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>

struct hkCollisionDispatcher;
struct hkCdBody;
struct hkCollisionInput;

// One child entry of the multi-sphere agent: a 32-bit key and the child agent (8 bytes per entry).
struct hkMultiSphereAgentEntry {
    u32 unk0; // 0x00 HYPOTHESIS: key of the child pair
    hkCollisionAgent* agent; // 0x04 child agent (cleanup is called on it)
};

// Multi-sphere agent: a list of child agents. Object size 0x38.
struct hkMultiSphereAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    hkInplaceArray<hkMultiSphereAgentEntry, 4> m_entries; // 0x0C (inline storage for four entries at 0x18)

    // One child agent per sphere of A's multi-sphere shape (unk0, unk1, unk2: bodyA, bodyB, input).
    hkMultiSphereAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, hkContactMgr* contactMgr);
    static void* operator new(unsigned long, void* p) { return p; }
    virtual ~hkMultiSphereAgent() {}

    // Registers the two list agents (multi-sphere against all shape types, both orders).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    static hkMultiSphereAgent* createListAAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);
    static hkMultiSphereAgent* createListBAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    static void staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                      void* target);
    static void staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                       void* target);
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d);
    static void staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                 void* target, void* d);
    virtual void processCollision(void* a, void* b, void* c, void* d); // not written yet
};

// Flipped (symmetric) variant created by createListBAgent; the class name follows the Havok TU map.
struct hkSymmetricAgent_18hkMultiSphereAgent_ : hkMultiSphereAgent {
    hkSymmetricAgent_18hkMultiSphereAgent_(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                           hkContactMgr* contactMgr)
        : hkMultiSphereAgent(bodyA, bodyB, input, contactMgr) {}
    virtual void linearCast(void* a, void* b, void* c, void* target, void* d);
};

// HYPOTHESIS: hkSymmetricAgentLinearCast<hkMultiSphereAgent> (map name); only its static linear cast is written.
struct hkSymmetricAgentLinearCast_18hkMultiSphereAgent_ : hkSymmetricAgent_18hkMultiSphereAgent_ {
    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    static void staticGetPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, void* target);
    static void staticLinearCast(void* a, void* b, void* c, void* target, void* d);
    virtual void processCollision(void* a, void* b, void* c, void* d);
};

// HYPOTHESIS: hkSymmetricAgentLinearCast<hkMultiSphereTriangleAgent> (map name, in hkMultiSphereAgent.o); the
// triangle agent itself is not recovered. Its static linear cast goes to the iterative linear cast.
struct hkMultiSphereTriangleAgent : hkCollisionAgent {
    virtual void getPenetrations(void* a, void* b, void* c, void* target); // not written yet
    virtual void getClosestPoints(void* a, void* b, void* c, void* target); // not written yet
    static void staticGetPenetrations(void* a, void* b, void* c, void* target); // not written yet
    static void staticGetClosestPoints(void* a, void* b, void* c, void* target); // not written yet
};

struct hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_ : hkMultiSphereTriangleAgent {
    virtual void getPenetrations(void* a, void* b, void* c, void* target);
    static void staticGetPenetrations(void* a, void* b, void* c, void* target);
    virtual void getClosestPoints(void* a, void* b, void* c, void* target);
    static void staticGetClosestPoints(void* a, void* b, void* c, void* target);
    static void staticLinearCast(void* a, void* b, void* c, void* target, void* d);
};
