#pragma once

#include <havok/hkIterativeLinearCastAgent.h>
#include <havok/hkBoxBoxManifold.h>
#include <havok/hkCdBody.h>

struct hkCollisionDispatcher;
struct hkCollisionInput;

// Collision agent for box x box pairs (and the box side of box x other pairs) with a contact manifold. Object size 0x60.
// The 0x08 word (inherited) holds the contact manager; the manifold sits at 0x10 (its point count is at 0x31).
struct hkBoxBoxAgent : hkIterativeLinearCastAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u8 unkC[0x10 - 0x0C]; // 0x0C not identified yet
    hkBoxBoxManifold m_manifold; // 0x10
    u8 unk34[0x60 - 0x34];       // 0x34 not identified yet

    hkBoxBoxAgent(hkContactMgr* contactMgr) : hkIterativeLinearCastAgent((int)contactMgr) {}
    virtual ~hkBoxBoxAgent();

    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // Creates a box agent when the pair is close to both boxes' sizes, otherwise a GSK agent (hkGskfAgent).
    // The size test compares the input's 0x08 value (HYPOTHESIS: the tolerance) with twice each box's m_halfExtents.w.
    static hkCollisionAgent* createBoxBoxAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                               hkContactMgr* contactMgr);
    static void staticGetClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);
    static void staticGetPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);

    virtual void cleanup();
    virtual void processCollision(const hkCdBody& unk0, const hkCdBody& unk1, hkProcessCollisionInput& unk2, void* unk3); // 0x1C, not written yet
    virtual void getClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);
    virtual void getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);
};
