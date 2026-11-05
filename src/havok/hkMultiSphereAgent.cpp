// Havok translation unit hkMultiSphereAgent.o (main.dol 0x802BABC4-0x802C0590).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802BABC4   204  registerAgent   [map: hkMultiSphereAgent__registerAgent]
//   0x802BAC90   124  createListAAgent   [map: hkMultiSphereAgent__createListAAgent]
//   0x802BAD0C   132  createListBAgent   [map: hkMultiSphereAgent__createListBAgent]
//   0x802BAD90   152  __dt   [map: hkMultiSphereAgent____dt]
//   0x802BAEB8  1172  __ct   [map: hkMultiSphereAgent____ct]
//   0x802BB3A8   148  cleanup   [map: hkMultiSphereAgent__cleanup]
//   0x802BB43C  1112  processCollision   [map: hkMultiSphereAgent__processCollision]
//   0x802BB894  1016  getClosestPoints   [map: hkMultiSphereAgent__getClosestPoints]
//   0x802BBC8C  1056  staticGetClosestPoints   [map: hkMultiSphereAgent__staticGetClosestPoints]
//   0x802BC0AC  1020  linearCast   [map: hkMultiSphereAgent__linearCast]
//   0x802BC4A8  1064  staticLinearCast   [map: hkMultiSphereAgent__staticLinearCast]
//   0x802BC8D0  1040  getPenetrations   [map: hkMultiSphereAgent__getPenetrations]
//   0x802BCCE0  1080  staticGetPenetrations   [map: hkMultiSphereAgent__staticGetPenetrations]
//   0x802BD118   352  linearCast   [map: hkSymmetricAgent_18hkMultiSphereAgent___linearCast]
//   0x802BD278    72  getPenetrations   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___getPenetrations]
//   0x802BD2C0    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___staticGetPenetrations]
//   0x802BD308    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___getClosestPoints]
//   0x802BD350    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___staticGetClosestPoints]
//   0x802BD398   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___staticLinearCast]
//   0x802BD4F8   412  processCollision   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___processCollision]
//   0x802BD694     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_18hkMultiSphereAgent___updateShapeCollectionFilter]
//   0x802BD698   160  __dt   [map: hkSymmetricAgent_18hkMultiSphereAgent_____dt]
//   0x802BD738   204  registerAgent   [map: hkMultiSphereTriangleAgent__registerAgent]
//   0x802BD804   212  createTriangleMultiSphereAgent   [map: hkMultiSphereTriangleAgent__createTriangleMultiSphereAgent]
//   0x802BD8D8    92  __dt   [map: hkMultiSphereTriangleAgent____dt]
//   0x802BD934   196  createMultiSphereTriangleAgent   [map: hkMultiSphereTriangleAgent__createMultiSphereTriangleAgent]
//   0x802BD9F8   144  cleanup   [map: hkMultiSphereTriangleAgent__cleanup]
//   0x802BDA88  1692  getClosestPoints   [map: hkMultiSphereTriangleAgent__getClosestPoints]
//   0x802BE124  1696  staticGetClosestPoints   [map: hkMultiSphereTriangleAgent__staticGetClosestPoints]
//   0x802BE7C4  1432  getPenetrations   [map: hkMultiSphereTriangleAgent__getPenetrations]
//   0x802BED5C  1440  staticGetPenetrations   [map: hkMultiSphereTriangleAgent__staticGetPenetrations]
//   0x802BF2FC  1844  processCollision   [map: hkMultiSphereTriangleAgent__processCollision]
//   0x802BFA30    72  getPenetrations   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___getPenetrations]
//   0x802BFA78    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___staticGetPenetrations]
//   0x802BFAC0    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___getClosestPoints]
//   0x802BFB08    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___staticGetClosestPoints]
//   0x802BFB50   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___staticLinearCast]
//   0x802BFCB0  2176  processCollision   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___processCollision]
//   0x802C0530     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent___updateShapeCollectionFilter]
//   0x802C0534    92  __dt   [map: hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_____dt]

#include <havok/hkMultiSphereAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkSymmetricAgentProcessCollision.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkShapeType.h>
#include <havok/hkMultiSphereShape.h>
#include <havok/hkSphereShape.h>
#include <havok/hkMotionState.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCdBody.h>
#include <havok/hkCdBodyPairCollector.h>

