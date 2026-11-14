#pragma once

#include <havok/hkBase.h>

struct hkCollisionDispatcher;
struct hkAgent3Input;
struct hkAgent3ProcessInput;
struct hkAgentEntry;
struct hkContactMgr;
struct hkVector4;
struct hkProcessCollisionOutput;

// Capsule x triangle agent of the agent3 (stream) machine (static functions, 0x20 bytes of agent data).
struct hkCapsuleTriangleAgent3 {
    static void registerAgent3(hkCollisionDispatcher* dispatcher);
    static void* create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData);
    static void sepNormal(const hkAgent3Input& input, void* agentData, hkVector4& separatingNormalOut);
    // Removes the pair's contact points from the contact manager (never inlined: destroy calls it out of line).
    static void* cleanup(hkAgentEntry* entry, void* agentData, hkContactMgr* contactMgr) __attribute__((never_inline));
    static void removePoint(hkAgentEntry* entry, void* agentData, u16 idToRemove);
    static void commitPotential(hkAgentEntry* entry, void* agentData, u16 newId);
    static void createZombie(hkAgentEntry* entry, void* agentData, u16 idToConvert);
    static void destroy(hkAgentEntry* entry, void* agentData, hkContactMgr* contactMgr);
    static void* process(const hkAgent3ProcessInput& input, hkAgentEntry* entry, void* agentData,
                         hkVector4* separatingNormal, hkProcessCollisionOutput& result);
};
