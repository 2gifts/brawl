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
#include <havok/hkMonitorStream.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkWorldCallbackUtil.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCollisionAgent.h>
#include <havok/hkAgentNnMachine.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkPointerMapBase.h>

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

// Integrates one step of physicsDeltaTime. When neither the thread stack nor the heap has room for the
// integration, the memory system is flagged and the step stops with result 1 (integrate again).
// HYPOTHESIS: the stack test (the free stack minus 16 bytes) as an hkThreadMemory query, and the meaning of
// hkMemory::unk4 (set to 1 = out of memory; collide tests it after the narrow phase).
int hkSimulation::integrate(hkReal physicsDeltaTime) {
    m_unk18 = physicsDeltaTime;
    int memUsage = m_world->getMemUsageForIntegration();
    if (memUsage > hkThreadMemory::s_instance->getAvailableStackSize() && memUsage > hkMemory::getAvailableMemory()) {
        hkMemory::getInstance().unk4 = 1;
        m_unk24 = 1;
        return 1;
    }
    hkStepInfo stepInfo(m_unk14, m_unk14 + m_unk18);
    m_world->m_dynamicsStepInfo = stepInfo;
    m_world->m_collisionInput->m_stepInfo = stepInfo;
    m_world->m_maintenanceMgr->performMaintenance(m_world, stepInfo);
    integrateInternal(stepInfo);
    m_unk0C = 0;
    m_unk24 = 0;
    return 0;
}

// Collides the step integrate prepared (a failed earlier step goes through reCollideAfterStepFailure), then moves
// the step start on and runs the post-collide callbacks. Running out of memory stops with result 2.
int hkSimulation::collide() {
    if (m_unk24 != 0) {
        return reCollideAfterStepFailure();
    }
    hkStepInfo stepInfo(m_unk14, m_unk14 + m_unk18);
    collideInternal(stepInfo);
    if (hkMemory::getInstance().unk4 == 1) {
        m_unk24 = 2;
        return 2;
    }
    m_unk14 += m_unk18;
    if (m_world->m_postCollideListeners.m_size != 0) {
        HK_TIMER_BEGIN("PostCollideCB");
        hkWorldCallbackUtil::firePostCollideCallback(m_world, &stepInfo);
        HK_TIMER_END();
    }
    m_unk0C = 1;
    m_unk24 = 0;
    return 0;
}

// Collides again after a failed step: the filters are rerun over the whole world, then the failed step (after a
// failed time advance: the span from the time advanced to up to the step start) is collided.
// HYPOTHESIS: the two arguments of updateCollisionFilterOnWorld (0 and 1).
int hkSimulation::reCollideAfterStepFailure() {
    m_world->updateCollisionFilterOnWorld(0, 1);
    hkStepInfo stepInfo;
    if (m_unk24 == 3) {
        stepInfo.set(m_unk10, m_unk14);
    } else {
        stepInfo.set(m_unk14, m_unk14 + m_unk18);
    }
    collideInternal(stepInfo);
    if (hkMemory::getInstance().unk4 == 1) {
        HK_TIMER_END();
        return m_unk24;
    }
    if (m_unk24 == 2) {
        m_unk14 += m_unk18;
        if (m_world->m_postCollideListeners.m_size != 0) {
            HK_TIMER_BEGIN("PostCollideCB");
            hkWorldCallbackUtil::firePostCollideCallback(m_world, &stepInfo);
            HK_TIMER_END();
        }
        m_unk0C = 1;
    }
    m_unk24 = 0;
    return 0;
}

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

// Advances the time to the snapped step end; once it reaches the frame marker at 0x1C the post-simulation
// callbacks run.
int hkSimulation::advanceTime() {
    m_unk10 = snapSimulateTimeAndGetTimeToAdvanceTo();
    if (m_unk10 >= m_unk1C) {
        if (m_world->m_postSimulationListeners.m_size != 0) {
            HK_TIMER_BEGIN("PostSimulateCb");
            hkWorldCallbackUtil::firePostSimulationCallback(m_world);
            HK_TIMER_END();
        }
    }
    m_unk24 = 0;
    return 0;
}

// One simulation step: integrate and collide inside the "Simulate" timer, then advance the time. Each part runs
// only in the step states that need it (m_unk24 is the previous step's result).
// HYPOTHESIS: the meaning of the state values (0 = done, 1 = integrate pending, 2 = collide pending, 3 = advance
// pending), inferred from which parts each value runs.
int hkSimulation::stepDeltaTime(hkReal physicsDeltaTime) {
    {
        HK_TIME_CODE_BLOCK("Simulate");
        if (m_unk24 == 0 || m_unk24 == 1) {
            integrate(physicsDeltaTime);
        }
        if (m_unk24 == 0 || m_unk24 == 2) {
            collide();
        }
    }
    if (m_unk24 == 0 || m_unk24 == 3) {
        advanceTime();
    }
    return m_unk24;
}

