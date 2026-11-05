// Havok translation unit hkCollisionAgent.o (main.dol 0x802B7A00-0x802B85EC).
// Functions in address order (method names from the Havok TU map):
//   0x802B7A00     4  invalidateTim   [map: hkCollisionAgent__invalidateTim]
//   0x802B7A04     4  warpTime   [map: hkCollisionAgent__warpTime]
//   0x802B7A08     4  removePoint   [map: hkCollisionAgent__removePoint]
//   0x802B7A0C     4  commitPotential   [map: hkCollisionAgent__commitPotential]
//   0x802B7A10     4  createZombie   [map: hkCollisionAgent__createZombie]
//   0x802B7A14  1468  linearCast   [map: hkIterativeLinearCastAgent__linearCast]
//   0x802B7FD0  1564  staticLinearCast   [map: hkIterativeLinearCastAgent__staticLinearCast]

#include <havok/hkCollisionAgent.h>
#include <havok/hkIterativeLinearCastAgent.h>
#include <havok/hkSimpleClosestContactCollector.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkMotionState.h>

// Default hooks of the base agent: empty in the original.
void hkCollisionAgent::invalidateTim(void* arg) {}
void hkCollisionAgent::warpTime(float t0, float t1, void* arg) {}
void hkCollisionAgent::removePoint(u16 key) {}
void hkCollisionAgent::commitPotential(u16 key) {}
void hkCollisionAgent::createZombie(u16 key) {}

