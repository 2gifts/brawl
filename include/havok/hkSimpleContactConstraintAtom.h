#pragma once

#include <havok/hkBase.h>
#include <havok/hkContactPoint.h>
#include <havok/hkContactPointMaterial.h>

// Contact data kept in the atom after the counts (read and updated by hkSimpleContactConstraintDataBuildJacobian).
// HYPOTHESIS: the member names; the offsets are asm-verified.
struct hkSimpleContactConstraintDataInfo {
    u16 m_flags;           // 0x08 bit 2 (value 4): the angular friction part is enabled
    u16 m_index;           // 0x0A axis index (0..2) used to build the friction directions
    hkReal m_data0C;       // 0x0C first word of the friction solver data (its address goes to init2dFriction)
    hkReal m_frictionRhs0; // 0x10 rhs of the first friction direction
    hkReal m_data14;       // 0x14
    hkReal m_frictionRhs1; // 0x18 rhs of the second friction direction
    hkReal m_data1C;       // 0x1C
    hkReal m_angularRhs;   // 0x20 rhs of the angular friction direction
    hkReal m_contactRadius; // 0x24 mean distance of the contact points from their centre

    // A fresh atom: no flags, friction axis 3 (none chosen yet), all solver data cleared.
    void init() {
        m_flags = 0;
        m_index = 3;
        // MATCH-ONLY: the seven solver words are cleared as integers (stw of a zero register).
        u32* data = (u32*)&m_data0C;
        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;
        data[4] = 0;
        data[5] = 0;
        data[6] = 0;
    }
};;

// Simple contact constraint atom. Contact points start at the first 16-byte aligned address after 0x37;
// the property records (0x14 bytes each) follow the reserved contact points (0x20 bytes each).
struct hkSimpleContactConstraintAtom {
    enum {
        TYPE_CONTACT = 0x16, // hkConstraintAtom type of the contact atom
    };

    u16 m_type;                     // 0x00 (hkConstraintAtom::TYPE_CONTACT)
    u16 m_sizeOfAllAtoms;           // 0x02 byte size of the atom with its points and properties
    u16 m_numContactPoints;         // 0x04
    u16 m_numReservedContactPoints; // 0x06
    hkSimpleContactConstraintDataInfo m_info; // 0x08
    u8 unk28[0x37 - 0x28];

    // Weak copies in hkSimpleContactConstraintInfo.o (its dont_inline region); inlined everywhere else.
    hkContactPoint* getContactPoints() {
        return (hkContactPoint*)(((u32)this + 0x37) & ~0xF);
    }
    hkContactPointProperties* getContactPointProperties() {
        return (hkContactPointProperties*)((u8*)getContactPoints() + (m_numReservedContactPoints << 5));
    }
    const hkContactPoint* getContactPoints() const {
        return (const hkContactPoint*)(((u32)this + 0x37) & ~0xF);
    }
    const hkContactPointProperties* getContactPointProperties() const {
        return (const hkContactPointProperties*)((const u8*)getContactPoints() + (m_numReservedContactPoints << 5));
    }
};

// Allocation and editing of contact atoms (the atom is replaced when its room changes).
struct hkSimpleContactConstraintAtomUtil {
    // BYTES: never inlined (expandOne and optimizeCapacity call the three out of line).
    static hkSimpleContactConstraintAtom* allocateAtom(int numReservedContactPoints) __attribute__((never_inline));
    static void copyContents(hkSimpleContactConstraintAtom* dst, const hkSimpleContactConstraintAtom* src)
        __attribute__((never_inline));
    static void deallocateAtom(hkSimpleContactConstraintAtom* atom) __attribute__((never_inline));
    static hkSimpleContactConstraintAtom* expandOne(hkSimpleContactConstraintAtom* atom);
    static void removeAtAndCopy(hkSimpleContactConstraintAtom* atom, int index);
    static hkSimpleContactConstraintAtom* optimizeCapacity(hkSimpleContactConstraintAtom* atom, int numFreeElemsLeft);
};
