#pragma once

#include <havok/hkBase.h>

struct hkContactMgr;
struct hkLinkedCollidable;
struct hkProcessCollisionInput;
struct hkProcessCollisionOutput;

// One agent entry of a collidable pair (layout from hkSimulation's agent loops).
// HYPOTHESIS: the member names; m_collisionQualityIndex indexes hkCollisionDispatcher::m_collisionQualityInfo.
struct hkAgentNnEntry {
    u8 unk0[0x08];                       // 0x00 not identified
    s8 m_collisionQualityIndex;          // 0x08
    u8 unk9[0x10 - 0x09];                // 0x09 not identified
    hkContactMgr* m_contactMgr;          // 0x10
    hkLinkedCollidable* m_collidable[2]; // 0x14

    // out of line in hkSimulation.o (map: hkAgentNnEntry::getCollidableA)
    hkLinkedCollidable* getCollidableA();
};

// The agent entries of one simulation island (0x18 bytes, hkSimulationIsland 0x48).
// HYPOTHESIS: the size (up to the island's action array); the contents are not recovered.
struct hkAgentNnTrack {
    u8 unk0[0x18];
};

struct hkSimulationIsland;

// Free functions of hkAgentNnMachine.o (names from the map). HYPOTHESIS: the parameter types beyond the call shape.
// Runs the entry's agent for one step and appends its contact points to the output.
void hkAgentNnMachine_ProcessAgent(hkAgentNnEntry* entry, const hkProcessCollisionInput& input,
                                   hkProcessCollisionOutput& output);
// Runs every agent entry of the track for one step (map name hkAgentNnMachine_ProcessTrack).
// HYPOTHESIS: the parameter types (the owner island, its track, the world's collision input).
void hkAgentNnMachine_ProcessTrack(hkSimulationIsland* island, hkAgentNnTrack& track,
                                   const hkProcessCollisionInput& input);
// Clears the entry's time-of-impact information.
void hkAgentNnMachine_InvalidateTimInAgent(hkAgentNnEntry* entry, const hkProcessCollisionInput& input);