// Lets every action of every active island act for this step.
// HYPOTHESIS: world +0x28 holds the active islands.
void hkSimulation::applyActions() {
    for (int i = 0; i < m_world->m_unk28.m_size; i++) {
        hkSimulationIsland* island = ((hkSimulationIsland**)m_world->m_unk28.m_data)[i];
        hkArray<hkAction*>& actions = island->m_actions;
        for (int j = 0; j < actions.getSize(); j++) {
            actions[j]->applyAction(m_world->m_dynamicsStepInfo);
        }
    }
}

// Collides the entities over the given step: the step goes into the world's collision input, then the broad phase
// and the narrow phase run with the world's critical operations locked. The step is written into the input again
// from a local copy at the end.
// HYPOTHESIS: why it is written again (something in between, e.g. a pending operation run by the unlock, may change
// the input's step).
void hkSimulation::collideEntitiesDiscrete(hkEntity** entities, int numEntities, hkWorld* world,
                                           const hkStepInfo& stepInfo, int mode) {
    hkProcessCollisionInput* input = world->m_collisionInput;
    hkStepInfo savedStepInfo = stepInfo;
    input->m_stepInfo = stepInfo;
    world->m_lockCount++;
    collideEntitiesBroadPhaseDiscrete(entities, numEntities, world);
    collideEntitiesNarrowPhaseDiscrete(entities, numEntities, *input, mode);
    world->unlockAndAttemptToExecutePendingOperations();
    input->m_stepInfo = savedStepInfo;
}

void hkSimulation::collideEntitiesNarrowPhaseDiscrete(hkEntity** entities, int numEntities,
                                                      hkProcessCollisionInput& input, int mode) {
    processAgentsOfEntities(entities, numEntities, input, &hkSimulation::processAgentCollideDiscrete, mode);
}

// Runs one agent entry for the step and hands its new contact points to the entry's contact manager. The input
// takes the pair's quality settings first; when the step ran the heap dry the memory system is flagged and the
// points are dropped.
// HYPOTHESIS: the fixed table entry 1 for the input's quality pointer (as the asm does it).
void hkSimulation::processAgentCollideDiscrete(hkAgentNnEntry* entry, hkProcessCollisionInput& input,
                                               hkProcessCollisionOutput& output) {
    output.reset();
    input.m_collisionQualityInfo = &input.m_dispatcher->m_collisionQualityInfo[1];
    input.m_createPredictiveAgents = input.m_dispatcher->m_collisionQualityInfo[entry->m_collisionQualityIndex].unk10;
    hkAgentNnMachine_ProcessAgent(entry, input, output);
    if (hkMemory::getAvailableMemory() == 0) {
        hkMemory::getInstance().unk4 = 1;
    }
    if (hkMemory::getInstance().unk4 != 1) {
        if (output.isEmpty() == false) {
            entry->m_contactMgr->processContact(*entry->m_collidable[0], *entry->m_collidable[1], input, output);
        }
    }
}

hkLinkedCollidable* hkAgentNnEntry::getCollidableA() {
    return m_collidable[0];
}

void hkSimulation::resetCollisionInformationForEntities(hkEntity** entities, int numEntities, hkWorld* world,
                                                        hkBool flag) {
    // HYPOTHESIS: the flag's meaning; a set flag skips the reset (hkWorldOperationUtil::removeEntityBP passes true)
    if (flag) {
        return;
    }
    processAgentsOfEntities(entities, numEntities, *world->m_collisionInput,
                            &hkSimulation::processAgentResetCollisionInformation, 0);
}

void hkSimulation::processAgentResetCollisionInformation(hkAgentNnEntry* entry, hkProcessCollisionInput& input,
                                                         hkProcessCollisionOutput& output) {
    hkAgentNnMachine_InvalidateTimInAgent(entry, input);
}

// Runs func once on every agent entry of the entities' collidables; a pair whose partner was one of the entities
// already done is skipped, so each pair between two of the entities runs once. In mode 1 an entry that comes back
// with a contact point is collided again with the body turned about the axis between the contact normal and the
// arm from the contact to the centre of mass (forward, then back with the inverse turn, with a 20 times larger
// tolerance), and finally once more at the original transform. Stops early when the step ran the heap dry.
// HYPOTHESIS: the meaning of mode 1 and of the turned collisions (they look like a probe that refreshes the
// contacts' manifold), and the names of the quality settings (0x04/0x08) the larger tolerance goes into.
// BYTES: the target calls every inline helper out of line in this function only.
// HYPOTHESIS: m_collisionEntries is an hkArray<CollisionEntry> in the original (see hkLinkedCollidable.h); the
// accesses below go through this typed view.
typedef hkArray<hkLinkedCollidable::CollisionEntry> CollisionEntryArray;

