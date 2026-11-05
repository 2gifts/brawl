// Havok translation unit hkBvTreeAgent.o (main.dol 0x802A3A4C-0x802A8E1C).
// Functions in address order (method names from the Havok TU map):
//   0x802A3A4C    84  __ct   [map: hkBvTreeAgent____ct]
//   0x802A3B30   248  registerAgent   [map: hkBvTreeAgent__registerAgent]
//   0x802A3C28   120  createBvTreeShapeAgent   [map: hkBvTreeAgent__createBvTreeShapeAgent]
//   0x802A3CA0   152  __dt   [map: hkBvTreeAgent____dt]
//   0x802A3D38   108  createShapeBvAgent   [map: hkBvTreeAgent__createShapeBvAgent]
//   0x802A3DA4   208  createBvBvAgent   [map: hkBvTreeAgent__createBvBvAgent]
//   0x802A3E74   148  cleanup   [map: hkBvTreeAgent__cleanup]
//   0x802A3F08   120  invalidateTim   [map: hkBvTreeAgent__invalidateTim]
//   0x802A3F80   152  warpTime   [map: hkBvTreeAgent__warpTime]
//   0x802A4018  3068  prepareCollisionPartners   [map: hkBvTreeAgent__prepareCollisionPartners]
//   0x802A4C14    44  quickSort<Ui>   [map: hkAlgorithm__quickSort_Ui_]
//   0x802A4C40    60  quickSort<Ui,Q211hkAlgorithm8less<Ui>>   [map: hkAlgorithm__quickSort_Ui_Q211hkAlgorithm8less_Ui__]
//   0x802A4C7C   296  quickSortRecursive<Ui,Q211hkAlgorithm8less<Ui>>   [map: hkAlgorithm__quickSortRecursive_Ui_Q211hkAlgorithm8less_Ui__]
//   0x802A4E34  1212  fastUpdate   [map: hkAgentDispatchUtil_Ui_Q213hkBvTreeAgent18hkBvAgentEntryInfo_25hkAgentDispatchUtilHelper___fastUpdate]
//   0x802A52F0  1392  update   [map: hkAgentDispatchUtil_Ui_Q213hkBvTreeAgent18hkBvAgentEntryInfo_25hkAgentDispatchUtilHelper___update]
//   0x802A5920  3048  prepareCollisionPartnersLinearCast   [map: hkBvTreeAgent__prepareCollisionPartnersLinearCast]
//   0x802A6508   524  updateShapeCollectionFilter   [map: hkBvTreeAgent__updateShapeCollectionFilter]
//   0x802A6714   804  processCollision   [map: hkBvTreeAgent__processCollision]
//   0x802A6A38   348  calcTimInfo   [map: hkSweptTransformUtil__calcTimInfo]
//   0x802A6B94  2944  calcAabbAndQueryTree   [map: hkBvTreeAgent__calcAabbAndQueryTree]
//   0x802A7714   484  linearCast   [map: hkBvTreeAgent__linearCast]
//   0x802A78F8  1116  staticLinearCast   [map: hkBvTreeAgent__staticLinearCast]
//   0x802A7D54   476  getClosestPoints   [map: hkBvTreeAgent__getClosestPoints]
//   0x802A7F30   724  staticGetClosestPoints   [map: hkBvTreeAgent__staticGetClosestPoints]
//   0x802A8204   500  getPenetrations   [map: hkBvTreeAgent__getPenetrations]
//   0x802A83F8   748  staticGetPenetrations   [map: hkBvTreeAgent__staticGetPenetrations]
//   0x802A86E4   248  calcStatistics   [map: hkBvTreeAgent__calcStatistics]
//   0x802A87DC   352  linearCast   [map: hkSymmetricAgent_13hkBvTreeAgent___linearCast]
//   0x802A893C    72  getPenetrations   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___getPenetrations]
//   0x802A8984    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___staticGetPenetrations]
//   0x802A89CC    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___getClosestPoints]
//   0x802A8A14    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___staticGetClosestPoints]
//   0x802A8A5C   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___staticLinearCast]
//   0x802A8BBC   412  processCollision   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___processCollision]
//   0x802A8D58    16  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_13hkBvTreeAgent___updateShapeCollectionFilter]
//   0x802A8D68   160  __dt   [map: hkSymmetricAgent_13hkBvTreeAgent_____dt]
//   0x802A8E08    20  __sinit_\hkBvTreeAgent_cpp   [map: hkBvTreeAgentcpp____sinit_]