// Stand-ins for the query functions of the symmetric multi-sphere agents (not written yet).
extern "C" void fn_802B7FD0(void* a, void* b, void* c, void* target, void* d);

void hkMultiSphereAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs listB;
    listB.create = (hkAgentFunc)createListBAgent;
    listB.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::staticGetPenetrations;
    listB.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::staticGetClosestPoints;
    listB.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::staticLinearCast;
    listB.symmetricA = 1;
    listB.symmetricB = 1;
    dispatcher->registerCollisionAgent(&listB, -1, HK_SHAPE_MULTI_SPHERE);

    hkAgentFuncs listA;
    listA.create = (hkAgentFunc)createListAAgent;
    listA.staticGetPenetrations = (hkAgentFunc)hkMultiSphereAgent::staticGetPenetrations;
    listA.staticGetClosestPoints = (hkAgentFunc)hkMultiSphereAgent::staticGetClosestPoints;
    listA.staticLinearCast = (hkAgentFunc)hkMultiSphereAgent::staticLinearCast;
    listA.symmetricA = 0;
    listA.symmetricB = 1;
    dispatcher->registerCollisionAgent(&listA, HK_SHAPE_MULTI_SPHERE, -1);
}

hkMultiSphereAgent* hkMultiSphereAgent::createListAAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    hkReferencedObject* mem = (hkReferencedObject*)hkMemory::getInstance().allocateChunk(0x38, 0x1d);
    mem->m_memSizeAndFlags = 0x38;
    hkMultiSphereAgent* agent = (hkMultiSphereAgent*)mem;
    new (agent) hkMultiSphereAgent(unk0, unk1, unk2, contactMgr);
    return agent;
}

hkMultiSphereAgent* hkMultiSphereAgent::createListBAgent(void* unk0, void* unk1, void* unk2, hkContactMgr* contactMgr) {
    return new hkSymmetricAgent_18hkMultiSphereAgent_(unk1, unk0, unk2, contactMgr);
}

void hkMultiSphereAgent::cleanup() {
    for (int i = 0; i < m_entries.m_size; i++) {
        m_entries.begin()[i].agent->cleanup();
    }
    delete this;
}

// Runs each child agent against one sphere of the shape: a sphere shape on the stack takes the sphere's radius and a
// copy of A's motion state the sphere's center (rotated into world space), so the child sees a plain sphere body.
// HYPOTHESIS: the motion state assignment of these queries copies the transform through four vector temporaries (the
// target keeps their dead stack copies, col0 at the top and the translation lowest). Kept local to this unit: an
// hkTransform::operator= of this form costs hkSweptTransformUtil::warpTo 30%.
static inline void hkMultiSphereAgent_copyMotionState(hkMotionState& dst, const hkMotionState& src) {
    hkVector4 c0 = src.m_transform.m_rotation.m_col0;
    hkVector4 c1 = src.m_transform.m_rotation.m_col1;
    hkVector4 c2 = src.m_transform.m_rotation.m_col2;
    hkVector4 tr = src.m_transform.m_translation;
    dst.m_transform.m_rotation.m_col0 = c0;
    dst.m_transform.m_rotation.m_col1 = c1;
    dst.m_transform.m_rotation.m_col2 = c2;
    dst.m_transform.m_translation = tr;
    dst.m_sweptTransform = src.m_sweptTransform;
    dst.m_deltaAngle = src.m_deltaAngle;
    dst.m_objectRadius = src.m_objectRadius;
    dst.m_maxLinearVelocity = src.m_maxLinearVelocity;
    dst.m_maxAngularVelocity = src.m_maxAngularVelocity;
    dst.m_linearDamping = src.m_linearDamping;
    dst.m_angularDamping = src.m_angularDamping;
    dst.m_deactivationClass = src.m_deactivationClass;
    dst.m_deactivationCounter = src.m_deactivationCounter;
}

