// Havok translation unit hkAgentNnMachine.o (main.dol 0x802FEDA0-0x80300794): the agent track machine of the
// collidable pairs (names from the DebugMode map). Functions in address order:
//   0x802FEDA0   284  hkAgentNnMachine_GetAgentType
//   0x802FEEBC   604  hkAgentNnMachine_CreateAgent
//   0x802FF118   352  hkAgentNnMachine_DestroyAgent
//   0x802FF278   284  hkAgentNnMachine_InternalDeallocateEntry
//   0x802FF394   104  hkAgentNnMachine_UpdateShapeCollectionFilter
//   0x802FF3FC   160  hkAgentNnMachine_InvalidateTimInAgent
//   0x802FF49C   160  hkAgentNnMachine_WarpTimeInAgent
//   0x802FF53C   328  hkAgentNnMachine_AppendTrack
//   0x802FF684   256  hkAgentNnMachine_CopyAndRelinkAgentEntry
//   0x802FF784  1440  hkAgentNnMachine_ProcessAgent
//   0x802FFD24  1892  hkAgentNnMachine_ProcessTrack
//   0x80300488   108  hkAgentNnMachine_FindAgent
//   0x803004F4   672  hkArray<hkAgentNnSector*>::insertAt (weak)

#include <havok/hkAgentNnMachine.h>
#include <havok/hkCollisionDispatcher.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkLinkedCollidable.h>
#include <havok/hkAgent1nTrack.h>
#include <havok/hkThreadMemory.h>
#include <havok/hkString.h>

typedef void (*hkAgent3UpdateFilterFunc)(hkAgentNnEntry* entry, void* agentData, hkLinkedCollidable* collA,
                                         hkLinkedCollidable* collB, const hkCollisionInput& input);
typedef void (*hkAgent3InvalidateTimFunc)(hkAgentNnEntry* entry, void* agentData, const hkCollisionInput& input);
typedef void (*hkAgent3WarpTimeFunc)(hkAgentNnEntry* entry, void* agentData, hkReal oldTime, hkReal newTime,
                                     const hkCollisionInput& input);

// Offsets of the agent data behind an entry without and with time-of-impact information.
enum {
    HK_AGENT_DATA_OFFSET = 0x20,
    HK_AGENT_DATA_OFFSET_WITH_TIM = 0x30,
};

// Time of separating normal of an entry whose time-of-impact information is invalid.
static const hkReal HK_INVALID_TIM_TIME = -1.0f;

// The agent3 type for the pair (the predictive table when the input asks for predictive agents) and whether the
// pair has to be swapped for it: an agent registered for the reverse pair only, or a 2-agent bridge whose
// registration is symmetric.
void hkAgentNnMachine_GetAgentType(hkLinkedCollidable* collA, hkLinkedCollidable* collB,
                                   hkProcessCollisionInput& inputIn, int* agentTypeOut, int* isFlippedOut) {
    hkCollisionInput& input = inputIn;
    hkCollisionDispatcher* dis = input.m_dispatcher;
    int typeA = collA->getShape()->getType();
    int typeB = collB->getShape()->getType();
    int agentType;
    if (input.m_createPredictiveAgents) {
        agentType = ((u8(*)[32])dis->m_lookup1294)[typeA][typeB];
    } else {
        agentType = ((u8(*)[32])dis->m_lookupE94)[typeA][typeB];
    }
    int isFlipped = dis->m_agent3Func[agentType].unk30 == 2;
    if (agentType == 1) {
        isFlipped = dis->m_agentFuncs[((u8(*)[32])dis->m_lookup190)[typeA][typeB]].symmetricA;
    }
    *isFlippedOut = isFlipped;
    if (isFlipped) {
        if (input.m_createPredictiveAgents) {
            agentType = ((u8(*)[32])dis->m_lookup1294)[typeB][typeA];
        } else {
            agentType = ((u8(*)[32])dis->m_lookupE94)[typeB][typeA];
        }
    }
    *agentTypeOut = agentType;
}

