// Havok translation unit hkSimulation.o (main.dol 0x802F0BA8-0x802F30BC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802F0BA8    68  __ct   [map: hkSimulation____ct]
//   0x802F0BEC    92  __dt   [map: hkSimulation____dt]
//   0x802F0C48   356  integrate   [map: hkSimulation__integrate]
//   0x802F0DAC   416  collide   [map: hkSimulation__collide]
//   0x802F0F4C   568  reCollideAfterStepFailure   [map: hkSimulation__reCollideAfterStepFailure]
//   0x802F1184    92  snapSimulateTimeAndGetTimeToAdvanceTo   [map: hkSimulation__snapSimulateTimeAndGetTimeToAdvanceTo]
//   0x802F11E0   248  advanceTime   [map: hkSimulation__advanceTime]
//   0x802F12D8   292  stepDeltaTime   [map: hkSimulation__stepDeltaTime]
//   0x802F13FC   132  __dt   [map: hkTimeFunctionHelper____dt]
//   0x802F1480  1764  collideEntitiesBroadPhaseDiscrete   [map: hkSimulation__collideEntitiesBroadPhaseDiscrete]
//   0x802F1B64  1104  collideInternal   [map: hkSimulation__collideInternal]
//   0x802F1FB4  1544  integrateInternal   [map: hkSimulation__integrateInternal]
//   0x802F25BC   148  applyActions   [map: hkSimulation__applyActions]
//   0x802F2650   264  collideEntitiesDiscrete   [map: hkSimulation__collideEntitiesDiscrete]
//   0x802F2758    68  collideEntitiesNarrowPhaseDiscrete   [map: hkSimulation__collideEntitiesNarrowPhaseDiscrete]
//   0x802F279C   280  processAgentCollideDiscrete   [map: hkSimulation__processAgentCollideDiscrete]
//   0x802F28B4     8  getCollidableA   [map: hkAgentNnEntry__getCollidableA]
//   0x802F28BC    84  resetCollisionInformationForEntities   [map: hkSimulation__resetCollisionInformationForEntities]
//   0x802F2910    12  processAgentResetCollisionInformation   [map: hkSimulation__processAgentResetCollisionInformation]
//   0x802F291C  1312  processAgentsOfEntities   [map: hkSimulation__processAgentsOfEntities]
//   0x802F2E3C     8  getLinkedCollidable   [map: hkEntity__getLinkedCollidable]
//   0x802F2E44    36  __ct   [map: hkQuaternion____ct1]
//   0x802F2E68     8  getRotation   [map: hkMotion__getRotation]
//   0x802F2E70     8  getRigidMotion   [map: hkRigidBody__getRigidMotion]
//   0x802F2E78     8  getTransform   [map: hkRigidBody__getTransform]
//   0x802F2E80     8  getSeparatingNormal   [map: hkContactPoint__getSeparatingNormal]
//   0x802F2E88     8  getCollidable   [map: hkWorldObject__getCollidable]
//   0x802F2E90    52  setNeg4   [map: hkVector4__setNeg4]
//   0x802F2EC4     8  getCenterOfMassInWorld   [map: hkRigidBody__getCenterOfMassInWorld]
//   0x802F2ECC     4  getPosition   [map: hkContactPoint__getPosition1]
//   0x802F2ED0    88  asin   [map: hkMath__asin]
//   0x802F2F28    48  __ct   [map: hkQuaternion____ct2]
//   0x802F2F58   228  setRotationAroundCentreOfMass   [map: hkRigidBody__setRotationAroundCentreOfMass]
//   0x802F303C   116  setInverseMul   [map: hkQuaternion__setInverseMul]
//   0x802F30B0    12  __ct   [map: hkProcessCollisionOutput____ct]

#include <havok/hkSimulation.h>
#include <havok/hkRigidBody.h>
#include <havok/hkProcessCollisionOutput.h>
#include <havok/hkCdPoint.h>
#include <havok/hkWorld.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkAction.h>
#include <havok/hkMath.h>
#include <math.h>

hkSimulation::hkSimulation(hkWorld* world) {
    m_world = world;
    m_unk10 = 0.0f;
    m_unk14 = 0.0f;
    m_unk1C = 1.0f;
    m_unk20 = -1.0f;
    m_unk24 = 0;
    m_unk0C = 1;
}

hkSimulation::~hkSimulation() {}

