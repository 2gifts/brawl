#include <types.h>

// HYPOTHESIS: a pair that the figure menus keep two of in every unit (an id that is 0xFF while it is not used, and a
// state); the pairs are made when the unit starts.
struct muFigPair {
    int m_id;
    int m_state;

    muFigPair(int id, int state) : m_id(id), m_state(state) { }
};

static muFigPair s_pair0(0xFF, 0);
static muFigPair s_pair1(0xFF, 1);

#include <cstdio>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gm/gm_global_mode_melee.h>
// TODO: clean up, give variables real names
namespace jj_348 {
extern "C" void fn_800B96E8(void* p, int v);

extern "C" void* __ct__16muMenuControllerFv(void* p) {
    fn_800B96E8(p, 0xff);
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_37C {
extern "C" {
void __dl__FPv(void*);

void* __dt__16muMenuControllerFv(void* obj, int deleting) {
    if (obj && deleting > 0) {
        __dl__FPv(obj);
    }
    return obj;
}
}
}

// TODO: clean up, give variables real names
namespace jj_3BC {
extern "C" {
void fn_2_3BC(int* arg0);
}

void fn_2_3BC(int* arg0) {
    *arg0 = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_2628 {
extern "C" {
void setUseCameraAnim__11gfSceneRootFb(void*, int);
void removeResAnmScn__11gfSceneRootFv(void*);
void removeInfoCamResAnm__11gfSceneRootFv(void*);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void* getBuffer__14gfFileIOHandleCFv(void*);
void free__13gfHeapManagerFPv(void*);
void release__14gfFileIOHandleFv(void*);
void* updateCamera__15tyFigListFigureFv(int, int);
void* getInstance__9itManagerFv(void*);
void __dt__20tyFigListDataManagerFv(void*, int);
void __dt__5MuMsgFv(void*, int);
void exit__6gfTaskFv(void*);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__14gfFileIOHandleFv(void*, int);
void __dt__16muMenuControllerFv(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
extern char lbl_2_data_720[];
extern void* g_gfSceneRoot;
}

class gfSlotNode {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12(int);
    char pad[0xE0];
    int count;
};

class gfPad5CProbe {
public:
    char pad[0x5C];
};

class gfVCall2 : public gfPad5CProbe {
public:
    virtual void f(int);
};

extern "C" {

void* fn_2_2628(void* self, int arg1) {
    char* s = (char*)self;

    if (self != 0) {
        char* q;

        *(void**)(s + 0x3C) = lbl_2_data_720;

        {
            gfSlotNode* o = (gfSlotNode*)*(void**)((char*)g_gfSceneRoot + 0x450);
            while (o->count != 0) {
                if (o->count != 0) {
                    o->v12(o->count - 1);
                }
            }
        }

        {
            gfSlotNode* o = (gfSlotNode*)*(void**)((char*)g_gfSceneRoot + 0x3C0);
            while (o->count != 0) {
                if (o->count != 0) {
                    o->v12(o->count - 1);
                }
            }
        }

        {
            gfSlotNode* o = (gfSlotNode*)*(void**)((char*)g_gfSceneRoot + 0x390);
            while (o->count != 0) {
                if (o->count != 0) {
                    o->v12(o->count - 1);
                }
            }
        }

        setUseCameraAnim__11gfSceneRootFb(g_gfSceneRoot, 0);
        removeResAnmScn__11gfSceneRootFv(g_gfSceneRoot);
        removeInfoCamResAnm__11gfSceneRootFv(g_gfSceneRoot);

        {
            char* p = s;
            unsigned int i = 0;
            do {
                void* obj = *(void**)(p + 0x5E4);
                if (obj != 0) {
                    Destroy__Q34nw4r3g3d6G3dObjFv(obj);
                    *(void**)(p + 0x5E4) = 0;
                }
                i++;
                p += 4;
            } while (i < 2);
        }

        {
            unsigned int i = 0;
            char* p = s;
            do {
                void* obj = *(void**)(p + 0x5B4);
                if (obj != 0) {
                    if (obj != 0) {
                        ((gfVCall2*)obj)->f(1);
                    }
                    *(void**)(p + 0x5B4) = 0;
                }
                i++;
                p += 4;
            } while (i < 0xC);
        }

        {
            unsigned int i = 0;
            q = s + 0x624;
            do {
                void* b = getBuffer__14gfFileIOHandleCFv(q);
                free__13gfHeapManagerFPv(b);
                release__14gfFileIOHandleFv(q);
                i++;
                q += 4;
            } while (i < 0xA);
        }

        {
            void* mgr = getInstance__9itManagerFv(updateCamera__15tyFigListFigureFv(*(int*)(s + 0x4C), 1));
            if (*(int*)((char*)mgr + 0x14B8) == 0) {
                __dt__20tyFigListDataManagerFv(*(void**)(s + 0x48), 1);
            }
        }
        __dt__5MuMsgFv(*(void**)(s + 0x5F0), 1);
        __dt__5MuMsgFv(*(void**)(s + 0x5F4), 1);
        __dt__5MuMsgFv(*(void**)(s + 0x5F8), 1);
        exit__6gfTaskFv(*(void**)(s + 0x5EC));
        {
            void* t = *(void**)(s + 0x594);
            *(void**)(s + 0x5EC) = 0;
            if (t != 0) {
                exit__6gfTaskFv(t);
                *(void**)(s + 0x594) = 0;
            }
        }

        __destroy_arr(s + 0x624, __dt__14gfFileIOHandleFv, 4, 0xA);
        __destroy_arr(s + 0x54, __dt__16muMenuControllerFv, 0xA8, 8);

        __dt__6gfTaskFv(s, 0);
        if (arg1 > 0) {
            __dl__FPv(s);
        }
    }

    return self;
}

}
}

// TODO: clean up, give variables real names
namespace jj_2FA0 {
typedef unsigned int u32;

extern "C" {
extern char* g_GameGlobal;
extern char lbl_2_data_7A8[];
extern const float lbl_2_rodata_98;

u32* fn_2_2FA0(u32* self, u32 arg) {
    u32 z = 0;
    self[0] = z;
    self[2] = z;
    self[4] = arg;
    self[5] = z;
    self[13] = z;
    self[14] = z;
    self[16] = (u32)lbl_2_data_7A8;
    self[21] = 0x270f;
    self[22] = 0x270f;
    self[23] = 0x270f;
    self[24] = z;
    self[25] = z;
    self[15] = z;
    float f = lbl_2_rodata_98;
    *(float*)(*(char**)(g_GameGlobal + 0x44) + 0x4) = f;
    *(float*)(*(char**)(g_GameGlobal + 0x44) + 0x8) = f;
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_300C {
extern "C" void __dl__FPv(void*);

extern "C" void* fn_2_300C(void* ptr, int flag)
{
    if ((int)ptr != 0) {
        if (flag > 0) {
            __dl__FPv(ptr);
        }
    }
    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_304C {
extern "C" void* fn_2_304C(void* p, s32 size) {
    if (p != nullptr && size > 0) {
        operator delete(p);
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_308C {
extern "C" {
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(void* self, const char* name, int heap, int a, int b);
void release__14gfFileIOHandleFv(void*);
int sprintf(char*, const char*, ...);
}
extern char lbl_2_data_680[];

struct Mgr {
    virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
    virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
    virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11();
    virtual void v12();
    virtual void rel(void*);
};
struct Info { int a; int b; int name; };

struct Obj {
    int f0, f4;
    void* f8;
    Mgr* fC;
    int f10, f14;
    float f18, f1C, f20;
    int pad[3];
    Info* f30;
    int f34;
    char h38[0x1c];
    int f54, f58;
    int f5c, f60, f64;
};

extern "C" void fn_2_308C(Obj* self, int* arg1, Mgr* arg2) {
    void* o = self->f8;
    self->f4 = 0;
    self->f0 = 0;
    self->f20 = 0.0f;
    self->f1C = 0.0f;
    self->f18 = 0.0f;
    self->fC = arg2;
    self->f30 = (Info*)arg1;
    if (o) {
        arg2->rel(o);
        Destroy__Q34nw4r3g3d6G3dObjFv(self->f8);
        self->f8 = 0;
        self->f34 = 1;
        self->f60 = 0;
        self->f64 = 0;
    }
    self->f54 = *arg1;
    switch (self->f34) {
    case 0: {
        char buf[0x40];
        release__14gfFileIOHandleFv(self->h38);
        int v = self->f54;
        sprintf(buf, lbl_2_data_680, self->f30->name);
        self->f58 = v;
        readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self->h38, buf, 0x2b, 0, 0);
        self->f34 = 4;
        break;
    }
    case 5: {
        char buf2[0x40];
        release__14gfFileIOHandleFv(self->h38);
        int v = self->f54;
        sprintf(buf2, lbl_2_data_680, self->f30->name);
        self->f58 = v;
        readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self->h38, buf2, 0x2b, 0, 0);
        self->f34 = 4;
        break;
    }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_3BC0 {
extern "C" {
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void cancelRequest__14gfFileIOHandleFv(void*);
void free__13gfHeapManagerFPv(void*);
void __dt__14gfFileIOHandleFv(void*, int);
void __dl__FPv(void*);
}

struct ResourceBase {
    char pad[0x5c];
};

struct Resource : ResourceBase {
    virtual ~Resource();
};

struct Menu {
    char pad0[8];
    void* object;
    char padC[8];
    Resource* resource;
    char pad18[0x20];
    unsigned int handle;
    void* buffer;
};

extern "C" Menu* updateCamera__15tyFigListFigureFv(Menu* self, int flag) {
    if (self) {
        if (self->object) {
            Destroy__Q34nw4r3g3d6G3dObjFv(self->object);
            self->object = 0;
        }
        if (self->resource) {
            Resource* resource = self->resource;
            delete resource;
            self->resource = 0;
        }
        cancelRequest__14gfFileIOHandleFv(&self->handle);
        if (self->buffer) {
            free__13gfHeapManagerFPv(self->buffer);
            self->buffer = 0;
        }
        __dt__14gfFileIOHandleFv(&self->handle, -1);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_3EC8 {

class CallbackView : public gmPlayerInitData {
public:
    virtual void callback(int) = 0;
};

extern "C" void fn_2_308C(void*, void*, void*);
extern "C" void fn_2_3C80(void*);
extern const char lbl_2_data_680[];
extern const char lbl_2_data_6B8[];

extern "C" int fn_2_3EC8(void* self) {
    char* p = (char*)self;
    int state = *(int*)(p + 0x34);

    if ((unsigned)(state - 4) <= 1u) {
        if (*(int*)(p + 0x38) == 0)
            return 0;
        if (!((gfFileIOHandle*)(p + 0x38))->isReady())
            return 0;
        if (((gfFileIOHandle*)(p + 0x38))->getBuffer() == 0) {
            ((gfFileIOHandle*)(p + 0x38))->readRequest(
                lbl_2_data_6B8, (HeapType)0x2b, 0, 0);
            return 0;
        }

        unsigned val58 = *(unsigned*)(p + 0x58);
        if (*(unsigned*)(p + 0x54) == val58) {
            *(int*)(p + 0x5c) = val58;
            *(int*)(p + 0x34) = 0;
            fn_2_3C80(self);
            return 0;
        }

        gfHeapManager::free(
            ((gfFileIOHandle*)(p + 0x38))->getBuffer());
        ((gfFileIOHandle*)(p + 0x38))->release();
        *(int*)(p + 0x34) = 5;
        fn_2_308C(self, *(void**)(p + 0x30), *(void**)(p + 0x0c));
    }

    state = *(int*)(p + 0x34);
    if (state == 1) {
        int val = *(int*)(p + 0x60) + 1;
        *(int*)(p + 0x60) = val;
        if (val >= 4) {
            void* obj = *(void**)(p + 0x14);
            if (obj)
                ((CallbackView*)obj)->callback(1);

            *(void**)(p + 0x14) = 0;
            void* obj3c = *(void**)(p + 0x3c);
            if (obj3c) {
                gfHeapManager::free(obj3c);
                *(void**)(p + 0x3c) = 0;
            }
            *(int*)(p + 0x34) = 2;
            *(int*)(p + 0x64) = 0;
        }
        return 1;
    }

    if (state == 2) {
        int val = *(int*)(p + 0x64) + 1;
        *(int*)(p + 0x64) = val;
        if (val >= 4) {
            ((gfFileIOHandle*)(p + 0x38))->release();
            int saved = *(int*)(p + 0x54);
            char buf[0x40];
            char* ptr30 = *(char**)(p + 0x30);
            sprintf(buf, lbl_2_data_680, *(int*)(ptr30 + 8));
            *(int*)(p + 0x58) = saved;
            ((gfFileIOHandle*)(p + 0x38))->readRequest(
                buf, (HeapType)0x2b, 0, 0);
            *(int*)(p + 0x34) = 4;
        }
        return 1;
    }

    return state == 0;
}
}

// TODO: clean up, give variables real names
namespace jj_4160 {
extern "C" void fn_2_4160(void) {}
}

// TODO: clean up, give variables real names
namespace jj_4164 {
extern "C" void fn_2_4164(void) {}
}

// TODO: clean up, give variables real names
namespace jj_4168 {
extern "C" void fn_2_4168(void) {}
}

