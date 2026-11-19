#include <types.h>

// TODO: clean up, give variables real names
namespace jj_225910 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void fn_27_225A14(void*);
extern "C" void loadPositionData__11stPositionsFPQ34nw4r3g3d7ResFile(void*, void*);

extern "C" void* create__11stPositionsFQ34nw4r3g3d7ResFile(void** arg) {
    void* data;
    void* obj = __nw__FUlQ25Heaps8HeapType(0x64, 0xf);
    if (obj) {
        char* p = (char*)obj;
        *(void**)(p + 0) = 0;
        data = *arg;
        *(unsigned char*)(p + 4) = 1;
        *(void**)(p + 8) = data;
        *(int*)(p + 0x50) = 0;
        *(unsigned char*)(p + 0x62) = 0;
        *(void**)p = __nw__FUlQ25Heaps8HeapType(0xEC, 0xf);
        fn_27_225A14(obj);
        void* resFile = data;
        loadPositionData__11stPositionsFPQ34nw4r3g3d7ResFile(obj, &resFile);
    }
    return obj;
}
}

// TODO: clean up, give variables real names
namespace jj_2259A4 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void fn_27_225A14(void*);

extern "C" {
void* create__11stPositionsFv() {
    unsigned char* p = (unsigned char*)__nw__FUlQ25Heaps8HeapType(0x64, 0xf);
    if (p != 0) {
        *(unsigned int*)(p + 0x00) = 0;
        *(unsigned char*)(p + 0x04) = 0;
        *(unsigned int*)(p + 0x08) = 0;
        *(unsigned int*)(p + 0x50) = 0;
        *(unsigned char*)(p + 0x62) = 0;
        *(void**)(p + 0x00) = __nw__FUlQ25Heaps8HeapType(0xEC, 0xf);
        fn_27_225A14(p);
    }
    return p;
}
}
}

// TODO: clean up, give variables real names
namespace jj_22633C {
namespace nw4r {
namespace g3d {
class G3dObj {
public:
    void Destroy();
};
} // namespace g3d
} // namespace nw4r

struct Fn27Sub {
    u32 unk;
};

struct Fn27Big {
    u32 unk;
};

struct Fn27Obj {
    Fn27Sub* m_ptr0;
    u8 m_type;
    u8 pad5[0x0c - 0x05];
    nw4r::g3d::G3dObj* m_obj0;
    nw4r::g3d::G3dObj* m_obj1;
    u8 pad14[0x50 - 0x14];
    Fn27Big* m_ptr50;
};

extern "C" {
Fn27Obj* __dt__11stPositionsFv(Fn27Obj* self, s32 flag) {
    if (self) {
        if (self->m_ptr50) {
            delete self->m_ptr50;
        }
        if (self->m_type == 1) {
            if (self->m_obj0) {
                self->m_obj0->Destroy();
            }
            if (self->m_obj1) {
                self->m_obj1->Destroy();
            }
        }
        if (self->m_ptr0) {
            delete self->m_ptr0;
        }
        if (flag > 0) {
            delete self;
        }
    }
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_2263D8 {
struct Inner;

struct Sub {
    char padding[4];
    float a;
    float b;
};

struct GameGlobal {
    char padding[0x44];
    Sub* sub;
};

extern "C" {
extern GameGlobal* g_GameGlobal;
}

struct Inner {
    virtual void unused2();
    virtual void unused3(int, int, int);
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10(void*, float);
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual Inner* unused16(int);
};

struct Outer {
    char padding[0xC];
    Inner* inner;
};

extern "C" void update__11stPositionsFv(Outer* self) {
    if (self->inner) {
        Inner* t = self->inner->unused16(0);
        if (t) {
            Sub* s = g_GameGlobal->sub;
            t->unused10(s, s->a * s->b);
        }
        self->inner->unused3(1, 0, 0);
        self->inner->unused3(8, 0, 0);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_227B68 {
extern "C" void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
extern "C" void __dl__FPv(void*);
extern "C" void fn_27_225A14(void*);

extern "C" void fn_27_227B68(void* this_, u8 arg) {
    u8* self = (u8*)this_;
    if (self[4] == 1) {
        if (*(void**)(self + 0x0C))
            Destroy__Q34nw4r3g3d6G3dObjFv(*(void**)(self + 0x0C));
        if (*(void**)(self + 0x10))
            Destroy__Q34nw4r3g3d6G3dObjFv(*(void**)(self + 0x10));
    }
    if (*(void**)(self + 0x50))
        __dl__FPv(*(void**)(self + 0x50));
    fn_27_225A14(this_);
    self[0x62] = arg;
    self[0x61] = 1;
}
}