// Snaps the current time to the step end when it is close enough, then returns the time to advance to.
hkReal hkSimulation::snapSimulateTimeAndGetTimeToAdvanceTo() {
    if (0.0f != m_unk1C) {
        hkReal diff = m_unk1C - m_unk14;
        if ((hkReal)fabs(diff) < m_unk20) {
            m_unk1C = m_unk14;
        }
    }
    if (m_unk1C == 0.0f) {
        return m_unk14;
    }
    if (m_unk14 < m_unk1C) {
        return m_unk14;
    }
    return m_unk1C;
}

// Lets every action of every active island act for this step.
// HYPOTHESIS: world +0x28 holds the active islands and +0x170 the step info.
void hkSimulation::applyActions() {
    for (int i = 0; i < m_world->m_unk28.m_size; i++) {
        hkSimulationIsland* island = ((hkSimulationIsland**)m_world->m_unk28.m_data)[i];
        hkArray<hkAction*>& actions = island->m_actions;
        for (int j = 0; j < actions.getSize(); j++) {
            actions[j]->applyAction(*(const hkStepInfo*)((char*)m_world + 0x170));
        }
    }
}

// HYPOTHESIS: an agent-entry helper in hkLinkedCollidable.o (not written, no map name) that clears the collision
// information of one agent entry
extern "C" void fn_802FF3FC(hkAgentNnEntry* entry, const hkProcessCollisionInput& input);

void hkSimulation::collideEntitiesNarrowPhaseDiscrete(hkEntity** entities, int numEntities,
                                                      const hkProcessCollisionInput& input, int mode) {
    processAgentsOfEntities(entities, numEntities, input, &hkSimulation::processAgentCollideDiscrete, mode);
}

void hkSimulation::resetCollisionInformationForEntities(hkEntity** entities, int numEntities, hkWorld* world,
                                                        hkBool flag) {
    // HYPOTHESIS: the flag's meaning; a set flag skips the reset (hkWorldOperationUtil::removeEntityBP passes true)
    if (flag) {
        return;
    }
    // HYPOTHESIS: world +0x68 is the world's collision input
    processAgentsOfEntities(entities, numEntities, **(const hkProcessCollisionInput**)((char*)world + 0x68),
                            &hkSimulation::processAgentResetCollisionInformation, 0);
}

void hkSimulation::processAgentResetCollisionInformation(hkAgentNnEntry* entry, const hkProcessCollisionInput& input,
                                                         hkProcessCollisionOutput& output) {
    fn_802FF3FC(entry, input);
}

// Accessors (8 bytes each), emitted as out-of-line functions in this TU.
hkLinkedCollidable* hkEntity::getLinkedCollidable() {
    return &m_collidable;
}

hkMotion* hkRigidBody::getRigidMotion() {
    return getMotion();
}

hkTransform& hkRigidBody::getTransform() {
    return *(hkTransform*)((u8*)this + 0xB0);
}

const hkQuaternion& hkMotion::getRotation() const {
    return m_motionState.m_sweptTransform.m_rotation1;
}

const hkVector4& hkContactPoint::getSeparatingNormal() const {
    return m_normal;
}

const hkCollidable* hkWorldObject::getCollidable() const {
    return &m_collidable;
}

hkVector4& hkRigidBody::getCenterOfMassInWorld() {
    return *(hkVector4*)((u8*)this + 0x100);
}

hkReal hkMath::asin(hkReal r) {
    if ((hkReal)fabs(r) >= 1.0f) {
        return r > 0.0f ? 1.5707964f : -1.5707964f;
    }
    return (hkReal)::asin(r);
}

hkQuaternion::hkQuaternion(const hkVector4& axis, hkReal angle) {
    setAxisAngle(axis, angle);
}

void hkQuaternion::setInverseMul(const hkQuaternion& q0, const hkQuaternion& q1) {
    const hkVector4& a = q0.m_vec;
    const hkVector4& b = q1.m_vec;
    hkReal cx = b.y * a.z - b.z * a.y;
    hkReal cy = b.z * a.x - b.x * a.z;
    hkReal cz = b.x * a.y - b.y * a.x;
    hkReal x = cx + a.w * b.x - b.w * a.x;
    hkReal y = cy + a.w * b.y - b.w * a.y;
    hkReal z = cz + a.w * b.z - b.w * a.z;
    hkReal w = a.w * b.w + a.dot3(b);
    m_vec.set(x, y, z, w);
}

hkProcessCollisionOutput::hkProcessCollisionOutput() {
    m_toi.m_time = HK_REAL_MAX;
}
