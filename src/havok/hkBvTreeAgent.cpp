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
#include <havok/hkAgentDispatchUtil.h>
#include <havok/hkAlgorithm.h>
#include <havok/hkSweptTransformUtil.h>

hkBvTreeAgent::hkBvTreeAgent(hkContactMgr* contactMgr) : hkCollisionAgent((int)contactMgr) {
    m_cachedAabb.m_max.x = 0.0f;
    m_cachedAabb.m_max.y = 0.0f;
    m_cachedAabb.m_max.z = 0.0f;
    m_cachedAabb.m_max.w = 0.0f;
    m_cachedAabb.m_min.x = 0.0f;
    m_cachedAabb.m_min.y = 0.0f;
    m_cachedAabb.m_min.z = 0.0f;
    m_cachedAabb.m_min.w = 0.0f;
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
                                                     hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSymmetricAgentLinearCast_13hkBvTreeAgent(contactMgr);
}

hkBvTreeAgent* hkBvTreeAgent::createShapeBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                 hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkBvTreeAgent(contactMgr);
}

// The tree with the smaller object radius is walked as the shape side.
hkBvTreeAgent* hkBvTreeAgent::createBvBvAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                              hkContactMgr* contactMgr) {
    hkReal radiusA = ((const hkMotionState*)bodyA.m_motion)->m_objectRadius;
    hkReal radiusB = ((const hkMotionState*)bodyB.m_motion)->m_objectRadius;
    if (radiusA < radiusB) {
        return new hkBvTreeAgent(contactMgr);
    }
    return new hkSymmetricAgentLinearCast_13hkBvTreeAgent(contactMgr);
}

// The entries without an agent are skipped (the child was filtered out).
void hkBvTreeAgent::cleanup() {
    hkBvAgentEntryInfo* entry = m_entries.begin();
    hkBvAgentEntryInfo* end = m_entries.begin() + m_entries.getSize();
    for (; entry != end; entry++) {
        if (entry->m_collisionAgent != 0) {
            entry->m_collisionAgent->cleanup();
        }
    }
    delete this;
}

void hkBvTreeAgent::invalidateTim(void* arg) {
    hkBvAgentEntryInfo* entry = m_entries.begin();
    hkBvAgentEntryInfo* end = m_entries.begin() + m_entries.getSize();
    for (; entry != end; entry++) {
        if (entry->m_collisionAgent != 0) {
            entry->m_collisionAgent->invalidateTim(arg);
        }
    }
}