extern "C" void fn_802FF278(hkAgentNnTrack& track, hkAgentNnEntry* entry);

typedef void (*hkAgent3DestroyFunc)(hkAgentNnEntry* entry, void* agentData, hkContactMgr* mgr,
                                    hkCollisionDispatcher* dispatcher);

typedef void* (*hkAgent3CreateFunc)(const hkAgent3Input& input, hkAgentNnEntry* entry, void* agentData);

// Appends a new entry for the pair to the track (opening a sector when the last one is full), links it into both
// collidables, picks the pair's collision quality and lets the agent3 registration build its agent data.
hkAgentNnEntry* hkAgentNnMachine_CreateAgent(hkAgentNnTrack& track, hkLinkedCollidable* collA,
                                             hkLinkedCollidable* collB, int agentType,
                                             hkProcessCollisionInput& input, hkContactMgr* mgr) {
    // MATCH-ONLY: the aligned local turns copy propagation off for the function (the target reloads
    // input.m_dispatcher and keeps its loads in source order).
    hkCollisionDispatcher* dis __attribute__((aligned(16))) = input.m_dispatcher;
    if ((u32)track.m_bytesUsedInLastSector == track.m_sectorSize) {
        hkAgentNnSector* sector = (hkAgentNnSector*)hkThreadMemory::s_instance->allocateChunk(track.m_sectorSize, 0x25);
        track.m_sectors.pushBack(sector);
        track.m_bytesUsedInLastSector = 0;
    }
    hkAgentNnEntry* entry =
        (hkAgentNnEntry*)((char*)track.getSector(track.getNumSectors() - 1) + track.m_bytesUsedInLastSector);
    track.m_bytesUsedInLastSector += track.m_agentSize;

    entry->m_contactMgr = mgr;
    entry->m_collidable[0] = collA;
    entry->m_collidable[1] = collB;
    entry->m_agentIndexOnCollidable[0] = collA->m_collisionEntries.m_size;
    entry->m_agentIndexOnCollidable[1] = collB->m_collisionEntries.m_size;
    hkLinkedCollidable::CollisionEntry& entryA = collA->m_collisionEntries.expandOne();
    hkLinkedCollidable::CollisionEntry& entryB = collB->m_collisionEntries.expandOne();
    entryA.m_agentEntry = entry;
    entryB.m_agentEntry = entry;
    entryA.m_partner = collB;
    entryB.m_partner = collA;

    hkAgent3Input in3;
    in3.m_bodyA = collA;
    in3.m_bodyB = collB;
    in3.m_contactMgr = entry->m_contactMgr;
    in3.m_input = &input;
    in3.m_aTb.setMulInverseMul(collA->getTransform(), collB->getTransform());

    s8 qualityIndex = input.m_dispatcher->m_collisionQualityIndex[collA->m_broadPhaseHandle.m_objectQualityType]
                                                                  [collB->m_broadPhaseHandle.m_objectQualityType];
    entry->m_collisionQualityIndex = qualityIndex;
    input.m_collisionQualityInfo =
        input.m_dispatcher->getCollisionQualityInfo(qualityIndex);
    entry->m_agentType = agentType;

    void* agentData;
    if (dis->m_agent3Func[agentType].m_sepNormalFunc != NULL) {
        entry->m_streamCommand = HK_AGENT_STREAM_CALL_WITH_TIM;
        hkAgentNnTimEntry* timEntry = (hkAgentNnTimEntry*)entry;
        timEntry->m_timeOfSeparatingNormal = HK_INVALID_TIM_TIME;
        timEntry->m_separatingNormal.setZero4();
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET_WITH_TIM);
    } else {
        entry->m_streamCommand = HK_AGENT_STREAM_CALL;
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET);
    }
    ((hkAgent3CreateFunc)dis->m_agent3Func[entry->m_agentType].m_createFunc)(in3, entry, agentData);
    entry->m_size = track.m_agentSize;
    return entry;
}

