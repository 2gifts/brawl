// Havok translation unit hkSphereSphereAgent.o (main.dol 0x802C6490-0x802C6F98).
// Functions in address order:
//   0x802C6490   104  registerAgent
//   0x802C64F8   120  createSphereSphereAgent
//   0x802C6570   116  cleanup
//   0x802C65E4    92  __dt
//   0x802C6640   752  processCollision
//   0x802C6930   632  getClosestPoints
//   0x802C6BA8   632  staticGetClosestPoints
//   0x802C6E20    20  getPenetrations
//   0x802C6E34   356  staticGetPenetrations

#include <havok/hkSphereSphereAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkSphereShape.h>
#include <havok/hkTransform.h>
#include <havok/hkCdBodyPairCollector.h>

// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
void hkSphereSphereAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.symmetricA = 0;
    funcs.symmetricB = 0;
    funcs.create = (hkAgentFunc)createSphereSphereAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)hkIterativeLinearCastAgent::staticLinearCast;
    dispatcher->registerCollisionAgent(&funcs, HK_SHAPE_SPHERE, HK_SHAPE_SPHERE);
}

hkSphereSphereAgent* hkSphereSphereAgent::createSphereSphereAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                               hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSphereSphereAgent(contactMgr);
}

void hkSphereSphereAgent::cleanup() {
    u16 slot = unkC;
    if (slot != 0xFFFF) {
        ((hkContactMgr*)unk8)->removeContactPoint(slot);
        unkC = 0xFFFF;
    }
    delete this;
}

hkSphereSphereAgent::~hkSphereSphereAgent() {}

void hkSphereSphereAgent::getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    staticGetPenetrations(unk0, unk1, unk2, unk3);
}

// Reports the pair when the centres are closer than the sum of the radii.
void hkSphereSphereAgent::staticGetPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    HK_TIMER_BEGIN("SphereSphere");
    const hkCdBody& bodyA = unk0;
    const hkCdBody& bodyB = unk1;
    const hkVector4& posB = ((const hkTransform*)bodyB.m_motion)->m_translation;
    const hkVector4& posA = ((const hkTransform*)bodyA.m_motion)->m_translation;
    const hkSphereShape* sphereA = (const hkSphereShape*)bodyA.m_shape;
    const hkSphereShape* sphereB = (const hkSphereShape*)bodyB.m_shape;
    hkVector4 vec;
    vec.setSub4(posB, posA);
    hkReal radiusB = sphereB->m_radius;
    hkReal radiusA = sphereA->m_radius;
    hkReal radiusSum = radiusA + radiusB;
    if (vec.lengthSquared3() < radiusSum * radiusSum) {
        ((hkCdBodyPairCollector*)unk3)->addCdBodyPair(bodyA, bodyB);
    }
    HK_TIMER_END();
}

#include <havok/hkCdPoint.h>

// One point on B's surface towards A, the normal from B to A and the distance between the surfaces, when the spheres
// are within the tolerance. Coincident centres use the x axis.
static inline void hkSphereSphereAgent_getClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1,
                                                       hkCollisionInput& unk2, void* unk3) {
    HK_TIMER_BEGIN("SphereSphere");
    hkCdPoint event;
    event.m_cdBodyA = &unk0;
    event.m_cdBodyB = &unk1;
    const hkVector4& posB = ((const hkTransform*)unk1.m_motion)->m_translation;
    const hkVector4& posA = ((const hkTransform*)unk0.m_motion)->m_translation;
    const hkSphereShape* sphereB = (const hkSphereShape*)unk1.m_shape;
    const hkSphereShape* sphereA = (const hkSphereShape*)unk0.m_shape;
    hkVector4 vec;
    vec.setSub4(posA, posB);
    hkReal radiusSum = sphereB->m_radius + sphereA->m_radius;
    hkReal checkRadius = unk2.getTolerance() + radiusSum;
    hkReal distSquared = vec.lengthSquared3();
    hkBool hit;
    if (distSquared < checkRadius * checkRadius) {
        hkReal dist;
        if (distSquared > 0.0f) {
            hkReal invDist = hkVector4_sqrtInverse(distSquared);
            dist = distSquared * invDist;
            event.m_contact.m_normal.setMul4(invDist, vec);
        } else {
            dist = 0.0f;
            const hkVector4& axis = hkTransform::getIdentity().getColumn(0);
            event.m_contact.m_normal.x = axis.x;
            event.m_contact.m_normal.y = axis.y;
            event.m_contact.m_normal.z = axis.z;
        }
        hkReal radiusB = sphereB->m_radius;
        event.m_contact.m_position.setAddMul4(posB, event.m_contact.m_normal, radiusB);
        event.m_contact.m_normal.w = dist - (sphereA->m_radius + radiusB);
        hit = true;
    } else {
        hit = false;
    }
    if (hit) {
        ((hkCdPointCollector*)unk3)->addCdPoint(&event);
    }
    HK_TIMER_END();
}

