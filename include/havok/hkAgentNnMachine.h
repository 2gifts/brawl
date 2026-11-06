#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>

struct hkContactMgr;
struct hkLinkedCollidable;
struct hkProcessCollisionInput;
struct hkProcessCollisionOutput;

// One agent entry of a collidable pair (layout from hkSimulation's agent loops).
// HYPOTHESIS: the member names; m_collisionQualityIndex indexes hkCollisionDispatcher::m_collisionQualityInfo.
struct hkAgentNnEntry {
    u8 unk0[0x03];                       // 0x00 not identified
    u8 m_size;                           // 0x03 bytes of this entry (the track walks add it to step to the next)
    u8 unk4[0x08 - 0x04];                // 0x04 not identified
    s8 m_collisionQualityIndex;          // 0x08
    u8 unk9[0x10 - 0x09];                // 0x09 not identified
    hkContactMgr* m_contactMgr;          // 0x10
    hkLinkedCollidable* m_collidable[2]; // 0x14

    // out of line in hkSimulation.o (map: hkAgentNnEntry::getCollidableA)
    hkLinkedCollidable* getCollidableA();
};

// A block of agent entries (opaque: the entries start at its address).
struct hkAgentNnSector;

// The agent entries of one simulation island (0x18 bytes, hkSimulationIsland 0x48). The agent entries are packed
// into sectors; every sector but the last is m_sectorSize bytes long (hkWorldAgentUtil::warpTime and
// getIslandFromAgentEntry walk them). HYPOTHESIS: the names, the 0x0C and 0x14 words, and m_sectors being an
// hkArray in the original (untyped here so the island's implicit constructors stay as they are).
struct hkAgentNnTrack {
    hkArrayBase m_sectors;               // 0x00 hkAgentNnSector* elements
    u8 unkC[0x10 - 0x0C];                // 0x0C not identified
    int m_bytesUsedInLastSector;         // 0x10
    u16 unk14;                           // 0x14 not identified
    u16 m_sectorSize;                    // 0x16

    int getNumSectors() const { return m_sectors.m_size; }
    hkAgentNnSector* getSector(int i) const { return ((hkAgentNnSector* const*)m_sectors.m_data)[i]; }
};

struct hkSimulationIsland;
struct hkCollisionDispatcher;

// Free functions of hkAgentNnMachine.o (names from the map). HYPOTHESIS: the parameter types beyond the call shape.
// Runs the entry's agent for one step and appends its contact points to the output.
void hkAgentNnMachine_ProcessAgent(hkAgentNnEntry* entry, const hkProcessCollisionInput& input,
                                   hkProcessCollisionOutput& output);
// Runs every agent entry of the track for one step (map name hkAgentNnMachine_ProcessTrack).
// HYPOTHESIS: the parameter types (the owner island, its track, the world's collision input).
void hkAgentNnMachine_ProcessTrack(hkSimulationIsland* island, hkAgentNnTrack& track,
                                   const hkProcessCollisionInput& input);
// Destroys the entry's agent and frees the entry in the track (map name). HYPOTHESIS: the parameter types.
void hkAgentNnMachine_DestroyAgent(hkAgentNnTrack& track, hkAgentNnEntry* entry, hkCollisionDispatcher* dispatcher);
// Clears the entry's time-of-impact information.
void hkAgentNnMachine_InvalidateTimInAgent(hkAgentNnEntry* entry, const hkProcessCollisionInput& input);
// Moves the entry's time-of-impact information from oldTime to newTime (map name).
// HYPOTHESIS: the parameter types (hkWorldAgentUtil::warpTime passes its own arguments through).
void hkAgentNnMachine_WarpTimeInAgent(hkAgentNnEntry* entry, float oldTime, float newTime,
                                      const hkProcessCollisionInput& input);