// Calls the agent's destroy function, unlinks the entry from both collidables (the last collision entry takes its
// place) and frees it. Not matched yet: the target dispatches the stream command through a jump table.
void hkAgentNnMachine_DestroyAgent(hkAgentNnTrack& track, hkAgentNnEntry* entry, hkCollisionDispatcher* dispatcher) {
    switch (entry->m_streamCommand) {
    case HK_AGENT_STREAM_CALL:
    case HK_AGENT_STREAM_CALL_AGENT:
        ((hkAgent3DestroyFunc)dispatcher->m_agent3Func[entry->m_agentType].m_destroyFunc)(
            entry, hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET), entry->m_contactMgr, dispatcher);
        break;
    case HK_AGENT_STREAM_CALL_WITH_TIM:
        ((hkAgent3DestroyFunc)dispatcher->m_agent3Func[entry->m_agentType].m_destroyFunc)(
            entry, hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET_WITH_TIM), entry->m_contactMgr, dispatcher);
        break;
    case HK_AGENT_STREAM_NULL:
    case HK_AGENT_STREAM_END:
    case HK_AGENT_STREAM_CALL_FLIP:
    case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
    default:
        break;
    }
    for (int i = 0; i < 2; i++) {
        hkLinkedCollidable* collidable = entry->m_collidable[i];
        int index = entry->m_agentIndexOnCollidable[i];
        collidable->m_collisionEntries.removeAt(index);
        if (index < collidable->m_collisionEntries.m_size) {
            hkAgentNnEntry* moved = collidable->m_collisionEntries[index].m_agentEntry;
            moved->m_agentIndexOnCollidable[moved->m_collidable[0] != collidable] = index;
        }
        if ((collidable->m_collisionEntries.m_size + 4) * 2 <= collidable->m_collisionEntries.getCapacity()) {
            hkArrayUtil::_reduce(&collidable->m_collisionEntries, sizeof(hkLinkedCollidable::CollisionEntry), 0, 0);
        }
    }
    fn_802FF278(track, entry);
}


// Points both collidables' collision entries at the entry (after it moved).
static inline void relinkEntry(hkAgentNnEntry* entry) {
    int indexA = entry->m_agentIndexOnCollidable[0];
    int indexB = entry->m_agentIndexOnCollidable[1];
    entry->m_collidable[0]->m_collisionEntries[indexA].m_agentEntry = entry;
    entry->m_collidable[1]->m_collisionEntries[indexB].m_agentEntry = entry;
}

// Frees the entry: the last entry of the track moves into its place and the last sector is released once empty
// (map name hkAgentNnMachine_InternalDeallocateEntry; keeps its label for hkWorldOperationUtil's callers).
extern "C" void fn_802FF278(hkAgentNnTrack& track, hkAgentNnEntry* entry) {
    hkAgentNnEntry* lastEntry = (hkAgentNnEntry*)((char*)track.getSector(track.getNumSectors() - 1) +
                                                  (track.m_bytesUsedInLastSector - track.m_agentSize));
    if (entry != lastEntry) {
        hkString::memCpy(entry, lastEntry, track.m_agentSize);
        relinkEntry(entry);
    }
    track.m_bytesUsedInLastSector -= track.m_agentSize;
    if (track.m_bytesUsedInLastSector == 0) {
        hkAgentNnSector* sector = track.getSector(track.m_sectors.m_size - 1);
        track.m_sectors.m_size = track.m_sectors.m_size - 1;
        if (!(track.m_sectors.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG)) {
            int n = track.m_sectors.m_size + 1;
            if (n < 1 || n * 2 < track.m_sectors.getCapacity()) {
                hkArrayUtil::_reduce(&track.m_sectors, sizeof(hkAgentNnSector*), track.m_sectors.m_storage, 1);
            }
        }
        hkThreadMemory::s_instance->deallocateChunk(sector, track.m_sectorSize, 0x25);
        track.m_bytesUsedInLastSector = track.m_sectorSize;
    }
}



