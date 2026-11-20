#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <cstring>
#include <mu/mu_object.h>
#include <mu/mu_wifi_cnt_wnd.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <types.h>

// HYPOTHESIS: one entry of the node table createObjResFile walks; the type's real name is unknown.
struct UnkModelNodeInfo {
    const char* m_name;
    u8 unk4;
    u8 unk5;
    u8 unk6;
};

muWifiCntWndTask::muWifiCntWndTask()
    : gfTask("WifiCntWnd", Category_Menu, 15, 8, true), unk58(0), unk5C(0), unk60(1), unk64(0) {
    unk2C_b1 = false;
    for (int i = 0; i < 5; i++) {
        unk40[i] = 0;
    }
    unk54 = 0;
}

void muWifiCntWndTask::createObjResFile(const void* nodeInfo, int count, nw4r::g3d::ResFile* resFile,
                                       int priority, HeapType heap) {
    int i;
    int j;
    const UnkModelNodeInfo* p = static_cast<const UnkModelNodeInfo*>(nodeInfo);
    char name[0x40];
    for (i = 0; i < count; i++, p++) {
        int copies = p->unk4 < p->unk5 ? p->unk5 - p->unk4 : 1;
        for (j = 0; j < copies; j++) {
            MuObject* obj = MuObject::create(resFile, p->m_name, p->unk6 + priority, 0, heap);
            unk40[p->unk4 + j] = obj;
            strcpy(name, p->m_name);
            strcat(name, "__0");
            obj->changeAnimN(name);
            if (obj->m_modelAnim) {
                obj->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void muWifiCntWndTask::setWndStyle(int style, int digits) {
    int maxDigits = 2;
    if (style == 0) {
        maxDigits = 3;
    }
    int count = digits > 1 ? digits : 1;
    unk60 = count < maxDigits ? count : maxDigits;
    unk64 = style;
    nw4r::g3d::ScnMdlExpand* group = unk54;
    while (!group->Empty()) {
        group->PopBack();
    }
    float frame;
    if (unk64 == 0) {
        dispNum(0, unk60, 1, 0, 1);
        frame = unk60 - 1;
    } else {
        dispNum(0, 2, 0, 0, 1);
        dispNum(0, 2, 1, 2, 3);
        frame = unk60 + 2;
    }
    unk40[0]->setFrameNode(frame);
}

void muWifiCntWndTask::setCounter(int primary, int secondary) {
    nw4r::g3d::ScnMdlExpand* group = unk54;
    while (!group->Empty()) {
        group->PopBack();
    }
    float frame;
    if (unk64 == 0) {
        dispNum(primary, unk60, 1, 0, 1);
        frame = unk60 - 1;
    } else {
        dispNum(secondary, 2, 0, 0, 1);
        dispNum(primary, 2, 1, 2, 3);
        frame = unk60 + 2;
    }
    unk40[0]->setFrameNode(frame);
}

// TODO: clean up, give variables real names
extern "C" void fn_800DA6E8(void) {}

// TODO: clean up, give variables real names
namespace jj_800DA9CC {
class Class_804664C0 {
public:
    virtual void fn_801B4A50();
    virtual void fn_801B45A0();
    virtual void fn_801B4918();
    virtual void fn_801B4AE8();
    virtual void fn_801B4AB8();
    virtual void ForEach__Q34nw4r3g3d8ScnGroupFPFPQ34nw4r3g3d6ScnObjPv_Q44nw4r3g3d6ScnObj13ForEachResultPvb();
    virtual void SetScnObjOption__Q34nw4r3g3d6ScnObjFUlUl();
    virtual void GetScnObjOption__Q34nw4r3g3d6ScnObjCFUlPUl();
    virtual void GetValueForSortOpa__Q34nw4r3g3d6ScnObjCFv();
    virtual void GetValueForSortXlu__Q34nw4r3g3d6ScnObjCFv();
    virtual void CalcWorldMtx__Q34nw4r3g3d6ScnObjFPCQ34nw4r4math5MTX34PUl();
    virtual void Insert__Q34nw4r3g3d8ScnGroupFUlPQ34nw4r3g3d6ScnObj();
    virtual void Remove__Q34nw4r3g3d8ScnGroupFUl();
    virtual void Remove__Q34nw4r3g3d8ScnGroupFPQ34nw4r3g3d6ScnObj(void*);
};
struct Self {
    char pad[0x54];
    void* field54;
    Class_804664C0* field58;
    int field5c;
};
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjUl(void*, void*, int);
extern "C" void fn_800DA9CC(Self* self, int flag) {
    self->field58->Remove__Q34nw4r3g3d8ScnGroupFPQ34nw4r3g3d6ScnObj(self->field54);
    if (flag) {
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjUl(self->field58, self->field54, self->field5c);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800DA15C {
class Object_vptr_0 {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3();
 virtual void slot4(); virtual void slot5(); virtual void slot6(); virtual void slot7();
 virtual void slot8(); virtual void slot9(); virtual void slot10(); virtual void slot11();
 virtual void slot12(int);
 char pad[0xE0]; int count;
};
struct SourceInner { char pad[0xE8]; void* mdl; };
struct Source { char pad[0xE8]; SourceInner* inner; };
struct Task { char pad[0x40]; MuObject* model; char pad2[0x10]; Object_vptr_0* scene; Source* source; int node; int count; int mode; };
extern unsigned char lbl_80408540[];
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
Task* __ct__16muWifiCntWndTaskFv(void*);
void createObjResFile__16muWifiCntWndTaskFPCviPQ34nw4r3g3d7ResFileiQ25Heaps8HeapType(void*, void*, int, int, int, int);
int dispNum__16muWifiCntWndTaskFiiiii(void*, int, int, int, int, int);
void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjUl(void*, void*, int);
nw4r::g3d::ResNodeData* GetResNode__Q34nw4r3g3d6ResMdlCFPCc(void*, const char*);
}
inline int clampCount(int count) {
 int n = 1;
 if (count > 1) n = count;
 int limit = 3;
 if (n < 3) limit = n;
 return limit;
}
// TODO: clean up, give variables real names
extern "C" Task* fn_800DA15C(int count, int priority, Source* source, const char* name, int draw, int heap, Heaps::HeapType allocator) {
 int node;
 Task* task = (Task*)__nw__FUlQ25Heaps8HeapType(0x68, heap);
 if (task) task = __ct__16muWifiCntWndTaskFv(task);
 int limit = clampCount(count);
 node = 0;
 nw4r::g3d::ResMdl mdl;
 task->count = limit;
 task->source = source;
 mdl = nw4r::g3d::ResMdl(source->inner->mdl);
 nw4r::g3d::ResNodeData* res = GetResNode__Q34nw4r3g3d6ResMdlCFPCc(&mdl, name);
 if (res) node = res->m_nodeIndex;
 task->node = node;
 createObjResFile__16muWifiCntWndTaskFPCviPQ34nw4r3g3d7ResFileiQ25Heaps8HeapType(task, lbl_80408540, 2, priority, draw, allocator);
 task->scene = (Object_vptr_0*)nw4r::g3d::ScnMdl::Construct(gfHeapManager::getMEMAllocator(allocator), 0, 4, task->model->m_sceneModel);
 Object_vptr_0* scene = task->scene;
 int c;
 while (c = scene->count) {
  if (c == 0) continue;
  scene->slot12(c - 1);
 }
 float f;
 if (task->mode == 0) {
  dispNum__16muWifiCntWndTaskFiiiii(task, 0, task->count, 1, 0, 1);
  f = (float)(task->count - 1);
 } else {
  dispNum__16muWifiCntWndTaskFiiiii(task, 0, 2, 0, 0, 1);
  dispNum__16muWifiCntWndTaskFiiiii(task, 0, 2, 1, 2, 3);
  f = (float)(task->count + 2);
 }
 task->model->setFrameNode(f);
 PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjUl(task->source, task->scene, task->node);
 return task;
}
}

// TODO: clean up, give variables real names
namespace jj_800DA81C {

extern "C" {
int sprintf(char* s, const char* fmt, ...);
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void* a, void* b, char* c);
extern char lbl_8059E188[8];
}

struct NumDisp {
    char pad0[0x40];
    MuObject* objs[5];
    void* scn;
    char pad1[0x8];
    int count;
};

extern "C" int dispNum__16muWifiCntWndTaskFiiiii(NumDisp* self, int value, int digits, int skipZeros, int base, int idx) {
    MuObject* obj;
    MuObject** p;
    int j;
    int n;
    void* scn;
    int max;
    char buf[8];
    int i;
    int mul;
    mul = 1;
    n = 0;
    i = 0;
    for (; i < digits; i++) {
        mul *= 10;
    }
    max = mul - 1;
    int v = value > 0 ? value : 0;
    if (v < max) {
        max = v;
    }
    scn = *(void**)((char*)self + 0x54);
    p = (MuObject**)((char*)self + idx * 4);
    j = 0;
    while (j < self->count && (skipZeros == 0 || max > 0 || j == 0)) {
        obj = p[0x40 / 4];
        obj->setFrameTex((float)(max % 10));
        sprintf(buf, lbl_8059E188, base + j);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(scn, *(void**)((char*)obj + 0x10), buf);
        p++;
        j++;
        n++;
        max /= 10;
    }
    return n;
}
}