void hkSphereSphereAgent::getClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2,
                                           void* unk3) {
    hkSphereSphereAgent_getClosestPoints(unk0, unk1, unk2, unk3);
}

void hkSphereSphereAgent::staticGetClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2,
                                                 void* unk3) {
    hkSphereSphereAgent_getClosestPoints(unk0, unk1, unk2, unk3);
}

#include <havok/hkProcessCollisionOutput.h>

// Keeps one contact point while the spheres are within the tolerance: written straight into the output's next free
// point, registered with the contact manager the first time, and removed again once the spheres separate.
void hkSphereSphereAgent::processCollision(const hkCdBody& unk0, const hkCdBody& unk1, hkProcessCollisionInput& unk2,
                                           void* unk3) {
    HK_TIMER_BEGIN("SphereSphere");
    hkProcessCollisionOutput& output = *(hkProcessCollisionOutput*)unk3;
    const hkVector4& posA = ((const hkTransform*)unk0.m_motion)->m_translation;
    const hkVector4& posB = ((const hkTransform*)unk1.m_motion)->m_translation;
    const hkSphereShape* sphereB = (const hkSphereShape*)unk1.m_shape;
    const hkSphereShape* sphereA = (const hkSphereShape*)unk0.m_shape;
    hkVector4 vec;
    vec.setSub4(posA, posB);
    hkReal radiusSum = sphereA->m_radius + sphereB->m_radius;
    hkReal checkRadius = ((hkCollisionInput&)unk2).getTolerance() + radiusSum;
    hkReal distSquared = vec.lengthSquared3();
    if (distSquared < checkRadius * checkRadius) {
        hkProcessCdPoint& point = *output.m_firstFreeContactPoint;
        hkReal dist;
        if (distSquared > 0.0f) {
            hkReal invDist = hkVector4_sqrtInverse(distSquared);
            point.m_contact.m_normal.setMul4(invDist, vec);
            dist = distSquared * invDist - (sphereA->m_radius + sphereB->m_radius);
        } else {
            point.m_contact.m_normal = hkTransform::getIdentity().getColumn(0);
            dist = -(sphereA->m_radius + sphereB->m_radius);
        }
        point.m_contact.m_position.setAddMul4(posB, point.m_contact.m_normal, sphereB->m_radius);
        point.m_contact.m_normal.w = dist;
        if (unkC == 0xFFFF) {
            unkC = ((hkContactMgr*)unk8)->addContactPoint(unk0, unk1, unk2, output, 0, point.m_contact);
        }
        if (unkC != 0xFFFF) {
            point.m_contactPointId = unkC;
            output.m_firstFreeContactPoint++;
        }
    } else if (unkC != 0xFFFF) {
        ((hkContactMgr*)unk8)->removeContactPoint(unkC);
        unkC = 0xFFFF;
    }
    HK_TIMER_END();
}