// Queries of the multi-sphere agent: each child agent (or, for the statics, the dispatcher's function for the sphere
// against B) runs against one sphere of the shape. A sphere shape on the stack takes the sphere's radius and a copy of
// A's motion state the sphere's center rotated into world space, so the child sees a plain sphere body.
// NONMATCHING: the dead stack copies of the transform vectors are scheduled differently (96-97%); the statics also
// differ in register coloring (86-97%). The linear cast always uses the first entry's agent (as in the target).
void hkMultiSphereAgent::getClosestPoints(void* a, void* b, void* c, void* target) {
    HK_TIMER_BEGIN("MultiSphere");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkMultiSphereShape* shape = (const hkMultiSphereShape*)bodyA->m_shape;
    hkMotionState motion;
    hkMotionState* childMotion = &motion;
    hkMultiSphereAgent_copyMotionState(motion, *(const hkMotionState*)bodyA->m_motion);
    hkSphereShape sphere(0.0f);
    const hkVector4* spheres = shape->m_spheres;
    hkCdBody childBody(bodyA);
    childBody.m_motion = (hkMotion*)childMotion;
    hkMultiSphereAgentEntry* entry = m_entries.begin();
    for (int i = m_entries.m_size - 1; i >= 0; i--) {
        const hkVector4& s = spheres[entry->unk0];
        hkVector4 center;
        center._setRotatedDir(motion.m_transform.m_rotation, s);
        const hkVector4& pos = ((const hkMotionState*)bodyA->m_motion)->m_transform.m_translation;
        motion.m_transform.m_translation.x = pos.x + center.x;
        motion.m_transform.m_translation.y = pos.y + center.y;
        motion.m_transform.m_translation.z = pos.z + center.z;
        motion.m_transform.m_translation.w = pos.w + center.w;
        hkSimdReal radius = s.w;
        sphere.m_radius = radius;
        childBody.m_shape = &sphere;
        childBody.m_shapeKey = i;
        entry->agent->getClosestPoints(&childBody, b, c, target);
        entry++;
    }
    HK_TIMER_END();
}

void hkMultiSphereAgent::staticGetClosestPoints(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, void* target) {
    HK_TIMER_BEGIN("MultiSphere");
    const hkMultiSphereShape* shape = (const hkMultiSphereShape*)bodyA.m_shape;
    hkMotionState motion;
    hkMotionState* childMotion = &motion;
    hkMultiSphereAgent_copyMotionState(motion, *(const hkMotionState*)bodyA.m_motion);
    hkSphereShape sphere(0.0f);
    hkCdBody childBody(&bodyA);
    childBody.m_motion = (hkMotion*)childMotion;
    int typeB = bodyB.m_shape->getType();
    const hkVector4* spheres = shape->m_spheres;
    for (int i = 0; i < shape->m_numSpheres; i++) {
        const hkVector4& s = spheres[i];
        hkVector4 center;
        center._setRotatedDir(motion.m_transform.m_rotation, s);
        const hkVector4& pos = ((const hkMotionState*)bodyA.m_motion)->m_transform.m_translation;
        motion.m_transform.m_translation.x = pos.x + center.x;
        motion.m_transform.m_translation.y = pos.y + center.y;
        motion.m_transform.m_translation.z = pos.z + center.z;
        motion.m_transform.m_translation.w = pos.w + center.w;
        hkSimdReal radius = s.w;
        sphere.m_radius = radius;
        childBody.m_shape = &sphere;
        childBody.m_shapeKey = i;
        typedef void (*QueryFunc)(void*, void*, void*, void*);
        ((QueryFunc)input.m_dispatcher->getGetClosestPointsFunc(sphere.getType(), typeB))(&childBody, (void*)&bodyB, (void*)&input, target);
    }
    HK_TIMER_END();
}

void hkMultiSphereAgent::linearCast(void* a, void* b, void* c, void* target, void* d) {
    HK_TIMER_BEGIN("MultiSphere");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkMultiSphereShape* shape = (const hkMultiSphereShape*)bodyA->m_shape;
    hkMotionState motion;
    hkMotionState* childMotion = &motion;
    hkMultiSphereAgent_copyMotionState(motion, *(const hkMotionState*)bodyA->m_motion);
    hkSphereShape sphere(0.0f);
    const hkVector4* spheres = shape->m_spheres;
    hkCdBody childBody(bodyA);
    childBody.m_motion = (hkMotion*)childMotion;
    hkMultiSphereAgentEntry* entry = m_entries.begin();
    for (int i = m_entries.m_size - 1; i >= 0; i--) {
        const hkVector4& s = spheres[i];
        hkVector4 center;
        center._setRotatedDir(motion.m_transform.m_rotation, s);
        const hkVector4& pos = ((const hkMotionState*)bodyA->m_motion)->m_transform.m_translation;
        motion.m_transform.m_translation.x = pos.x + center.x;
        motion.m_transform.m_translation.y = pos.y + center.y;
        motion.m_transform.m_translation.z = pos.z + center.z;
        motion.m_transform.m_translation.w = pos.w + center.w;
        hkSimdReal radius = s.w;
        sphere.m_radius = radius;
        childBody.m_shape = &sphere;
        childBody.m_shapeKey = i;
        entry->agent->linearCast(&childBody, b, c, target, d);
    }
    HK_TIMER_END();
}

