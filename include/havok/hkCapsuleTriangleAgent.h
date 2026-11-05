#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkCdBody.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>

struct hkCollisionDispatcher;
struct hkCollisionInput;

// Collision agent for triangle x capsule pairs. Object size 0x28: the 0x08 word (inherited) is the contact manager,
// 0x0C three 16-bit slots (set to 0xFFFF by the constructor), and 0x14 a 0x14-byte sub-object that the constructor
// fills from the triangle's vertices (fn_80325334, not recovered yet).
struct hkCapsuleTriangleAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16 unkC[3];    // 0x0C, set to 0xFFFF by the constructor
    u8 unk12[2];    // 0x12 padding (the sub-object starts at 0x14)
    u8 unk14[0x14]; // 0x14 sub-object, built from the triangle's vertices

    // triangleBody: the body holding the triangle.
    hkCapsuleTriangleAgent(const hkCdBody& triangleBody, hkContactMgr* contactMgr);
    virtual ~hkCapsuleTriangleAgent() {}

    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Triangle first: the triangle is bodyA. Returns the flipped (symmetric) variant.
    static hkCapsuleTriangleAgent* createTriangleCapsuleAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                              const hkCollisionInput& input, hkContactMgr* contactMgr);
    // Capsule first: the triangle is bodyB.
    static hkCapsuleTriangleAgent* createCapsuleTriangleAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                              const hkCollisionInput& input, hkContactMgr* contactMgr);

    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);

    virtual void cleanup();
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
};

// Flipped (symmetric) variant; the class name follows the Havok TU map.
struct hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_ : hkCapsuleTriangleAgent {
    hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_(const hkCdBody& triangleBody, hkContactMgr* contactMgr)
        : hkCapsuleTriangleAgent(triangleBody, contactMgr) {}

    virtual void getPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    virtual void getClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticLinearCast(void* a, void* b, void* c, void* target, void* d);
    virtual ~hkSymmetricAgentLinearCast_22hkCapsuleTriangleAgent_();

    static void staticGetPenetrations(void* unk0, void* unk1, void* unk2, void* unk3);
    static void staticGetClosestPoints(void* unk0, void* unk1, void* unk2, void* unk3);
};