void hkBvTreeAgent::warpTime(float t0, float t1, void* arg) {
    hkBvAgentEntryInfo* entry = m_entries.begin();
    hkBvAgentEntryInfo* end = m_entries.begin() + m_entries.getSize();
    for (; entry != end; entry++) {
        if (entry->m_collisionAgent != 0) {
            entry->m_collisionAgent->warpTime(t0, t1, arg);
        }
    }
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkBvTreeAgent::getPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkBvTreeAgent::staticGetPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkBvTreeAgent::getClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkBvTreeAgent::staticGetClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {
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

void hkSymmetricAgentLinearCast_13hkBvTreeAgent::processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d) {
    hkSymmetricAgent_processCollision<hkBvTreeAgent>(this, a, b, c, d);
}

// Static queries against a BV tree shape (B): A's bounding box, expressed in B's space, is queried against the tree and
// the dispatcher's function for A against each hit child of B's collection runs (after the collection filter).
// NONMATCHING (86-91%): register coloring only (the loop end and the collection swap r29/r30, the timer's scratch
// registers), plus the child shape stored before the key reload; permuter 180 s found nothing valid.
void hkBvTreeAgent::staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                           void* target) {
    hkShapeContainer::ShapeBuffer shapeBuffer;
    const u32* end;
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
    end = hitList.begin() + hitList.getSize();
    hkCdBody childBody(&bodyB);
    const hkShapeCollection* collection = bvTree->m_collection;
    for (const u32* key = hitList.begin(); key != end; key++) {
        if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *collection, *key)) {
            const hkShape* child = collection->m_container.getChildShape(*key, shapeBuffer);
            u32 childKey = *key;
            childBody.m_shape = (hkShape*)child;
            childBody.m_shapeKey = childKey;
            int typeB = child->getType();
            typedef void (*QueryFunc)(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
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

void hkBvTreeAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
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
void hkBvTreeAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
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


// HYPOTHESIS: global hkBool switches read here (sda21): use the cached aabb at 0x20, and update the entry list
// without sorting the hits first.
extern hkBool lbl_805A0F14;
extern hkBool lbl_805A0F10;

// Helper handed to the dispatch-util templates (map name hkAgentDispatchUtilHelper): the tree's collection and a child
// body of B pointed at the child for a key (the templates read the collection at 0x00, the body at 0x04 and the shape
// buffer at 0x20).
struct hkAgentDispatchUtilHelper {
    const hkShapeCollection* m_collection;        // 0x00
    hkCdBody m_bodyB;                             // 0x04
    hkShapeContainer::ShapeBuffer m_shapeBuffer;  // 0x20

    explicit hkAgentDispatchUtilHelper(const hkCdBody& body) : m_bodyB(&body) {}

    const hkCdBody& getBodyB(u32 key) {
        m_bodyB.m_shape = (hkShape*)m_collection->m_container.getChildShape(key, m_shapeBuffer);
        m_bodyB.m_shapeKey = key;
        return m_bodyB;
    }
};

typedef hkAgentDispatchUtil<u32, hkBvTreeAgent::hkBvAgentEntryInfo, hkAgentDispatchUtilHelper> hkBvTreeAgentDispatchUtil;

// Queries the tree with the aabb of A swept over the step and updates the entry list from the hits (one child agent
// per hit key). Inlined into processCollision (the hit list is destroyed on both exits).
static inline void hkBvTreeAgent_updateEntries(hkBvTreeAgent* agent, const hkCdBody& bodyA, const hkCdBody& bodyB,
                                               hkProcessCollisionInput& input) {
    hkInplaceArray<u32, 128> hitList;
    hkTransform bTa;
    bTa.setMulInverseMul(*(const hkTransform*)bodyB.m_motion, *(const hkTransform*)bodyA.m_motion);
    hkVector4 timInfo;
    hkSweptTransformUtil::calcTimInfo(*(const hkMotionState*)bodyA.m_motion, *(const hkMotionState*)bodyB.m_motion, input.unk18,
                                      timInfo);
    hkAabb* cachedAabb = lbl_805A0F14 ? &agent->m_cachedAabb : 0;
    if (hkBvTreeAgent::calcAabbAndQueryTree(bodyA, bodyB, bTa, timInfo, input, cachedAabb, hitList) != 0) {
        return;
    }
    const hkShapeCollection* collection = ((const hkBvTreeShape*)bodyB.m_shape)->m_collection;
    hkAgentDispatchUtilHelper helper(bodyB);
    helper.m_collection = collection;
    if (lbl_805A0F10) {
        hkBvTreeAgentDispatchUtil::fastUpdate(agent->m_entries, hitList, bodyA, bodyB, input, (hkContactMgr*)agent->unk8,
                                              helper);
    } else {
        hkAlgorithm::quickSort(hitList.begin(), hitList.getSize());
        hkBvTreeAgentDispatchUtil::update(agent->m_entries, hitList, bodyA, bodyB, input, (hkContactMgr*)agent->unk8,
                                          helper);
    }
}

// Updates the entries, then runs every entry's agent on its child of B.
void hkBvTreeAgent::processCollision(const hkCdBody& bodyA, const hkCdBody& bodyB, hkProcessCollisionInput& input,
                                     void* output) {
    HK_TIMER_BEGIN_LIST("BvTree", "QueryTree");
    hkBvTreeAgent_updateEntries(this, bodyA, bodyB, input);
    hkBvAgentEntryInfo* end = m_entries.begin() + m_entries.getSize();
    hkBvAgentEntryInfo* entry = m_entries.begin();
    hkCdBody childBody(&bodyB);
    HK_TIMER_SPLIT_LIST("NarrowPhase");
    const hkBvTreeShape* bvTree = (const hkBvTreeShape*)bodyB.m_shape;
    const hkShapeCollection* collection = bvTree->m_collection;
    hkShapeContainer::ShapeBuffer shapeBuffer;
    for (; entry != end; entry++) {
        const hkShape* child = collection->m_container.getChildShape(entry->m_key, shapeBuffer);
        u32 key = entry->m_key;
        childBody.m_shape = (hkShape*)child;
        childBody.m_shapeKey = key;
        entry->m_collisionAgent->processCollision(bodyA, childBody, input, output);
    }
    HK_TIMER_END_LIST();
}

// Out of line in this unit (an hkSweptTransformUtil inline; the map places it after processCollision). The two center
// of mass differences are written lane by lane: the target stores them x first (set() would evaluate w first).
void hkSweptTransformUtil::calcTimInfo(const hkMotionState& ms0, const hkMotionState& ms1, hkReal deltaTime,
                                       hkVector4& timInfo) {
    hkVector4 diff0;
    diff0.x = ms0.m_sweptTransform.m_centerOfMass0.x - ms0.m_sweptTransform.m_centerOfMass1.x;
    diff0.y = ms0.m_sweptTransform.m_centerOfMass0.y - ms0.m_sweptTransform.m_centerOfMass1.y;
    diff0.z = ms0.m_sweptTransform.m_centerOfMass0.z - ms0.m_sweptTransform.m_centerOfMass1.z;
    diff0.w = ms0.m_sweptTransform.m_centerOfMass0.w - ms0.m_sweptTransform.m_centerOfMass1.w;
    hkVector4 diff1;
    diff1.x = ms1.m_sweptTransform.m_centerOfMass1.x - ms1.m_sweptTransform.m_centerOfMass0.x;
    diff1.y = ms1.m_sweptTransform.m_centerOfMass1.y - ms1.m_sweptTransform.m_centerOfMass0.y;
    diff1.z = ms1.m_sweptTransform.m_centerOfMass1.z - ms1.m_sweptTransform.m_centerOfMass0.z;
    diff1.w = ms1.m_sweptTransform.m_centerOfMass1.w - ms1.m_sweptTransform.m_centerOfMass0.w;
    // HYPOTHESIS: the center of mass 1 w lane is the inverse step length (Havok's swept transform keeps it there).
    hkReal f0 = deltaTime * ms0.m_sweptTransform.m_centerOfMass1.w;
    hkReal f1 = deltaTime * ms1.m_sweptTransform.m_centerOfMass1.w;
    timInfo.setMul4(f0, diff0);
    timInfo.addMul4(f1, diff1);
    timInfo.w = f0 * ms0.m_deltaAngle.w * ms0.m_objectRadius + f1 * ms1.m_deltaAngle.w * ms1.m_objectRadius;
}

// A's aabb in B's space for this step. With the swept form (unk28->unk10) it is A's aabb grown by A's rotation over the
// step, clipped to the sphere around A's end center that bounds it, and then grown by A's linear motion relative to B
// (timInfo turned into B's space, plus the motion B's rotation gives A's center). With a cached aabb the query is
// skipped while the cached box still contains the new one; otherwise the cache is refreshed with a box grown by the
// tolerance and by up to twice the motion backwards (at most 0.4 of the box extent).
// HYPOTHESIS: the deltaAngle w lanes are angular step lengths (calcTimInfo multiplies them by the object radius too).
int hkBvTreeAgent::calcAabbAndQueryTree(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkTransform& bTa,
                                        const hkVector4& timInfo, hkProcessCollisionInput& input, hkAabb* cachedAabb,
                                        hkArray<u32>& hitList) {
    const hkMotionState* msA;
    const hkMotionState* msB = (const hkMotionState*)bodyB.m_motion;
    hkAabb aabb;
    hkVector4 linearTimInfo;
    linearTimInfo._setRotatedInverseDir(msB->m_transform.m_rotation, timInfo);
    msA = (const hkMotionState*)bodyA.m_motion;
    hkVector4 extent;
    if (input.unk28->unk10) {
        hkReal angularA = msA->m_objectRadius * (msA->m_deltaAngle.w + msB->m_deltaAngle.w);
        hkReal angularB = msB->m_objectRadius * (msB->m_deltaAngle.w * msB->m_deltaAngle.w);
        bodyA.m_shape->getAabb(bTa, 0.5f * input.unk8 + (angularA + angularB), aabb);
        hkVector4 center;
        hkVector4 radius;
        radius.setAll(angularB + (0.5f * input.unk8 + msA->m_objectRadius));
        center._setTransformedInversePos(msB->m_transform, msA->m_sweptTransform.m_centerOfMass1);
        hkVector4 sphereMax;
        sphereMax.setAdd4(center, radius);
        hkVector4 sphereMin;
        sphereMin.setSub4(center, radius);
        aabb.m_min.setMax4(aabb.m_min, sphereMin);
        aabb.m_max.setMin4(aabb.m_max, sphereMax);
        extent.setSub4(aabb.m_max, aabb.m_min);
        if (msB->m_deltaAngle.w > 0.0f) {
            hkVector4 arm;
            arm.setSub4(center, msB->m_sweptTransform.m_centerOfMassLocal);
            hkVector4 rotation;
            rotation.setCross(arm, msB->m_deltaAngle);
            linearTimInfo.addMul4(input.unk18 * msB->m_sweptTransform.m_centerOfMass1.w, rotation);
        }
        hkVector4 zero;
        zero.setZero4();
        hkVector4 pathMin;
        pathMin.setMin4(zero, linearTimInfo);
        hkVector4 pathMax;
        pathMax.setMax4(zero, linearTimInfo);
        aabb.m_min.add4(pathMin);
        aabb.m_max.add4(pathMax);
    } else {
        bodyA.m_shape->getAabb(bTa, 0.5f * input.unk8, aabb);
        extent.setSub4(aabb.m_max, aabb.m_min);
    }
    if (cachedAabb != 0) {
        if (cachedAabb->contains(aabb)) {
            return 1;
        }
        hkVector4 zero;
        zero.setZero4();
        hkVector4 pathMin;
        pathMin.setMin4(zero, linearTimInfo);
        hkVector4 pathMax;
        pathMax.setMax4(zero, linearTimInfo);
        hkVector4 tolerance;
        tolerance.setAll(0.5f * input.unk8);
        aabb.m_min.sub4(tolerance);
        aabb.m_max.add4(tolerance);
        hkVector4 growMin;
        growMin.setMul4(-2.0f, pathMax);
        hkVector4 growMax;
        growMax.setMul4(-2.0f, pathMin);
        hkVector4 limit;
        limit.setMul4(0.4f, extent);
        growMax.setMin4(growMax, limit);
        hkVector4 negLimit;
        negLimit.setNeg4(limit);
        growMin.setMax4(growMin, negLimit);
        aabb.m_min.add4(growMin);
        aabb.m_max.add4(growMax);
        *cachedAabb = aabb;
    }
    ((const hkBvTreeShape*)bodyB.m_shape)->queryAabb(aabb, hitList);
    return 0;
}

void hkBvTreeAgent::staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                          void* target) {
    hkShapeContainer::ShapeBuffer shapeBuffer;
    const u32* end;
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
    end = hitList.begin() + hitList.getSize();
    hkCdBody childBody(&bodyB);
    const hkShapeCollection* collection = bvTree->m_collection;
    for (const u32* key = hitList.begin(); key != end; key++) {
        if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *collection, *key)) {
            const hkShape* child = collection->m_container.getChildShape(*key, shapeBuffer);
            u32 childKey = *key;
            childBody.m_shape = (hkShape*)child;
            childBody.m_shapeKey = childKey;
            int typeB = child->getType();
            typedef void (*QueryFunc)(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
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
    hkShapeContainer::ShapeBuffer shapeBuffer;
    const u32* end;
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
    end = hitList.begin() + hitList.getSize();
    hkCdBody childBody(&bodyB);
    const hkShapeCollection* collection = bvTree->m_collection;
    for (const u32* key = hitList.begin(); key != end; key++) {
        if (input.m_filter->isCollisionEnabled(input, bodyA, bodyB, *collection, *key)) {
            const hkShape* child = collection->m_container.getChildShape(*key, shapeBuffer);
            u32 childKey = *key;
            childBody.m_shape = (hkShape*)child;
            childBody.m_shapeKey = childKey;
            int typeB = child->getType();
            typedef void (*CastFunc)(const hkCdBody&, const hkCdBody&, hkLinearCastCollisionInput&, void*, void*);
            ((CastFunc)input.m_dispatcher->getLinearCastFunc(typeA, typeB))(bodyA, childBody, input, target, startTarget);
        }
    }
    HK_TIMER_END_LIST();
}