void hkMultiSphereAgent::staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, void* target, void* d) {
    HK_TIMER_BEGIN("MultiSphere");
    const hkMultiSphereShape* shape = (const hkMultiSphereShape*)bodyA.m_shape;
    hkMotionState motion;
    hkMotionState* childMotion = &motion;
    hkMultiSphereAgent_copyMotionState(motion, *(const hkMotionState*)bodyA.m_motion);
    hkSphereShape sphere(0.0f);
    hkCdBody childBody(&bodyA);
    childBody.m_motion = (hkMotion*)childMotion;
    int typeB = bodyB.m_shape->getType();
    const hkVector4* spheres = shape->m_spheres;
    for (int i = 0; i < shape->m_numSpheres; i++) {
        const hkVector4& s = spheres[i];
        hkVector4 center;
        center._setRotatedDir(motion.m_transform.m_rotation, s);
        const hkVector4& pos = ((const hkMotionState*)bodyA.m_motion)->m_transform.m_translation;
        motion.m_transform.m_translation.x = pos.x + center.x;
        motion.m_transform.m_translation.y = pos.y + center.y;
        motion.m_transform.m_translation.z = pos.z + center.z;
        motion.m_transform.m_translation.w = pos.w + center.w;
        hkSimdReal radius = s.w;
        sphere.m_radius = radius;
        childBody.m_shape = &sphere;
        childBody.m_shapeKey = i;
        typedef void (*QueryFunc)(void*, void*, void*, void*, void*);
        ((QueryFunc)input.m_dispatcher->getLinearCastFunc(sphere.getType(), typeB))(&childBody, (void*)&bodyB, (void*)&input, target, d);
    }
    HK_TIMER_END();
}

void hkMultiSphereAgent::getPenetrations(void* a, void* b, void* c, void* target) {
    HK_TIMER_BEGIN("MultiSphere");
    const hkCdBody* bodyA = (const hkCdBody*)a;
    const hkMultiSphereShape* shape = (const hkMultiSphereShape*)bodyA->m_shape;
    hkMotionState motion;
    hkMotionState* childMotion = &motion;
    hkMultiSphereAgent_copyMotionState(motion, *(const hkMotionState*)bodyA->m_motion);
    hkSphereShape sphere(0.0f);
    const hkVector4* spheres = shape->m_spheres;
    hkCdBody childBody(bodyA);
    childBody.m_motion = (hkMotion*)childMotion;
    hkMultiSphereAgentEntry* entry = m_entries.begin();
    for (int i = m_entries.m_size - 1; i >= 0; i--) {
        const hkVector4& s = spheres[entry->unk0];
        hkVector4 center;
        center._setRotatedDir(motion.m_transform.m_rotation, s);
        const hkVector4& pos = ((const hkMotionState*)bodyA->m_motion)->m_transform.m_translation;
        motion.m_transform.m_translation.x = pos.x + center.x;
        motion.m_transform.m_translation.y = pos.y + center.y;
        motion.m_transform.m_translation.z = pos.z + center.z;
        motion.m_transform.m_translation.w = pos.w + center.w;
        hkSimdReal radius = s.w;
        sphere.m_radius = radius;
        childBody.m_shape = &sphere;
        childBody.m_shapeKey = i;
        entry->agent->getPenetrations(&childBody, b, c, target);
        if (((hkCdBodyPairCollector*)target)->m_earlyOut) {
            break;
        }
        entry++;
    }
    HK_TIMER_END();
}

