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

// TODO: clean up, give variables real names
namespace jj_21D70 {
class soVirtBase {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual bool vf3();
};

extern "C" {
bool fn_27_21D70(soVirtBase* p) {
    return !p->vf3();
}
}
}

// TODO: clean up, give variables real names
namespace jj_22A38 {
extern "C" unsigned char lbl_27_data_24C0[];
extern "C" void __dl__FPv(void*);

class Object {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
};

extern "C" Object* __dt__30soMotionAnimObjCacheModuleImplFv(Object* self, int flag) {
    if (self) {
        *(void**)self = lbl_27_data_24C0;
        self->v4();
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_22AA0 {
extern "C" s32 fn_27_22AA0(void) {
    return 0x0;
}
}

// TODO: clean up, give variables real names
namespace jj_22AA8 {
class SearchSource {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void* getAt(int index);
};

typedef int (*CompareFn)(void*, void*);

extern "C" int fn_27_22AA8(void* ctx, SearchSource* src, CompareFn cmp, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    int c = cmp(ctx, src->getAt(mid));
    if (c == 0) {
        return mid;
    }
    if (lo == hi) {
        return -(lo + 1);
    }
    if (c > 0) {
        return fn_27_22AA8(ctx, src, cmp, mid + 1, hi);
    }
    return fn_27_22AA8(ctx, src, cmp, lo, mid);
}
}

// TODO: clean up, give variables real names
namespace jj_23334 {
typedef int s32;

struct FnObj {
    s32 pad;
    s32 val;
};

extern "C" {
int fn_27_23334(const FnObj* a, const FnObj* b) {
    s32 x = a->val >> 16;
    s32 y = b->val >> 16;
    if (x == y) {
        return 0;
    }
    return x > y ? 1 : -1;
}
}
}

