#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <mu/wifi/mu_wifi_data_load_task.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <types.h>

// The window of the collection viewer that sends a collection to the server that the other players download it from
// (module sora_menu_collect_viewer). The class is only described by the members the decompiled functions touch.
// Its functions have no names in the map of the module (the names here are made of the address of the function).
// something of the save data that the window asks about (HYPOTHESIS: if the contribution is already made)
extern "C" int fn_80154DF8();

class muCollectCntrbWndTask : public gfTask {
public:
    MuObject* m_obj40;                      // 0x40
    MuObject* m_obj44;                      // 0x44
    MuObject* m_obj48;                      // 0x48
    u8 m_unk4C[0x70 - 0x4C];
    nw4r::g3d::ScnMdlExpand* m_group70;     // 0x70
    nw4r::g3d::ScnGroup* m_scnGroup;        // 0x74
    MuMsg* m_msg;                           // 0x78
    void* m_msgData;                        // 0x7C
    u8 m_unk80[0x128 - 0x80];
    int m_stepResult;                       // 0x128 HYPOTHESIS: what the main steps return while they wait
    u8 m_unk12C[0x140 - 0x12C];
    muNoticeWndTask* m_wnd;                 // 0x140
    nw4r::g3d::ResFile* m_resFile;          // 0x144
    int m_priority;                         // 0x148
    u8 m_unk14C[0x150 - 0x14C];
    muWifiDataLoadTask* m_dataLoadTask;     // 0x150
    u8 m_unk154[0x261 - 0x154];
    u8 m_flag261;                           // 0x261

    int fn_B0B8();
    void fn_B0FC();
    int fn_B4A8();
    void fn_B4EC();
    void fn_B6E0();
    void fn_B910();
    int fn_BF0C();
    void fn_BF60();
    int fn_BF7C();
    void fn_B758();
    int fn_B7CC();
    void fn_B820();
    int fn_B8A4();
    int fn_BCA8();
    int fn_BD70();
};

int muCollectCntrbWndTask::fn_B0B8() {
    if (m_obj40->isNodeAnimFinished()) {
        return 1;
    }
    return m_stepResult;
}

void muCollectCntrbWndTask::fn_B0FC() { }

int muCollectCntrbWndTask::fn_B4A8() {
    if (m_obj40->isNodeAnimFinished()) {
        return 3;
    }
    return m_stepResult;
}

void muCollectCntrbWndTask::fn_B4EC() {
    if (m_flag261 == 0) {
        return;
    }
    m_group70->PushBack(m_obj48->m_sceneModel, "pos02");
}

void muCollectCntrbWndTask::fn_B6E0() {
    MuObject* obj = m_obj40;
    int anim = 2;
    if (m_flag261 != 0) {
        anim = 4;
    }
    obj->changeAnimN(anim, 1, true, false);
}

void muCollectCntrbWndTask::fn_B910() {
    m_dataLoadTask = muWifiDataLoadTask::create(m_resFile, m_scnGroup, m_msgData, m_priority + 7);
}

int muCollectCntrbWndTask::fn_BF0C() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xC;
    }
    return m_stepResult;
}

void muCollectCntrbWndTask::fn_BF60() {
    m_scnGroup->Remove(m_group70);
}

int muCollectCntrbWndTask::fn_BF7C() {
    return 0xD;
}

void muCollectCntrbWndTask::fn_B758() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 0x10, m_resFile, m_scnGroup, m_priority + 7, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectCntrbWndTask::fn_B7CC() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 1;
    }
    return m_stepResult;
}

void muCollectCntrbWndTask::fn_B820() {
    int msgId = 0x11 + (fn_80154DF8() != 0);
    m_wnd = fn_800BEEB8(1, m_msg->getBinary(), msgId, m_resFile, m_scnGroup, m_priority + 7, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectCntrbWndTask::fn_B8A4() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        u8 selected = wnd->m_selected;
        wnd->exit();
        m_wnd = NULL;
        int step = 1;
        if (selected != 0) {
            step = 7;
        }
        return step;
    }
    return m_stepResult;
}

int muCollectCntrbWndTask::fn_BCA8() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xC;
    }
    return m_stepResult;
}

int muCollectCntrbWndTask::fn_BD70() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        u8 selected = wnd->m_selected;
        wnd->exit();
        m_wnd = NULL;
        int step = 1;
        if (selected != 0) {
            step = 0xC;
        }
        return step;
    }
    return m_stepResult;
}

#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <string.h>
#include <gf/gf_pad_system.h>
#include <snd/snd_system.h>
#include <ms/ms_message.h>
#include <stdio.h>
// TODO: clean up, give variables real names
namespace jj_A3B4 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* fn_6_A454(void*);
extern "C" void fn_6_A6BC(void*, void*, void*, void*, void*, void*, void*, void*, void*, void*);

