// Havok translation unit hkSimulationIsland.o (main.dol 0x802E9BFC-0x802EA88C).
// Functions in address order:
//   0x802E9BFC   252  __ct
//   0x802E9CF8   260  __dt
//   0x802E9DFC   120  internalAddEntity
//   0x802E9E74   104  internalRemoveEntity
//   0x802E9EDC   796  calcStatistics
//   0x802EA1F8   112  addAction
//   0x802EA268   104  removeAction
//   0x802EA2D0   864  isFullyConnected
//   0x802EA630   476  shouldDeactivate
//   0x802EA80C    12  addConstraintToCriticalLockedIsland
//   0x802EA818    12  removeConstraintFromCriticalLockedIsland
//   0x802EA824    96  mergeConstraintInfo
//   0x802EA884     4  markForWrite
//   0x802EA888     4  checkAccessRw

#include <havok/hkSimulationIsland.h>
#include <havok/hkEntity.h>
#include <havok/hkEntityDeactivator.h>
#include <havok/hkStepInfo.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkWorld.h>
#include <havok/hkCollisionDispatcher.h>

// The agent track takes its sizes from the world's collision dispatcher. HYPOTHESIS: the meaning of the scalar
// fields (only their initial values are known).
hkSimulationIsland::hkSimulationIsland(hkWorld* world)
    : m_world(world), m_dirtyListIndex(0xFFFF), m_highFrequencyDeactivationCounter(0), m_lowFrequencyDeactivationCounter(0),
      m_timeSinceLastHighFrequencyCheck(0.0f), m_timeSinceLastLowFrequencyCheck(0.0f),
      m_agentTrack(world->m_collisionDispatcher->m_agentEntrySize, world->m_collisionDispatcher->m_agentSectorSize) {
    m_unk6C = -10.0f;
    m_flags.m_stateC = 0;
    m_constraintInfo.m_maxSizeOfJacobians = 0;
    m_constraintInfo.m_sizeOfJacobians = 0;
    m_constraintInfo.m_sizeOfSchemas = 0;
    m_constraintInfo.m_numSolverResults = 0;
    m_unk1C = 0;
}

// HYPOTHESIS: the destructor starts with a check that is empty in this build (a call of an empty inline
// function keeps the vtable store at the top of the destructor, as in the target; an empty body drops it).
hkSimulationIsland::~hkSimulationIsland() {
    hkConstraintOwner::checkAccessRw();
}

// Adds an entity to the island: the entity keeps the island pointer and its index in the island array.
void hkSimulationIsland::internalAddEntity(hkEntity* entity) {
    entity->m_simulationIsland = this;
    entity->m_storageIndex = m_entities.m_size;
    m_entities.pushBack(entity);
}

// Removes an entity by moving the last entity into its slot.
void hkSimulationIsland::internalRemoveEntity(hkEntity* entity) {
    m_entities[entity->m_storageIndex] = m_entities[m_entities.getSize() - 1];
    m_entities[entity->m_storageIndex]->m_storageIndex = entity->m_storageIndex;
    m_entities.popBack();
    entity->m_simulationIsland = 0;
    entity->m_storageIndex = 0xFFFF;
    // HYPOTHESIS: bits 0xC0 of 0x26 hold the island's state; "entity removed" sets it to 1 (a split check).
    m_flags.m_stateA = 1;
}

void hkSimulationIsland::addAction(hkAction* action) {
    m_actions.pushBack(action);
    *(hkSimulationIsland**)((char*)action + 0x0C) = this;
}

// Clears the matching action slot (nulls are compacted later) and the action's island pointer.

static int findActionIndex(const hkArray<hkAction*>& actions, hkAction* action) {
    for (int i = 0; i < actions.getSize(); i++) {
        if (actions[i] == action) {
            return i;
        }
    }
    return -1;
}

void hkSimulationIsland::removeAction(hkAction* action) {
    int index = findActionIndex(m_actions, action);
    m_actions[index] = 0;
    *(hkSimulationIsland**)((char*)action + 0x0C) = 0;
    // HYPOTHESIS: bit fields 0xC0 and 0x30 of 0x26, set as in internalRemoveEntity.
    hkSimulationIslandFlags* flags = &m_flags; // MATCH-ONLY: the bit-field view makes MWCC emit rlwimi
    flags->m_stateA = 1;
    flags->m_stateB = 1;
}