// Each helper switches on a copy of the stream command (a jump table over all seven commands); only the agent
// commands reach the registration.
void hkAgentNnMachine_UpdateShapeCollectionFilter(hkAgentNnEntry* entry, const hkCollisionInput& input) {
    void* agentData;
    u8 command = entry->m_streamCommand;
    switch (command) {
    case HK_AGENT_STREAM_CALL:
    case HK_AGENT_STREAM_CALL_AGENT:
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET);
        break;
    case HK_AGENT_STREAM_CALL_WITH_TIM:
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET_WITH_TIM);
        break;
    case HK_AGENT_STREAM_NULL:
        return;
    case HK_AGENT_STREAM_END:
        return;
    case HK_AGENT_STREAM_CALL_FLIP:
        return;
    case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
        return;
    default:
        return;
    }
    hkAgent3UpdateFilterFunc func =
        (hkAgent3UpdateFilterFunc)input.m_dispatcher->m_agent3Func[entry->m_agentType].m_updateFilterFunc;
    if (func != NULL) {
        func(entry, agentData, entry->m_collidable[0], entry->m_collidable[1], input);
    }
}

void hkAgentNnMachine_InvalidateTimInAgent(hkAgentNnEntry* entry, const hkProcessCollisionInput& input) {
    void* agentData;
    u8 command = entry->m_streamCommand;
    switch (command) {
    case HK_AGENT_STREAM_CALL:
    case HK_AGENT_STREAM_CALL_AGENT:
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET);
        break;
    case HK_AGENT_STREAM_CALL_WITH_TIM: {
        hkAgentNnTimEntry* timEntry = (hkAgentNnTimEntry*)entry;
        timEntry->m_timeOfSeparatingNormal = HK_INVALID_TIM_TIME;
        timEntry->m_separatingNormal.setZero4();
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET_WITH_TIM);
        break;
    }
    case HK_AGENT_STREAM_NULL:
        return;
    case HK_AGENT_STREAM_END:
        return;
    case HK_AGENT_STREAM_CALL_FLIP:
        return;
    case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
        return;
    default:
        return;
    }
    hkAgent3InvalidateTimFunc func =
        (hkAgent3InvalidateTimFunc)input.m_dispatcher->m_agent3Func[entry->m_agentType].m_invalidateTimFunc;
    if (func != NULL) {
        func(entry, agentData, input);
    }
}

// An entry whose separating plane was computed at the old time keeps it for the new time; any other is invalidated.
void hkAgentNnMachine_WarpTimeInAgent(hkAgentNnEntry* entry, float oldTime, float newTime,
                                      const hkProcessCollisionInput& input) {
    void* agentData;
    u8 command = entry->m_streamCommand;
    switch (command) {
    case HK_AGENT_STREAM_CALL:
    case HK_AGENT_STREAM_CALL_AGENT:
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET);
        break;
    case HK_AGENT_STREAM_CALL_WITH_TIM: {
        hkAgentNnTimEntry* timEntry = (hkAgentNnTimEntry*)entry;
        if (timEntry->m_timeOfSeparatingNormal == oldTime) {
            timEntry->m_timeOfSeparatingNormal = newTime;
        } else {
            timEntry->m_timeOfSeparatingNormal = HK_INVALID_TIM_TIME;
        }
        agentData = hkAddByteOffset((void*)entry, HK_AGENT_DATA_OFFSET_WITH_TIM);
        break;
    }
    case HK_AGENT_STREAM_NULL:
        return;
    case HK_AGENT_STREAM_END:
        return;
    case HK_AGENT_STREAM_CALL_FLIP:
        return;
    case HK_AGENT_STREAM_CALL_WITH_TIM_FLIP:
        return;
    default:
        return;
    }
    hkAgent3WarpTimeFunc func =
        (hkAgent3WarpTimeFunc)input.m_dispatcher->m_agent3Func[entry->m_agentType].m_warpTimeFunc;
    if (func != NULL) {
        func(entry, agentData, oldTime, newTime, input);
    }
}

