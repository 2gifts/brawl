#include <types.h>
// Havok translation unit hkShapeCollection.o (main.dol 0x802D01B0-0x802D0828). Names from the DebugMode map.
// Functions in address order:
//   0x802D01B0   656  hkShapeCollection::castRay
//   0x802D0440   400  hkShapeCollection::getMaximumProjection
//   0x802D05D0   584  hkShapeCollection::castRayWithCollector
//   0x802D0818    16  hkShapeCollection::getContainer
// hkShapeCollection::getAabb sits between castRay and getMaximumProjection in the source (its timer name is in the
// unit's string pool) but every collection overrides it, so the linker dropped it.

#include <havok/hkShapeCollection.h>
#include <havok/hkShapeRayCastInput.h>
#include <havok/hkCollisionFilter.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCdBody.h>
#include <havok/hkAabb.h>

// Casts the ray against every child (through the input's ray filter when it has one); the key of the last child
// hit is pushed on the output's key path.
hkBool hkShapeCollection::castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const {
    HK_TIMER_BEGIN("rcShpCollect");
    ShapeBuffer buffer;
    u32 hitKey = 0xFFFFFFFF;
    results.unk38++;
    if (input.unk24 == 0) {
        for (u32 key = getFirstKey(); key != 0xFFFFFFFF; key = getNextKey(key)) {
            const hkShape* child = getChildShape(key, buffer);
            if (child->castRay(input, results)) {
                hitKey = key;
            }
        }
    } else {
        for (u32 key = getFirstKey(); key != 0xFFFFFFFF; key = getNextKey(key)) {
            if (((const hkRayShapeCollectionFilter*)input.unk24)->isCollisionEnabled(input, *this, key)) {
                    const hkShape* child = getChildShape(key, buffer);
                if (child->castRay(input, results)) {
                    hitKey = key;
                }
            }
        }
    }
    results.unk38--;
    if (hitKey != 0xFFFFFFFF) {
        results.unk18[results.unk38] = hitKey;
    }
    HK_TIMER_END();
    return hitKey != 0xFFFFFFFF;
}

// Union of the children's boxes. Not in the image (see above). HYPOTHESIS: the body.
void hkShapeCollection::getAabb(const hkTransform& localToWorld, hkReal tolerance, hkAabb& out) const {
    HK_TIMER_BEGIN("hkShapeCollection::getAabb");
    u32 key = getFirstKey();
    {
        ShapeBuffer buffer;
        getChildShape(key, buffer)->getAabb(localToWorld, tolerance, out);
    }
    for (key = getNextKey(key); key != 0xFFFFFFFF; key = getNextKey(key)) {
        ShapeBuffer buffer;
        hkAabb childAabb;
        getChildShape(key, buffer)->getAabb(localToWorld, tolerance, childAabb);
        out.m_min.setMin4(out.m_min, childAabb.m_min);
        out.m_max.setMax4(out.m_max, childAabb.m_max);
    }
    HK_TIMER_END();
}

// The largest projection of any child onto the direction.
hkReal hkShapeCollection::getMaximumProjection(const hkVector4& direction) const {
    HK_TIMER_BEGIN("hkShapeCollection::getMaximumProjection");
    hkReal result = -3.40282e38f;
    for (u32 key = getFirstKey(); key != 0xFFFFFFFF; key = getNextKey(key)) {
        ShapeBuffer buffer;
        const hkShape* child = getChildShape(key, buffer);
        hkReal p = child->getMaximumProjection(direction);
        result = (result > p) ? result : p;
    }
    HK_TIMER_END();
    return result;
}

// Casts the ray against every child (through the input's ray filter when it has one), each with a body for the
// child below the collection's body.
void hkShapeCollection::castRayWithCollector(const hkShapeRayCastInput& input, const hkCdBody& body,
                                             hkShapeRayCastCollector& collector) const {
    HK_TIMER_BEGIN("rcShpCollect");
    ShapeBuffer buffer;
    if (input.unk24 == 0) {
        for (u32 key = getFirstKey(); key != 0xFFFFFFFF; key = getNextKey(key)) {
            const hkShape* child = getChildShape(key, buffer);
            hkCdBody childBody(&body);
            childBody.m_shape = (hkShape*)child;
            childBody.m_shapeKey = key;
            child->castRayWithCollector(input, childBody, collector);
        }
    } else {
        for (u32 key = getFirstKey(); key != 0xFFFFFFFF; key = getNextKey(key)) {
            if (((const hkRayShapeCollectionFilter*)input.unk24)->isCollisionEnabled(input, *this, key)) {
                    const hkShape* child = getChildShape(key, buffer);
                hkCdBody childBody(&body);
                childBody.m_shape = (hkShape*)child;
                childBody.m_shapeKey = key;
                child->castRayWithCollector(input, childBody, collector);
            }
        }
    }
    HK_TIMER_END();
}

// Function body below is taken from https://github.com/jj-link/brawl (src/havok/fn_802D0818.cpp).
// Null-checked upcast to the second base at +0xC (unnamed in the map).
// TODO: cleanup (shape from jj-link/brawl fn_802D0818)
extern "C" char* fn_802D0818(char* p) {
    return p ? p + 0xC : p;
}
