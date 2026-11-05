#pragma once

#include <havok/hkBase.h>

struct hkCdBody;
struct hkShapeCollection;
struct hkCollisionDispatcher;
struct hkCollisionInput;

// Filter asked once per child of a shape collection whether that child collides with the other body.
// HYPOTHESIS: the name. The slot (first virtual, 0x08) and the arguments come from the shape collection agent's
// calls; hkGroupFilter::isCollisionEnabled2 reads them in the same order (input, other body, collection body,
// collection, child key).
struct hkShapeCollectionFilter {
    virtual hkBool isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b,
                                      const hkShapeCollection& collection, u32 key) const = 0;
};

// Collision input handed to every agent call (the third argument of the agent slots).
struct hkCollisionInput {
    hkCollisionDispatcher* m_dispatcher; // 0x00 (hkBvAgent's static functions read it too)
    hkShapeCollectionFilter* m_filter;   // 0x04
    u32 unk8;                            // 0x08
    // 0x0C HYPOTHESIS: picks the dispatcher's second lookup table when the agents create their children.
    hkBool m_createPredictiveAgents;
};
