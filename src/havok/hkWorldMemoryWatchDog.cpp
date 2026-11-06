// Havok translation unit split as hkWorldMemoryWatchDog.o (main.dol 0x802F0714-0x802F0B40); the map places these
// functions in hkDefaultWorldMaintenanceMgr. Functions in address order (names from the map):
//   0x802F0714    40  hkDefaultWorldMaintenanceMgr::__ct
//   0x802F073C    28  hkDefaultWorldMaintenanceMgr::init
//   0x802F0758   904  hkDefaultWorldMaintenanceMgr::performMaintenance
//   0x802F0AE0     4  hkSimulation::warpTime
//   0x802F0AE4    92  hkDefaultWorldMaintenanceMgr::__dt

#include <havok/hkWorldMaintenanceMgr.h>
#include <havok/hkWorld.h>
#include <havok/hkSimulation.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkEntity.h>
#include <havok/hkMotion.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkWorldOperationUtil.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkWorldAgentUtil.h>


hkDefaultWorldMaintenanceMgr::hkDefaultWorldMaintenanceMgr() {
    m_restartTime = 33.0f;
    m_resetTime = 63.0f;
}

void hkDefaultWorldMaintenanceMgr::init(hkWorld* world) {
    world->m_simulation->m_unk10 = m_restartTime;
    world->m_simulation->m_unk14 = m_restartTime;
}

// Splits the islands, moves the clock back when the step start reached m_resetTime (the step, the simulation's
// times, every body's swept transform, the agents and the simulation's own time-dependent state all move by the
// same amount), and marks the islands that should sleep inactive.
void hkDefaultWorldMaintenanceMgr::performMaintenance(hkWorld* world, hkStepInfo& stepInfo) {
    HK_TIMER_BEGIN_LIST("Maintenance", "Split");
    hkWorldOperationUtil::splitSimulationIslands(world);
    if (stepInfo.m_startTime >= m_resetTime) {
        HK_TIMER_SPLIT_LIST("ResetTime");
        hkStepInfo& newStep = world->m_dynamicsStepInfo.m_stepInfo;
        hkReal deltaTime = stepInfo.m_endTime - stepInfo.m_startTime;
        newStep.m_startTime = m_restartTime;
        newStep.m_endTime = m_restartTime + deltaTime;
        newStep.m_deltaTime = deltaTime;
        newStep.m_invDeltaTime = 1.0f / deltaTime;
        hkReal warpDeltaTime = newStep.m_startTime - stepInfo.m_startTime;
        world->m_simulation->m_unk10 = warpDeltaTime + world->m_simulation->m_unk10;
        world->m_simulation->m_unk14 = newStep.m_startTime;
        if (world->m_simulation->m_unk1C != -1.0f) {
            world->m_simulation->m_unk1C = warpDeltaTime + world->m_simulation->m_unk1C;
        }
        for (int i = 0; i < world->m_unk28.m_size; i++) {
            hkSimulationIsland* island = ((hkSimulationIsland**)world->m_unk28.m_data)[i];
            for (int j = 0; j < island->m_entities.getSize(); j++) {
                // the swept transform's start time
                island->m_entities[j]->getMotion()->m_motionState.m_sweptTransform.m_centerOfMass0.w += warpDeltaTime;
            }
        }
        for (int i = 0; i < world->m_unk28.m_size; i++) {
            hkWorldAgentUtil::warpTime(((hkSimulationIsland**)world->m_unk28.m_data)[i], stepInfo.m_endTime,
                                       newStep.m_endTime, *world->m_collisionInput);
        }
        world->m_simulation->warpTime(warpDeltaTime);
        stepInfo = newStep;
        world->m_collisionInput->m_stepInfo = newStep;
    }
    HK_TIMER_SPLIT_LIST("CheckDeact");
    if (world->m_unkB5) {
        for (int i = world->m_unk28.m_size - 1; i >= 0; i--) {
            hkSimulationIsland* island = ((hkSimulationIsland**)world->m_unk28.m_data)[i];
            if (island->shouldDeactivate(stepInfo)) {
                hkWorldOperationUtil::markIslandInactive(world, island);
            }
        }
    }
    HK_TIMER_END_LIST();
}

// Empty in this build (the continuous simulation overrides it).
void hkSimulation::warpTime(hkReal warpDeltaTime) {}
