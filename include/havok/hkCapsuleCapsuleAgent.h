#pragma once

#include <havok/hkIterativeLinearCastAgent.h>

struct hkCollisionDispatcher;

// Collision agent for capsule x capsule pairs. Object size 0x14: the 0x08 word (inherited) is the contact manager,
// and three 16-bit slots at 0x0C hold per-query state; the constructor sets all three to 0xFFFF.
struct hkCapsuleCapsuleAgent : hkIterativeLinearCastAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    u16 unkC[3]; // 0x0C, set to 0xFFFF by the constructor; cleanup releases the contact manager for slots != 0xFFFF

    hkCapsuleCapsuleAgent(hkContactMgr* contactMgr) : hkIterativeLinearCastAgent((int)contactMgr) {
        unkC[0] = 0xFFFF;
        unkC[1] = 0xFFFF;
        unkC[2] = 0xFFFF;
    }
    virtual ~hkCapsuleCapsuleAgent() {}

    static void registerAgent(hkCollisionDispatcher* dispatcher);
    static hkCapsuleCapsuleAgent* createCapsuleCapsuleAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr);

    static void staticGetClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);
    static void staticGetPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);

    virtual void cleanup();
    virtual void getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3);
};
