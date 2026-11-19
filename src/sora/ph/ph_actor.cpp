#include <sr/sr_common.h>
#include <types.h>
// Brawl physics wrapper translation unit ph_actor.o (main.dol 0x80099978-0x8009A048).
// Not yet decompiled. Functions in address order with their map names:
//   0x80099978    28  __ct
//   0x80099994   480  __dt
//   0x80099B74   836  createCapsuleShape
//   0x80099EB8    92  __dt
//   0x80099F14    92  __dt
//   0x80099F70    92  __dt
//   0x80099FCC   116  draw
//   0x8009A040     8  getContainer
#include <ph/ph_actor.h>

#include <ph/ph_shape.h>
#include <havok/hkShape.h>

phActor::phActor() {
    m_entries = NULL;
    m_count = 0;
    m_capacityAndFlags = 0x80000000;
    unkC = 0;
}

void phActor::draw() {
    for (int i = 0; i < m_count; i++) {
        m_entries[i].m_shape->draw();
    }
}

// Out-of-line in the target TU (shape base class method, returns no container).
hkShapeContainer* hkShape::getContainer() const {
    return NULL;
}

// TODO: clean up, give variables real names
namespace jj_80099EB8 {
class SoHeapAllocator {
public:
    virtual void unk00();
    virtual void unk04();
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void free(void* p, unsigned short type, int tag);
};

extern SoHeapAllocator* lbl_805A0EB8;

extern "C" {
// TODO: clean up, give variables real names
void* fn_80099EB8(void* arg0, int arg1) {
    if (arg0 != 0 && arg1 > 0) {
        lbl_805A0EB8->free(arg0, *(unsigned short*)((char*)arg0 + 4), 0x25);
    }
    return arg0;
}
}
}

namespace jj_80099F14 {
extern "C" {

class SoHeapAllocator {
public:
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void free(void* ptr, u16 type, s32 align);
};

extern SoHeapAllocator* lbl_805A0EB8;

struct S {
    void* vtable;
    u16 type;
};

// TODO: clean up, give variables real names
void* fn_80099F14(void* arg0, s32 arg1) {
    if (arg0 != 0 && arg1 > 0) {
        S* s = (S*)arg0;
        lbl_805A0EB8->free(arg0, s->type, 0x25);
    }
    return arg0;
}

}
}

// TODO: clean up, give variables real names
namespace jj_80099994 {

struct Elem {
    s32 key;
    void* data;
};

struct Vec;
inline void remove(Vec*, const s32&);

struct Vec {
    Elem* elems;
    s32 count;
    u32 flags;

    bool isOwned() const {
        return (flags & 0x80000000) == 0;
    }

    bool isLast(s32 found) const {
        return found == count - 1;
    }

    void clearElements() {
        for (s32 i = 0; i < count; i++)
            remove(this, elems[i].key);
    }
};

extern "C" void fn_80096048(void*, s32);
extern "C" void fn_8027EABC(void*, void*, u32, s32);
extern "C" void* lbl_805A0EC8;
extern "C" void __dl__FPv(void*);

inline int find(Vec* v, const s32& key) {
    int i;
    for (i = 0; i < v->count; i++) {
        if (key == v->elems[i].key)
            return i;
    }
    return -1;
}

inline void erase(Vec* v, s32 found) {
    if (found >= 0) {
        if (v->isLast(found)) {
            v->count--;
        } else {
            v->count--;
            v->elems[found] = v->elems[v->count];
        }
    }
}

// TODO: clean up, give variables real names
inline void remove(Vec* v, const s32& key) {
    s32 found = find(v, key);
    if (v->elems[found].data) {
        fn_80096048(v->elems[found].data, 1);
        v->elems[found].data = 0;
    }
    erase(v, found);
}

// TODO: clean up, give variables real names
inline void destroy(Vec* v) {
    if (v && v->isOwned())
        fn_8027EABC(lbl_805A0EC8, v->elems, v->flags << 3, 0x15);
}

// TODO: clean up, give variables real names
extern "C" Vec* fn_80099994(Vec* self, s32 destroyFlag) {
    s32 i;
    s32 found;
    Vec* v = self;
    s32 flag = destroyFlag;

    if (v) {
        s32 offset;
        for (i = 0, offset = 0; i < v->count; offset += sizeof(Elem), i++) {
            if (((Elem*)((u8*)v->elems + offset))->data) {
                fn_80096048(((Elem*)((u8*)v->elems + offset))->data, 1);
                ((Elem*)((u8*)v->elems + offset))->data = 0;
            }
        }

        v->clearElements();

        if (v->isOwned())
            fn_8027EABC(lbl_805A0EC8, v->elems, v->flags << 3, 0x15);

        v->elems = 0;
        v->count = 0;
        v->flags = (v->flags & 0x40000000) | 0x80000000;
        destroy(v);

        if (flag > 0)
            __dl__FPv(v);
    }

    return self;
}
}