extern "C" hkAgentNnEntry* fn_802FF684(hkAgentNnTrack& track, hkAgentNnEntry* entry);

// Moves every agent entry of appendee to the end of track: entries are copied one by one until the last sector
// of track is full, then the remaining sectors are taken over whole.
void hkAgentNnMachine_AppendTrack(hkAgentNnTrack& track, hkAgentNnTrack& appendee) {
    while ((u32)track.m_bytesUsedInLastSector != track.m_sectorSize && appendee.m_sectors.m_size != 0) {
        hkAgentNnEntry* last = (hkAgentNnEntry*)((char*)appendee.getSector(appendee.getNumSectors() - 1) +
                                                 (appendee.m_bytesUsedInLastSector - appendee.m_agentSize));
        fn_802FF684(track, last);
        appendee.m_bytesUsedInLastSector -= appendee.m_agentSize;
        if (appendee.m_bytesUsedInLastSector == 0) {
            hkAgentNnSector* sector = appendee.getSector(appendee.m_sectors.m_size - 1);
            appendee.m_sectors.m_size = appendee.m_sectors.m_size - 1;
            if (!(appendee.m_sectors.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG)) {
                int n = appendee.m_sectors.m_size + 1;
                if (n < 1 || n * 2 < appendee.m_sectors.getCapacity()) {
                    hkArrayUtil::_reduce(&appendee.m_sectors, sizeof(hkAgentNnSector*), appendee.m_sectors.m_storage, 1);
                }
            }
            hkThreadMemory::s_instance->deallocateChunk(sector, appendee.m_sectorSize, 0x25);
            appendee.m_bytesUsedInLastSector = appendee.m_sectorSize;
        }
    }
    if (appendee.m_sectors.m_size != 0) {
        track.m_sectors.insertAt(track.m_sectors.m_size, appendee.m_sectors);
        track.m_bytesUsedInLastSector = appendee.m_bytesUsedInLastSector;
        appendee.m_sectors.m_size = 0;
        appendee.m_bytesUsedInLastSector = appendee.m_sectorSize;
    }
}
// Copies the entry to the end of the track and relinks it (map name hkAgentNnMachine_CopyAndRelinkAgentEntry; keeps
// its label for hkWorldOperationUtil's callers).
#pragma push
#pragma auto_inline off
extern "C" hkAgentNnEntry* fn_802FF684(hkAgentNnTrack& track, hkAgentNnEntry* entry) {
    if ((u32)track.m_bytesUsedInLastSector == track.m_sectorSize) {
        hkAgentNnSector* sector = (hkAgentNnSector*)hkThreadMemory::s_instance->allocateChunk(track.m_sectorSize, 0x25);
        track.m_sectors.pushBack(sector);
        track.m_bytesUsedInLastSector = 0;
    }
    hkAgentNnEntry* newEntry =
        (hkAgentNnEntry*)((char*)track.getSector(track.getNumSectors() - 1) + track.m_bytesUsedInLastSector);
    track.m_bytesUsedInLastSector += track.m_agentSize;
    hkString::memCpy(newEntry, entry, track.m_agentSize);
    relinkEntry(newEntry);
    return newEntry;
}
#pragma pop

// The agent entry of the pair, found in the collision entries of whichever collidable has fewer.
hkAgentNnEntry* hkAgentNnMachine_FindAgent(const hkLinkedCollidable* collA, const hkLinkedCollidable* collB) {
    const hkLinkedCollidable* scanned;
    const hkLinkedCollidable* partner;
    if (collA->m_collisionEntries.getSize() <= collB->m_collisionEntries.getSize()) {
        scanned = collA;
        partner = collB;
    } else {
        scanned = collB;
        partner = collA;
    }
    const hkArray<hkLinkedCollidable::CollisionEntry>& entries = scanned->m_collisionEntries;
    for (int i = 0; i < entries.getSize(); i++) {
        if (entries[i].m_partner == partner) {
            return entries[i].m_agentEntry;
        }
    }
    return NULL;
}
