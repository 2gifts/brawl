#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stdio.h>
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

// The trophy screen that shows one trophy and its stand (module sora_menu_main). The class is only described by the members
// the decompiled functions touch.
// Function names come from the sora_menu_main map.
// HYPOTHESIS: the list of the trophies of the screen: the entries are 0x60 bytes, the name of the file is at 8.
struct tyFigDisp2Entry {
    u8 m_unk0[8];
    const char* m_name; // 0x08
    u8 m_unkC[0x60 - 0xC];
};

struct tyFigDisp2List {
    tyFigDisp2Entry* m_entries; // 0x00
};

class tyFigDisp2Task : public gfTask {
public:
    u8 m_unk40[0x44 - 0x40];
    u8 m_bgNo;                              // 0x44
    u8 m_unk45[0xB0 - 0x45];
    nw4r::g3d::ResFile m_resFile;           // 0xB0
    u8 m_unkB4[0xD4 - 0xB4];
    MuMsg* m_msg;                           // 0xD4
    tyFigDisp2List* m_list;                 // 0xD8
    u8 m_unkDC[0x244 - 0xDC];
    int m_figSlot;                          // 0x244
    int m_figIndex;                         // 0x248
    u8 m_unk24C[0x290 - 0x24C];
    gfFileIOHandle m_handle;                // 0x290
    u8 m_unk294[0x298 - 0x294];
    muNoticeWndTask* m_wnd;                 // 0x298
    nw4r::g3d::ScnGroup* m_scnGroup;        // 0x29C
    void* m_figBuf[4];                      // 0x2A0
    void* m_bgBuf[2];                       // 0x2B0
    int m_bgSlot;                           // 0x2B8
    u8 m_unk2BC[0x300 - 0x2BC];

    tyFigDisp2Task();
    static tyFigDisp2Task* create(int a, int b);
    void initProc(int a, int b);

    void mainStepNoFigureInit();
    void mainStepExitConfirmInit();
    void mainStepLoadFigureInit();
    void mainStepLoadBGInit();
};

void tyFigDisp2Task::mainStepNoFigureInit() {
    m_msg->printIndex(6, 5, NULL);
}

void tyFigDisp2Task::mainStepExitConfirmInit() {
    m_wnd = fn_800BEEB8(1, m_msg->getBinary(), 6, &m_resFile, m_scnGroup, 0x17, 0, 0x2A, 0x2B, 0, 1, 0xF0);
}

void tyFigDisp2Task::mainStepLoadFigureInit() {
    char path[0x40];
    int slot = m_figSlot;
    sprintf(path, "/toy/fig/%s.brres", m_list->m_entries[m_figIndex].m_name);
    m_handle.readRequest(path, m_figBuf[slot], 0, 0);
}

void tyFigDisp2Task::mainStepLoadBGInit() {
    char path[0x40];
    sprintf(path, "/menu/collection/FigureDisp2_bg%d.brres", m_bgNo);
    m_handle.readRequest(path, m_bgBuf[m_bgSlot], 0, 0);
    m_bgSlot++;
    if (m_bgSlot >= 2) {
        m_bgSlot = 0;
    }
}

static tyFigDisp2Task* s_task;

tyFigDisp2Task* tyFigDisp2Task::create(int a, int b) {
    s_task = new ((Heaps::HeapType)2) tyFigDisp2Task();
    s_task->initProc(a, b);
    return s_task;
}

#include <gf/gf_heap_manager.h>
#include <gf/gf_camera.h>
#include <nw4r/g3d/g3d_obj.h>
#include <string.h>
#include <it/it_manager.h>
#include <revolution/MTX/mtx.h>
#include <gf/gf_pad_system.h>
#include <cm/cm_camera_controller.h>
#include <mu/mu_object.h>
#include <mt/mt_vector.h>
#include <gf/gf_pad_status.h>
#include <snd/snd_system.h>
// TODO: clean up, give variables real names
namespace jj_8228 {
extern "C" void GXSetClipMode(int);
extern "C" void CalcWorld__Q34nw4r3g3d7ScnRootFv(void*);
extern "C" void CalcMaterial__Q34nw4r3g3d7ScnRootFv(void*);
extern "C" void CalcVtx__Q34nw4r3g3d7ScnRootFv(void*);
extern "C" void SetCurrentCamera__Q34nw4r3g3d7ScnRootFi(void*, int);
extern "C" void* GetCamera__Q34nw4r3g3d7ScnRootFi(void*, int);
extern "C" void setNwCamera__8gfCameraFPQ34nw4r3g3d6Camera(void*, void*);
extern "C" void CalcView__Q34nw4r3g3d7ScnRootFv(void*);
extern "C" void GatherDrawScnObj__Q34nw4r3g3d7ScnRootFv(void*);
extern "C" void ZSort__Q34nw4r3g3d7ScnRootFv(void*);
extern "C" void* getManager__15gfCameraManagerFv();
extern int lbl_2_rodata_210;

struct Class_728_base { char pad[0x3c]; };
class Class_728 : public Class_728_base {
public:
    virtual void processDefault();
    virtual void processBegin();
    virtual void processAnim();
    virtual void processUpdate();
    virtual void processPreMapCorrection();
    virtual void processMapCorrection();
    virtual void processFixPosition();
    virtual void processPreCollision();
    virtual void processCollision();
    virtual void processCatch();
    virtual void processHit();
    virtual void processCamera();
    virtual void processFixCamera();
    virtual void processEffect();
    virtual void processGameProc();
    virtual void processEnd();
    virtual void renderPre__18tyFigDisp1RootTaskFv();
    virtual void renderOpa__18tyFigDisp1RootTaskFv();
    virtual void renderXlu__18tyFigDisp1RootTaskFv();
    virtual void processDebug();
    virtual void renderDebug();
    virtual void init();
    virtual void __dt__18tyFigDisp1RootTaskFv();
    virtual void updateAnim__18tyFigDisp1RootTaskFv();
};

extern "C" void renderPre__18tyFigDisp1RootTaskFv(char* root) {
    void* obj;
    char* dst;
    int off;
    int* p;
    int i;
    GXSetClipMode(0);
    p = &lbl_2_rodata_210;
    i = 0;
    off = 0;
    for (; i < 2; i++) {
        char* base = *reinterpret_cast<char**>(root + 0x40);
        obj = *reinterpret_cast<void**>(base + off);
        if (obj != 0) {
            CalcWorld__Q34nw4r3g3d7ScnRootFv(obj);
            CalcMaterial__Q34nw4r3g3d7ScnRootFv(obj);
            CalcVtx__Q34nw4r3g3d7ScnRootFv(obj);
            SetCurrentCamera__Q34nw4r3g3d7ScnRootFi(obj, 0);
            char* cam = (char*)getManager__15gfCameraManagerFv();
            dst = cam + *p * 0x134;
            void* sp8 = GetCamera__Q34nw4r3g3d7ScnRootFi(obj, 0);
            setNwCamera__8gfCameraFPQ34nw4r3g3d6Camera(dst, &sp8);
            CalcView__Q34nw4r3g3d7ScnRootFv(obj);
            GatherDrawScnObj__Q34nw4r3g3d7ScnRootFv(obj);
            ZSort__Q34nw4r3g3d7ScnRootFv(obj);
        }
        off += 8;
        p++;
    }
    Class_728* self = (Class_728*)root;
    self->updateAnim__18tyFigDisp1RootTaskFv();
}
}