void hkMultiSphereAgent::staticGetPenetrations(const hkCdBody& bodyA, const hkCdBody& bodyB, const hkCollisionInput& input, void* target) {
    HK_TIMER_BEGIN("MultiSphere");
    const hkMultiSphereShape* shape = (const hkMultiSphereShape*)bodyA.m_shape;
    hkMotionState motion;
    hkMotionState* childMotion = &motion;
    hkMultiSphereAgent_copyMotionState(motion, *(const hkMotionState*)bodyA.m_motion);
    hkSphereShape sphere(0.0f);
    hkCdBody childBody(&bodyA);
    childBody.m_motion = (hkMotion*)childMotion;
    int typeB = bodyB.m_shape->getType();
    const hkVector4* spheres = shape->m_spheres;
    for (int i = 0; i < shape->m_numSpheres; i++) {
        const hkVector4& s = spheres[i];
        hkVector4 center;
        center._setRotatedDir(motion.m_transform.m_rotation, s);
        const hkVector4& pos = ((const hkMotionState*)bodyA.m_motion)->m_transform.m_translation;
        motion.m_transform.m_translation.x = pos.x + center.x;
        motion.m_transform.m_translation.y = pos.y + center.y;
        motion.m_transform.m_translation.z = pos.z + center.z;
        motion.m_transform.m_translation.w = pos.w + center.w;
        hkSimdReal radius = s.w;
        sphere.m_radius = radius;
        childBody.m_shape = &sphere;
        childBody.m_shapeKey = i;
        typedef void (*QueryFunc)(void*, void*, void*, void*);
        ((QueryFunc)input.m_dispatcher->getGetPenetrationsFunc(sphere.getType(), typeB))(&childBody, (void*)&bodyB, (void*)&input, target);
        if (((hkCdBodyPairCollector*)target)->m_earlyOut) {
            break;
        }
    }
    HK_TIMER_END();
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_18hkMultiSphereAgent_::linearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkMultiSphereAgent::linearCast(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        hkMultiSphereAgent::linearCast(b, a, &flippedInput, &flipped, 0);
    }
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::staticLinearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkMultiSphereAgent::staticLinearCast(*(const hkCdBody*)b, *(const hkCdBody*)a, flippedInput, &flipped, &flippedStart);
    } else {
        hkMultiSphereAgent::staticLinearCast(*(const hkCdBody*)b, *(const hkCdBody*)a, flippedInput, &flipped, 0);
    }
}

void hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::processCollision(void* a, void* b, void* c, void* d) {
    hkSymmetricAgent_processCollision<hkMultiSphereAgent>(this, a, b, c, d);
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_::staticLinearCast(void* a, void* b, void* c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = *(const hkLinearCastCollisionInput*)c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        fn_802B7FD0(b, a, &flippedInput, &flipped, &flippedStart);
    } else {
        fn_802B7FD0(b, a, &flippedInput, &flipped, 0);
    }
}

// Queries with the bodies swapped: the collector is wrapped in one that turns the results back.
void hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::getPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    hkMultiSphereAgent::getPenetrations(b, a, c, &wrapped);
}

void hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::staticGetPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    hkMultiSphereAgent::staticGetPenetrations(*(const hkCdBody*)b, *(const hkCdBody*)a, *(const hkCollisionInput*)c, &wrapped);
}

void hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::getClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    hkMultiSphereAgent::getClosestPoints(b, a, c, &wrapped);
}

void hkSymmetricAgentLinearCast_18hkMultiSphereAgent_::staticGetClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    hkMultiSphereAgent::staticGetClosestPoints(*(const hkCdBody*)b, *(const hkCdBody*)a, *(const hkCollisionInput*)c, &wrapped);
}

void hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_::getPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    hkMultiSphereTriangleAgent::getPenetrations(b, a, c, &wrapped);
}

void hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_::staticGetPenetrations(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrapped(target);
    hkMultiSphereTriangleAgent::staticGetPenetrations(b, a, c, &wrapped);
}

void hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_::getClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    hkMultiSphereTriangleAgent::getClosestPoints(b, a, c, &wrapped);
}

void hkSymmetricAgentLinearCast_26hkMultiSphereTriangleAgent_::staticGetClosestPoints(void* a, void* b, void* c, void* target) {
    hkSymmetricAgentFlipCollector wrapped(target);
    hkMultiSphereTriangleAgent::staticGetClosestPoints(b, a, c, &wrapped);
}
