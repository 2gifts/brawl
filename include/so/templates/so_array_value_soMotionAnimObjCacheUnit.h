#pragma once

#include <StaticAssert.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class AnmObjChrRes;
}
}  // namespace nw4r

// Local copy with the default constructor the sora_melee array instantiations rely on; the plain view is in
// include/ft/builder/ft_dol_types.h. SDK unchanged.
template <typename T>
struct soMotionAnimObjCacheUnit {
    T* m_animObj;
    s32 m_kind : 16;
    s32 m_size : 16;
    soMotionAnimObjCacheUnit() : m_animObj(0) {
        m_kind = -1;
        m_size = -1;
    }
};
static_assert(sizeof(soMotionAnimObjCacheUnit<void*>) == 8, "Class is wrong size!");
