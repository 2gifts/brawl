#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkGskManifold.h>
#include <havok/hkPredGskAgent3.h>

struct hkCollisionDispatcher;

// Predictive GSK agent for cylinders (map name hkPredGskCylinderAgent3): the predictive GSK agent with each cylinder
// replaced by a capsule while it runs.
struct hkPredGskCylinderAgent3 : hkCollisionAgent {
    hkGskManifold m_manifold; // 0x0C HYPOTHESIS: same manifold layout as hkPredGskAgent3

    static void registerAgent3(hkCollisionDispatcher* dispatcher);
    // Flags the bodies that are cylinders in the entry, then creates the predictive GSK agent.
    static void* create(const hkAgent3Input& input, hkAgentEntry* entry, void* agentData);
    static void process(); // not written yet. HYPOTHESIS: parameters
};
