#pragma once

#include <havok/hkBase.h>

struct hkSimulationIsland;
struct hkProcessCollisionInput;

// Agent helpers of the world (hkWorldAgentUtil.o). Only the members called from other units so far.
struct hkWorldAgentUtil {
    // Moves the agents of the island from the old step end to the new one (map name; hkDefaultWorldMaintenanceMgr
    // calls it when it moves the clock back). HYPOTHESIS: the parameter types and meaning.
    static void warpTime(hkSimulationIsland* island, hkReal oldTime, hkReal newTime, hkProcessCollisionInput& input);
};
