#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkConstraintData.h>
#include <havok/hkAgentNnMachine.h>

struct hkEntity;
struct hkWorld;
struct hkAction; // include/havok/hkAction.h; the island stores itself in the action at 0x0C
struct hkStatisticsCollector;
struct hkConstraintInstance;
struct hkStepInfo;

// The island's state bits at 0x26 (MWCC allocates bit fields from the top: m_stateA is 0xC0, m_stateB 0x30).
// HYPOTHESIS: the names. Setting m_stateA to 1 marks the island for a split check (internalRemoveEntity,
// hkWorldAgentUtil::removeAgent).
struct hkSimulationIslandFlags {
    unsigned char m_stateA : 2;
    unsigned char m_stateB : 2;
    unsigned char m_rest : 4;
};

// Simulation island (layout partly recovered; base and tail not recovered). Fields used by the island code:
// 0x26 bit field (bits 0xC0 and 0x30, written as flags by internalRemoveEntity/removeAction),
// 0x38 entity array (pointers), 0x60 action array (pointers).
struct hkSimulationIsland : hkReferencedObject {
    hkConstraintInfo m_constraintInfo; // 0x08 merged constraint info (see mergeConstraintInfo)
    hkWorld* m_world;                // 0x18 (hkWorldAgentUtil::removeAgentAndItsToiEvents reads the simulation off it)
    u8 m_unk1C[0x04];                // 0x1C (not recovered)
    u16 m_storageIndex;              // 0x20 index in the world's island list; 0xFFFF for the fixed island
                                     // HYPOTHESIS: name (hkWorldAgentUtil::addAgent never puts agents there)
    u8 m_unk22[0x04];                // 0x22 (not recovered; u16 dirty-list index at 0x22)
    union {
        u8 m_flags26;                    // 0x26 bit field, see above
        hkSimulationIslandFlags m_flags; // 0x26 the same byte as bit fields
    };
    u8 m_unk27[0x11];                // 0x27 (not recovered)
    hkArray<hkEntity*> m_entities;   // 0x38
    u8 m_unk44[0x04];                // 0x44 (not recovered)
    hkAgentNnTrack m_agentTrack;     // 0x48 the island's agent entries (hkSimulation::collideInternal processes it)
    hkArray<hkAction*> m_actions;    // 0x60

    hkBool isFixed() const { return m_storageIndex == 0xFFFF; }
    void internalAddEntity(hkEntity* entity);
    void internalRemoveEntity(hkEntity* entity);
    void addAction(hkAction* action);
    void removeAction(hkAction* action);
    void addConstraintToCriticalLockedIsland(hkConstraintInstance* constraint);
    void removeConstraintFromCriticalLockedIsland(hkConstraintInstance* constraint);
    void mergeConstraintInfo(const hkSimulationIsland* other);
    void markForWrite();
    // whether the island can go to sleep after this step (map name; hkDefaultWorldMaintenanceMgr asks it)
    // HYPOTHESIS: the parameter type
    hkBool shouldDeactivate(const hkStepInfo& stepInfo);
};
