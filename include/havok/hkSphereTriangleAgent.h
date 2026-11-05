#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkCdBody.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>

struct hkCollisionDispatcher;
struct hkCollisionInput;

// Collision agent for sphere x triangle (sphere first; the triangle-first pairs use the symmetric variant). Object size
// 0x20: the 0x08 word is the contact manager, 0x0C a 16-bit slot released by cleanup when it is not 0xFFFF, and 0x10 the
// triangle's closest-point cache (hkCollideTriangleUtil::setupClosestPointTriangleCache).
struct hkSphereTriangleAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16 unkC;     // 0x0C, set to 0xFFFF by the constructor
    u16 unkE;     // 0x0E padding
    u8 unk10[16]; // 0x10 HYPOTHESIS: sub-object, see the constructor

    // triangleBody: the body holding the triangle; the triangle closest-point cache at 0x10 is built from its vertices.
    hkSphereTriangleAgent(const hkCdBody& triangleBody, hkContactMgr* contactMgr);
    virtual ~hkSphereTriangleAgent() {} // inline: the symmetric destructor calls it without a bl

    // Registers the triangle/sphere agent functions with the dispatcher (two pairs, see the .cpp).
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Triangle first: the triangle is bodyA; builds the symmetric variant.
    static hkSphereTriangleAgent* createTriangleSphereAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                            const hkCollisionInput& input, hkContactMgr* contactMgr);
    // Sphere first: the triangle is bodyB; builds the plain agent.
    static hkSphereTriangleAgent* createSphereTriangleAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                            const hkCollisionInput& input, hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    // The first out-of-line virtual getPenetrations is the key function (vtable in this unit).
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3) __attribute__((never_inline));
    // Destructor of the symmetric triangle agent; defined in hkSphereTriangleAgent.cpp.
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
};

// Flipped (symmetric) variant; the class name follows the Havok TU map (see hkSphereBoxAgent.h).
struct hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_ : hkSphereTriangleAgent {
    hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_(const hkCdBody& triangleBody, hkContactMgr* contactMgr)
        : hkSphereTriangleAgent(triangleBody, contactMgr) {}

    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void updateShapeCollectionFilter() {}
    static void staticLinearCast(void* a, void* b, void* c, void* target, void* d);
    virtual ~hkSymmetricAgentLinearCast_21hkSphereTriangleAgent_();

    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
};
