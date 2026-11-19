#include <mu/adv/mu_adv_game_over.h>
#include <mu/menu.h>
#include <sr/sr_common.h>
#include <ty/ty_fig_listmng.h>
#include <ty/ty_seal_listmng.h>
#include <types.h>

// Tells whether the pack of the scene module is read (scResourceLoader of the scene module).
extern "C" bool fn_1_3E50(scResourceLoader* loader);

extern "C" muAdvGameOverTask* fn_33_44(muAdvGameOverTask* task,
        const muAdvGameOverTaskParam* param);

muAdvGameOverTask* muAdvGameOverTask::create(const muAdvGameOverTaskParam* param) {
    muAdvGameOverTask* task = static_cast<muAdvGameOverTask*>(
            operator new(0x3F8, Heaps::MenuInstance));
    if (task != nullptr) {
        task = fn_33_44(task, param);
    }
    return task;
}

// The lists of the stickers and of the trophies are loaded.
bool muAdvGameOverTask::preLoad() {
    bool loaded = false;
    if (unk280->isLoadFinish()) {
        if (unk27C->isLoadFinish()) {
            loaded = true;
        }
    }
    return loaded;
}

bool muAdvGameOverTask::checkLoadFinish() const {
    bool loaded = false;
    if (fn_1_3E50(unk3F4)) {
        if (muMenu::isLoadFinishMenuSound()) {
            loaded = true;
        }
    }
    return loaded;
}

// What the screen was left with: the entry the cursor was on (the continue entry counts as 4 or 5 when it cannot be
// used).
s32 muAdvGameOverTask::getSelection() const {
    s32 selection = unk234[unk22C];
    if (selection == 1) {
        if (unk1B0 == 0) {
            selection = 5;
        } else {
            muAdvGameOverObj* obj = unk2BC;
            if (obj != NULL) {
                if (obj->unk1AC == 1) {
                    selection = 5;
                    if (obj->unk1B4 == 0) {
                        selection = 4;
                    }
                }
            }
        }
    }
    return selection;
}

