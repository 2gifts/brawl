#pragma once

// Local BrawlHeaders shadow: the collision search data that soCollisionSearchModuleImpl::set copies in and getSize/setSize
// read and write.

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <types.h>

// HYPOTHESIS: the search sphere (its offset from the model node and its radius), the node and the flag bits; the meanings
// of the bits are not known. The copies the Bombhead's and the R.O.B. missile's searches make are member by member (a Vec3f,
// a float and two words). Ike's Quick Draw and Link's and Toon Link's final smashes fill it; the Bombhead and the missile
// set the 10-bit field at 0x10 to 2 when they start to search.
struct soCollisionSearchData {
    Vec3f m_offset;
    float m_size;        // 0xC, the radius
    u32 m_node : 9;      // 0x10
    u32 m_unk10a : 7;
    u32 m_unk10b : 10;
    u32 m_unk10c : 3;
    u32 m_unk10d : 1;
    u32 m_unk10e : 2;
    u32 m_unk14a : 4;    // 0x14
    u32 m_unk14b : 4;
    u32 m_unk14c : 1;
    u32 m_unk14d : 17;
    u32 m_unk14e : 1;
    u32 m_unk14f : 1;
    u32 m_unk14g : 4;
};
static_assert(sizeof(soCollisionSearchData) == 0x18, "Class is wrong size!");
