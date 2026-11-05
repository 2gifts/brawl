// Havok translation unit hkSphereSphereAgent.o (main.dol 0x802C6490-0x802C6F98).
// Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802C6490   104  registerAgent   [map: hkSphereSphereAgent__registerAgent]
//   0x802C64F8   120  createSphereSphereAgent   [map: hkSphereSphereAgent__createSphereSphereAgent]
//   0x802C6570   116  cleanup   [map: hkSphereSphereAgent__cleanup]
//   0x802C65E4    92  __dt   [map: hkSphereSphereAgent____dt]
//   0x802C6640   752  processCollision   [map: hkSphereSphereAgent__processCollision]
//   0x802C6930   632  getClosestPoints   [map: hkSphereSphereAgent__getClosestPoints]
//   0x802C6BA8   632  staticGetClosestPoints   [map: hkSphereSphereAgent__staticGetClosestPoints]
//   0x802C6E20    20  getPenetrations   [map: hkSphereSphereAgent__getPenetrations]
//   0x802C6E34   356  staticGetPenetrations   [map: hkSphereSphereAgent__staticGetPenetrations]

#include <havok/hkSphereSphereAgent.h>
#include <havok/hkShapeType.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkSphereShape.h>
#include <havok/hkTransform.h>
#include <havok/hkCdBodyPairCollector.h>

// Stand-in for hkIterativeLinearCastAgent::staticLinearCast (other unit).
extern "C" void fn_802B7FD0();

void hkSphereSphereAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs funcs;
    funcs.symmetricA = 0;
    funcs.symmetricB = 0;
    funcs.create = (hkAgentFunc)createSphereSphereAgent;
    funcs.staticGetPenetrations = (hkAgentFunc)staticGetPenetrations;
    funcs.staticGetClosestPoints = (hkAgentFunc)staticGetClosestPoints;
    funcs.staticLinearCast = (hkAgentFunc)fn_802B7FD0;
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