#include <it/it_manager.h>
#include <nw4r/g3d/g3d_obj.h>
#include <gf/gf_task.h>
#include <gf/gf_archive.h>
#include <cstdio>
#include <ut/ut_list.h>
#include <mu/mu_object.h>
#include <snd/snd_system.h>
#include <string.h>
#include <stdio.h>
#include <gf/gf_pad_system.h>
// TODO: clean up, give variables real names
namespace jj_44 {


typedef adList<u32, Heaps::MenuInstance> FigureList;

extern "C" {
extern char lbl_33_data_0[];
extern char lbl_33_data_10[];
extern char lbl_33_data_164[];

void fn_33_45C(void*);
void fn_33_5E8(void*);
void __ct__Q27utility12StandardMenuFPCciQ25Heaps8HeapType(void*, const char*, u8, int);
void __ct__Q27utility12DepthClearerFQ211gfSceneRoot9LayerType(void*, int, const char*);
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
char* strcat(char*, const char*);
void* memset(void*, int, unsigned long);
void* __ct__21tySealListDataManagerFb(void*, int);
void fn_800B96E8(void*, int);
}

extern "C" void* fn_33_44(void* arg0, const muAdvGameOverTaskParam* args) {
    int off3;
    int lim1;
    int lim3;
    int i3;
    FigureList* it3;
    char* p = (char*)arg0;

    __ct__Q27utility12StandardMenuFPCciQ25Heaps8HeapType(p, lbl_33_data_0, (u8)args->unk4, 0x2A);

    u32 heap = args->unk4;
    *(u32*)(p + 0x1AC) = 0;
    *(char**)(p + 0x3C) = lbl_33_data_164;
    *(u32*)(p + 0x1B4) = heap;
    *(u32*)(p + 0x1C4) = 0;
    *(u32*)(p + 0x1F0) = 0;
    *(u32*)(p + 0x21C) = 0;
    *(u32*)(p + 0x22C) = 0;
    *(u32*)(p + 0x230) = 0;
    *(u32*)(p + 0x240) = 0;

    __ct__Q27utility12DepthClearerFQ211gfSceneRoot9LayerType(p + 0x244, 8, lbl_33_data_164);

    *(u32*)(p + 0x248) = 0;
    *(u32*)(p + 0x24C) = 0;
    *(u32*)(p + 0x254) = 0;
    *(u32*)(p + 0x258) = 0;
    *(u32*)(p + 0x25C) = 0;
    *(u32*)(p + 0x260) = 0;
    *(u32*)(p + 0x2BC) = 0;

    __construct_array(p + 0x2C0, fn_33_45C, fn_33_5E8, 0x34, 3);

    *(u32*)(p + 0x3D8) = 0;
    *(u32*)(p + 0x3DC) = 0;
    *(u32*)(p + 0x3E0) = 0;
    *(u32*)(p + 0x3EC) = 0;
    *(u32*)(p + 0x3F0) = 0;
    *(u32*)(p + 0x3F4) = 0;
    p[0x7C] = 0;

    strcat(p + 0x7C, lbl_33_data_10);
    memset(p + 0x234, 0, 4);

    *(u32*)(p + 0x284) = 0;
    *(u32*)(p + 0x288) = 0;
    *(u32*)(p + 0x28C) = 0;
    *(u32*)(p + 0x290) = 0;
    *(u32*)(p + 0x294) = 0;
    *(u32*)(p + 0x298) = 0;
    *(u32*)(p + 0x29C) = 0;
    *(u32*)(p + 0x2A0) = 0;
    *(u32*)(p + 0x2A4) = 0;
    *(u32*)(p + 0x2A8) = 0;
    *(u32*)(p + 0x2AC) = 0;
    *(u32*)(p + 0x2B0) = 0;
    *(u32*)(p + 0x2B4) = 0;
    *(u32*)(p + 0x2B8) = 0;

    *(u32*)(p + 0x35C) = 0;
    *(u32*)(p + 0x360) = 0;
    *(u32*)(p + 0x364) = 0;
    *(u32*)(p + 0x368) = 0;
    *(u32*)(p + 0x36C) = 0;
    *(u32*)(p + 0x370) = 0;
    *(u32*)(p + 0x374) = 0;
    *(u32*)(p + 0x378) = 0;
    *(u32*)(p + 0x37C) = 0;
    *(u32*)(p + 0x380) = 0;
    *(u32*)(p + 0x384) = 0;
    *(u32*)(p + 0x388) = 0;
    *(u32*)(p + 0x38C) = 0;
    *(u32*)(p + 0x390) = 0;
    *(u32*)(p + 0x394) = 0;
    *(u32*)(p + 0x398) = 0;
    *(u32*)(p + 0x39C) = 0;
    *(u32*)(p + 0x3A0) = 0;
    *(u32*)(p + 0x3A4) = 0;
    *(u32*)(p + 0x3A8) = 0;
    *(u32*)(p + 0x3AC) = 0;
    *(u32*)(p + 0x3B0) = 0;
    *(u32*)(p + 0x3B4) = 0;
    *(u32*)(p + 0x3B8) = 0;
    *(u32*)(p + 0x3BC) = 0;
    *(u32*)(p + 0x3C0) = 0;
    *(u32*)(p + 0x3C4) = 0;
    *(u32*)(p + 0x3C8) = 0;
    *(u32*)(p + 0x3CC) = 0;
    *(u32*)(p + 0x3D0) = 0;
    *(u32*)(p + 0x3D4) = 0;

    *(u32*)(p + 0x264) = 0;
    *(u32*)(p + 0x270) = 0;
    *(u32*)(p + 0x268) = 0;
    *(u32*)(p + 0x274) = 0;
    *(u32*)(p + 0x26C) = 0;
    *(u32*)(p + 0x278) = 0;

    void* obj = operator new(0x3EB4, Heaps::MenuInstance);
    if (obj != 0) {
        obj = __ct__21tySealListDataManagerFb(obj, 1);
    }
    *(void**)(p + 0x280) = obj;

    char* item = *(char**)((char*)itManager::getInstance() + 0x14B8);
    if (item != 0) {
        *(u32*)(p + 0x27C) = *(u32*)(item + 4);
    }

    int n = 0;
    ((u8*)p)[0x1B0] = args->unk0;
    *(u32*)(p + 0x234 + n++ * 4) = 0;
    if (((u8*)p)[0x1B0] != 0) {
        *(u32*)(p + 0x234 + n++ * 4) = 2;
    }
    *(u32*)(p + 0x234 + n++ * 4) = 1;
    *(u32*)(p + 0x240) = n;

    fn_800B96E8(p + 0x100, (u8)*(u32*)(p + 0x1B4));

    int i1;
    *(FigureList**)(p + 0x220) = args->unk14;
    FigureList* it1 = args->unk8;
    *(FigureList**)(p + 0x224) = args->unk18;
    *(FigureList**)(p + 0x228) = args->unk1C;
    it1->reset();

    u32 c1 = it1->count();
    lim1 = 10;
    if (c1 < 10) {
        lim1 = c1;
    }

    i1 = 0;
    for (int off = 0; i1 < lim1; off += 4, i1++) {
        *(u32*)(p + 0x1F4 + off) = it1->get();
        it1->next();
    }

    int off2;
    FigureList* it2 = args->unkC;
    *(int*)(p + 0x21C) = lim1;
    it2->reset();

    u32 c2 = it2->count();
    int i2;
    int lim2;
    lim2 = 10;
    if (c2 < 10) {
        lim2 = c2;
    }

    i2 = 0;
    for (off2 = 0; i2 < lim2; off2 += 4, i2++) {
        *(u32*)(p + 0x1C8 + off2) = it2->get();
        it2->next();
    }

    it3 = args->unk10;
    *(int*)(p + 0x1F0) = lim2;
    it3->reset();

    u32 c3 = it3->count();
    lim3 = 3;
    if (c3 < 3) {
        lim3 = c3;
    }

    i3 = 0;
    for (off3 = 0; i3 < lim3; off3 += 4, i3++) {
        *(u32*)(p + 0x1B8 + off3) = it3->get();
        it3->next();
    }

    *(int*)(p + 0x1C4) = lim3;
    return arg0;
}
}

