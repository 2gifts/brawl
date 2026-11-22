#pragma once

#include <havok/hkCollisionFilter.h>
#include <havok/hkMemory.h>

struct hkConstraintInstance;

// HYPOTHESIS: fifth base of hkConstrainedSystemFilter, a listener for constraint add/remove events (virtual
// destructor plus the two callbacks). The callbacks take a non-const instance: the added callback keeps its loads
// after its call the way a non-const pointer does.
struct hkConstraintListenerInterface {
    virtual ~hkConstraintListenerInterface() {}
    virtual void constraintAddedCallback(hkConstraintInstance* constraint) = 0;
    virtual void constraintRemovedCallback(hkConstraintInstance* constraint) = 0;
};

// Collision filter that forwards to a wrapped filter and listens for constraint events (0x20 bytes).
// Layout from the symbol names (hkConstrainedSystemFilter__*). Six vptrs: the four hkCollisionFilter
// interfaces, the constraint listener, and the primary vtable at offset 0.
struct hkConstrainedSystemFilter : hkCollisionFilter, hkConstraintListenerInterface {
    HK_DECLARE_REF_ALLOCATOR(0x25)

    hkCollisionFilter* m_childFilter; // 0x1C; HYPOTHESIS: the wrapped filter, reference counted

    hkConstrainedSystemFilter(hkCollisionFilter* childFilter);
    hkConstrainedSystemFilter(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkConstrainedSystemFilter();

    static void finishLoadedObjecthkConstrainedSystemFilter(void* p);
    static void cleanupLoadedObjecthkConstrainedSystemFilter(void* p);
    static const void* getVtablehkConstrainedSystemFilter();

    virtual void constraintAddedCallback(hkConstraintInstance* constraint);
    virtual void constraintRemovedCallback(hkConstraintInstance* constraint);

    // Forward to the wrapped filter; true when there is no child.
    virtual hkBool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const;
    virtual hkBool isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b,
                                      const hkShapeCollection& bContainer, u32 bKey) const;
    virtual hkBool isCollisionEnabled(const hkShapeRayCastInput& aInput, const hkShapeCollection& bContainer,
                                      u32 bKey) const;
    virtual hkBool isCollisionEnabled(const hkWorldRayCastInput& a, const hkCollidable& collidableB) const;
};