// TODO: clean up, give variables real names
namespace jj_8310 {
struct Obj {
    char pad[0x40];
    char* list;
};

extern "C" {
void G3DState_Invalidate__FUl(unsigned int);
void DrawOpa__Q34nw4r3g3d7ScnRootFv(void*);
}

extern "C" void renderOpa__18tyFigDisp1RootTaskFv(Obj* p) {
    void* item;
    unsigned int off = 0;
    int i = 0;
    for (; i < 2; i++) {
        item = *(void**)(p->list + off);
        if (item != 0) {
            G3DState_Invalidate__FUl(0x7ff);
            DrawOpa__Q34nw4r3g3d7ScnRootFv(item);
        }
        off += 8;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8388 {
extern "C" {
void G3DState_Invalidate__FUl(s32 a);
void DrawXlu__Q34nw4r3g3d7ScnRootFv(s32 a);
void renderXlu__18tyFigDisp1RootTaskFv(s32* p) {
    s32 v;
    s32 offset = 0;
    for (s32 i = 0; i < 2; i++) {
        s32* arr = (s32*)*(s32**)((char*)p + 0x40);
        v = *(s32*)((char*)arr + offset);
        if (v != 0) {
            G3DState_Invalidate__FUl(0x7ff);
            DrawXlu__Q34nw4r3g3d7ScnRootFv(v);
        }
        offset += 8;
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_8400 {
extern "C" void UpdateFrame__Q34nw4r3g3d7ScnRootFv(void*);

class VBase {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
};

extern "C" void updateAnim__18tyFigDisp1RootTaskFv(char* root) {
    int off = 0;
    for (int i = 0; i < 2; i++) {
        char* base = *reinterpret_cast<char**>(root + 0x40);
        void* first = *reinterpret_cast<void**>(base + off);
        if (first != 0) {
            UpdateFrame__Q34nw4r3g3d7ScnRootFv(first);
        }
        VBase* obj = *reinterpret_cast<VBase**>(base + off + 4);
        if (obj != 0) {
            obj->f9();
        }
        off += 8;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_848C {

extern "C" {
void RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(void*);
nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d9AnmScnResFP12MEMAllocatorPUlQ34nw4r3g3d9ResAnmScnb(void*, int, int*, int);
void SetAnmScn__Q34nw4r3g3d7ScnRootFPQ34nw4r3g3d6AnmScn(void*, nw4r::g3d::G3dObj*);
void CalcWorld__Q34nw4r3g3d7ScnRootFv(void*);
void* GetCamera__Q34nw4r3g3d7ScnRootFi(void*, int);
void setFromNwCameraAllParam__8gfCameraFPQ34nw4r3g3d6Camera(void*, void**);
void calc__8gfCameraFv(void*);
void setNwCamera__8gfCameraFPQ34nw4r3g3d6Camera(void*, void**);
}
extern int lbl_2_rodata_210[];

struct Entry {
    void* object;
    nw4r::g3d::G3dObj* animation;
};
struct Owner {
    char pad[0x40];
    Entry* entries;
};

extern "C" void setScnAnm__18tyFigDisp1RootTaskFv(Owner* self, int index, int* resource) {
    Entry* entry;
    void* allocator = gfHeapManager::getMEMAllocator((Heaps::HeapType)0x2a);
    entry = &self->entries[index];
    if (entry->animation) {
        RemoveAnmScn__Q34nw4r3g3d7ScnRootFv(entry->object);
        entry->animation->Destroy();
        entry->animation = 0;
    }
    int res = *resource;
    entry->animation = Construct__Q34nw4r3g3d9AnmScnResFP12MEMAllocatorPUlQ34nw4r3g3d9ResAnmScnb(allocator, 0, &res, 0);
    void* obj = entry->object;
    SetAnmScn__Q34nw4r3g3d7ScnRootFPQ34nw4r3g3d6AnmScn(obj, entry->animation);
    CalcWorld__Q34nw4r3g3d7ScnRootFv(obj);
    if (index == 1) {
        gfCameraManager* manager = gfCameraManager::getManager();
        gfCamera* camera = manager->getCamera(lbl_2_rodata_210[index]);
        void* value = GetCamera__Q34nw4r3g3d7ScnRootFi(obj, 0);
        setFromNwCameraAllParam__8gfCameraFPQ34nw4r3g3d6Camera(camera, &value);
        calc__8gfCameraFv(camera);
        void* value2 = GetCamera__Q34nw4r3g3d7ScnRootFi(obj, 0);
        setNwCamera__8gfCameraFPQ34nw4r3g3d6Camera(camera, &value2);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_85AC {
typedef unsigned int u32;

struct Obj {
    virtual void vf0();
    virtual void vf1();
    virtual void vf2();
    virtual void vf3();
    virtual void vf4();
    virtual void vf5();
    virtual void vf6();
    virtual void vf7();
    virtual void vf8();
    virtual void vf9();
    virtual void vf10();
    virtual u32 vf13(u32 arg);   // vtable slot 0x34 (after the 2-entry destructor slot)
    char pad[0xe0];
    u32 m_e4;                    // at offset 0xe4
};

struct Slot {
    Obj* obj;
    u32 unk4;                    // 8-byte slot stride
};

struct Owner {
    char pad[0x40];
    Slot* slots;                 // at offset 0x40
};

extern "C" {
u32 addObj__18tyFigDisp1RootTaskFv(Owner* self, u32 idx) {
    Obj* o = self->slots[idx].obj;
    return o->vf13(o->m_e4);
}
}
}

// TODO: clean up, give variables real names
namespace jj_85CC {
typedef unsigned char u8;
typedef unsigned int u32;

struct Obj {
    virtual void v0() = 0;
    virtual void v1() = 0;
    virtual void v2() = 0;
    virtual void v3() = 0;
    virtual void v4() = 0;
    virtual void v5() = 0;
    virtual void v6() = 0;
    virtual void v7() = 0;
    virtual void v8() = 0;
    virtual void v9() = 0;
    virtual void v10() = 0;
    virtual void v11() = 0;
    virtual void v12() = 0;
    virtual void target(int arg) = 0;
};

struct Entry {
    Obj* obj;
    u32 unk4;
};

struct Holder {
    u8 pad[0x40];
    Entry* entries;
};

extern "C" {
void removeObj__18tyFigDisp1RootTaskFv(Holder* self, int idx, int arg) {
    self->entries[idx].obj->target(arg);
}
}
}

// TODO: clean up, give variables real names
namespace jj_85EC {
struct Node {
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual void f3();
    virtual void f4();
    virtual void f5();
    virtual void f6();
    virtual void f7();
    virtual void f8();
    virtual void f9();
    virtual void f10();
    virtual void f11();
    virtual void setCount(int);

    char padding[0xE0];
    int count;
};

struct Entry {
    Node* node;
    unsigned int unused;
};

struct Menu {
    char padding[0x40];
    Entry* entries;
};

extern "C" void fn_2_85EC(Menu* menu, int index) {
    Node* node = menu->entries[index].node;
    while (node->count != 0) {
        if (node->count != 0) {
            node->setCount(node->count - 1);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_86B8 {

extern "C" {
extern char lbl_2_data_EF0[];
extern char lbl_2_data_12B8[];

void* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(void*, const char*, int, int, int, bool);
void fn_800B96E8(void*, int);
void __ct__14muFileIOHandleFv(void*);
void __dt__14muFileIOHandleFv(void*);
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
}

inline void clearWords(int* p, int* e) {
    do {
        *p = 0;
        p++;
    } while (p < e);
}

extern "C" char* __ct__14tyFigDisp2TaskFv(char* self) {
    __ct__6gfTaskFPCcQ26gfTask8Categoryiib(self, lbl_2_data_EF0, 8, 0xF, 8, true);
    *(int*)(self + 0x9C) = 0;
    *(char**)(self + 0x3C) = lbl_2_data_12B8;
    *(int*)(self + 0xA0) = 0;
    *(int*)(self + 0xA4) = 0;
    *(int*)(self + 0xA8) = 0;
    *(int*)(self + 0xAC) = 0;
    *(int*)(self + 0xB0) = 0;
    clearWords((int*)(self + 0xB4), (int*)(self + 0xC4));
    clearWords((int*)(self + 0xC4), (int*)(self + 0xD4));
    *(int*)(self + 0xE4) = 0;
    fn_800B96E8(self + 0x190, 0xFF);
    *(u8*)(self + 0x258) = 0;
    *(u8*)(self + 0x259) = 1;
    *(int*)(self + 0x25C) = 0;
    __construct_array(self + 0x264, __ct__14muFileIOHandleFv, __dt__14muFileIOHandleFv, 4, 0xC);
    *(int*)(self + 0x298) = 0;
    *(int*)(self + 0x29C) = 0;
    *(int*)(self + 0x2B8) = 0;
    *(int*)(self + 0x2BC) = 0;
    *(u8*)(self + 0x2E8) = 0;
    *(int*)(self + 0x2FC) = -1;
    *(u8*)(self + 0x2C) &= ~2;
    memset(self + 0x2A0, 0, 0x10);
    memset(self + 0x2B0, 0, 8);
    memset(self + 0x2C0, 0, 0x28);
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_89F8 {
typedef unsigned char u8;

class MenuObject {
    char pad[0x5C];

public:
    virtual void f0(int);
};

extern "C" {
extern u8 lbl_2_data_12B8[];
extern u8 __dt__14muFileIOHandleFv[];
extern int lbl_2_bss_38;

void exit__6gfTaskFv(void*);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void free__13gfHeapManagerFPv(void*);
void __dt__5MuMsgFv(void*, int);
void cancelRequest__14gfFileIOHandleFv(void*);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);

void* __dt__14tyFigDisp2TaskFv(u8* object, int deleting) {
    if (object) {
        void* p0 = *(void**)(object + 0x298);
        *(void**)(object + 0x3C) = lbl_2_data_12B8;
        if (p0) {
            exit__6gfTaskFv(p0);
        }

        void* p1 = *(void**)(object + 0x29C);
        if (p1) {
            Destroy__Q34nw4r3g3d6G3dObjFv(p1);
        }

        u8* cursor = object;
        for (int i = 0; i < 7; ++i, cursor += 4) {
            void* p = *(void**)(cursor + 0x168);
            if (p) {
                Destroy__Q34nw4r3g3d6G3dObjFv(p);
            }
        }

        cursor = object;
        for (int i = 0; i < 0x20; ++i, cursor += 4) {
            MenuObject* p = *(MenuObject**)(cursor + 0xE8);
            if (p && p) {
                p->f0(1);
            }
        }

        void* q;
        q = *(void**)(object + 0x9C);
        if (q) free__13gfHeapManagerFPv(q);
        q = *(void**)(object + 0xA4);
        if (q) free__13gfHeapManagerFPv(q);
        q = *(void**)(object + 0xA8);
        if (q) free__13gfHeapManagerFPv(q);
        q = *(void**)(object + 0xAC);
        if (q) free__13gfHeapManagerFPv(q);
        q = *(void**)(object + 0xB0);
        if (q) free__13gfHeapManagerFPv(q);
        q = *(void**)(object + 0xE4);
        if (q) free__13gfHeapManagerFPv(q);

        cursor = object;
        for (int i = 0; i < 4; ++i, cursor += 4) {
            void* p = *(void**)(cursor + 0xB4);
            if (p) free__13gfHeapManagerFPv(p);
        }

        q = *(void**)(object + 0xDC);
        if (q) free__13gfHeapManagerFPv(q);
        q = *(void**)(object + 0xE0);
        if (q) free__13gfHeapManagerFPv(q);

        q = *(void**)(object + 0x184);
        if (q) exit__6gfTaskFv(q);
        q = *(void**)(object + 0x188);
        if (q) exit__6gfTaskFv(q);

        q = *(void**)(object + 0x18C);
        if (q) Destroy__Q34nw4r3g3d6G3dObjFv(q);

        q = *(void**)(object + 0xD4);
        if (q) {
            free__13gfHeapManagerFPv(*(void**)((u8*)q + 4));
            __dt__5MuMsgFv(*(void**)(object + 0xD4), 1);
        }

        cursor = object;
        for (int i = 0; i < 4; ++i, cursor += 4) {
            free__13gfHeapManagerFPv(*(void**)(cursor + 0x2A0));
        }

        cursor = object;
        for (int i = 0; i < 2; ++i, cursor += 4) {
            free__13gfHeapManagerFPv(*(void**)(cursor + 0x2B0));
        }

        cursor = object + 0x264;
        for (int i = 0; i < 0xC; ++i, cursor += 4) {
            cancelRequest__14gfFileIOHandleFv(cursor);
        }

        lbl_2_bss_38 = 0;
        __destroy_arr(object + 0x264, (void (*)(void*, int))__dt__14muFileIOHandleFv, 4, 0xC);
        __dt__6gfTaskFv(object, 0);
        if (deleting > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}
}

// TODO: clean up, give variables real names
namespace jj_8C74 {
struct Data { u32 first; u32 rest[20]; };
struct Param { u8 kind; Data data; };
extern "C" void initWork__14tyFigDisp2TaskFv(void*);
extern "C" void fn_1_6D4C(void*, void*, int);
extern "C" void* fn_801AB6CC(MEMAllocator*, void*, int);
extern "C" void initProc__14tyFigDisp2TaskFv(u8* self, void* scene, Param* param) {
    int i;
    u8* p;
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator((Heaps::HeapType)0x2b);
    initWork__14tyFigDisp2TaskFv(self);
    *(void**)(self + 0x40) = scene;
    *(u32*)(self + 0x25c) = *(u32*)(self + 0x48) = param->data.first;
    fn_1_6D4C(scene, self + 0x108, 4);
    p = self;
    for (i = 0; i < 4; i++, p += 4)
        *(void**)(p + 0x2a0) = gfHeapManager::alloc((Heaps::HeapType)0x2b, 0x400000);
    p = self;
    for (i = 0; i < 2; i++, p += 4)
        *(void**)(p + 0x2b0) = gfHeapManager::alloc((Heaps::HeapType)0x2b, 0x132000);
    void* figure = itManager::getInstance()->m_figureManager;
    if (figure)
        *(u32*)(self + 0xd8) = *(u32*)((u8*)figure + 4);
    else
        *(u32*)(self + 0xd8) = 0;
    *(u8*)(self + 0x44) = param->kind;
    *(Data*)(self + 0x48) = param->data;
    *(u8*)(self + 0x44) = param->kind;
    *(u32*)(self + 0x254) = param->kind;
    *(void**)(self + 0x29c) = fn_801AB6CC(allocator, 0, 1);
    *(u32*)(self + 0x238) = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_8DB8 {
extern "C" void fn_800B96E8(void*, int);
extern "C" float lbl_2_rodata_2A0;
struct Flags { unsigned char flag : 1; unsigned char rest : 7; };
extern "C" void initWork__14tyFigDisp2TaskFv(u8* p) {
    *(u32*)(p + 0x40) = 0;
    *(u32*)(p + 0x9c) = 0;
    *(u32*)(p + 0xa0) = 0;
    *(u32*)(p + 0xa4) = 0;
    *(u32*)(p + 0xa8) = 0;
    *(u32*)(p + 0xe4) = 0;
    *(u32*)(p + 0xb0) = 0;
    *(u32*)(p + 0xe8) = 0;
    *(u32*)(p + 0xec) = 0;
    *(u32*)(p + 0xf0) = 0;
    *(u32*)(p + 0xf4) = 0;
    *(u32*)(p + 0xf8) = 0;
    *(u32*)(p + 0xfc) = 0;
    *(u32*)(p + 0x100) = 0;
    *(u32*)(p + 0x104) = 0;
    *(u32*)(p + 0x108) = 0;
    *(u32*)(p + 0x10c) = 0;
    *(u32*)(p + 0x110) = 0;
    *(u32*)(p + 0x114) = 0;
    *(u32*)(p + 0x118) = 0;
    *(u32*)(p + 0x11c) = 0;
    *(u32*)(p + 0x120) = 0;
    *(u32*)(p + 0x124) = 0;
    *(u32*)(p + 0x128) = 0;
    *(u32*)(p + 0x12c) = 0;
    *(u32*)(p + 0x130) = 0;
    *(u32*)(p + 0x134) = 0;
    *(u32*)(p + 0x138) = 0;
    *(u32*)(p + 0x13c) = 0;
    *(u32*)(p + 0x140) = 0;
    *(u32*)(p + 0x144) = 0;
    *(u32*)(p + 0x148) = 0;
    *(u32*)(p + 0x14c) = 0;
    *(u32*)(p + 0x150) = 0;
    *(u32*)(p + 0x154) = 0;
    *(u32*)(p + 0x158) = 0;
    *(u32*)(p + 0x15c) = 0;
    *(u32*)(p + 0x160) = 0;
    *(u32*)(p + 0x164) = 0;
    *(u32*)(p + 0x168) = 0;
    *(u32*)(p + 0x16c) = 0;
    *(u32*)(p + 0x170) = 0;
    *(u32*)(p + 0x174) = 0;
    *(u32*)(p + 0x178) = 0;
    *(u32*)(p + 0x17c) = 0;
    *(u32*)(p + 0x180) = 0;
    *(u32*)(p + 0x184) = 0;
    *(u32*)(p + 0xd4) = 0;
    *(u32*)(p + 0xd8) = 0;
    fn_800B96E8(p + 0x190, 0xf0);
    float zero = lbl_2_rodata_2A0;
    *(u32*)(p + 0x238) = 2;
    *(u32*)(p + 0x23c) = 0x11;
    p[0x44] = 0;
    ((Flags*)(p + 0x4c))->flag = 0;
    *(u32*)(p + 0x50) = 0;
    *(float*)(p + 0x54) = zero;
    *(float*)(p + 0x58) = zero;
    *(float*)(p + 0x5c) = zero;
    ((Flags*)(p + 0x60))->flag = 0;
    *(u32*)(p + 0x64) = 0;
    *(float*)(p + 0x68) = zero;
    *(float*)(p + 0x6c) = zero;
    *(float*)(p + 0x70) = zero;
    ((Flags*)(p + 0x74))->flag = 0;
    *(u32*)(p + 0x78) = 0;
    *(float*)(p + 0x7c) = zero;
    *(float*)(p + 0x80) = zero;
    *(float*)(p + 0x84) = zero;
    ((Flags*)(p + 0x88))->flag = 0;
    *(u32*)(p + 0x8c) = 0;
    *(float*)(p + 0x90) = zero;
    *(float*)(p + 0x94) = zero;
    *(float*)(p + 0x98) = zero;
    *(u32*)(p + 0x240) = 0;
    *(u32*)(p + 0x244) = 0;
    *(u32*)(p + 0x248) = -1;
    *(u32*)(p + 0x24c) = 0;
    p[0x250] = 0;
    *(u32*)(p + 0x188) = 0;
    *(u32*)(p + 0x18c) = 0;
    *(u32*)(p + 0x254) = 0;
    *(u32*)(p + 0x2ec) = 0;
    *(u32*)(p + 0x2f0) = 0;
    *(u32*)(p + 0x2f4) = 0;
    *(u32*)(p + 0x2f8) = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_9708 {
extern "C" {
void* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32, u32, u32);
void allocMsgBuf__5MuMsgFUlUl(void*, u32, u32);
void setMsgData__5MuMsgFPv(void*, void*);
void attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(void*, u32, void*, u32, float);
void getHScale__5MuMsgFUlf(void*, u32, float);
void setBufField34__7MessageFii(void*, u32, u32);
int getReturnStatus__14gfFileIOHandleCFv(const void*);
void* getBuffer__14gfFileIOHandleCFv(const void*);
void release__14gfFileIOHandleFv(void*);
extern float lbl_2_rodata_354;
extern float lbl_2_rodata_358;
}

extern "C" void initMsg__14tyFigDisp2TaskFv(u8* s) {
    void* data;
    void** holder;
    u8* p;
    int i;
    void* msg;
    msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, 0x2a, 0x2b);
    *reinterpret_cast<void**>(s + 0xd4) = msg;
    allocMsgBuf__5MuMsgFUlUl(msg, 0x100, 7);
    if (getReturnStatus__14gfFileIOHandleCFv(s + 0x284) == 0x15) {
        data = 0;
    } else {
        data = getBuffer__14gfFileIOHandleCFv(s + 0x284);
        release__14gfFileIOHandleFv(s + 0x284);
    }
    setMsgData__5MuMsgFPv(msg, data);

    holder = *reinterpret_cast<void***>(s + 0x118);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 0, holder[4], 0, lbl_2_rodata_354);
    getHScale__5MuMsgFUlf(msg, 0, lbl_2_rodata_358);
    setBufField34__7MessageFii(*reinterpret_cast<void**>(reinterpret_cast<u8*>(msg) + 8), 0, 7);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 6, holder[4], 1, lbl_2_rodata_354);
    setBufField34__7MessageFii(*reinterpret_cast<void**>(reinterpret_cast<u8*>(msg) + 8), 6, 7);

    p = s;
    for (i = 0; i < 4; i++) {
        void** h = *reinterpret_cast<void***>(p + 0x11c);
        attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, i + 1, h[4], 0, lbl_2_rodata_354);
        getHScale__5MuMsgFUlf(msg, i + 1, lbl_2_rodata_358);
        setBufField34__7MessageFii(*reinterpret_cast<void**>(reinterpret_cast<u8*>(msg) + 8), i + 1, 7);
        p += 4;
    }

    if (getReturnStatus__14gfFileIOHandleCFv(s + 0x288) == 0x15) {
        data = 0;
    } else {
        data = getBuffer__14gfFileIOHandleCFv(s + 0x288);
        release__14gfFileIOHandleFv(s + 0x288);
    }
    *reinterpret_cast<void**>(s + 0xdc) = data;
    if (getReturnStatus__14gfFileIOHandleCFv(s + 0x28c) == 0x15) {
        data = 0;
    } else {
        data = getBuffer__14gfFileIOHandleCFv(s + 0x28c);
        release__14gfFileIOHandleFv(s + 0x28c);
    }
    *reinterpret_cast<void**>(s + 0xe0) = data;
}
}

// TODO: clean up, give variables real names
namespace jj_9BC0 {
typedef unsigned char u8;
typedef int s32;

typedef void (*FnA)(void*);
typedef s32 (*FnB)(void*);

struct StateEntry {
    FnA a;
    FnB b;
};

extern "C" void fn_800B9984(void* arg);
extern "C" void figureBlinkMain__14tyFigDisp2TaskFv(void* self);
extern "C" StateEntry lbl_2_rodata_218[];

extern "C" void processDefault__14tyFigDisp2TaskFv(void* self) {
    u8* p = (u8*)self;
    fn_800B9984(p + 0x190);

    s32 cur = *(s32*)(p + 0x238);
    if (*(s32*)(p + 0x23c) != cur) {
        FnA fa = lbl_2_rodata_218[cur].a;
        if (fa) {
            fa(self);
        }
        *(s32*)(p + 0x23c) = *(s32*)(p + 0x238);
    }

    FnB fb = lbl_2_rodata_218[*(s32*)(p + 0x238)].b;
    if (fb) {
        *(s32*)(p + 0x238) = fb(self);
    }

    figureBlinkMain__14tyFigDisp2TaskFv(self);
}
}

// TODO: clean up, give variables real names
namespace jj_9F34 {
extern "C" void fn_1_6FDC(void*);

extern "C" void processAnim__14tyFigDisp2TaskFv(void* self) {
    fn_1_6FDC(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x40));
}
}

// TODO: clean up, give variables real names
namespace jj_A420 {
struct Vec3 {
    float x, y, z;
};

struct Flags {
    unsigned char active : 1;
    unsigned char rest : 7;
};

struct Matrix {
    float m[3][4];
};

extern "C" {
extern const float lbl_2_rodata_218[];
Matrix getFigStandNodeMatrix__14tyFigDisp2TaskFv(void*);
void getScalingFactor__FP6MatrixP5Vec3f(const Matrix*, Vec3*);
int getCollisionArea__14tyFigDisp2TaskFv(void*, float*, float*, float*, float*);
int correctFig__14tyFigDisp2TaskFv(void*, Vec3*, int, float, float);
}

extern "C" int findFigAppearPosX__14tyFigDisp2TaskFv(void* object, float* result, int index, float value) {
    const float* constants = lbl_2_rodata_218;
    Vec3 position;
    Matrix matrix = getFigStandNodeMatrix__14tyFigDisp2TaskFv(object);
    Vec3 scale;
    getScalingFactor__FP6MatrixP5Vec3f(&matrix, &scale);

    char* entries = **(char***)((char*)object + 0xd8);
    char* entry = (char*)object + index * 0x14;
    int selected = ((Flags*)(entry + 0x4c))->active
        ? *(int*)(entry + 0x50)
        : -1;

    float scaleX = scale.x;
    float width = *(float*)(entries + selected * 0x60 + 0x38);
    float step = constants[0x154 / 4] * (width * scaleX);

    float left, top, right, bottom;
    if (getCollisionArea__14tyFigDisp2TaskFv(object, &left, &top, &right, &bottom) == 0) {
        left = constants[0x15c / 4];
        right = constants[0x160 / 4];
    }

    float distance = constants[0x88 / 4];
    for (; distance < right - step; distance += step) {
        if (correctFig__14tyFigDisp2TaskFv(object, &position, index, distance, value) == 0) {
            *result = distance;
            return 1;
        }
        if (distance > constants[0x88 / 4]) {
            if (correctFig__14tyFigDisp2TaskFv(object, &position, index, -distance, value) == 0) {
                *result = -distance;
                return 1;
            }
        }
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_B048 {

extern "C" void* GetResVtxPos__Q34nw4r3g3d6ResMdlCFi(void* handle, int index);
extern "C" s32 GetResVtxPosNumEntries__Q34nw4r3g3d6ResMdlCFv(void* handle, void* obj);

extern "C" s32 getCollisionArea__14tyFigDisp2TaskFv(char* self, f32* arg1, f32* arg2, f32* arg3, f32* arg4) {
    void* sp8;
    Vec a;
    Vec b;
    char* base;
    char* p;
    char* obj;
    s32 count;
    s32 i;
    char* q;

    sp8 = 0;
    base = self + (*(s32*)(self + 0x2BC) + 2) * 4;
    p = *(char**)(base + 0xE8);
    obj = *(char**)(p + 0x10);
    sp8 = *(void**)(obj + 0xE8);
    count = GetResVtxPosNumEntries__Q34nw4r3g3d6ResMdlCFv(&sp8, obj);
    if (count == 0) {
        return 0;
    }
    for (i = 0; (u32)i < (u32)count; i++) {
        q = (char*)GetResVtxPos__Q34nw4r3g3d6ResMdlCFi(&sp8, i);
        a = *(Vec*)(q + 0x20);
        b = *(Vec*)(q + 0x2C);
        if (i <= 0) {
            *arg1 = a.x;
            *arg2 = a.z;
            *arg3 = b.x;
            *arg4 = b.z;
        } else {
            if (a.x < *arg1) {
                *arg1 = a.x;
            }
            if (b.x > *arg3) {
                *arg3 = b.x;
            }
            if (a.z < *arg2) {
                *arg2 = a.z;
            }
            if (b.z > *arg4) {
                *arg4 = b.z;
            }
        }
    }
    return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_B918 {

static inline unsigned char readControllerMap(const unsigned char* map, int index)
{
    return index[map];
}

extern "C" {
extern const float lbl_2_rodata_218[];
void fn_1_6DB4(void*, void*);
void fn_1_6DA0(void*, void*);
void setTrans__8MuObjectFP5Vec3f(void*, const float*);
}

extern "C" void setPhotoHelpVisible__14tyFigDisp2TaskFv(char* self, int flag)
{
    gfPadStatus status;
    const char* base = (const char*)lbl_2_rodata_218;
    unsigned int index;

    fn_1_6DB4(*(void**)(self + 0x40),
              *(void**)(*(char**)(self + 0x14c) + 0x10));

    const unsigned char* table = (const unsigned char*)(base + 0x17c);
    for (unsigned int i = 0; i < 4; ++i, ++table) {
        fn_1_6DB4(*(void**)(self + 0x40),
                  *(void**)(*(char**)(self + 0xe8 + *table * 4) + 0x10));
    }

    fn_1_6DA0(*(void**)(self + 0x40),
              *(void**)(*(char**)(self + 0x14c) + 0x10));

    g_gfPadSystem->getSysPadStatus(*(int*)(self + 0x25c), &status);

    void* context = *(void**)(self + 0x40);
    table = (const unsigned char*)(base + 0x17c);
    unsigned int padType = status.m_controllerType;
    table += padType;
    index = *table;

    fn_1_6DA0(context,
              *(void**)(*(char**)(self + 0xe8 + index * 4) + 0x10));

    void* object = *(void**)(self + 0xe8 + (unsigned char)index * 4);
    if (flag) {
        float pos[3];
        pos[0] = *(const float*)(base + 0x88);
        pos[1] = *(const float*)(base + 0x88);
        pos[2] = *(const float*)(base + 0x88);
        setTrans__8MuObjectFP5Vec3f(object, pos);
    } else {
        float pos[3];
        pos[0] = *(const float*)(base + 0x88);
        pos[1] = *(const float*)(base + 0x180);
        pos[2] = *(const float*)(base + 0x88);
        setTrans__8MuObjectFP5Vec3f(object, pos);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_BA2C {
typedef unsigned char u8;
extern "C" {
void readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(void* self, const char* name, int heap, int a, int b);
void readRequest__14gfFileIOHandleFPCcPvii(void* self, const char* name, void* buf, int a, int b);
void* getGlobalRecordMenuDatap__10GameGlobalFv(void* global);
int sprintf(char*, const char*, ...);
extern void* g_GameGlobal;
extern char lbl_2_data_EF0[];

void fn_2_BA2C(u8* self) {
    char* base = lbl_2_data_EF0;
    char buf[64];
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x264, base + 0xc, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x268, base + 0x30, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x26c, base + 0x48, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x270, base + 0x68, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x274, base + 0x80, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x278, base + 0x9c, 0x2b, 0, 0);
    u8* rec = (u8*)getGlobalRecordMenuDatap__10GameGlobalFv(g_GameGlobal);
    u8* handle = self + 0x280;
    char* name = base + 0xf4;
    if ((rec[0x28] >> 7) & 1) {
        name = base + 0xc4;
    }
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(handle, name, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x284, base + 0x120, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x288, base + 0x140, 0x2b, 0, 0);
    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x28c, base + 0x160, 0x2b, 0, 0);
    sprintf(buf, base + 0x2c8, self[0x44]);
    readRequest__14gfFileIOHandleFPCcPvii(self + 0x290, buf, *(void**)(self + *(int*)(self + 0x2b8) * 4 + 0x2b0), 0, 0);
    int n = *(int*)(self + 0x2b8) + 1;
    *(int*)(self + 0x2b8) = n;
    if (n >= 2) {
        *(int*)(self + 0x2b8) = 0;
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_BBC4 {
extern "C" {
int isReady__14gfFileIOHandleCFv(const void*);
int isLoadFinish__20tyFigListDataManagerFv(void*);
int getReturnStatus__14gfFileIOHandleCFv(const void*);
void* getBuffer__14gfFileIOHandleCFv(const void*);
void release__14gfFileIOHandleFv(void*);
void Init__Q34nw4r3g3d7ResFileFPv(void*);
void* GetResAnmScn__Q34nw4r3g3d7ResFileFi(void*, int);
void Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(void*, void*);
void fn_1_6FD0(int, void*);
void fn_1_6E30(int);
void fn_1_6DA0(int, int);
void fn_2_8F7C(void*);
void changeBG__14tyFigDisp2TaskFv(void*, int, void*);
void initMsg__14tyFigDisp2TaskFv(void*);
void setAnimIdx__8MuObjectFP13MuAnimIdxDatab(int, void*, int);
void endKeepScreen__17gfKeepFrameBufferFv(void*);
extern char* g_gfApplication;
extern char lbl_2_data_127C[];
}

extern "C" int fn_2_BBC4(void* arg0) {
    char* self = (char*)arg0;
    void* buf;
    int i = 0;
    char* h = self + 0x264;
    do {
        if (!isReady__14gfFileIOHandleCFv(h)) {
            return 0;
        }
        i++;
        h += 4;
    } while (i < 11);

    if (!isLoadFinish__20tyFigListDataManagerFv(*(void**)(self + 0xD8))) {
        return 0;
    }
    if (!isReady__14gfFileIOHandleCFv(self + 0x290)) {
        return 0;
    }

    void* sp10;
    void* spC;
    int sp8;
    fn_2_8F7C(self);
    sp8 = 0;
    buf = 0;
    fn_1_6E30(*(int*)(self + 0x40));
    spC = buf;
    if (getReturnStatus__14gfFileIOHandleCFv(self + 0x280) == 0x15) {
        buf = 0;
    } else {
        buf = getBuffer__14gfFileIOHandleCFv(self + 0x280);
        release__14gfFileIOHandleFv(self + 0x280);
    }
    if (buf != 0) {
        spC = buf;
        Init__Q34nw4r3g3d7ResFileFPv(&spC);
    }
    *(void**)(self + 0xE4) = spC;

    void* r = GetResAnmScn__Q34nw4r3g3d7ResFileFi(self + 0xE4, 0);
    sp8 = (int)r;
    sp10 = r;
    Bind__Q34nw4r3g3d9ResAnmScnFQ34nw4r3g3d9ResAnmScn(&sp8, &sp10);
    fn_1_6FD0(*(int*)(self + 0x40), &sp8);
    initMsg__14tyFigDisp2TaskFv(self);

    int n = *(int*)(self + 0x2B8) - 1;
    if (n < 0) {
        n = 1;
    }
    *(void**)(self + 0xA0) = *(void**)(self + 0x2B0 + n * 4);
    Init__Q34nw4r3g3d7ResFileFPv(self + 0xA0);
    release__14gfFileIOHandleFv(self + 0x290);

    unsigned char* u = (unsigned char*)self;
    int cnt = 0;
    if (((u[0x4C]) >> 7) & 1) {
        cnt = 1;
    }
    if (((u[0x60]) >> 7) & 1) {
        cnt += 1;
    }
    if (((u[0x74]) >> 7) & 1) {
        cnt += 1;
    }
    if (((u[0x88]) >> 7) & 1) {
        cnt += 1;
    }
    if (cnt > 0) {
        return 1;
    }

    endKeepScreen__17gfKeepFrameBufferFv(*(char**)&g_gfApplication + 0xD0);
    fn_1_6DA0(*(int*)(self + 0x40), *(int*)(self + 0x178));
    changeBG__14tyFigDisp2TaskFv(self, *(int*)(self + 0x254), self + 0xA0);
    fn_1_6DA0(*(int*)(self + 0x40), *(int*)(self + 0x29C));

    char* p = self;
    for (int j = 0; j < 4; j++) {
        setAnimIdx__8MuObjectFP13MuAnimIdxDatab(*(int*)(p + 0x108), lbl_2_data_127C, 0);
        p += 4;
    }
    return 2;
}
}

// TODO: clean up, give variables real names
namespace jj_BDDC {
extern "C" int sprintf(char*, const char*, ...);

class gfFileIOHandle {
public:
    void readRequest(const char*, void*, int, int);
};

extern const char lbl_2_data_1238[];

extern "C" void mainStepInitialFigLoadInit__14tyFigDisp2TaskFv(char* self) {
    char buffer[0x40];
    int i = 0;
    char* base = self;
    int n;
    self += 0x4C;
    for (n = 4; n > 0; n--) {
        if (((*(unsigned char*)self) >> 7) & 1) {
            int idx;
            char* entry = *(char**)(base + 0xD8);
            char* e = base + i * 0x14;
            char* table = *(char**)entry;
            if (((*(unsigned char*)(e + 0x4C)) >> 7) & 1) {
                idx = *(int*)(e + 0x50);
            } else {
                idx = -1;
            }
            char* item = table + idx * 0x60;
            sprintf(buffer, lbl_2_data_1238, *(const char**)(item + 8));
            ((gfFileIOHandle*)(base + 0x290))->readRequest(buffer, *(void**)(base + 0x2A0 + i * 4), 0, 0);
            *(int*)(base + 0x294) = i;
            return;
        }
        self += 0x14;
        i++;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_C1F8 {

extern char lbl_2_data_1234[4];

struct Vec2 {
    float x, y;
    Vec2() {}
    Vec2(const Vec2& other) { x = other.x; y = other.y; }
};
struct PadCopy {
    int words[6];
    Vec2 vectors[3];
    unsigned char b0, b1, b2, b3, b4, b5, b6, b7, b8;
    int last;
};

static inline gfPadStatus getSysPad() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0xf0, &status);
    return status;
}

extern "C" int mainStepNoFigureMain__14tyFigDisp2TaskFv(char* self) {
    PadCopy status(*(PadCopy*)&getSysPad());
    if ((status.words[3] != 0) || (*(int*)(self + 0x19c) != 0)) {
        MuMsg* msg = *(MuMsg**)(self + 0xd4);
        msg->printf(6, lbl_2_data_1234);
        if (status.words[3] & 0x1100) {
            return 4;
        }
        return 3;
    }
    return *(int*)(self + 0x238);
}
}

// TODO: clean up, give variables real names
namespace jj_C398 {
typedef unsigned char u8;
typedef unsigned int u32;

extern "C" {
void* getInstance__16CameraControllerFv();
void* __dynamic_cast(void*, long, const void*, const void*, short);
void fn_2_9A84(void*, u32, u32);
extern const char lbl_2_data_1360[];
extern const char __RTTI__18gfCameraController[];
}

extern "C" void mainStepSelCommandInit__14tyFigDisp2TaskFv(u8* self) {
    char* cam = (char*)getInstance__16CameraControllerFv();
    char* p = (char*)__dynamic_cast(*(void**)(cam + 0x48), 4, lbl_2_data_1360,
                                    __RTTI__18gfCameraController, 0);
    fn_2_9A84(self, *(u32*)(self + 0x240), *(u32*)(self + 0x244));
    self[0x230] = 1;
    *(u32*)(self + 0x2fc) = *(u32*)(p + 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_C8C0 {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__14tyFigTableTaskFv(void*, void*, int, int, int);
void drawListPage__14tyFigTableTaskFv(void*);
void setFigureData__14tyFigTableTaskFv(void*, unsigned int, int);
void fn_1_6DB4(void*, void*);
}

struct Class_728_base {
    char pad[0x3c];
};

class Class_728 : public Class_728_base {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    char pad[0x1CC];
    unsigned short owned[1];
};

struct Entry {
    unsigned char flag;
    char pad1[3];
    unsigned int idx;
    char pad2[0xC];
};

struct Self {
    char pad0[0x40];
    void* f40;
    char pad44[0x4C - 0x44];
    Entry entries[4];
    char padA0[0xA4 - 0x9C];
    char sub0A4[0xD8 - 0xA4];
    int fD8;
    int fDC;
    int fE0;
    char padE4[0x178 - 0xE4];
    void* f178;
    char pad17C[0x184 - 0x17C];
    Class_728* f184;
    char pad188[0x248 - 0x188];
    unsigned int f248;
    int f24C;
};

extern "C" void mainStepInFigListInit__14tyFigDisp2TaskFv(Self* self) {
    int dc;
    int e0;
    int d8;
    d8 = self->fD8;
    e0 = self->fE0;
    dc = self->fDC;
    Class_728* obj = (Class_728*)__nw__FUlQ25Heaps8HeapType(0x720, 0x2A);
    if (obj != 0) {
        obj = (Class_728*)__ct__14tyFigTableTaskFv(obj, self->sub0A4, dc, e0, d8);
    }
    obj->slot21();
    self->f184 = obj;
    Entry* e = self->entries;
    char* p = (char*)self;
    for (int i = 0; i < 4; i++) {
        unsigned int idx;
        if ((e->flag >> 7) & 1) {
            idx = *(unsigned int*)(p + 0x50);
        } else {
            idx = 0xFFFFFFFF;
        }
        if (idx != 0xFFFFFFFF) {
            Class_728* o = self->f184;
            if (o->owned[idx] != 0) {
                o->owned[idx] = o->owned[idx] - 1;
                drawListPage__14tyFigTableTaskFv(o);
            }
        }
        e++;
        p += 0x14;
    }
    if (self->f248 != 0xFFFFFFFF) {
        setFigureData__14tyFigTableTaskFv(self->f184, self->f248, self->f24C);
    }
    fn_1_6DB4(self->f40, self->f178);
}
}

// TODO: clean up, give variables real names
namespace jj_C9D4 {
extern "C" {
void fn_1_6DA0(void*, void*);
void exit__6gfTaskFv(void*);

int mainStepInFigListMain__14tyFigDisp2TaskFv(void* self)
{
    char* s = (char*)self;
    char* obj = *(char**)(s + 0x184);
    int state = *(int*)(obj + 0x44);
    switch (state)
    {
    case 3:
        exit__6gfTaskFv(obj);
        *(char**)(s + 0x184) = 0;
        fn_1_6DA0(*(char**)(s + 0x40), *(char**)(s + 0x178));
        return *(unsigned char*)(s + 0x250) ? 5 : 3;
    case 0:
        break;
    case 1:
    {
        int idx = *(int*)(obj + 0x140);
        *(int*)(s + 0x248) = *(int*)(obj + idx * 4 + 0x50);
        *(int*)(s + 0x24c) = *(int*)(obj + 0x12c);
        *(unsigned char*)(s + 0x250) = 1;
        break;
    }
    case 2:
        *(unsigned char*)(s + 0x250) = 0;
        break;
    default:
        break;
    }
    return *(int*)(s + 0x238);
}
}
}

// TODO: clean up, give variables real names
namespace jj_CB10 {
extern "C" bool isReady__14gfFileIOHandleCFv(const void*);
extern "C" void Init__Q34nw4r3g3d7ResFileFPv(void*);
extern "C" void appearFig__14tyFigDisp2TaskFv(void*, int, int, void*);
extern "C" void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);
extern "C" void release__14gfFileIOHandleFv(void*);
extern void* g_sndSystem;

extern "C" int mainStepLoadFigureMain__14tyFigDisp2TaskFv(void* self) {
    char* p = (char*)self;
    if (!isReady__14gfFileIOHandleCFv(p + 0x290)) return 5;

    int i = *(int*)(p + 0x244);
    char* q = p + i * 4;
    int x = *(int*)(q + 0x2A0);
    *(int*)(q + 0xC4) = x;

    i = *(int*)(p + 0x244);
    q = p + i * 4;
    Init__Q34nw4r3g3d7ResFileFPv(q + 0xC4);

    i = *(int*)(p + 0x244);
    appearFig__14tyFigDisp2TaskFv(self, i, *(int*)(p + 0x248), p + i * 4 + 0xC4);
    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1EE6, -1, 0, 0, -1);
    release__14gfFileIOHandleFv(p + 0x290);
    return 3;
}
}

// TODO: clean up, give variables real names
namespace jj_CBC0 {

extern "C" {
void* __dynamic_cast(void*, long, const void*, const void*, long);
void fn_1_6D80(void*, void*);
void setTrans__8MuObjectFP5Vec3f(MuObject*, const Vec3f*);
extern const char lbl_2_data_1360[];
extern const char __RTTI__18gfCameraController[];
extern float lbl_2_rodata_2A0;
extern unsigned char lbl_2_rodata_38C[];
}
inline void setFlag(void* p) { *(int*)((char*)p + 12) = 1; }
struct Entry {
    float x;
    float y;
    char pad[12];
};
inline void setPosition(Vec3f* p, float x, float z) { p->m_x = x; p->m_y = lbl_2_rodata_2A0; p->m_z = z; }
struct Menu {
    char pad0[0x40];
    void* field40;
    char pad44[0x10];
    Entry entries[1];
    char pad68[0xe8 - 0x68];
    MuObject* objects[18];
    MuObject* object130;
    char pad134[0x48];
    void* field17c;
    char pad180[0xc4];
    int index;
};
extern "C" void mainStepMoveFigInit__14tyFigDisp2TaskFv(Menu* self) {
    char* cam = (char*)CameraController::getInstance();
    setFlag(__dynamic_cast(*(void**)(cam + 0x48), 4, lbl_2_data_1360, __RTTI__18gfCameraController, 0));
    fn_1_6D80(self->field40, self->field17c);
    Entry* entry = &self->entries[self->index];
    Vec3f pos;
    float x = entry->x;
    float z = entry->y;
    setPosition(&pos, x, z);
    setTrans__8MuObjectFP5Vec3f(self->object130, &pos);
    float zero = lbl_2_rodata_2A0;
    MuObject* obj;
    unsigned char* p = lbl_2_rodata_38C;
    int i = 0;
    for (; i < 4; ++i, ++p) {
        obj = self->objects[*p];
        if (zero != obj->m_modelAnim->m_anmObjChrRes->GetUpdateRate()) {
            obj->m_modelAnim->m_anmObjChrRes->SetUpdateRate(zero);
            obj->setFrameNode(lbl_2_rodata_2A0);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_D3E8 {
extern "C" {
void* __dynamic_cast(void*, long, const void*, const void*, long);
void setFigSlotIconCursorState__14tyFigDisp2TaskFv(void*, int, int);
extern char lbl_2_data_1360[];
extern char __RTTI__18gfCameraController[];
extern float lbl_2_rodata_360;
}
extern "C" void mainStepClearFigInit__14tyFigDisp2TaskFv(char* self) {
    char* cam = (char*)CameraController::getInstance();
    char* p = (char*)__dynamic_cast(*(void**)(cam + 0x48), 4, lbl_2_data_1360, __RTTI__18gfCameraController, 0);
    int index = -1;
loop:
    index++;
    if (index >= 4) index = 0;
    if (index == -1) {
        index = -1;
    } else if (!(((unsigned char)self[index * 0x14 + 0x4c] >> 7) & 1)) {
        goto loop;
    }
    *(int*)(self + 0x244) = index;
    for (int i = 0; i < 4; i++) {
        setFigSlotIconCursorState__14tyFigDisp2TaskFv(self, i, 0);
    }
    setFigSlotIconCursorState__14tyFigDisp2TaskFv(self, index, 1);
    MuObject* obj = *(MuObject**)(self + 0x118);
    float frame = lbl_2_rodata_360;
    obj->setFrameVisible(frame);
    obj->setFrameNode(frame);
    *(char*)(self + 0x230) = 1;
    *(int*)(self + 0x2fc) = *(int*)(p + 0x40);
}
}

// TODO: clean up, give variables real names
namespace jj_DA98 {
extern "C" void fn_1_6DB4(void*, void*);
extern "C" void fn_1_6DA0(void*, void*);
extern "C" void setFrameNode__8MuObjectFf(void*, float);
extern "C" void setFrameVisible__8MuObjectFf(void*, float);
extern "C" float lbl_2_rodata_2A0;
extern "C" float lbl_2_rodata_350;

class Base {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void setFrame(float);
};

struct Inner {
    char pad[0xc];
    Base* base;
};

struct MuObject {
    char pad[0x14];
    Inner* inner;
};

struct Self {
    char pad0[0x40];
    void* f40;
    unsigned char flag;
    char pad1[0x144 - 0x45];
    MuObject* obj144;
    MuObject* obj148;
    char pad2[0x178 - 0x14C];
    void* f178;
    char pad2b[0x180 - 0x17C];
    void* f180;
    char pad3[0x254 - 0x184];
    int field254;
};

extern "C" void mainStepOpenBGListInit__14tyFigDisp2TaskFv(Self* self) {
    fn_1_6DB4(self->f40, self->f178);
    MuObject* obj = self->obj144;
    setFrameNode__8MuObjectFf(obj, lbl_2_rodata_2A0);
    obj->inner->base->setFrame(lbl_2_rodata_350);
    int idx = self->flag;
    self->field254 = idx;
    MuObject* obj2 = self->obj148;
    float f = (float)idx;
    setFrameNode__8MuObjectFf(obj2, f);
    setFrameVisible__8MuObjectFf(obj2, f);
    fn_1_6DA0(self->f40, self->f180);
}
}

// TODO: clean up, give variables real names
namespace jj_DB68 {
extern "C" void setFrameNode__8MuObjectFf(void* obj, float frame);

extern "C" float lbl_2_rodata_3D0;
extern "C" float lbl_2_rodata_2A0;

class Base {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual float getFrame();
    virtual void v7();
    virtual void setFrame(float f);
};

struct Inner {
    char pad[0xc];
    Base* base;
};

struct Obj {
    char pad[0x14];
    Inner* inner;
};

struct Self {
    char pad[0x144];
    Obj* obj;
    char pad2[0x238 - 0x148];
    int field238;
};

extern "C" int mainStepOpenBGListMain__14tyFigDisp2TaskFv(Self* self) {
    Obj* obj = self->obj;
    float f = obj->inner->base->getFrame();
    int result;
    if (f >= lbl_2_rodata_3D0) {
        setFrameNode__8MuObjectFf(obj, lbl_2_rodata_3D0);
        obj->inner->base->setFrame(lbl_2_rodata_2A0);
        result = 10;
    } else {
        result = self->field238;
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_DC00 {
struct Target {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void setValue(float);
};

struct ObjectData {
    char pad[0x0C];
    Target* target;
};

struct MuObject {
    char pad[0x14];
    ObjectData* data;
    void setFrameNode(float);
};

extern "C" {
extern float lbl_2_rodata_3D0;
extern float lbl_2_rodata_2A0;

void mainStepInBGListInit__14tyFigDisp2TaskFv(MuObject* object) {
    object = *(MuObject**)((char*)object + 0x144);
    object->setFrameNode(lbl_2_rodata_3D0);
    object->data->target->setValue(lbl_2_rodata_2A0);
}
}
}

// TODO: clean up, give variables real names
namespace jj_DC58 {

extern "C" void initRepeatCounter__16muMenuControllerFi(void*, u32);

static inline gfPadStatus getSysPad() {
    gfPadStatus status;
    g_gfPadSystem->getSysPadStatus(0xF0, &status);
    return status;
}

static inline void copyPadStatus(gfPadStatus& destination, const gfPadStatus& source) {
    destination.m_buttonsCurrentFrame = source.m_buttonsCurrentFrame;
    destination.m_buttonsCurrentFrame2 = source.m_buttonsCurrentFrame2;
    destination.m_buttonsHeld = source.m_buttonsHeld;
    destination.m_buttonsPressedThisFrame = source.m_buttonsPressedThisFrame;
    destination.m_buttonsReleasedThisFrame = source.m_buttonsReleasedThisFrame;
    destination.m_buttonsPressedThisFrame2 = source.m_buttonsPressedThisFrame2;
    destination._0x18 = source._0x18;
    destination._0x1c = source._0x1c;
    destination._0x20 = source._0x20;
    destination._0x24 = source._0x24;
    destination._0x28 = source._0x28;
    destination._0x2c = source._0x2c;
    destination.m_stickX = source.m_stickX;
    destination.m_stickY = source.m_stickY;
    destination.m_subStickX = source.m_subStickX;
    destination.m_subStickY = source.m_subStickY;
    destination.m_lTriggerAnalog = source.m_lTriggerAnalog;
    destination.m_rTriggerAnalog = source.m_rTriggerAnalog;
    destination._0x36 = source._0x36;
    destination._0x37 = source._0x37;
    destination.m_error = source.m_error;
    destination.m_controllerType = source.m_controllerType;
}

extern "C" int mainStepInBGListMain__14tyFigDisp2TaskFv(char* self) {
    int flags = *(int*)(self + 0x1A0);
    gfPadStatus status;
    copyPadStatus(status, getSysPad());
    int old = *(int*)(self + 0x254);
    int col = old % 2;
    int row = old / 2;
    if (flags & 2) {
        col = col < 1 ? col + 1 : 0;
        if (col == 1) initRepeatCounter__16muMenuControllerFi(self + 0x190, 2);
    }
    if (flags & 1) {
        col = col > 0 ? col - 1 : 1;
        if (col == 0) initRepeatCounter__16muMenuControllerFi(self + 0x190, 1);
    }
    if (flags & 4) {
        row = row < 1 ? row + 1 : 0;
        if (row == 1) initRepeatCounter__16muMenuControllerFi(self + 0x190, 4);
    }
    if (flags & 8) {
        row = row > 0 ? row - 1 : 1;
        if (row == 0) initRepeatCounter__16muMenuControllerFi(self + 0x190, 8);
    }
    MuObject* object;
    int selected = col + row * 2;
    if (selected != old) {
        g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
        object = *(MuObject**)(self + 0x148);
        float frame = selected;
        object->setFrameNode(frame);
        object->setFrameVisible(frame);
        *(int*)(self + 0x254) = (u8)selected;
    }
    if (status.m_buttonsPressedThisFrame.bits & 0x1300) {
        if (status.m_buttonsPressedThisFrame.bits & 0x1100) {
            *(u8*)(self + 0x44) = selected;
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
            return 0xB;
        }
        g_sndSystem->playSE((SndID)2, -1, 0, 0, -1);
        return 0xC;
    }
    return *(int*)(self + 0x238);
}
}

// TODO: clean up, give variables real names
namespace jj_E224 {
struct Target {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void setValue(float);
};

struct ObjectData {
    char pad[0x0C];
    Target* target;
};

struct MuObject {
    char pad[0x14];
    ObjectData* data;
    void setFrameNode(float);
};

extern "C" {
extern float lbl_2_rodata_3D4;
extern float lbl_2_rodata_350;

void mainStepCloseBGListInit__14tyFigDisp2TaskFv(MuObject* object) {
    object = *(MuObject**)((char*)object + 0x144);
    object->setFrameNode(lbl_2_rodata_3D4);
    object->data->target->setValue(lbl_2_rodata_350);
}
}
}

// TODO: clean up, give variables real names
namespace jj_E27C {
extern "C" void setFrameNode__8MuObjectFf(void* obj, float frame);
extern "C" void fn_1_6DA0(void* a, void* b);
extern "C" void fn_1_6DB4(void* a, void* b);

extern "C" float lbl_2_rodata_2A0;
extern "C" float lbl_2_rodata_3D8;

class Base {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual float getFrame();
    virtual void v7();
    virtual void setFrame(float f);
};

struct Inner {
    char pad[0xc];
    Base* base;
};

struct Obj {
    char pad[0x14];
    Inner* inner;
};

struct Self {
    char pad0[0x40];
    void* f40;
    char pad1[0x144 - 0x44];
    Obj* obj;
    char pad2[0x178 - 0x148];
    void* f178;
    char pad2b[0x180 - 0x17C];
    void* f180;
    char pad3[0x238 - 0x184];
    int field238;
};

extern "C" int mainStepCloseBGListMain__14tyFigDisp2TaskFv(Self* self) {
    Obj* obj = self->obj;
    float f = obj->inner->base->getFrame();
    int result;
    if (f >= lbl_2_rodata_3D8) {
        setFrameNode__8MuObjectFf(obj, lbl_2_rodata_3D8);
        obj->inner->base->setFrame(lbl_2_rodata_2A0);
        fn_1_6DB4(self->f40, self->f180);
        fn_1_6DA0(self->f40, self->f178);
        result = 3;
    } else {
        result = self->field238;
    }
    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_EC88 {
extern "C" {
void* create__14IfSnapSaveTaskFPQ34nw4r3g3d7ResFilePQ34nw4r3g3d7ResFilePvPQ34nw4r3g3d8ScnGroupiQ25Heaps8HeapTypeQ25Heaps8HeapTypeQ25Heaps8HeapType(void* a, void* b, void* c, void* d, int e, int f, int g, int h);
void fn_1_6DA0(void*, void*);
void mainStepSnapSaveInit__14tyFigDisp2TaskFv(char* s);
}

void mainStepSnapSaveInit__14tyFigDisp2TaskFv(char* s) {
    *(char*)(*(char**)(*(char**)(s + 0x40) + 0x8) + 0xc) = 1;
    *(void**)(s + 0x188) = create__14IfSnapSaveTaskFPQ34nw4r3g3d7ResFilePQ34nw4r3g3d7ResFilePvPQ34nw4r3g3d8ScnGroupiQ25Heaps8HeapTypeQ25Heaps8HeapTypeQ25Heaps8HeapType(s + 0xa8, s + 0xb0, *(void**)(s + 0xac), *(void**)(s + 0x18c), 1, 0x2a, 0x2b, 0xff);
    fn_1_6DA0(*(void**)(s + 0x40), *(void**)(s + 0x18c));
}
}

// TODO: clean up, give variables real names
namespace jj_ECF4 {
extern "C" {
int fn_800F2894(void*);
void exit__6gfTaskFv(void*);
void fn_1_6DB4(void*, void*);

int mainStepSnapSaveMain__14tyFigDisp2TaskFv(void* self) {
    void* ptr = *(void**)((unsigned char*)self + 0x188);
    if (fn_800F2894(ptr)) {
        exit__6gfTaskFv(ptr);
        *(void**)((unsigned char*)self + 0x188) = 0;
        fn_1_6DB4(*(void**)((unsigned char*)self + 0x40), *(void**)((unsigned char*)self + 0x18c));
        void* c = *(void**)((unsigned char*)*(void**)((unsigned char*)self + 0x40) + 8);
        *(unsigned char*)((unsigned char*)c + 0xc) = 0;
        return 0xe;
    }
    return *(int*)((unsigned char*)self + 0x238);
}
}
}

// TODO: clean up, give variables real names
namespace jj_EDE0 {
extern "C" {
void exit__6gfTaskFv(void* t);
void* fn_1_7058(void* a, const void* b);
void* getInstance__14gfSceneManagerFv();
void playSE__9sndSystemF5SndIDiiii(void* s, int, int, int, int, int);
extern void* g_sndSystem;
}

struct TaskObj {
    char pad0[0x100];
    u8 flag100;
    char pad1[0x3];
    s32 status104;
};

struct Holder {
    char pad0[0x298];
    TaskObj* task298;
};

extern "C" s32 mainStepExitConfirmMain__14tyFigDisp2TaskFv(char* self) {
    Holder* h = (Holder*)self;
    TaskObj* task = h->task298;
    if (task->status104 == 3) {
        u8 b = task->flag100;
        exit__6gfTaskFv(task);
        h->task298 = NULL;
        if (b) {
            void* obj = fn_1_7058(*(void**)(self + 0x40), self + 0x44);
            void* mgr = getInstance__14gfSceneManagerFv();
            *(s32*)((char*)mgr + 0x284) = 0;
            *(s32*)((char*)mgr + 0x288) = 2;
        }
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 6, -1, 0, 0, -1);
        return 3;
    }
    return *(s32*)(self + 0x238);
}
}