// TODO: clean up, give variables real names
namespace jj_45C {
extern "C" {
extern char lbl_8049E550[];
extern char lbl_33_data_2D0[];
extern char lbl_33_data_280[];
extern char __vt__Q27utility11AutoResFile[];
void addTail__6utListFP10utListNode(void* list, void* node);
void removeExist__6utListFP10utListNode(void* list, void* node);

void* fn_33_45C(void* self) {
    char* s = (char*)self;
    char* n = s + 4;
    char* L = lbl_8049E550;
    *(void**)(s + 0x0) = 0;
    *(void**)(s + 0xC) = 0;
    *(void**)(n + 0x0) = 0;
    *(void**)(n + 0x4) = 0;
    addTail__6utListFP10utListNode(L, n);
    *(void**)(n + 0xC) = lbl_33_data_2D0;
    *(void**)(n + 0x8) = L;
    *(void**)(s + 0x30) = __vt__Q27utility11AutoResFile;
    *(void**)(s + 0x10) = __vt__Q27utility11AutoResFile + 8;
    *(void**)(s + 0x1C) = 0;
    *(void**)(s + 0x14) = 0;
    *(void**)(s + 0x18) = 0;
    char* m = s + 0x14;
    addTail__6utListFP10utListNode(L, m);
    *(void**)(m + 0x8) = L;
    *(void**)(m + 0xC) = lbl_33_data_280;
    *(void**)(m + 0x10) = 0;
    *(char*)(m + 0x14) = 0;
    *(char*)(s + 0x2C) = 0;
    removeExist__6utListFP10utListNode(L, m);
    *(void**)(s + 0x1C) = 0;
    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_528 {
typedef unsigned char u8;

struct gfFileIOHandle {
    void release();
    ~gfFileIOHandle();
};

struct GameOverObject {
    u8 pad0[8];
    void* node;
    void* vtable;
    gfFileIOHandle handle;
};

extern "C" void clear__Q27utility8AutoFileFv(void*);
extern "C" void removeExist__6utListFP10utListNode(void*, void*);
extern "C" void __dl__FPv(void*);
extern "C" u8 lbl_33_data_280[];
extern "C" u8 lbl_8049E550[];

extern "C" GameOverObject* fn_33_528(GameOverObject* self, int deleting) {
    if (self != 0) {
        self->vtable = lbl_33_data_280;
        clear__Q27utility8AutoFileFv(self);
        self->handle.release();
        self->handle.~gfFileIOHandle();
        if (self != 0) {
            if (self != 0) {
                removeExist__6utListFP10utListNode(lbl_8049E550, self);
                self->node = 0;
                if (self != 0) {
                    if (self->node != 0) {
                        removeExist__6utListFP10utListNode(0, self);
                        self->node = 0;
                    }
                }
            }
        }
        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_5E8 {
typedef unsigned char u8;
typedef unsigned int u32;

struct gfFileIOHandle {
    void release();
    ~gfFileIOHandle();
};

struct Sub {
    u8 pad0[8];
    void* node;
    void* vtable;
    gfFileIOHandle handle;
};

struct Sub2 {
    u8 pad0[8];
    void* node;
};

struct Obj {
    u32 field0;
    Sub2 sub2;
    void* vtable;
    Sub sub;
    u8 pad28[4];
    u8 byte2c;
    void* vtable2;
};

extern "C" {
void clear__Q27utility8AutoFileFv(void*);
void release__14gfFileIOHandleFv(void*);
void __dt__14gfFileIOHandleFv(void*, int);
void removeExist__6utListFP10utListNode(void*, void*);
void __dl__FPv(void*);
extern u8 __vt__Q27utility11AutoResFile[];
extern u8 lbl_33_data_280[];
extern u8 lbl_8049E550[];
}

extern "C" void* fn_33_5E8(Obj* object, int deleting) {
    if (object) {
        object->vtable2 = __vt__Q27utility11AutoResFile;
        object->vtable = __vt__Q27utility11AutoResFile + 8;

        clear__Q27utility8AutoFileFv(&object->sub);

        int zero = 0;
        object->field0 = zero;
        object->byte2c = zero;

        if (&object->sub != 0) {
            object->sub.vtable = lbl_33_data_280;
            clear__Q27utility8AutoFileFv(&object->sub);
            object->sub.handle.release();
            object->sub.handle.~gfFileIOHandle();
            if (&object->sub != 0) {
                if (&object->sub != 0) {
                    removeExist__6utListFP10utListNode(lbl_8049E550, &object->sub);
                    object->sub.node = (void*)zero;
                    if (&object->sub != 0) {
                        if (object->sub.node != 0) {
                            removeExist__6utListFP10utListNode(0, &object->sub);
                            object->sub.node = (void*)zero;
                        }
                    }
                }
            }
        }

        if (&object->sub2 != 0) {
            if (&object->sub2 != 0) {
                removeExist__6utListFP10utListNode(lbl_8049E550, &object->sub2);
                int zero2 = 0;
                object->sub2.node = (void*)zero2;
                if (&object->sub2 != 0) {
                    if (object->sub2.node != 0) {
                        removeExist__6utListFP10utListNode(0, &object->sub2);
                        object->sub2.node = (void*)zero2;
                    }
                }
            }
        }

        if (deleting > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_708 {
typedef unsigned char u8;
class MenuObject {
    char pad[0x5c];
public:
    virtual ~MenuObject();
};
extern "C" {
extern u8 lbl_33_data_164[];
extern void* g_efScreen;
void fn_800AD87C(void*, int);
void fn_1_406C(void*);
void __dt__14gfFileIOHandleFv(void*, int);
void __dl__FPv(void*);
void reset__8efScreenFv(void*);
void fn_33_5E8(void*, int);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__Q27utility12DepthClearerFv(void*, int);
void __dt__Q27utility12StandardTaskFv(void*, int);
void* fn_33_708(u8* object, int deleting) {
    void* h = 0;
    if (object) {
        nw4r::g3d::G3dObj* first = *(nw4r::g3d::G3dObj**)(object + 0x284);
        *(void**)(object + 0x3c) = lbl_33_data_164;
        if (first) first->Destroy();
        u8* cursor = object;
        for (int i = 0; i < *(int*)(object + 0x1c4); cursor += 4, ++i) {
            nw4r::g3d::G3dObj* p = *(nw4r::g3d::G3dObj**)(cursor + 0x288);
            if (p) p->Destroy();
        }
        cursor = object;
        for (int i = 0; i < *(int*)(object + 0x1f0); cursor += 4, ++i) {
            nw4r::g3d::G3dObj* p = *(nw4r::g3d::G3dObj**)(cursor + 0x294);
            if (p) p->Destroy();
        }
        cursor = object;
        for (int i = 0; i < 31; cursor += 4, ++i) {
            MenuObject* p = *(MenuObject**)(cursor + 0x35c);
            if (p) delete p;
        }
        cursor = object;
        for (int i = 0; i < 3; cursor += 4, ++i) {
            MenuObject* p = *(MenuObject**)(cursor + 0x264);
            if (p) delete p;
            p = *(MenuObject**)(cursor + 0x270);
            if (p) delete p;
        }
        if (*(void**)(object + 0x280)) fn_800AD87C(*(void**)(object + 0x280), 1);
        gfArchive* archive = *(gfArchive**)(object + 0x3f0);
        if (archive) {
            delete archive;
            *(gfArchive**)(object + 0x3f0) = 0;
        }
        h = *(void**)(object + 0x3f4);
        if (h) {
            if (h) {
                fn_1_406C(h);
                __dt__14gfFileIOHandleFv(h, -1);
                __dl__FPv(h);
            }
            *(void**)(object + 0x3f4) = 0;
        }
        reset__8efScreenFv(g_efScreen);
        __destroy_arr(object + 0x2c0, fn_33_5E8, 0x34, 3);
        if (object + 0x2bc) {
            gfTask* p = *(gfTask**)(object + 0x2bc);
            if (p) p->exit();
            *(gfTask**)(object + 0x2bc) = 0;
        }
        if (object + 0x260) {
            gfTask* p = *(gfTask**)(object + 0x260);
            if (p) p->exit();
            *(gfTask**)(object + 0x260) = 0;
        }
        if (object + 0x25c) delete *(MenuObject**)(object + 0x25c);
        __dt__Q27utility12DepthClearerFv(object + 0x244, -1);
        if (object) __dt__Q27utility12StandardTaskFv(object, 0);
        if (deleting > 0) __dl__FPv(object);
    }
    return object;
}
}
}

// TODO: clean up, give variables real names
namespace jj_9F0 {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__9gfArchiveFv(void*);
void fn_1_3C68(void*, int, void*, void*, int);
void clear__Q27utility8AutoFileFv(void*);
void request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(void*, const void*, int, int);
void loadMenuSound__6muMenuFv(void);
extern char lbl_33_data_30[];
extern char lbl_33_data_4C[];
extern utList lbl_8049E550;
}
struct FigMenuData { unsigned int fields[7]; bool flag1 : 1; bool flag2 : 1; bool flag3 : 1; unsigned char other : 5; };
extern "C" void fn_33_9F0(char* self) {
    char buf[24]; void* arc; void* obj; char* pa; char* pc; char* pb; utListNode* pd; int i; int zero;
    arc = __nw__FUlQ25Heaps8HeapType(0x80, 0x2A);
    if (arc != 0) { arc = __ct__9gfArchiveFv(arc); }
    *(void**)(self + 0x3F0) = arc;
    obj = __nw__FUlQ25Heaps8HeapType(0x20, 0x2A);
    if (obj != 0) {
        FigMenuData* fields = (FigMenuData*)obj;
        fields->fields[0] = 0; fields->fields[1] = 0; fields->fields[2] = 0; fields->fields[3] = 0; fields->fields[4] = 0;
        fields->flag1 = false; fields->flag2 = false; fields->fields[6] = 0; fields->fields[5] = 0xFF; fields->flag3 = false;
    }
    *(void**)(self + 0x3F4) = obj;
    fn_1_3C68(obj, 0x2A, lbl_33_data_30, *(void**)(self + 0x3F0), 0x7D000);
    pa = self; pc = self + 0x2C0; pb = self; pd = (utListNode*)(self + 0x2C4); i = 0; zero = 0;
    for (; i < 3U; i++, pa += 4, pc += 0x34, pb += 0x34, pd = (utListNode*)((char*)pd + 0x34)) {
        if (i < *(int*)(self + 0x1C4)) {
            tyFigListDataManager* manager = *(tyFigListDataManager**)(self + 0x27C);
            unsigned int id = *(unsigned int*)(pa + 0x1B8);
            tyFigListData* data = manager->getFigDataId(id);
            sprintf(buf, lbl_33_data_4C, data->name);
            clear__Q27utility8AutoFileFv(pc + 0x14);
            *(int*)(pb + 0x2C0) = zero;
            *(unsigned char*)(pb + 0x2EC) = zero;
            request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(pc + 0x14, buf, 0x2B, zero);
        } else { lbl_8049E550.removeExist(pd); *(int*)((char*)pd + 8) = zero; }
    }
    loadMenuSound__6muMenuFv();
}
}

// TODO: clean up, give variables real names
namespace jj_E4C {
extern "C" void fn_33_E50(void);

extern "C" void fn_33_E4C(void) {
    fn_33_E50();
}
}

// TODO: clean up, give variables real names
namespace jj_1B3C {
struct Provider {
    virtual int m1();
    virtual void m2();
    virtual void m3();
    virtual void m4();
    virtual int m5();
    virtual void m6();
    virtual void m7();
};

struct Param {
    u32 w0, w4, w8, wC, w10, w14, w18, w1C;
    u8 port;
    u8 pad[3];
    void* p1;
    void* p2;
    void* p3;
};

struct Pair {
    union {
        bool flag;
        u32 word;
    } a;
    u32 b;
};

struct Adv {
    char pad[0x4928];
    u32 a;
    u32 b;
};

struct Global {
    char pad[0x30];
    Adv* adv;
};

struct Self {
    char pad0[0x220];
    Provider* a;
    Provider* b;
    Provider* c;
    char pad1[0x248 - 0x22C];
    u32 f248;
    char pad2[4];
    u32 f250;
    u32 f254;
    char pad3[8];
    void* work;
    char pad4[0x27C - 0x264];
    u32 f27C;
    u32 f280;
    char pad5[0x3F0 - 0x284];
    void* f3F0;
};

extern "C" {
extern const Param lbl_33_rodata_18;
extern Global* g_GameGlobal;
extern u8 lbl_33_data_3C8[];
extern u8 lbl_33_data_3D4[];
extern u8 lbl_33_data_3DC[];
void* getData__9gfArchiveFi11ARCNodeTypeiUs(void*, int, int, int, u16);
void* fn_800F7CCC(Param*, int, int);
void fn_800FA7B8(void*, int, int, void*, int, void*, void*);
void exit__6gfTaskFv(void*);

void fn_33_1B3C(Self* self) {
    Param param = lbl_33_rodata_18;
    param.w0 = self->f248;
    param.w4 = self->f254;
    param.w10 = self->f27C;
    param.w14 = self->f280;
    param.w18 = self->f250;

    Pair fallback;
    Pair value;
    Pair copy;

    Adv* adv = g_GameGlobal->adv;
    if (adv) {
        value.a.word = adv->a;
        value.b = adv->b;
    } else {
        fallback.a.flag = false;
        fallback.b = -1;
        value = fallback;
    }

    copy = value;
    param.port = copy.b;
    param.p1 = getData__9gfArchiveFi11ARCNodeTypeiUs(self->f3F0, 0, 1, 5, (u16)0xFFFE);
    param.p2 = getData__9gfArchiveFi11ARCNodeTypeiUs(self->f3F0, 0, 1, 6, (u16)0xFFFE);
    param.p3 = getData__9gfArchiveFi11ARCNodeTypeiUs(self->f3F0, 0, 1, 7, (u16)0xFFFE);

    void* w = fn_800F7CCC(&param, 0x2A, 0x2B);
    if (self->work != 0) {
        exit__6gfTaskFv(self->work);
    }
    self->work = w;

    if (self->a != 0) {
        for (int i = 0; i < self->a->m1(); ) {
            int id = self->a->m5();
            fn_800FA7B8(self->work, id, 1, (char*)self->work + 0x3A4, 0x400, (char*)self->work + 0x13A4, lbl_33_data_3C8);
            i++;
            self->a->m7();
        }
    }
    if (self->b != 0) {
        for (int i = 0; i < self->b->m1(); ) {
            int id = self->b->m5();
            fn_800FA7B8(self->work, id, 1, (char*)self->work + 0x13A6, 0x400, (char*)self->work + 0x23A6, lbl_33_data_3D4);
            i++;
            self->b->m7();
        }
    }
    if (self->c != 0) {
        for (int i = 0; i < self->c->m1(); ) {
            int id = self->c->m5();
            fn_800FA7B8(self->work, id, 1, (char*)self->work + 0x23A8, 0x100, (char*)self->work + 0x27A8, lbl_33_data_3DC);
            i++;
            self->c->m7();
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_2100 {
extern "C" {
extern int lbl_33_data_C0[3];
extern char lbl_33_data_3E0[];
extern char lbl_33_data_3E4[];
extern int lbl_33_data_A0[8];
void fn_800B9F90(void*, void*, int);
gfTask* fn_33_2AE0(void*);
}
struct Name { char s[65]; };
struct Values { int v[3]; };
struct Params { int v[8]; };
struct Status { char pad[12]; int buttons; char rest[0x30]; };
struct Node { char pad[0x10]; int value; };
struct Controller {
 virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
 virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
 virtual void f8(); virtual void f9(); virtual void f10(); virtual void f11();
 virtual void f12(); virtual void f13(int); virtual void f14(); virtual void f15(int);
};
struct Self {
 char pad0[0x48]; int field48;
 char pad4c[0x110-0x4c]; int input;
 char pad114[0x1b0-0x114]; u8 flag; char pad1b1[3]; int field1b4;
 char pad1b8[0x21c-0x1b8]; int count;
 char pad220[0xc]; int selection; int pad230; int options[3]; int numOptions;
 char pad244[8]; int field24c; int field250; char pad254[8]; MuObject* obj;
 char pad260[0x24]; Controller* controller; char pad288[0x34]; gfTask* task;
 char pad2c0[0x9c]; Node* nodes[1];
};
inline int getValue(Self* self) {
 int value = self->options[self->selection];
  if (value == 1) {
   if (!self->flag) value = 5;
   else if (self->task && *(int*)((char*)self->task + 0x1ac) == 1) {
    value = 5;
    if (*(int*)((char*)self->task + 0x1b4) == 0) value = 4;
   }
  }
 return value;
}
extern "C" int fn_33_2100(Self* self) {
 int value;
 MuObject* obj;
 int input = self->input;
 int old = self->selection;
 int selection = old;
 if ((input & 8) && old > 0) selection = old - 1;
 if ((input & 4) && selection < self->numOptions - 1) selection++;
 if (selection != old) {
  g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
  obj = self->obj;
  Values values = *(Values*)lbl_33_data_C0;
  value = values.v[self->options[selection]];
  char* mdl = *(char**)((char*)obj + 8);
  int off = *(int*)(mdl + 0x3c);
  const char* src = off ? mdl + off : 0;
  Name n4; Name n3; Name n2; Name n1;
  n1.s[0] = 0;
  strcat(n1.s, src);
  n2 = n1;
  strcat(n2.s, lbl_33_data_3E4);
  n3 = n2;
  sprintf(n3.s + strlen(n3.s), lbl_33_data_3E0, value);
  n4 = n3;
  obj->changeClrAnimNIf(n4.s);
  self->selection = selection;
 }
 Status status;
 fn_800B9F90(&status, (char*)self + 0x100, 0);
 if (status.buttons & 0x100) {
  int value = getValue(self);
  if (value == 0) g_sndSystem->playSE((SndID)0x13, -1, 0, 0, -1);
  else g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
  if (self->options[self->selection] == 1) {
   Params p = *(Params*)lbl_33_data_A0;
   p.v[0] = self->field1b4;
   p.v[1] = self->field48;
   p.v[2] = self->field24c;
   p.v[3] = self->field250;
   p.v[4] = 4;
   gfTask* task = fn_33_2AE0(&p);
   if (self->task) self->task->exit();
   self->task = task;
  }
  for (selection = 0; selection < self->count; selection++) self->controller->f13((*(Node**)((char*)self + (selection + 1) * 4 + 0x35c))->value);
  return 1;
 }
 return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_24E4 {
typedef unsigned char u8;

struct Entry {
    const char* name;
    u8 begin;
    u8 end;
    u8 kind;
};

struct Owner {
    char pad[0x35c];
    void* objs[1];
};

extern "C" {
extern char lbl_33_data_D0[];
extern const float lbl_33_rodata_8;

void* create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(void*, const char*, int, void*, int);
void changeAnimN__8MuObjectFPCc(void*, const char*);
void setUpdateRate__16gfModelAnimationFf(void*, float);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
}

extern "C" void fn_33_24E4(void* self, Entry* entries, int count, void* resFile) {
    char buf[64];
    int n;
    Entry* entry = entries;
    void* obj;
    int i;
    int j;

    for (i = 0; i < count; i++, entry++) {
        n = 1;
        if (entry->begin < entry->end) {
            n = entry->end - entry->begin;
        }
        for (j = 0; j < n; j++) {
            obj = create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
                resFile, entry->name, entry->kind, 0, 0x2a);
            const char* name = entry->name;
            ((Owner*)self)->objs[entry->begin + j] = obj;
            strcpy(buf, name);
            strcat(buf, lbl_33_data_D0);
            changeAnimN__8MuObjectFPCc(obj, buf);
            if (*(void**)((char*)obj + 0x14) != 0) {
                setUpdateRate__16gfModelAnimationFf(*(void**)((char*)obj + 0x14), lbl_33_rodata_8);
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_2AE0 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void fn_33_2B24(void* obj, void* self);

extern "C" void fn_33_2AE0(void* self) {
    void* p = __nw__FUlQ25Heaps8HeapType(0x1dc, 0x2a);
    if (p) {
        fn_33_2B24(p, self);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_2C08 {
typedef unsigned char u8;

class Obj {
    char pad[0x5C];
public:
    virtual void m(int);
};

extern "C" {
extern u8 lbl_33_data_420[];
void detachMuObject__5MuMsgFP8MuObject(void*, void*);
void __dt__5MuMsgFv(void*, int);
void __dt__Q27utility12StandardTaskFv(void*, int);
void __dl__FPv(void*);

void* fn_33_2C08(u8* object, int deleting)
{
    if (object) {
        *reinterpret_cast<void**>(object + 0x3C) = lbl_33_data_420;
        detachMuObject__5MuMsgFP8MuObject(*reinterpret_cast<void**>(object + 0x1D4), *reinterpret_cast<void**>(object + 0x1D0));
        if (object + 0x1D4) {
            __dt__5MuMsgFv(*reinterpret_cast<void**>(object + 0x1D4), 1);
        }
        if (object + 0x1D0) {
            Obj* t = *reinterpret_cast<Obj**>(object + 0x1D0);
            if (t) {
                t->m(1);
            }
        }
        if (object) {
            __dt__Q27utility12StandardTaskFv(object, 0);
        }
        if (deleting > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}
}

// TODO: clean up, give variables real names
namespace jj_2CC0 {
extern "C" void fn_33_2CC0(void) {}
}

// TODO: clean up, give variables real names
namespace jj_31CC {
extern "C" void fn_33_31D0(void);

extern "C" void fn_33_31CC(void) {
    fn_33_31D0();
}
}

// TODO: clean up, give variables real names
namespace jj_31D0 {

extern "C" {
int isNodeAnimFinished__8MuObjectFv(void*);
void DetachFromParent__Q34nw4r3g3d6G3dObjFv(void*);
extern char lbl_33_data_3E0[];
extern char lbl_33_data_3E4[];
}

struct Name { char s[65]; };
struct Self {
    char pad0[0x110];
    int input;
    char pad114[0x1ac-0x114];
    int done;
    int pad;
    int selection;
    char pad1b8[0x1d0-0x1b8];
    MuObject* obj;
    char pad1d4[4];
    int state;
};

extern "C" void fn_33_31D0(Self* self) {
    switch (self->state) {
    case 0:
        self->state = 0xc9;
    case 0xc9:
        if (isNodeAnimFinished__8MuObjectFv(self->obj)) self->state = 0xcf;
        break;
    case 0xcf:
        self->state = 0xd0;
        break;
    case 0xd0: {
        int input = self->input;
        int selection = self->selection;
        if ((input & 1) && selection != 0) selection = 0;
        if ((input & 2) && selection < 1) selection = 1;
        if (selection != *(int*)((char*)self + 0x1b4)) {
            g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
            self->obj->setFrameMatCol((float)(int)(selection != 0));
            self->selection = selection;
        }
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(self->pad, &status);
        if (status.m_buttonsPressedThisFrame.bits & 0x100) {
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
        } else if (status.m_buttonsPressedThisFrame.bits & 0x200) {
            self->selection = 1;
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
        } else {
            return;
        }
        MuObject* obj = self->obj;
        char* mdl = *(char**)((char*)obj + 8);
        int off = *(int*)(mdl + 0x3c);
        const char* src = off ? mdl + off : 0;
        Name n1;
        Name n2;
        Name n3;
        Name n4;
        n1.s[0] = 0;
        strcat(n1.s, src);
        n2 = n1;
        strcat(n2.s, lbl_33_data_3E4);
        n3 = n2;
        sprintf(n3.s + strlen(n3.s), lbl_33_data_3E0, 1);
        n4 = n3;
        obj->changeNodeAnimNIf(n4.s);
        self->state = 0x102;
        break;
    }
    case 0x102:
        if (isNodeAnimFinished__8MuObjectFv(self->obj)) {
            self->done = 1;
            DetachFromParent__Q34nw4r3g3d6G3dObjFv(self->obj->m_sceneModel);
            self->state = 0x10b;
        }
        break;
    case 0x10b:
        break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_34B8 {
extern "C" s32 checkLoadFinish__13muVCTrialTaskCFv(void) {
    return 0x1;
}
}

// TODO: clean up, give variables real names
namespace jj_34C0 {
extern "C" s32 fn_33_34C0(void) {
    return 0x1;
}
}