#pragma push
#pragma dont_inline on
void hkSimulation::processAgentsOfEntities(hkEntity** entities, int numEntities, hkProcessCollisionInput& input,
                                           AgentEntryProcessFunction func, int mode) {
    hkPointerMapBase<hkUlong> entitiesDone;
    entitiesDone.reserve(numEntities);
    hkProcessCollisionOutput output;
    for (int i = 0; i < numEntities; i++) {
        hkEntity* entity = entities[i];
        hkLinkedCollidable* collidable = entity->getLinkedCollidable();
        hkRigidBody* body = static_cast<hkRigidBody*>(entity);
        entitiesDone.insert((hkUlong)collidable, 0);
        for (int j = 0; j < ((const CollisionEntryArray&)collidable->m_collisionEntries).getSize(); j++) {
            const hkLinkedCollidable::CollisionEntry& collisionEntry =
                ((const CollisionEntryArray&)collidable->m_collisionEntries)[j];
            if (entitiesDone.isValid(entitiesDone.findKey((hkUlong)collisionEntry.m_partner))) {
                continue;
            }
            hkAgentNnEntry* entry = collisionEntry.m_agentEntry;
            hkAgentNnMachine_InvalidateTimInAgent(entry, input);
            (this->*func)(entry, input, output);
            if (mode == 1 && output.m_firstFreeContactPoint != output.m_contactPoints) {
                hkQuaternion oldRotation(body->getRigidMotion()->getRotation());
                hkTransform oldTransform(body->getTransform());
                hkVector4 normal(output.m_contactPoints[0].m_contact.getSeparatingNormal());
                if (collisionEntry.m_agentEntry->getCollidableA() != body->getCollidable()) {
                    normal.setNeg4(normal);
                }
                hkVector4 arm;
                const hkContactPoint& contact = output.m_contactPoints[0].m_contact;
                arm.setSub4(body->getCenterOfMassInWorld(), contact.getPosition());
                hkVector4 axis;
                axis.setCross(normal, arm);
                hkReal sinAngle = axis.length3();
                hkReal projected = normal.dot3(arm);
                hkReal armLength = arm.length3();
                hkReal cosAngle = projected / armLength;
                if (sinAngle > 1e-5f && cosAngle < 0.9999f) {
                    hkReal savedTolerance = input.unk8;
                    hkReal tolerance = 20.0f * savedTolerance;
                    input.unk8 = tolerance;
                    hkReal savedQuality4 = input.m_collisionQualityInfo->unk0[1];
                    hkReal savedQuality8 = input.m_collisionQualityInfo->unk0[2];
                    input.m_collisionQualityInfo->unk0[1] = tolerance;
                    input.m_collisionQualityInfo->unk0[2] = tolerance;
                    hkReal limitAngle = hkMath::asin(hkMath::max2f((projected - 0.05f * tolerance) / armLength, 0.0f));
                    hkReal angle = hkMath::asin(cosAngle) - limitAngle;
                    axis.mul4(1.0f / sinAngle);
                    hkQuaternion turn(axis, angle);
                    hkQuaternion rotation;
                    rotation.setMul(turn, body->getRigidMotion()->getRotation());
                    body->setRotationAroundCentreOfMass(rotation);
                    hkAgentNnMachine_InvalidateTimInAgent(entry, input);
                    (this->*func)(entry, input, output);

                    hkVector4 axis2;
                    axis2.setCross(axis, normal);
                    axis2.normalize3();
                    hkQuaternion turn2(axis2, angle);
                    hkQuaternion rotation2;
                    rotation2.setMul(turn2, oldRotation);
                    body->setRotationAroundCentreOfMass(rotation2);
                    hkAgentNnMachine_InvalidateTimInAgent(entry, input);
                    (this->*func)(entry, input, output);

                    rotation2.setInverseMul(turn2, oldRotation);
                    body->setRotationAroundCentreOfMass(rotation2);
                    hkAgentNnMachine_InvalidateTimInAgent(entry, input);
                    (this->*func)(entry, input, output);

                    input.unk8 = savedTolerance;
                    input.m_collisionQualityInfo->unk0[1] = savedQuality4;
                    input.m_collisionQualityInfo->unk0[2] = savedQuality8;
                    body->getRigidMotion()->setTransform(oldTransform);
                    hkAgentNnMachine_InvalidateTimInAgent(entry, input);
                    (this->*func)(entry, input, output);
                }
            }
            if (hkMemory::getInstance().unk4 == 1) {
                return;
            }
        }
    }
}
#pragma pop

// Accessors (8 bytes each), emitted as out-of-line functions in this TU.
hkLinkedCollidable* hkEntity::getLinkedCollidable() {
    return &m_collidable;
}

hkMotion* hkRigidBody::getRigidMotion() {
    return getMotion();
}

hkTransform& hkRigidBody::getTransform() {
    return getMotion()->m_motionState.m_transform;
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
    return getMotion()->m_motionState.m_sweptTransform.m_centerOfMass1;
}

const hkVector4& hkContactPoint::getPosition() const {
    return m_position;
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

// The translation is moved so that the centre of mass stays put: translation = centre of mass - R * local centre.
void hkRigidBody::setRotationAroundCentreOfMass(const hkQuaternion& newRotation) {
    hkTransform& trans = getMotion()->m_motionState.m_transform;
    trans.m_rotation.set(newRotation);
    hkVector4 centerShift;
    centerShift._setRotatedDir(trans.m_rotation, getMotion()->m_motionState.m_sweptTransform.m_centerOfMassLocal);
    trans.m_translation.setSub4(getMotion()->m_motionState.m_sweptTransform.m_centerOfMass1, centerShift);
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
