#pragma once

#include <havok/hkBase.h>
#include <havok/hkMemory.h>

struct hkConstraintData;
struct hkModifierConstraintAtom;
struct hkEntity;

// A constraint between two entities (0x2C bytes). Layout and names from main.dol's hkConstraintInstanceClass;
// the owner and internal pointers are runtime-only (not serialized). Only the methods other units call are
// declared.
struct hkConstraintInstance : hkReferencedObject {
    enum ConstraintPriority {
        PRIORITY_INVALID = 0,
        PRIORITY_PSI = 1,
        PRIORITY_TOI = 2,
        PRIORITY_TOI_HIGHER = 3,
        PRIORITY_TOI_FORCED = 4
    };
    enum InstanceType {
        TYPE_NORMAL = 0,
        TYPE_CHAIN = 1
    };
    enum AddReferences {
        DO_NOT_ADD_REFERENCES = 0,
        DO_ADD_REFERENCES = 1
    };

    void* m_owner;                                   // 0x08
    hkConstraintData* m_data;                        // 0x0C
    hkModifierConstraintAtom* m_constraintModifiers; // 0x10
    hkEntity* m_entities[2];                         // 0x14
    s8 m_priority;                                   // 0x1C (ConstraintPriority)
    hkBool m_wantRuntime;                            // 0x1D
    const char* m_name;                              // 0x20
    u32 m_userData;                                  // 0x24
    void* m_internal;                                // 0x28

    // Freed as class 0x2A (the deleting destructor).
    HK_DECLARE_REF_ALLOCATOR(0x2a)
    static void* operator new(unsigned long, void* p) { return p; } // the loaded-object constructors

    hkConstraintInstance(ConstraintPriority priority);
    hkConstraintInstance(hkFinishLoadedObjectFlag flag) {}
    virtual ~hkConstraintInstance();
    // calcStatistics                                          0x0C (inherited)
    virtual void entityAddedCallback(hkEntity* entity);   // 0x10
    virtual void entityRemovedCallback(hkEntity* entity); // 0x14 (detaches the constraint, see fireEntityRemoved)
    virtual void entityDeletedCallback(hkEntity* entity); // 0x18
    virtual InstanceType getType() const;                 // 0x1C (TYPE_CHAIN for hkConstraintChainInstance)
    void setPriority(ConstraintPriority priority);
    void pointNullsToFixedRigidBody();

    // Loaded-object support (static init registration, see src/havok/hkConstraintInstance_part0.cpp).
    static void finishLoadedObjecthkConstraintInstance(void* p);
    static void cleanupLoadedObjecthkConstraintInstance(void* p);
    static const void* getVtablehkConstraintInstance();

    hkConstraintData* getData() const { return m_data; }
    hkEntity* getEntityA() const { return m_entities[0]; }
    hkEntity* getEntityB() const { return m_entities[1]; }
};
