#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkArray.h>
#include <havok/hkBvAgent.h>
#include <havok/hkAabb.h>

struct hkCollisionDispatcher;
struct hkCdBody;
struct hkPenetrationTarget;
struct hkCollisionInput;
struct hkLinearCastCollisionInput;
struct hkProcessCollisionInput;
struct hkTransform;
struct hkVector4;

// Bounding-volume-tree collision agent (object size 0x40, memory class 0x1d). Derives from hkCollisionAgent: the
// 0x08 word is the contact manager passed to the constructor. The constructor zeroes the entry list at 0x0C
// (an hkArray of 12-byte entries, empty with the don't-deallocate flag set), the two vectors at 0x20 and 0x30 and
// the word at 0x14 (set to 0x80000000, the flag word of the empty list).
struct hkBvTreeAgent : hkCollisionAgent {
    HK_DECLARE_REF_ALLOCATOR(0x1d)

    // 12-byte entry of the list at 0x0C (map name hkBvTreeAgent::hkBvAgentEntryInfo): one child of the tree's
    // collection with its agent (the queries rebuild the child body from the key and call the agent).
    struct hkBvAgentEntryInfo {
        u32 m_key;                          // 0x00 child shape key in the tree's collection
        u32 unk4;                           // 0x04 HYPOTHESIS
        hkCollisionAgent* m_collisionAgent; // 0x08
    };

    hkArray<hkBvAgentEntryInfo> m_entries; // 0x0C
    int unk18;                             // 0x18 not written by the constructor
    int unk1C;                             // 0x1C not written by the constructor
    // 0x20 aabb of A (in B's space) that the last tree query covered, grown by the expected motion
    // (calcAabbAndQueryTree); zeroed by the constructor, max first. HYPOTHESIS: the name.
    hkAabb m_cachedAabb;

    hkBvTreeAgent(hkContactMgr* contactMgr) __attribute__((never_inline));
    virtual ~hkBvTreeAgent() {}

    static void registerAgent(hkCollisionDispatcher* dispatcher);
    // HYPOTHESIS: parameter types (the queries pass their pair and input through unchanged).
    void prepareCollisionPartners(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input);
    void prepareCollisionPartnersLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                            const hkLinearCastCollisionInput& input);
    static hkBvTreeAgent* createBvTreeShapeAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                         hkContactMgr* contactMgr);
    static hkBvTreeAgent* createShapeBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                         hkContactMgr* contactMgr);
    static hkBvTreeAgent* createBvBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                         hkContactMgr* contactMgr);

    virtual void calcStatistics(hkStatisticsCollector* collector) const;
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d);
    virtual void cleanup();
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);
    virtual void invalidateTim(void* arg);
    virtual void warpTime(float t0, float t1, void* arg);

    static void staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                      void* target);
    static void staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                       void* target);
    static void staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB, hkLinearCastCollisionInput& input,
                                 void* target, void* startTarget);

    // Queries the tree with A's aabb (in B's space, grown by the step's time of impact info) into hitList; returns
    // non-zero when the cached aabb (cachedAabb, the agent's 0x20 when the global switch is set) still covers it and
    // the tree need not be queried again. HYPOTHESIS: parameter and return types.
    static int calcAabbAndQueryTree(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkTransform& bTa,
                                    const hkVector4& timInfo, hkProcessCollisionInput& input, hkAabb* cachedAabb,
                                    hkArray<u32>& hitList);
};

// hkSymmetricAgent<hkBvTreeAgent> (map name): the base of the linear-cast variant below, as for hkMultiSphereAgent.
// Evidence: the deleting dtor at 0x802A8D68 inlines three destructor levels (variant, this class, hkBvTreeAgent) before
// the entry list; the map labels it hkSymmetricAgent's, but it is the variant's (one level more than this class's).
// Only its linear cast is written.
struct hkSymmetricAgent_13hkBvTreeAgent_ : hkBvTreeAgent {
    virtual void linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);

    hkSymmetricAgent_13hkBvTreeAgent_(hkContactMgr* contactMgr) : hkBvTreeAgent(contactMgr) {}
};

// HYPOTHESIS: the symmetric variant is the object created by createBvTreeShapeAgent (its vtable is set after the base
// constructor). Declared here; the members are defined in hkBvTreeAgent.cpp.

// HYPOTHESIS: symmetric variant of hkBvTreeAgent; the class name is the map name (hkSymmetricAgentLinearCast<hkBvTreeAgent>): each wrapper swaps the
// first two pair arguments and forwards to the hkBvTreeAgent function.
struct hkSymmetricAgentLinearCast_13hkBvTreeAgent : hkSymmetricAgent_13hkBvTreeAgent_ {
    virtual void getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    static void staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target);
    virtual void updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c);
    static void staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d);
    virtual void processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d);

    hkSymmetricAgentLinearCast_13hkBvTreeAgent(hkContactMgr* contactMgr) : hkSymmetricAgent_13hkBvTreeAgent_(contactMgr) {}
};