extern "C" void* fn_6_A3B4(void* a, void* b, void* c, void* d, void* e, void* f, void* g, void* h, void* i) {
    void* p = __nw__FUlQ25Heaps8HeapType(0x268, 0x2a);
    if (p)
        p = fn_6_A454(p);
    fn_6_A6BC(p, a, b, c, d, e, f, g, h, i);
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_A454 {
typedef unsigned char u8;
typedef unsigned int u32;

extern "C" {
extern char lbl_6_data_7D8[];
extern u8 lbl_6_data_8F0[];

void* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(void*, const char*, int, int, int, bool);
void fn_800B96E8(void*, int);
void __ct__19MuSelctChrNameEntryFv(void*);
void* memset(void*, int, unsigned long);
}

extern "C" void* fn_6_A454(u8* self)
{
    __ct__6gfTaskFPCcQ26gfTask8Categoryiib(self, lbl_6_data_7D8, 8, 0xF, 8, true);

    *reinterpret_cast<u32*>(self + 0x74) = 0;
    *reinterpret_cast<void**>(self + 0x3C) = lbl_6_data_8F0;
    *reinterpret_cast<u32*>(self + 0x78) = 0;
    *reinterpret_cast<u32*>(self + 0x7C) = 0;
    fn_800B96E8(self + 0x80, 0xFF);

    *reinterpret_cast<u32*>(self + 0x128) = 7;
    *reinterpret_cast<u32*>(self + 0x12C) = 0xE;
    *reinterpret_cast<u32*>(self + 0x130) = 0;
    *reinterpret_cast<u32*>(self + 0x134) = 0;
    *reinterpret_cast<u32*>(self + 0x138) = 0;
    *reinterpret_cast<u32*>(self + 0x13C) = 0;
    *reinterpret_cast<u32*>(self + 0x140) = 0;
    *reinterpret_cast<u32*>(self + 0x144) = 0;
    *reinterpret_cast<u32*>(self + 0x14C) = 0;
    *reinterpret_cast<u32*>(self + 0x150) = 0;
    *reinterpret_cast<u32*>(self + 0x154) = 0;
    *reinterpret_cast<u32*>(self + 0x158) = 0;
    *reinterpret_cast<u32*>(self + 0x15C) = 0;
    *reinterpret_cast<u32*>(self + 0x160) = 0;
    __ct__19MuSelctChrNameEntryFv(self + 0x1A4);

    self[0x261] = 0;
    *reinterpret_cast<u32*>(self + 0x264) = 0;
    self[0x2C] &= ~2;

    *reinterpret_cast<u32*>(self + 0x70) = 0;
    *reinterpret_cast<u32*>(self + 0x40) = 0;
    *reinterpret_cast<u32*>(self + 0x44) = 0;
    *reinterpret_cast<u32*>(self + 0x48) = 0;
    *reinterpret_cast<u32*>(self + 0x4C) = 0;
    *reinterpret_cast<u32*>(self + 0x50) = 0;
    *reinterpret_cast<u32*>(self + 0x54) = 0;
    *reinterpret_cast<u32*>(self + 0x58) = 0;
    *reinterpret_cast<u32*>(self + 0x5C) = 0;
    *reinterpret_cast<u32*>(self + 0x60) = 0;
    *reinterpret_cast<u32*>(self + 0x64) = 0;
    *reinterpret_cast<u32*>(self + 0x68) = 0;
    *reinterpret_cast<u32*>(self + 0x6C) = 0;

    memset(self + 0x164, 0, 0x15);
    memset(self + 0x179, 0, 0x29);
    memset(self + 0x238, 0, 0x29);

    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_A58C {
typedef unsigned char u8;

class LoopObj {
    u8 pad[0x5C];
public:
    virtual void fn(int);
};

class Obj150 {
    u8 pad[0x3C];
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
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
};

extern "C" {
extern u8 lbl_6_data_8F0[];
void exit__6gfTaskFv(void*);
void free__13gfHeapManagerFPv(void*);
void fn_80154420(void*, int);
void __dt__5MuMsgFv(void*, int);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void fn_8014C960(void);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);

void* fn_6_A58C(void* self, int deleting)
{
    if (self) {
        u8* obj = (u8*)self;

        void* prev = *reinterpret_cast<void**>(obj + 0x140);
        *reinterpret_cast<void**>(obj + 0x3C) = (void*)lbl_6_data_8F0;
        if (prev) {
            exit__6gfTaskFv(prev);
        }

        void* heap = *reinterpret_cast<void**>(obj + 0x14C);
        if (heap) {
            free__13gfHeapManagerFPv(heap);
        }

        void* a = *reinterpret_cast<void**>(obj + 0x130);
        if (a) {
            fn_80154420(a, 1);
        }

        void* b = *reinterpret_cast<void**>(obj + 0x78);
        if (b) {
            __dt__5MuMsgFv(b, 1);
        }

        void* c = *reinterpret_cast<void**>(obj + 0x70);
        if (c) {
            Destroy__Q34nw4r3g3d6G3dObjFv(c);
        }

        u8* cursor = obj;
        for (int i = 0; i < 0xC; ++i, cursor += 4) {
            LoopObj* entry = *reinterpret_cast<LoopObj**>(cursor + 0x40);
            if (entry && entry) {
                entry->fn(1);
            }
        }

        Obj150* d = *reinterpret_cast<Obj150**>(obj + 0x150);
        if (d) {
            d->v23();
        }

        fn_8014C960();
        __dt__6gfTaskFv(self, 0);

        if (deleting > 0) {
            __dl__FPv(self);
        }
    }

    return self;
}
}
}

// TODO: clean up, give variables real names
namespace jj_A6BC {
struct Entry { const char* name; unsigned char first, last, argument, pad; };
struct Anim { char pad[0x18]; void* controller; };
struct Object { char pad[0x10]; void* model; Anim* anim; };
struct Menu {
    char pad0[0x40]; Object* objects[12];
    nw4r::g3d::ScnMdl* scenes[4];
    char pad80[0xa8]; int state; char pad12c[4]; void* manager;
    char pad134[0x10]; int arg7; int offset;
    char pad14c[8]; int error; int error2;
    char pad15c[0x108]; void* context;
};
class Controller {
public:
    virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
    virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
    virtual void f10(float);
};
extern "C" {
extern Entry lbl_6_rodata_6F0[], lbl_6_rodata_710[], lbl_6_rodata_750[];
extern unsigned char lbl_6_rodata_708[];
extern char lbl_6_data_7EC[];
extern float lbl_6_rodata_6E8;
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
void fn_6_AB48(Menu*, Entry*, int, void*, int, int);
void fn_6_AC44(Menu*, void*, int, int);
void* fn_801542C4(void*, int);
void fn_800B96E8(void*, int);
void changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);
void fn_80153488(void*);
void fn_80153498(void*, int);
int fn_80153504(void*, int);
int getStateType__10nteSectionFv(void*);
int isAbleToContribute__15nteContributionFv();
}
extern "C" void fn_6_A6BC(Menu* self, char* context, void* a2, void* a3, void* a4, void* a5, int a6, int a7, int a8, int offset) {
    int temp[2];
    int i;
    int count;
    unsigned char* data;
    void* heap;
    int failed;
    self->scenes[1] = (nw4r::g3d::ScnMdl*)a8;
    self->arg7 = a7;
    self->context = context;
    failed = 0;
    self->offset = offset;
    *(int*)((char*)self + 0x7c) = a6;
    fn_6_AB48(self, lbl_6_rodata_6F0, 3, a2, offset, 0x2b);
    data = (unsigned char*)((unsigned long)lbl_6_rodata_708 | 0);
    heap = gfHeapManager::getMEMAllocator((Heaps::HeapType)0x2b);
    count = data[1] - data[0];
    if (count <= 0) count = 1;
    for (i = 0; i < count; i++) {
        nw4r::g3d::ScnMdl* mdl = nw4r::g3d::ScnMdl::Construct(heap, 0, data[3], self->objects[data[2] + i]->model);
        self->scenes[data[0] + i] = mdl;
    }
    fn_6_AB48(self, lbl_6_rodata_710, 8, a3, offset, 0x2b);
    fn_6_AB48(self, lbl_6_rodata_750, 1, a4, offset, 0x2b);
    fn_6_AC44(self, a5, 0x2a, 0x2b);
    void* p = operator new(0x390, (Heaps::HeapType)0x2a);
    if (p) p = fn_801542C4(p, 0x2b);
    self->manager = p;
    fn_800B96E8((char*)self + 0x80, 0xf0);
    Object* obj1 = self->objects[1];
    Object* obj2 = self->objects[2];
    Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(self->scenes[0], obj1->model, lbl_6_data_7EC);
    changeAnimN__8MuObjectFiUlbb(obj1, 1, 3, 0, 0);
    ((Controller*)obj1->anim->controller)->f10(lbl_6_rodata_6E8);
    changeAnimN__8MuObjectFiUlbb(obj2, 1, 1, 0, 0);
    ((Controller*)obj2->anim->controller)->f10(lbl_6_rodata_6E8);
    fn_80153488(temp);
    int ok;
    if (!fn_80153504(temp, *(int*)(context + 0xe4))) {
        fn_80153498(temp, -1);
        ok = 0;
    } else if (getStateType__10nteSectionFv(temp)) {
        fn_80153498(temp, -1);
        ok = 0;
    } else {
        fn_80153498(temp, -1);
        ok = 1;
    }
    if (!ok) {
        self->error2 = 0;
        failed = 1;
        self->error = 14;
    } else if (!isAbleToContribute__15nteContributionFv()) {
        self->error2 = 0;
        failed = 1;
        self->error = 13;
    }
    int state = 0;
    if (failed) state = 11;
    self->state = state;
}
}

// TODO: clean up, give variables real names
namespace jj_A970 {

struct Obj {
    int type;
    int pad4;
    virtual void destroy(int);
    char* data;
};

struct Owner {
    char pad[0xE4];
    int value;
};

extern "C" {
void fn_80153488(void*);
void fn_80153498(void*, int);
void fn_80153504(void*, int);
void __ct__7nteFileFQ25Heaps8HeapType(void*, int);
void __dt__7nteFileFv(void*, int);
void fn_80153EC4(void*, void*);
void addSection__7nteFileFP14nteSectionData(void*, int);
int getDataFileSize__7nteFileFv(void*);
int fn_80153810(void*);
Obj* fn_80153818(void*, int);
u64 getCreated__10nteSectionFv(void*);
void fn_80153610(void*, void*, u64, int, int, int);
}

extern "C" void* fn_6_A970(void* unused, int* out1, int* out2,
                            Owner* owner, void* arg4, void* arg5) {
    char writer[0xB4];
    char source[8];
    char target[8];
    Obj* obj;
    void* res;
    int size;

    fn_80153488(source);
    fn_80153488(target);
    __ct__7nteFileFQ25Heaps8HeapType(writer, 0x2B);
    fn_80153504(source, owner->value);
    obj = fn_80153818(source, 0x2B);

    switch (obj->type) {
    case 1:
        memcpy(obj->data, arg4, 0xA);
        memcpy(obj->data + 0xA, arg5, 0x28);
        arg5 = (void*)1;
        break;
    case 2:
        memcpy(obj->data, arg4, 0xA);
        memcpy(obj->data + 0xA, arg5, 0x28);
        arg5 = (void*)0;
        break;
    case 3:
        memcpy(obj->data, arg4, 0xA);
        memcpy(obj->data + 0xA, arg5, 0x28);
        arg5 = (void*)2;
        break;
    }

    fn_80153610(target, obj, getCreated__10nteSectionFv(source), 0x2B, 2, 0);
    if (obj != 0) {
        obj->destroy(1);
    }
    addSection__7nteFileFP14nteSectionData(writer, fn_80153810(target));
    size = getDataFileSize__7nteFileFv(writer);
    res = gfHeapManager::alloc((Heaps::HeapType)0x2B, size, 0x20);
    fn_80153EC4(writer, res);
    *out1 = size;
    *out2 = (int)arg5;
    __dt__7nteFileFv(writer, -1);
    fn_80153498(target, -1);
    fn_80153498(source, -1);
    return res;
}
}

// TODO: clean up, give variables real names
namespace jj_AB48 {
struct MuObject {
    char pad[0x14];
    void* animation;
};

struct Entry {
    const char* name;
    unsigned char first, last, argument, pad;
};

struct Menu {
    char pad[0x40];
    MuObject* objects[1];
};

extern "C" {
extern char lbl_6_data_8D8[];
extern const float lbl_6_rodata_758;

void* create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
    void*, const char*, int, void*, int);
void changeAnimN__8MuObjectFPCc(void*, const char*);
void setUpdateRate__16gfModelAnimationFf(void*, float);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
}

extern "C" void fn_6_AB48(Menu* self, Entry* entries, int count,
                         void* resourceFile, int offset, int heap) {
    int num;
    int i = 0;
    int j;
    Entry* entry = entries;
    MuObject* object;
    const char* name;
    char animationName[0x40];

    while (i < count) {
        num = 1;
        if (entry->first < entry->last)
            num = entry->last - entry->first;

        j = 0;
        while (j < num) {
            object = (MuObject*)create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
                resourceFile, entry->name, entry->argument + offset, 0, heap);
            name = entry->name;
            self->objects[entry->first + j] = object;
            strcpy(animationName, name);
            strcat(animationName, lbl_6_data_8D8);
            changeAnimN__8MuObjectFPCc(object, animationName);
            if (object->animation)
                setUpdateRate__16gfModelAnimationFf(object->animation, lbl_6_rodata_758);
            ++j;
        }
        ++i;
        ++entry;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_AC44 {

struct CollectViewer {
    char pad00[0x40];
    MuObject* objects[12];
    char pad70[8];
    MuMsg* msg;
};

extern "C" {
extern const float lbl_6_rodata_6E8[];
MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32, HeapType, u32);
void setFontWidthModeFixed__5MuMsgFUlf(MuMsg*, u32, float);
void setFontType__5MuMsgFUlUl(MuMsg*, u32, u32);
}

extern "C" void fn_6_AC44(CollectViewer* self, void* data, HeapType heap, u32 heap2) {
    const float* const ro = lbl_6_rodata_6E8;
    MuObject* obj;
    MuMsg* msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(3, heap, heap2);
    self->msg = msg;
    msg->allocMsgBuf(0x100, 5);
    msg->setMsgData(data);

    obj = self->objects[0];
    msg->attachScnMdlSimple(0, obj->m_sceneModel, 0, ro[0x74 / 4]);
    msg->setAlignMode(0, MuMsg::Align_Left);
    msg->setFontColor(0, 0, 0, 0, 0xff);
    setFontWidthModeFixed__5MuMsgFUlf(msg, 0, ro[0x78 / 4]);
    setFontType__5MuMsgFUlUl(msg, 0, 1);

    msg->attachScnMdlSimple(1, obj->m_sceneModel, 1, ro[0x74 / 4]);
    msg->setAlignMode(1, MuMsg::Align_Left);
    msg->setFontColor(1, 0, 0, 0, 0xff);
    setFontWidthModeFixed__5MuMsgFUlf(msg, 1, ro[0x78 / 4]);
    setFontType__5MuMsgFUlUl(msg, 1, 1);

    msg->attachScnMdlSimple(2, obj->m_sceneModel, 2, ro[0x74 / 4]);
    msg->printIndex(2, 4);
    msg->attachScnMdlSimple(3, obj->m_sceneModel, 3, ro[0x74 / 4]);
    msg->printIndex(3, 5);

    msg->attachScnMdlSimple(4, self->objects[3]->m_sceneModel, 0, ro[0x74 / 4]);
    setFontType__5MuMsgFUlUl(msg, 4, 1);
    msg->setAlignMode(4, MuMsg::Align_Left);
    setFontWidthModeFixed__5MuMsgFUlf(msg, 4, ro[0x7c / 4]);
}
}

// TODO: clean up, give variables real names
namespace jj_AE30 {
struct Viewer {
    unsigned char padding[0x128];
    union {
        int state;
        unsigned int stateIndex;
    };
    int previousState;
};

typedef int (*StateFunction)(Viewer*);

extern "C" {
void fn_800B9984(void*);
extern StateFunction lbl_6_rodata_768[14][2];

void fn_6_AE30(Viewer* self) {
    fn_800B9984((unsigned char*)self + 0x80);

    if (self->previousState != self->state) {
        StateFunction enter = lbl_6_rodata_768[self->stateIndex][0];
        if (enter) {
            enter(self);
        }
        self->previousState = self->state;
    }

    StateFunction update = lbl_6_rodata_768[self->stateIndex][1];
    if (update) {
        self->state = update(self);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_AECC {
extern float lbl_6_rodata_6E8;
extern float lbl_6_rodata_758;
extern char lbl_6_data_8DC[];
extern "C" void init__19MuSelctChrNameEntryFPvPQ34nw4r3g3d6ScnMdlP8MuObjectPP8MuObjectP8MuObjectP8MuObjectiP5MuMsgib(void*, int, int, MuObject*, void*, MuObject*, int, int, MuMsg*, int, int);
struct Viewer {
 char pad[0x4c];
 MuObject* obj1;
 char field50[0x14];
 MuObject* obj2;
 int field68;
 char pad6c[4];
 int field70;
 int field74;
 MuMsg* msg;
 char pad7c[0x128];
};
extern "C" void fn_6_AECC(Viewer* self, int flag) {
 float frame;
 int a, b;
 MuMsg* c;
 int d;
 if (flag) {
  frame = lbl_6_rodata_6E8;
  a = 0;
  b = 1;
  c = 0;
  d = 1;
  self->msg->printf(4, lbl_6_data_8DC);
 } else {
  a = self->field68;
  c = self->msg;
  b = 4;
  frame = lbl_6_rodata_758;
  d = 0;
 }
 self->obj1->setFrameVisible(frame);
 self->obj2->setFrameVisible(frame);
 init__19MuSelctChrNameEntryFPvPQ34nw4r3g3d6ScnMdlP8MuObjectPP8MuObjectP8MuObjectP8MuObjectiP5MuMsgib((char*)self + 0x1a4, self->field74, self->field70, self->obj1, self->field50, self->obj2, a, b, c, 0, d);
}
}

// TODO: clean up, give variables real names
namespace jj_AFAC {
extern "C" void printf__5MuMsgFUlPCce(void*, unsigned long, const char*, ...);
extern "C" void setFrameNode__8MuObjectFf(void*, float);
extern "C" void setFrameVisible__8MuObjectFf(void*, float);
extern "C" int changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);
extern float lbl_6_rodata_6E8;
struct Display {
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
    virtual void call(void*, void*);
    char pad[0xe4 - 4];
    void* value;
};
struct ViewerData {
    char pad0[0x40];
    void* object40;
    void* object44;
    char pad48[0x70 - 0x48];
    void* object70;
    Display* object74;
    void* msg;
    char pad7c[0x134 - 0x7c];
    int mode;
    int extra;
    char pad13c[0x164 - 0x13c];
    char text0[0x15];
    char text1[1];
};
extern "C" void fn_6_AFAC(ViewerData* self) {
    printf__5MuMsgFUlPCce(self->msg, 0, self->text0);
    printf__5MuMsgFUlPCce(self->msg, 1, self->text1);
    int mode = self->mode;
    int extra = self->extra;
    void* object = self->object44;
    float frame = lbl_6_rodata_6E8 + (float)mode;
    if (mode == 2) {
        frame += (float)extra;
    }
    setFrameNode__8MuObjectFf(object, frame);
    setFrameVisible__8MuObjectFf(object, frame);
    Display* display = self->object74;
    display->call(display->value, self->object70);
    changeAnimN__8MuObjectFiUlbb(self->object40, 0, 1, 1, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_B100 {

extern "C" {
extern gfPadSystem* g_gfPadSystem;
extern sndSystem* g_sndSystem;
extern const float lbl_6_rodata_6E8;
extern const double lbl_6_rodata_7D8;

int getSysPadStatus__11gfPadSystemFiP11gfPadStatus(gfPadSystem*, int, gfPadStatus*);
void setFrameNode__8MuObjectFf(void*, float);
void setFrameVisible__8MuObjectFf(void*, float);
int playSE__9sndSystemF5SndIDiiii(sndSystem*, int, int, int, int, int);
int getAButtonPushedControllerID__16muMenuControllerFv();
int strlenUtf8__14msTagProcessorFPCci(void*, int);

struct Menu {
    char pad0[0x44];
    void* object;
    char pad48[0x48];
    u32 buttons;
    char pad94[0x94];
    int state;
    char pad12c[8];
    int row;
    int column;
    int accepted;
    char pad140[0x121];
    u8 mode;
};

int fn_6_B100(Menu* self) {
    gfPadStatus status;
    int action = 0;
    int row = self->row;
    int column = self->column;
    u32 buttons = self->buttons;

    getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, 0xF0, &status);

    if (buttons & 8) {
        if (--row < 0)
            row = 0;
    }
    if (buttons & 4) {
        if (++row >= 3)
            row = 2;
    }
    if (row == 2) {
        if (buttons & 1) {
            if (--column < 0)
                column = 0;
        }
        if (buttons & 2) {
            if (++column >= 2)
                column = 1;
        }
    }

    if (row != self->row || column != self->column) {
        self->row = row;
        self->column = column;
        void* object = self->object;
        float frame = lbl_6_rodata_6E8 + row;
        if (row == 2)
            frame += column;
        setFrameNode__8MuObjectFf(object, frame);
        setFrameVisible__8MuObjectFf(object, frame);
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
    }

    if (status.m_buttonsPressedThisFrame.bits & 0x100) {
        self->accepted = getAButtonPushedControllerID__16muMenuControllerFv();
        switch (row) {
        case 0:
            action = 1;
            break;
        case 1:
            action = 2;
            break;
        case 2:
            action = 4;
            if (column == 0)
                action = 3;
            break;
        }
    }
    if (status.m_buttonsPressedThisFrame.bits & 0x200)
        action = 4;

    int state;
    switch (action) {
    case 1:
        self->mode = 0;
        state = 2;
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 1, -1, 0, 0, -1);
        break;
    case 2:
        self->mode = 1;
        state = 2;
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 1, -1, 0, 0, -1);
        break;
    case 3:
        if (strlenUtf8__14msTagProcessorFPCci((char*)self + 0x164, 0x7FFF) > 0 &&
            strlenUtf8__14msTagProcessorFPCci((char*)self + 0x179, 0x7FFF) > 0) {
            state = 6;
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x13, -1, 0, 0, -1);
        } else {
            state = 5;
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 3, -1, 0, 0, -1);
        }
        break;
    case 4:
        state = 10;
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 2, -1, 0, 0, -1);
        break;
    default:
        state = self->state;
        break;
    }
    return state;
}
}
}

// TODO: clean up, give variables real names
namespace jj_B404 {

extern "C" {
void fn_6_AECC(void* self, int flag);
void open__19MuSelctChrNameEntryFPCcPci(void* self, const char* name, int a, int c);
int changeAnimN__8MuObjectFiUlbb(void* p, int a, int b, int c, int d);
}

extern "C" void fn_6_B404(char* self) {
    char buf[0x30];
    int a;
    int c;
    fn_6_AECC(self, *(unsigned char*)(self + 0x261));
    if (*(unsigned char*)(self + 0x261)) {
        strcpy(buf, self + 0x179);
        a = (int)(self + 0x238);
        c = 10;
    } else {
        strcpy(buf, self + 0x164);
        a = 0;
        c = 5;
    }
    *(int*)(self + 0x234) = *(int*)(self + 0x13c);
    open__19MuSelctChrNameEntryFPCcPci(self + 0x1a4, buf, a, c);
    changeAnimN__8MuObjectFiUlbb(*(void**)(self + 0x40), *(unsigned char*)(self + 0x261) ? 3 : 1, 1, 1, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_B514 {

extern "C" {
extern float lbl_6_rodata_7E0;
int main__19MuSelctChrNameEntryFP11gfPadStatusPcbb(void*, gfPadStatus*, char*, int, int);
}

class ViewerTarget {
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
    virtual void setModel(nw4r::g3d::ScnMdl*);
};

extern "C" int fn_6_B514(unsigned char* self) {
    gfPadStatus pad;
    char text[0x30];
    int result = *(int*)(self + 0x128);
    int accepted = 0;

    g_gfPadSystem->getSysPadStatus(0xf0, &pad);
    pad.m_buttonsCurrentFrame2.bits = *(unsigned int*)(self + 0x88);
    pad.m_buttonsPressedThisFrame.bits = *(unsigned int*)(self + 0x8c);
    pad.m_buttonsPressedThisFrame2.bits = *(unsigned int*)(self + 0x90);

    switch (main__19MuSelctChrNameEntryFP11gfPadStatusPcbb(self + 0x1a4, &pad, text, 1, 1)) {
    case 0:
        accepted = 1;
        result = 4;
        break;
    case 1:
        result = 4;
        break;
    case 2:
        if (self[0x261] != 0) {
            int value = *(int*)(self + 0x228);
            float frame;
            if (value < 10) {
                frame = (float)value;
            } else {
                frame = lbl_6_rodata_7E0;
            }
            (*(MuObject**)(self + 0x48))->setFrameNode(frame);
            (*(MuMsg**)(self + 0x78))->printf(1, (const char*)(self + 0x238));
        }
        break;
    case 0x7fffffff:
        break;
    default:
        break;
    }

    if (result != *(int*)(self + 0x128)) {
        if (accepted != 0) {
            if (self[0x261] != 0) {
                strcpy((char*)(self + 0x179), (const char*)(self + 0x238));
                (*(MuMsg**)(self + 0x78))->printf(1, (const char*)(self + 0x179));
            } else {
                strcpy((char*)(self + 0x164), text);
                (*(MuMsg**)(self + 0x78))->printf(0, (const char*)(self + 0x164));
            }
        } else if (self[0x261] != 0) {
            (*(MuMsg**)(self + 0x78))->printf(1, (const char*)(self + 0x179));
        }

        if (self[0x261] != 0) {
            (*(ViewerTarget**)(self + 0x70))->setModel(
                (*(MuObject**)(self + 0x48))->m_sceneModel);
        }
    }

    return result;
}
}

// TODO: clean up, give variables real names
namespace jj_B70C {
extern "C" int isNodeAnimFinished__8MuObjectFv(void*);
extern "C" void close__19MuSelctChrNameEntryFv(void*);

struct MenuCollectViewer {
    unsigned char pad_000[0x40];
    void* resource;
    unsigned char pad_044[0xE4];
    int state;
    unsigned char pad_12C[0x78];
    unsigned char member_1A4;
};

extern "C" int controlMedia__18muCollectionViewerFv(MenuCollectViewer* self) {
    if (isNodeAnimFinished__8MuObjectFv(self->resource)) {
        close__19MuSelctChrNameEntryFv(&self->member_1A4);
        return 1;
    }
    return self->state;
}
}

// TODO: clean up, give variables real names
namespace jj_B954 {
struct DialogBase {
    char padding[0x3C];
};

struct Dialog : DialogBase {
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
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void close();

    char padding1[8];
    int result;
    char padding2[4];
    int state;
};

struct Viewer {
    char padding0[0x128];
    int state;
    char padding1[0x24];
    Dialog* dialog;
};

extern "C" int fn_6_B954(Viewer* self) {
    Dialog* dialog = self->dialog;
    if (dialog->state == 5) {
        int result = dialog->result;
        dialog->close();
        self->dialog = 0;
        return result == 1 ? 12 : 8;
    }
    return self->state;
}
}

// TODO: clean up, give variables real names
namespace jj_B9CC {
extern "C" {
extern const float lbl_6_rodata_6E8[];
s32 fn_6_A970(char*, s32*, s32*, s32, void*, void*);
void upload__15nteContributionFlPvUl(s32, s32, s32, s32);
s32 fn_800BEEB8(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void setTrans__8MuObjectFP5Vec3f(void*, float*);
void setFrameTexSrt__8MuObjectFf(void*, float);
}
class Scene {
public:
 virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
 virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
 virtual void f8(); virtual void f9(); virtual void f10(); virtual void f11(void*, void*);
 virtual void f12(); virtual void f13(void*, void*);
};
class Controller {
public:
 virtual void f0(); virtual void f1(); virtual void f2(); virtual void f3();
 virtual void f4(); virtual void f5(); virtual void f6(); virtual void f7();
 virtual void f8(float);
};
extern "C" void fn_6_B9CC(char* self) {
    const float* ro = (const float*)((unsigned long)lbl_6_rodata_6E8 | 0);
    u16 buf2[6];
    u16 buf1[6];
    float vec[3];
    s32 a1c;
    s32 a18;
    void* obj;
    Message::utf8to16((wchar_t*)buf1, self + 0x164);
    Message::utf8to16((wchar_t*)buf2, self + 0x179);
    *(s32*)(self + 0x14C) = fn_6_A970(self, &a1c, &a18, *(s32*)(self + 0x264), buf1, buf2);
    upload__15nteContributionFlPvUl(*(s32*)(self + 0x130), a18, *(s32*)(self + 0x14C), a1c);
    *(s32*)(self + 0x140) = fn_800BEEB8(3, *(s32*)(*(char**)(self + 0x78) + 4), 0x13,
                                        *(s32*)(self + 0x144), *(s32*)(self + 0x74),
                                        *(s32*)(self + 0x148) + 7, 0, 0x2A,
                                        0x2B, 0, 0, 0xF0);
    obj = *(void**)(self + 0x6C);
    vec[0] = ro[0x70 / 4];
    vec[1] = ro[0xFC / 4];
    vec[2] = ro[0x70 / 4];
    setTrans__8MuObjectFP5Vec3f(obj, vec);
    Scene* scene = *(Scene**)(self + 0x74);
    scene->f11(*(void**)((char*)scene + 0xE4), *(void**)((char*)obj + 0x10));
    ((MuObject*)obj)->setFrameNode(ro[0x70 / 4]);
    setFrameTexSrt__8MuObjectFf(obj, ro[0x70 / 4]);
    (*(Controller**)(*(char**)((char*)obj + 0x14) + 0xc))->f8(ro[0]);
    (*(Controller**)(*(char**)((char*)obj + 0x14) + 0x14))->f8(ro[0]);
}
}

// TODO: clean up, give variables real names
namespace jj_BB34 {
extern "C" void exit__6gfTaskFv(void*);
extern "C" int exchangeDWCErrorCode__19muWifiInterfaceTaskFUl(int);
extern "C" int getErrorMsgID__19muWifiInterfaceTaskFi(int);

struct VObj { virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3(); virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7(); virtual void v8(); virtual void v9(); virtual void v10(); virtual void v11(); virtual void v12(); virtual void v13(int); };
struct Sub { char pad[0x10]; int f10; };
struct Data { char pad[0x34c]; unsigned char f34c; char p2[3]; int f350; char p3[0x378-0x354]; int f378; char p4[4]; unsigned int f380; };
struct Self {
    char pad[0x6c]; Sub* f6c; char p1[4]; VObj* f74; char p2[0x128-0x78];
    int f128; char p3[4]; Data* f130; char p4[0x140-0x134]; void* f140; char p5[0x154-0x144];
    int f154; int f158; int f15c; int f160;
};

extern "C" int fn_6_BB34(Self* self) {
    Data* d = self->f130;
    int r = self->f128;
    if (d->f34c) {
        exit__6gfTaskFv(self->f140);
        self->f140 = 0;
        self->f74->v13(self->f6c->f10);
        if (d->f350 != 0) {
            switch (d->f350) {
            case 1:
                break;
            case 3:
                self->f158 = 2;
                self->f160 = d->f380;
                if (d->f380 == 0x27e0) self->f154 = 0x18;
                else self->f154 = 0x17;
                break;
            case 2: {
                self->f158 = 1;
                int v = d->f378;
                self->f160 = v;
                self->f15c = exchangeDWCErrorCode__19muWifiInterfaceTaskFUl(v);
                self->f154 = getErrorMsgID__19muWifiInterfaceTaskFi(self->f15c);
                break;
            }
            }
            r = 11;
        } else {
            r = 9;
        }
    }
    return r;
}
}

// TODO: clean up, give variables real names
namespace jj_BC2C {
extern "C" int fn_800BEEB8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);

extern "C" void fn_6_BC2C(void* self) {
    char* p = (char*)self;
    int* q = *(int**)(p + 0x78);
    int val = q[1];
    int r = fn_800BEEB8(0, val, 0x14, *(int*)(p + 0x144), *(int*)(p + 0x74), *(int*)(p + 0x148) + 7, 0, 0x2a, 0x2b, 0, 0, 0xf0);
    *(int*)(p + 0x140) = r;
    *(int*)((char*)r + 0x128) = 0x1ef3;
}
}

// TODO: clean up, give variables real names
namespace jj_BCFC {
extern "C" int fn_800BEEB8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8, int a9, int a10, int a11, int a12);

extern "C" void fn_6_BCFC(char* p) {
    int r = fn_800BEEB8(1, *(int*)(*(int*)(p + 0x78) + 4), 0xf, *(int*)(p + 0x144), *(int*)(p + 0x74), *(int*)(p + 0x148) + 7, 0, 0x2a, 0x2b, 0, 0, 0xf0);
    *(int*)(p + 0x140) = r;
}
}

// TODO: clean up, give variables real names
namespace jj_BDDC {

extern "C" gfTask* fn_800BEEB8(int, void*, int, int, int, int, int, int, int, int, int, int);
extern "C" int haveErrorCode__19muWifiInterfaceTaskFi(int);
extern "C" void dispSubMsg__15muNoticeWndTaskFPCc(gfTask*, const char*);
extern "C" char lbl_6_data_8E8[];

extern "C" void fn_6_BDDC(void* p) {
    char* self = (char*)p;
    gfTask* task = *(gfTask**)(self + 0x140);
    if (task) {
        task->exit();
        *(gfTask**)(self + 0x140) = 0;
    }
    void* msg;
    if (*(int*)(self + 0x158) == 1)
        msg = *(void**)(self + 0x7c);
    else
        msg = *(void**)(*(char**)(self + 0x78) + 4);
    *(gfTask**)(self + 0x140) = fn_800BEEB8(0, msg, *(int*)(self + 0x154), *(int*)(self + 0x144), *(int*)(self + 0x74), *(int*)(self + 0x148) + 7, 0, 0x2a, 0x2b, 0, 0, 0xf0);
    int mode = *(int*)(self + 0x158);
    if ((unsigned)(mode - 1) <= 1) {
        if (mode == 1 ? haveErrorCode__19muWifiInterfaceTaskFi(*(int*)(self + 0x15c)) : 1) {
            unsigned long size;
            char* data;
            char buffer[128];
            Message::getPrintIndexData(*(void**)(self + 0x7c), 0x13, &data, &size);
            memcpy(buffer, data, size);
            size += sprintf(buffer + size, lbl_6_data_8E8, *(int*)(self + 0x160));
            dispSubMsg__15muNoticeWndTaskFPCc(*(gfTask**)(self + 0x140), buffer);
        }
    }
}
}

