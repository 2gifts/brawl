#pragma once

#include <so/collision/so_collision_hit_part.h>
#include <so/so_array.h>
#include <types.h>

// MATCH-ONLY: the copy of a hit sphere to its simple form in blocks. The real form is `*simple = *hit` (the implicit
// soCollisionHitSphere assignment, see so/collision/templates/so_collision_hit_part.h), which needs the aggregate Vec3f
// copy (MT_VEC3F_IMPLICIT_ASSIGN), but the other Vec3f assignments of the units that use this helper (ashiba02, 03, 08)
// are float-wise.
static inline void grJungleCopyHit(soCollisionHitData::Simple* dst, soCollisionHitData* src) {
    u32* srcWords = reinterpret_cast<u32*>(src);
    u32* dstWords = reinterpret_cast<u32*>(dst);
    struct F3 { float x, y, z; };
    struct F6 { F3 a, b; };
    *reinterpret_cast<F6*>(dstWords) = *reinterpret_cast<F6*>(srcWords);
    dst->m_size = src->m_size;
    reinterpret_cast<u8*>(dst)[0x1C] = reinterpret_cast<u8*>(src)[0x1C];
}
