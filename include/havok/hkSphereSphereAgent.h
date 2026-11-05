#pragma once

#include <havok/hkIterativeLinearCastAgent.h>

struct hkCdBody;
struct hkCollisionDispatcher;
struct hkCollisionInput;

// Collision agent for two spheres. Object size 0x10. The 0x08 word (inherited from hkCollisionAgent) holds the
// contact manager passed to the create function; 0x0C is a 16-bit slot that cleanup releases when it is not 0xFFFF.
struct hkSphereSphereAgent : hkIterativeLinearCastAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16 unkC; // 0x0C, set to 0xFFFF by the constructor

    hkSphereSphereAgent(hkContactMgr* contactMgr) : hkIterativeLinearCastAgent((int)contactMgr) {
        unkC = 0xFFFF;
    }
    virtual ~hkSphereSphereAgent();

    // Registers the create/static-query functions for sphere x sphere with the dispatcher.
    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // HYPOTHESIS: the first three parameters are the two bodies and the collision input; only the
    // contact manager (last parameter) is stored.
    static hkSphereSphereAgent* createSphereSphereAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                                        hkContactMgr* contactMgr);

    virtual void cleanup();
    virtual void processCollision(const hkCdBody& unk0, const hkCdBody& unk1, hkProcessCollisionInput& unk2, void* unk3);
    virtual void getClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);
    virtual void getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);

    static void staticGetClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);
    static void staticGetPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);
};
