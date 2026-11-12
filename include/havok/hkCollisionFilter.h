#pragma once

#include <havok/hkBase.h>
#include <havok/hkCollidable.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkMemory.h>
#include <havok/hkRegistry.h>

struct hkShapeRayCastInput;
struct hkWorldRayCastInput;

// Type info of the filter classes, filled at static-init time through a constructor (the __sinit stores the four
// words one by one, after the getVtable call).
struct hkCollisionFilterTypeInfo : hkTypeInfo {
    hkCollisionFilterTypeInfo(const char* name, void (*finish)(void*), void (*cleanup)(void*), const void* vtable) {
        m_name = name;
        m_finish = finish;
        m_cleanup = cleanup;
        m_vtable = vtable;
    }
};

// The four filter interfaces of hkCollisionFilter (0x18 bytes: hkReferencedObject plus the interface vptrs at 0x08,
// 0x0C, 0x10 and 0x14; layout from hkCollisionFilterClass.cpp). Each interface vtable is 0x10 bytes. The filters'
// vtables put the destructor first in the first and last interface and second in the middle two (the thunk tables
// after hkGroupFilter, hkNullCollisionFilter and hkConstrainedSystemFilter), which is the Havok declaration order.
// hkShapeCollectionFilter (0x0C) is declared in hkCollisionInput.h.
struct hkCollidableCollidableFilter {
    virtual ~hkCollidableCollidableFilter() {}
    virtual hkBool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const = 0;
};

// HYPOTHESIS: the container type (calls go through the collection's slot 0x44, the first virtual after the embedded
// shape container vtable: getCollisionFilterInfo in the hkMeshShape map names).
struct hkRayShapeCollectionFilter {
    virtual hkBool isCollisionEnabled(const hkShapeRayCastInput& aInput, const hkShapeCollection& bContainer,
                                      u32 bKey) const = 0;
    virtual ~hkRayShapeCollectionFilter() {}
};

struct hkRayCollidableFilter {
    virtual ~hkRayCollidableFilter() {}
    virtual hkBool isCollisionEnabled(const hkWorldRayCastInput& a, const hkCollidable& collidableB) const = 0;
};

// The filter redeclares the four overloads, so they also get primary slots 0x50..0x5C (hkConstrainedSystemFilter
// calls its child filter through them).
struct hkCollisionFilter : hkReferencedObject,
                           hkCollidableCollidableFilter,
                           hkShapeCollectionFilter,
                           hkRayShapeCollectionFilter,
                           hkRayCollidableFilter {
    hkCollisionFilter() {}
    hkCollisionFilter(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkCollisionFilter() {}

    virtual hkBool isCollisionEnabled(const hkCollidable& a, const hkCollidable& b) const = 0;
    virtual hkBool isCollisionEnabled(const hkCollisionInput& input, const hkCdBody& a, const hkCdBody& b,
                                      const hkShapeCollection& bContainer, u32 bKey) const = 0;
    virtual hkBool isCollisionEnabled(const hkShapeRayCastInput& aInput, const hkShapeCollection& bContainer,
                                      u32 bKey) const = 0;
    virtual hkBool isCollisionEnabled(const hkWorldRayCastInput& a, const hkCollidable& collidableB) const = 0;
};
