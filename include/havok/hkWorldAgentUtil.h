#pragma once

#include <havok/hkBase.h>

struct hkSimulationIsland;
struct hkProcessCollisionInput;
struct hkAgentNnEntry;
struct hkEntity;
struct hkLinkedCollidable;

// Agent helpers of the world (hkWorldAgentUtil.o, map names). HYPOTHESIS: the parameter types where noted.
struct hkWorldAgentUtil {
    // Merges the two entities' islands when needed and creates the agent of the pair in the island that keeps it.
    // HYPOTHESIS: the parameter types.
    static hkAgentNnEntry* addAgent(hkLinkedCollidable* collA, hkLinkedCollidable* collB,
                                    const hkProcessCollisionInput& input);
    // Destroys the agent entry and its contact manager.
    static void removeAgent(hkAgentNnEntry* entry);
    // Drops the simulation's pending time-of-impact events of the agent, then removes it.
    static void removeAgentAndItsToiEvents(hkAgentNnEntry* entry);
    // The one of the two islands whose agent track holds the entry: searches the island with fewer sectors.
    // removeAgent calls it out of line (MWCC's file-level inliner would splice it in without never_inline).
    static hkSimulationIsland* getIslandFromAgentEntry(hkAgentNnEntry* entry, hkSimulationIsland* candidateA,
                                                       hkSimulationIsland* candidateB) __attribute__((never_inline));
    // Clears the time-of-impact information of every agent of the entity. HYPOTHESIS: the parameter types.
    static void invalidateTim(hkEntity* entity, hkProcessCollisionInput& input);
    // Moves the agents of the island from the old step end to the new one (map name; hkDefaultWorldMaintenanceMgr
    // calls it when it moves the clock back). HYPOTHESIS: the parameter types and meaning.
    static void warpTime(hkSimulationIsland* island, hkReal oldTime, hkReal newTime, hkProcessCollisionInput& input);
};