// Forwards to the world operation with the world taken from the constraint's owner (world at +0x08 of the
// owner pointer stored at +0x14). HYPOTHESIS: field meanings of the constraint record.
// Every 5th consecutive interval in which all entities' deactivators agree, the island asks to be deactivated.
hkBool hkSimulationIsland::shouldDeactivate(const hkStepInfo& stepInfo) {
    hkReal deltaTime = stepInfo.m_deltaTime;
    hkReal timeHigh = m_timeSinceLastHighFrequencyCheck + deltaTime;
    hkReal timeLow = m_timeSinceLastLowFrequencyCheck + deltaTime;
    m_timeSinceLastHighFrequencyCheck = timeHigh;
    m_timeSinceLastLowFrequencyCheck = timeLow;

    hkWorld* world = m_world;
    hkReal highPeriod = world->m_highFrequencyDeactivationPeriod;
    // MATCH-ONLY: an int flag narrowed in the test (as hkBool it is compared after the low-frequency check).
    int highFreqCheck = timeHigh > highPeriod;
    hkReal lowPeriod = world->m_lowFrequencyDeactivationPeriod;
    const hkBool lowFreqCheck = timeLow > lowPeriod;

    if ((signed char)highFreqCheck) {
        m_timeSinceLastHighFrequencyCheck -= highPeriod;
        hkBool deactivate = true;
        for (int i = 0; i < m_entities.getSize(); ++i) {
            hkEntity* entity = m_entities[i];
            if (entity->m_deactivator == NULL || !(bool)entity->m_deactivator->shouldDeactivateHighFrequency(entity)) {
                deactivate = false;
                continue;
            }
        }
        if (deactivate) {
            if (++m_highFrequencyDeactivationCounter >= 5) {
                m_highFrequencyDeactivationCounter = 0;
                return true;
            }
        } else {
            m_highFrequencyDeactivationCounter = 0;
        }
    } else if (lowFreqCheck) {
        m_timeSinceLastLowFrequencyCheck -= lowPeriod;
        hkBool deactivate = true;
        for (int i = 0; i < m_entities.getSize(); ++i) {
            hkEntity* entity = m_entities[i];
            if (entity->m_deactivator == NULL || !(bool)entity->m_deactivator->shouldDeactivateLowFrequency(entity)) {
                deactivate = false;
                continue;
            }
        }
        if (deactivate) {
            if (++m_lowFrequencyDeactivationCounter >= 5) {
                m_lowFrequencyDeactivationCounter = 0;
                return true;
            }
        } else {
            m_lowFrequencyDeactivationCounter = 0;
        }
    }
    return false;
}

void hkSimulationIsland::addConstraintToCriticalLockedIsland(hkConstraintInstance* constraint) {
    hkWorld* world = *(hkWorld**)(*(u8**)((u8*)constraint + 0x14) + 0x08);
    hkWorldOperationUtil::addConstraintToCriticalLockedIsland(world, constraint);
}

void hkSimulationIsland::removeConstraintFromCriticalLockedIsland(hkConstraintInstance* constraint) {
    hkWorld* world = *(hkWorld**)(*(u8**)((u8*)constraint + 0x14) + 0x08);
    hkWorldOperationUtil::removeConstraintFromCriticalLockedIsland(world, constraint);
}

// Merges the constraint info of another island into this one: the first word takes the larger of the two
// first words, then the larger of that and the other island's second word; the remaining three words are summed.
// HYPOTHESIS: field meanings of hkConstraintInfo.
void hkSimulationIsland::mergeConstraintInfo(const hkSimulationIsland* other) {
    m_constraintInfo.add(other->m_constraintInfo);
}

// No-op in this build (the target function body is a bare return).
void hkSimulationIsland::markForWrite() {
}

#include <havok/hkConstraintInstance.h>
#include <havok/hkConstraintInternal.h>

#include <havok/hkStatisticsCollector.h>

// hkAgentMachineUtil (carved into hkAgent1nMachine.cpp under their fn_ labels).
// TODO: clean up: name them in symbols.txt (calcNnStatisticsContactMgrsOnly / calcNnStatistics) and declare them in a
// header.
extern "C" void fn_802FECD0(const hkAgentNnTrack& track, hkStatisticsCollector* collector);
extern "C" void fn_802FEB48(const hkAgentNnTrack& track, hkStatisticsCollector* collector);

// The island's entities with their constraint arrays, its actions and the agent track.
void hkSimulationIsland::calcStatistics(hkStatisticsCollector* collector) const {
    collector->beginObject(0, 8, this);
    collector->addArrayStorage("EntityPtrs", 8, m_entities, sizeof(hkEntity*));
    for (int i = 0; i < m_entities.getSize(); i++) {
        hkEntity* entity = m_entities[i];
        collector->addChildObject("Entity", 2, entity);
        collector->pushDir("Constraints");
        collector->addArrayStorage("ConMstPtr", 8, entity->m_constraintsMaster, sizeof(hkConstraintInternal));
        collector->addArrayStorage("ConSlvPtr", 8, entity->m_constraintsSlave, sizeof(hkConstraintInstance*));
        for (int j = 0; j < entity->m_constraintsMaster.getSize(); j++) {
            // TODO: clean up: the first word of hkConstraintInternal is the constraint (name the member).
            hkConstraintInstance* constraint = (hkConstraintInstance*)entity->m_constraintsMaster[j].m_unk00;
            collector->addChildObject("ConInstance", 2, constraint);
            collector->addChildObject("ConData", 1,
                                      ((hkConstraintInstance*)entity->m_constraintsMaster[j].m_unk00)->m_data);
        }
        collector->addArrayStorage("Runtime", 4, entity->m_constraintRuntime, sizeof(u8));
        collector->popDir();
    }
    collector->addArrayStorage("ActionPtrs", 8, m_actions, sizeof(hkAction*));
    for (int k = 0; k < m_actions.getSize(); k++) {
        collector->addChildObject("Actions", 2, m_actions[k]);
    }
    fn_802FECD0(m_agentTrack, collector);
    collector->pushDir("CollAgents");
    fn_802FEB48(m_agentTrack, collector);
    collector->popDir();
    collector->endObject();
}