// Copy of a motion state through four vector temporaries for the transform (the target writes them to the stack
// although they are dead; hkMultiSphereAgent.cpp has the same unit-local copy).
static inline void hkIterativeLinearCastAgent_copyMotionState(hkMotionState& dst, const hkMotionState& src) {
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

// Closest points of the pair (tolerance widened by input.unk24). A point closer than the input tolerance goes to the
// start collector. If the bodies do not approach along the path, nothing is reported. If they are already within the
// config distance, the hit fraction is the distance over the approach. Otherwise a copy of A's motion state is moved
// along the path, the pair is queried again and the fraction grows by distance / approach, at most unk28->unk4 more
// times or until the distance is within the config distance.
void hkIterativeLinearCastAgent::linearCast(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                            hkLinearCastCollisionInput& input, void* target, void* d) {
    hkCdPointCollector& collector = *(hkCdPointCollector*)target;
    hkCdPointCollector* startCollector = (hkCdPointCollector*)d;
    hkSimpleClosestContactCollector closest;
    hkCollisionInput in = input;
    in.unk8 = in.unk8 + input.unk24;
    getClosestPoints(bodyA, bodyB, in, &closest);
    if (!(bool)closest.m_hit) {
        return;
    }

    hkCdPoint event;
    event.m_cdBodyA = &bodyA;
    event.m_cdBodyB = &bodyB;
    event.m_contact = closest.m_point;
    if (event.m_contact.m_normal.w < input.unk8 && startCollector != 0) {
        startCollector->addCdPoint(&event);
    }

    hkReal projected = closest.m_point.m_normal.dot3(input.m_path);
    hkReal startDistance = closest.m_point.m_normal.w;
    hkReal endDistance = startDistance + projected;
    if (endDistance > 0.0f) {
        return;
    }
    if (projected + input.unk20 >= 0.0f) {
        return;
    }

    if (startDistance <= input.unk28->unk0) {
        if (startDistance > 0.0f) {
            hkReal fraction = startDistance / (startDistance - endDistance);
            hkReal earlyOut = collector.m_earlyOutDistance;
            if (fraction > earlyOut) {
                return;
            }
            event.m_contact.m_normal.w = fraction;
        } else {
            event.m_contact.m_normal.w = 0.0f;
        }
        collector.addCdPoint(&event);
        return;
    }

    hkReal fraction = startDistance / (startDistance - endDistance);
    hkSimpleClosestContactCollector checker;
    event.m_contact.m_normal.w = fraction;
    hkMotionState motion;
    hkMotionState* movedMotion = &motion;
    hkIterativeLinearCastAgent_copyMotionState(motion, *(const hkMotionState*)bodyA.m_motion);
    hkCdBody body(&bodyA, movedMotion);
    hkShape* shape = bodyA.m_shape;
    body.m_shapeKey = bodyA.m_shapeKey;
    body.m_shape = shape;

    for (int i = input.unk28->unk4 - 1; i >= 0; i--) {
        checker.reset();
        hkVector4 position;
        position.setAddMul4(((const hkMotionState*)bodyA.m_motion)->m_transform.m_translation, input.m_path,
                            event.m_contact.m_normal.w);
        motion.m_transform.m_translation = position;
        getClosestPoints(body, bodyB, in, &checker);

        hkReal approach = checker.m_point.m_normal.dot3(input.m_path);
        if (approach >= 0.0f) {
            return;
        }
        approach = -approach;
        hkReal current = event.m_contact.m_normal.w;
        hkReal distance = checker.m_point.m_normal.w;
        hkReal advanced = approach * current;
        if (distance + advanced > approach) {
            return;
        }
        hkReal next = distance / approach + current;
        if (next > collector.m_earlyOutDistance) {
            return;
        }
        event.m_contact.m_position = checker.m_point.m_position;
        event.m_contact.m_normal = checker.m_point.m_normal;
        event.m_contact.m_normal.w = next;
        if (distance <= input.unk28->unk0) {
            break;
        }
    }
    collector.addCdPoint(&event);
}

// The static form of linearCast: the same algorithm with the dispatcher's closest point function for the pair's
// shape types instead of the agent's own query; it also stops when a later query finds nothing.
void hkIterativeLinearCastAgent::staticLinearCast(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                  hkLinearCastCollisionInput& input, void* target, void* d) {
    typedef void (*QueryFunc)(const hkCdBody&, const hkCdBody&, hkCollisionInput&, void*);
    hkCdPointCollector& collector = *(hkCdPointCollector*)target;
    hkCdPointCollector* startCollector = (hkCdPointCollector*)d;
    hkSimpleClosestContactCollector closest;
    hkCollisionInput in = input;
    in.unk8 = in.unk8 + input.unk24;
    int typeA = bodyA.m_shape->getType();
    int typeB = bodyB.m_shape->getType();
    QueryFunc getClosest = (QueryFunc)input.m_dispatcher->getGetClosestPointsFunc(typeA, typeB);
    getClosest(bodyA, bodyB, in, &closest);
    if (!(bool)closest.m_hit) {
        return;
    }

    hkCdPoint event;
    event.m_cdBodyA = &bodyA;
    event.m_cdBodyB = &bodyB;
    event.m_contact.m_normal = closest.m_point.m_normal;
    event.m_contact.m_position = closest.m_point.m_position;
    if (event.m_contact.m_normal.w < input.unk8 && startCollector != 0) {
        startCollector->addCdPoint(&event);
    }

    hkReal projected = closest.m_point.m_normal.dot3(input.m_path);
    hkReal startDistance = closest.m_point.m_normal.w;
    hkReal endDistance = startDistance + projected;
    if (endDistance > 0.0f) {
        return;
    }
    if (projected + input.unk20 >= 0.0f) {
        return;
    }

    if (startDistance <= input.unk28->unk0) {
        if (startDistance > 0.0f) {
            hkReal fraction = startDistance / (startDistance - endDistance);
            hkReal earlyOut = collector.m_earlyOutDistance;
            if (fraction > earlyOut) {
                return;
            }
            event.m_contact.m_normal.w = fraction;
        } else {
            event.m_contact.m_normal.w = 0.0f;
        }
        collector.addCdPoint(&event);
        return;
    }

    hkReal fraction = startDistance / (startDistance - endDistance);
    hkSimpleClosestContactCollector checker;
    event.m_contact.m_normal.w = fraction;
    hkMotionState motion;
    hkMotionState* movedMotion = &motion;
    hkIterativeLinearCastAgent_copyMotionState(motion, *(const hkMotionState*)bodyA.m_motion);
    hkCdBody body(&bodyA, movedMotion);
    hkShape* shape = bodyA.m_shape;
    body.m_shapeKey = bodyA.m_shapeKey;
    body.m_shape = shape;

    for (int i = input.unk28->unk4 - 1; i >= 0; i--) {
        checker.reset();
        hkVector4 position;
        position.setAddMul4(((const hkMotionState*)bodyA.m_motion)->m_transform.m_translation, input.m_path,
                            event.m_contact.m_normal.w);
        motion.m_transform.m_translation = position;
        getClosest(body, bodyB, in, &checker);
        if (!(bool)checker.m_hit) {
            return;
        }

        hkReal approach = checker.m_point.m_normal.dot3(input.m_path);
        if (approach >= 0.0f) {
            return;
        }
        approach = -approach;
        hkReal current = event.m_contact.m_normal.w;
        hkReal distance = checker.m_point.m_normal.w;
        hkReal advanced = approach * current;
        if (distance + advanced > approach) {
            return;
        }
        hkReal next = distance / approach + current;
        if (next > collector.m_earlyOutDistance) {
            return;
        }
        event.m_contact.m_position = checker.m_point.m_position;
        event.m_contact.m_normal = checker.m_point.m_normal;
        event.m_contact.m_normal.w = next;
        if (distance <= input.unk28->unk0) {
            break;
        }
    }
    collector.addCdPoint(&event);
}
