#include <nw4r/g3d/g3d_anmchr.h>
#include <so/motion/so_motion_anim_obj_cache_module_impl.h>
#include <so/so_array.h>

bool soMotionAnimObjCacheModuleImpl::setCacheAnimChr(s32 kind, nw4r::g3d::AnmObjChrRes* animObj, s32 cacheSize) {
    if (m_units->isFull() == true) {
        return false;
    }
    s32 index = findCacheIndex(kind);
    if (index < 0) {
        return false;
    }
    soMotionAnimObjCacheUnit<nw4r::g3d::AnmObjChrRes> unit;
    unit.m_animObj = animObj;
    unit.m_kind = kind;
    unit.m_size = cacheSize;
    m_units->insert(index, unit);
    return true;
}

s32 soMotionAnimObjCacheModuleImpl::getCacheSize() {
    s32 total = 0;
    for (s32 index = m_units->size() - 1; index >= 0; index--) {
        total += m_units->at(index).m_size;
    }
    return total;
}

void soMotionAnimObjCacheModuleImpl::destroyAnimChrAll() {
    for (s32 index = m_units->size() - 1; index >= 0; index--) {
        nw4r::g3d::AnmObjChrRes* animObj = m_units->at(index).m_animObj;
        if (animObj != NULL) {
            animObj->Destroy();
        }
    }
    m_units->clear();
}