#include <havok/hkBvTreeAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkBvTreeShape.h>
#include <havok/hkAabb.h>
#include <havok/hkTransform.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCdBody.h>
#include <havok/hkMotionState.h>
#include <havok/hkCdBodyPairCollector.h>

hkBvTreeAgent::hkBvTreeAgent(hkContactMgr* contactMgr) : hkCollisionAgent((int)contactMgr) {
    unk30[0] = 0.0f;
    unk30[1] = 0.0f;
    unk30[2] = 0.0f;
    unk30[3] = 0.0f;
    unk20[0] = 0.0f;
    unk20[1] = 0.0f;
    unk20[2] = 0.0f;
    unk20[3] = 0.0f;
}

void hkBvTreeAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.create = (hkAgentFunc)createBvTreeShapeAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticLinearCast;
    funcs.symmetricA = 1;
    funcs.symmetricB = 1;
    dispatcher->registerCollisionAgent(&funcs, 3, -1);

    funcs.create = (hkAgentFunc)createShapeBvAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)staticLinearCast;
    funcs.symmetricA = 0;
    funcs.symmetricB = 1;
    dispatcher->registerCollisionAgent(&funcs, -1, 3);

    funcs.create = (hkAgentFunc)createBvBvAgent;
    dispatcher->registerCollisionAgent(&funcs, 3, 3);
}

hkBvTreeAgent* hkBvTreeAgent::createBvTreeShapeAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                     const hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_13hkBvTreeAgent(contactMgr);
}

hkBvTreeAgent* hkBvTreeAgent::createShapeBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                 const hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkBvTreeAgent(contactMgr);
}

// The tree with the smaller object radius is walked as the shape side.
hkBvTreeAgent* hkBvTreeAgent::createBvBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                              hkContactMgr* contactMgr) {
    hkReal radiusA = ((const hkMotionState*)bodyA.m_motion)->m_objectRadius;
    hkReal radiusB = ((const hkMotionState*)bodyB.m_motion)->m_objectRadius;
    if (radiusA < radiusB) {
        return new hkBvTreeAgent(contactMgr);
    }
    return new hkSymmetricAgentLinearCast_13hkBvTreeAgent(contactMgr);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkBvTreeAgent::getPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkBvTreeAgent::staticGetPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkBvTreeAgent::getClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkBvTreeAgent::staticGetClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c) {
    hkBvTreeAgent::updateShapeCollectionFilter(b, a, c);
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_13hkBvTreeAgent_::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkBvTreeAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkBvTreeAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkBvTreeAgent::staticLinearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkBvTreeAgent::staticLinearCast(b, a, flippedInput, &flipped, 0);
    }
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::processCollision(const hkCdBody& a, const hkCdBody& b, const hkProcessCollisionInput& c, void* d) {
    hkSymmetricAgent_processCollision<hkBvTreeAgent>(this, a, b, c, d);
}

// Static queries against a BV tree shape (B): A's bounding box, expressed in B's space, is queried against the tree and
// the dispatcher's function for A against each hit child of B's collection runs (after the collection filter).
// NONMATCHING (86-91%): register coloring only (the loop end and the collection swap r29/r30, the timer's scratch
// registers), plus the child shape stored before the key reload; permuter 180 s found nothing valid.
void hkBvTreeAgent::staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                           void* target) {
    HK_TIMER_BEGIN_LIST("BvTree", "QueryTree");
    const hkTransform& ta = *(const hkTransform*)bodyA.m_motion;
    const hkTransform& tb = *(const hkTransform*)bodyB.m_motion;
    hkTransform bTa;
    bTa.setMulInverseMul(tb, ta);
    hkAabb aabb;
    bodyA.m_shape->getAabb(bTa, input.unk8, aabb);
    hkInplaceArray<u32, 128> hitList;
    const hkBvTreeShape* bvTree = (const hkBvTreeShape*)bodyB.m_shape;
    bvTree->queryAabb(aabb, hitList);
    HK_TIMER_SPLIT_LIST("NarrowPhase");
    int typeA = bodyA.m_shape->getType();
    hkCdBody childBody(&bodyB);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    const u32* end = hitList.begin() + hitList.getSize();
    const hkShapeCollection* collection = bvTree->m_collection;
    for (const u32* key = hitList.begin(); key != end; key++) {
        if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *collection, *key)) {
            childBody.m_shape = (hkShape*)collection->m_container.getChildShape(*key, shapeBuffer);
            childBody.m_shapeKey = *key;
            int typeB = childBody.m_shape->getType();
            typedef void (*QueryFunc)(const hkCdBody&, const hkCdBody&, const hkCollisionInput&, void*);
            ((QueryFunc)input.m_dispatcher->getGetClosestPointsFunc(typeA, typeB))(bodyA, childBody, input, target);
        }
    }
    HK_TIMER_END_LIST();
}

void hkBvTreeAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* startTarget) {
    HK_TIMER_BEGIN_LIST("BvTree", "QueryTree");
    prepareCollisionPartnersLinearCast(a, b, c);
    HK_TIMER_SPLIT_LIST("NarrowPhase");
    hkBvAgentEntryInfo* end = m_entries.begin() + m_entries.getSize();
    hkBvAgentEntryInfo* entry = m_entries.begin();
    hkCdBody childBody(&b);
    const hkBvTreeShape* bvTree = (const hkBvTreeShape*)b.m_shape;
    const hkShapeCollection* collection = bvTree->m_collection;
    hkShapeContainer::ShapeBuffer shapeBuffer;
    for (; entry != end; entry++) {
        const hkShape* child = collection->m_container.getChildShape(entry->m_key, shapeBuffer);
        u32 key = entry->m_key;
        childBody.m_shape = (hkShape*)child;
        childBody.m_shapeKey = key;
        entry->m_collisionAgent->linearCast(a, childBody, c, target, startTarget);
    }
    HK_TIMER_END_LIST();
}

void hkBvTreeAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* target) {
    HK_TIMER_BEGIN_LIST("BvTree", "QueryTree");
    prepareCollisionPartners(a, b, c);
    HK_TIMER_SPLIT_LIST("NarrowPhase");
    hkBvAgentEntryInfo* end = m_entries.begin() + m_entries.getSize();
    hkBvAgentEntryInfo* entry = m_entries.begin();
    hkCdBody childBody(&b);
    const hkBvTreeShape* bvTree = (const hkBvTreeShape*)b.m_shape;
    const hkShapeCollection* collection = bvTree->m_collection;
    hkShapeContainer::ShapeBuffer shapeBuffer;
    for (; entry != end; entry++) {
        const hkShape* child = collection->m_container.getChildShape(entry->m_key, shapeBuffer);
        u32 key = entry->m_key;
        childBody.m_shape = (hkShape*)child;
        childBody.m_shapeKey = key;
        entry->m_collisionAgent->getClosestPoints(a, childBody, c, target);
    }
    HK_TIMER_END_LIST();
}

// Brings the child agent list up to date with the tree, then asks each child's agent.
void hkBvTreeAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, const hkCollisionInput& c, void* target) {
    HK_TIMER_BEGIN_LIST("BvTree", "QueryTree");
    prepareCollisionPartners(a, b, c);
    HK_TIMER_SPLIT_LIST("NarrowPhase");
    hkBvAgentEntryInfo* end = m_entries.begin() + m_entries.getSize();
    hkBvAgentEntryInfo* entry = m_entries.begin();
    hkCdBody childBody(&b);
    const hkBvTreeShape* bvTree = (const hkBvTreeShape*)b.m_shape;
    const hkShapeCollection* collection = bvTree->m_collection;
    hkShapeContainer::ShapeBuffer shapeBuffer;
    for (; entry != end; entry++) {
        const hkShape* child = collection->m_container.getChildShape(entry->m_key, shapeBuffer);
        u32 key = entry->m_key;
        childBody.m_shape = (hkShape*)child;
        childBody.m_shapeKey = key;
        entry->m_collisionAgent->getPenetrations(a, childBody, c, target);
        if (((hkCdBodyPairCollector*)target)->m_earlyOut) {
            break;
        }
    }
    HK_TIMER_END_LIST();
}

