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
    virtual void fn_801AB788();
    virtual void fn_801AAC9C();
    virtual void fn_801AAE34();
    virtual void fn_801AAFDC();
    virtual void fn_801AAFE8();
    virtual void fn_801AAA70();
    virtual void fn_801ABE70();
    virtual void fn_801ABFFC();
    virtual void fn_801AC0B8(void*);
};
struct Self {
    char pad[0x54];
    void* field54;
    Class_804664C0* field58;
    int field5c;
};
extern "C" void fn_801B4CB0(void*, void*, int);
extern "C" void fn_800DA9CC(Self* self, int flag) {
    self->field58->fn_801AC0B8(self->field54);
    if (flag) {
        fn_801B4CB0(self->field58, self->field54, self->field5c);
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
Task* fn_800DA354(void*);
void fn_800DA4A8(void*, void*, int, int, int, int);
int fn_800DA81C(void*, int, int, int, int, int);
void fn_801B4CB0(void*, void*, int);
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
 if (task) task = fn_800DA354(task);
 int limit = clampCount(count);
 node = 0;
 nw4r::g3d::ResMdl mdl;
 task->count = limit;
 task->source = source;
 mdl = nw4r::g3d::ResMdl(source->inner->mdl);
 nw4r::g3d::ResNodeData* res = GetResNode__Q34nw4r3g3d6ResMdlCFPCc(&mdl, name);
 if (res) node = res->m_nodeIndex;
 task->node = node;
 fn_800DA4A8(task, lbl_80408540, 2, priority, draw, allocator);
 task->scene = (Object_vptr_0*)nw4r::g3d::ScnMdl::Construct(gfHeapManager::getMEMAllocator(allocator), 0, 4, task->model->m_sceneModel);
 Object_vptr_0* scene = task->scene;
 int c;
 while (c = scene->count) {
  if (c == 0) continue;
  scene->slot12(c - 1);
 }
 float f;
 if (task->mode == 0) {
  fn_800DA81C(task, 0, task->count, 1, 0, 1);
  f = (float)(task->count - 1);
 } else {
  fn_800DA81C(task, 0, 2, 0, 0, 1);
  fn_800DA81C(task, 0, 2, 1, 2, 3);
  f = (float)(task->count + 2);
 }
 task->model->setFrameNode(f);
 fn_801B4CB0(task->source, task->scene, task->node);
 return task;
}
}

