#pragma once

#include <ft/builder/ft_dol_types.h>
#include <so/so_array.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class AnmObjChrRes;
}
}

class soMotionAnimObjCacheModuleImpl {
public:
    virtual ~soMotionAnimObjCacheModuleImpl();
    soMotionAnimObjCacheModuleImpl(soArray<soMotionAnimObjCacheUnit<nw4r::g3d::AnmObjChrRes> >* units);
    bool setCacheAnimChr(s32 kind, nw4r::g3d::AnmObjChrRes* animObj, s32 cacheSize);
    s32 getCacheSize();
    void destroyAnimChrAll();

private:
    // HYPOTHESIS: inlined insertion-point search (no out-of-line symbol).
    s32 findCacheIndex(s32 kind) {
        if (kind < 0) {
            return -1;
        }
        s32 count = m_units->size();
        s32 index;
        for (index = 0; index < count; index++) {
            s32 storedKind = m_units->at(index).m_kind;
            if (storedKind > kind) {
                return index;
            }
        }
        return count;
    }
    soArray<soMotionAnimObjCacheUnit<nw4r::g3d::AnmObjChrRes> >* m_units;
};
static_assert(sizeof(soMotionAnimObjCacheModuleImpl) == 8, "Class is wrong size!");