void hkBvTreeAgent::staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input,
                                          void* target) {
    HK_TIMER_BEGIN_LIST("BvTree", "QueryTree");
    const hkTransform& ta = *(const hkTransform*)bodyA.m_motion;
    const hkTransform& tb = *(const hkTransform*)bodyB.m_motion;
    hkTransform bTa;
    bTa.setMulInverseMul(tb, ta);
    hkAabb aabb;
    bodyA.m_shape->getAabb(bTa, input.unk8, aabb);
    hkInplaceArray<u32, 128> hitList;
    const hkBvTreeShape* bvTree = (const hkBvTreeShape*)bodyB.m_shape;
    bvTree->queryAabb(aabb, hitList);
    HK_TIMER_SPLIT_LIST("NarrowPhase");
    int typeA = bodyA.m_shape->getType();
    hkCdBody childBody(&bodyB);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    const u32* end = hitList.begin() + hitList.getSize();
    const hkShapeCollection* collection = bvTree->m_collection;
    for (const u32* key = hitList.begin(); key != end; key++) {
        if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *collection, *key)) {
            childBody.m_shape = (hkShape*)collection->m_container.getChildShape(*key, shapeBuffer);
            childBody.m_shapeKey = *key;
            int typeB = childBody.m_shape->getType();
            typedef void (*QueryFunc)(const hkCdBody&, const hkCdBody&, const hkCollisionInput&, void*);
            ((QueryFunc)input.m_dispatcher->getGetPenetrationsFunc(typeA, typeB))(bodyA, childBody, input, target);
            if (((hkCdBodyPairCollector*)target)->m_earlyOut) {
                break;
            }
        }
    }
    HK_TIMER_END_LIST();
}

// The linear cast queries the box swept along the cast path (the path turned into B's space).
void hkBvTreeAgent::staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB, hkLinearCastCollisionInput& input,
                                     void* target, void* startTarget) {
    HK_TIMER_BEGIN_LIST("BvTree", "QueryTree");
    const hkTransform& ta = *(const hkTransform*)bodyA.m_motion;
    const hkTransform& tb = *(const hkTransform*)bodyB.m_motion;
    hkTransform bTa;
    bTa.setMulInverseMul(tb, ta);
    hkAabb aabb;
    bodyA.m_shape->getAabb(bTa, input.unk8, aabb);
    {
        hkVector4 pathB;
        pathB.setRotatedInverseDir(((const hkTransform*)bodyB.m_motion)->m_rotation, input.m_path);
        hkVector4 zero;
        zero.setZero4();
        hkVector4 pathMin;
        pathMin.setMin4(zero, pathB);
        hkVector4 pathMax;
        pathMax.setMax4(zero, pathB);
        aabb.m_min.add4(pathMin);
        aabb.m_max.add4(pathMax);
    }
    hkInplaceArray<u32, 128> hitList;
    const hkBvTreeShape* bvTree = (const hkBvTreeShape*)bodyB.m_shape;
    bvTree->queryAabb(aabb, hitList);
    HK_TIMER_SPLIT_LIST("NarrowPhase");
    int typeA = bodyA.m_shape->getType();
    hkCdBody childBody(&bodyB);
    hkShapeContainer::ShapeBuffer shapeBuffer;
    const u32* end = hitList.begin() + hitList.getSize();
    const hkShapeCollection* collection = bvTree->m_collection;
    for (const u32* key = hitList.begin(); key != end; key++) {
        if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *collection, *key)) {
            childBody.m_shape = (hkShape*)collection->m_container.getChildShape(*key, shapeBuffer);
            childBody.m_shapeKey = *key;
            int typeB = childBody.m_shape->getType();
            typedef void (*CastFunc)(const hkCdBody&, const hkCdBody&, const hkLinearCastCollisionInput&, void*, void*);
            ((CastFunc)input.m_dispatcher->getLinearCastFunc(typeA, typeB))(bodyA, childBody, input, target, startTarget);
        }
    }
    HK_TIMER_END_LIST();
}
