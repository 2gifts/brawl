#include <gf/gf_task.h>
#include <types.h>

#include <mu/mu_select_stage_task.h>

// The stage selection screen (module sora_menu_sel_stage). Only a part of its functions is decompiled.
/// Function names come from the sora_menu_sel_stage map.

// The task of the stage selection that lists and loads the custom stage files (it is created by the screen).
class muSelectStageFileTask : public gfTask {
public:
    void exit();
};

static muSelectStageFileTask* s_fileTask;

void muSelectStageTask::exit() {
    if (s_fileTask != NULL) {
        s_fileTask->exit();
        s_fileTask = NULL;
    }
    gfTask::exit();
}

void muSelectStageTask::processDefault() {
    if ((u32)m_mode <= 2) {
        processDefaultWithScreen();
    } else {
        processDefaultWithoutScreen();
    }
}

#include <string.h>
#include <mu/mu_object.h>
#include <mu/mu_msg.h>
#include <nw4r/math/math_types.h>
// TODO: clean up, give variables real names
namespace jj_0 {
struct Arg { int f0; int f4,f8,fC,f10; };
struct Obj { char pad[0x40]; int f40; char pad2[0x264-0x44]; int f264,f268,f26c,f270; char pad3[0x6160-0x274]; int f6160; };
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long,int);
extern "C" Obj* __ct__17muSelectStageTaskFv(Obj*);
extern "C" void initProcWithScreen__17muSelectStageTaskFv(Obj*,const Arg&);
extern "C" void makeStageThumbTbl__17muSelectStageTaskFv(Obj*);
extern "C" void* fn_11_7590(int);
extern void* lbl_11_bss_8;
extern "C" Obj* create__17muSelectStageTaskFPQ217muSelectStageTask5Param(const Arg& p) {
 Obj* obj=(Obj*)__nw__FUlQ25Heaps8HeapType(0x616c,0x2a);
 if(obj) obj=__ct__17muSelectStageTaskFv(obj);
 int b,a,t,c,d;
 t=p.f0;
 a=p.f4;
 obj->f40=t;
 b=p.f8;
 obj->f264=a;
 c=p.f10;
 obj->f268=b;
 d=p.fC;
 obj->f26c=c;
 obj->f270=d;
 if((unsigned int)t<=2) initProcWithScreen__17muSelectStageTaskFv(obj,p);
 else { if(t==3) lbl_11_bss_8=fn_11_7590(1); makeStageThumbTbl__17muSelectStageTaskFv(obj); obj->f6160=0; }
 return obj;
}
}

// TODO: clean up, give variables real names
namespace jj_BC {
extern "C" {
extern const char lbl_11_data_0[];
extern char lbl_11_data_5C8[];
extern const float lbl_11_rodata_0;
extern int lbl_11_bss_8;
gfTask* __ct__6gfTaskFPCcQ26gfTask8Categoryiib(gfTask*, const char*, gfTask::Category, int, int, bool);
void fn_800B96E8(void*, int);
}
struct Arr { char pad[0x54]; int arr[112]; };
extern "C" gfTask* __ct__17muSelectStageTaskFv(gfTask* obj) {
__ct__6gfTaskFPCcQ26gfTask8Categoryiib(obj,lbl_11_data_0,gfTask::Category_Menu,15,8,true);
char* self=(char*)obj;
*(int*)(self+0x40)=0;
*(char**)(self+0x3c)=lbl_11_data_5C8;
*(char*)(self+0x44)=1;
*(int*)(self+0x48)=0;
*(int*)(self+0x4c)=0;
*(int*)(self+0x50)=0;
*(int*)(self+0x224)=0;
*(int*)(self+0x228)=0;
*(int*)(self+0x244)=0;
*(int*)(self+0x248)=-1;
*(int*)(self+0x24c)=-1;
*(int*)(self+0x250)=0;
*(int*)(self+0x260)=0;
*(int*)(self+0x264)=0;
*(int*)(self+0x268)=0;
*(int*)(self+0x274)=0;
*(int*)(self+0x288)=0;
*(int*)(self+0x604c)=0;
*(float*)(self+0x6050)=lbl_11_rodata_0;
*(int*)(self+0x6054)=0;
*(char*)(self+0x6058)=0;
*(char*)(self+0x6059)=0;
*(int*)(self+0x605c)=-1;
*(int*)(self+0x6060)=-1;
*(int*)(self+0x6064)=0;
*(int*)(self+0x6068)=0;
*(int*)(self+0x606c)=0;
*(int*)(self+0x6070)=0;
*(int*)(self+0x6078)=0;
*(int*)(self+0x607c)=0;
*(int*)(self+0x6080)=0;
*(char*)(self+0x6084)=0;
*(char*)(self+0x6085)=0;
*(float*)(self+0x6088)=lbl_11_rodata_0;
*(int*)(self+0x608c)=0;
*(float*)(self+0x6090)=lbl_11_rodata_0;
*(char*)(self+0x6094)=0;
fn_800B96E8(self+0x6098,255);
*(int*)(self+0x6140)=0;
*(char*)(self+0x6144)=0;
*(int*)(self+0x6148)=0;
*(int*)(self+0x614c)=1;
*(char*)(self+0x6150)=1;
*(int*)(self+0x6160)=0;
*(int*)(self+0x6164)=0;
*(char*)(self+0x6168)=0;
*(char*)(self+0x6169)=1;
*(u8*)(self+0x2c) &= ~2;
lbl_11_bss_8=0;
*(int*)(self+0x22c)=0;
*(char*)(self+0x230)=0;
*(int*)(self+0x234)=0;
*(char*)(self+0x238)=0;
*(int*)(self+0x23c)=0;
*(char*)(self+0x240)=0;
for(int i=0;i<112;i++) ((Arr*)obj)->arr[i]=0;
*(int*)(self+0x214)=0;
*(int*)(self+0x218)=0;
*(int*)(self+0x21c)=0;
*(int*)(self+0x220)=0;
memset(self+0x28c,0,0x5dc0);
memset(self+0x254,0,12);
memset(self+0x27c,0,12);
memset(self+0x6154,0,12);
return obj;
}
}

// TODO: clean up, give variables real names
namespace jj_C04 {
extern "C" unsigned char lbl_8059E742;

extern "C" {
unsigned char fn_11_C04(void)
{
    return lbl_8059E742;
}
}
}

// TODO: clean up, give variables real names
namespace jj_E4C {
class Group {
public:
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
 virtual void v8(); virtual void v9(); virtual void v10();
 virtual void insert(int, nw4r::g3d::ScnMdl*);
 virtual void v12();
 virtual void remove(nw4r::g3d::ScnMdl*);
 char pad[0xe0];
 int count;
};
struct Menu {
 char pad[0x50]; Group* group; char pad54[0xc]; MuObject* obj60; MuObject* obj64;
 char pad68[0x60e8]; unsigned char flag;
};
struct Global { char pad[0x10]; unsigned char* a; char pad14[8]; unsigned char* b; };
inline bool inRange(int mode) { if (mode >= 4 && mode < 12) return true; return false; }
extern "C" {
extern Global* g_GameGlobal;
extern const float lbl_11_rodata_0[];
extern const float lbl_11_rodata_164[];
int getRuleDispKind__17muSelectStageTaskFv(Menu*);
void dispMeleeRuleNum__17muSelectStageTaskFv(Menu*, int);
void dispPlayerFace__17muSelectStageTaskFv(Menu*, int, int, int, int, int, int);
void dispMelee__17muSelectStageTaskFv(Menu* self) {
 unsigned char* a = g_GameGlobal->a;
 unsigned char* b = g_GameGlobal->b;
 int frame = getRuleDispKind__17muSelectStageTaskFv(self);
 if (frame != 0) {
  MuObject* obj = self->obj60;
  self->group->remove(obj->m_sceneModel);
  obj->m_modelAnim->setUpdateRate(lbl_11_rodata_0[0]);
  obj->setFrameTex((float)frame);
  self->group->insert(self->group->count, obj->m_sceneModel);
  int mode = a[4];
  bool special = inRange(mode);
  if (!special && ((unsigned char)mode != 3 || !self->flag)) {
   MuObject* obj2 = self->obj64;
   int kind = b[2];
   self->group->remove(obj2->m_sceneModel);
   obj2->m_modelAnim->setUpdateRate(lbl_11_rodata_0[0]);
   obj2->setFrameTex(lbl_11_rodata_164[kind]);
   self->group->insert(self->group->count, obj2->m_sceneModel);
   int value;
   if (b[2] != 1) {
    value = b[3];
    if (value <= 0) value = -1;
   } else value = b[4];
   dispMeleeRuleNum__17muSelectStageTaskFv(self, value);
   unsigned char* p = a;
   unsigned char* q = a;
   for (int i = 0; i < 4; ++i, p += 0x5c, q += 4) {
    dispPlayerFace__17muSelectStageTaskFv(self, i, p[0xb9], q[0xa], p[0xbd], a[0x33], p[0xc3]);
   }
  }
 }
}
}
}

// TODO: clean up, give variables real names
namespace jj_1018 {
extern "C" {
void* getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(u32);
void* Construct__Q34nw4r3g3d6ScnMdlFPviiPv(void*, s32, s32, void*);
void changeNodeAnimN__8MuObjectFPCc(void*, const char*);
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, const char*);
void createObjResFile__17muSelectStageTaskFv(u8* self, const char* name, s32 count, s32 a, s32 b, s32 c);
void getPos__8MuObjectFP5Vec3fPCc(void* obj, float* out, const char* name);
extern char lbl_11_rodata_0[];
extern char lbl_11_data_0[];
}

extern "C" void createObj__17muSelectStageTaskFv(u8* self, s32 a2, s32 flag, s32 a4, s32 a5, s32 a6) {
    float pos[3];
    u8* rodata = (u8*)lbl_11_rodata_0;
    u8* data = (u8*)lbl_11_data_0;
    void* heap;
    void* mdl;
    const char* name;

    createObjResFile__17muSelectStageTaskFv(self, (const char*)(rodata + 0xc8), 8, a2, 0, a6);
    heap = getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(0x2a);
    mdl = Construct__Q34nw4r3g3d6ScnMdlFPviiPv(heap, 0, 0x51, *(void**)(*(u8**)(self + 0x84) + 0x10));
    name = (const char*)(data + 0x250);
    *(void**)(self + 0x214) = mdl;
    if (*(s32*)(self + 0x40) == 2) {
        name = (const char*)(data + 0x268);
    }
    changeNodeAnimN__8MuObjectFPCc(*(void**)(self + 0x84), name);
    if (flag != 0) {
        createObjResFile__17muSelectStageTaskFv(self, (const char*)(rodata + 0x20), 0x15, flag, a4, a6);
        createObjResFile__17muSelectStageTaskFv(self, (const char*)(rodata + 0x108), 1, a5, 0, a6);
        *(void**)(self + 0x220) = Construct__Q34nw4r3g3d6ScnMdlFPviiPv(heap, 0, 1, *(void**)(*(u8**)(self + 0x58) + 0x10));
        *(void**)(self + 0x218) = Construct__Q34nw4r3g3d6ScnMdlFPviiPv(heap, 0, 0xa, *(void**)(*(u8**)(self + 0x88) + 0x10));
        mdl = Construct__Q34nw4r3g3d6ScnMdlFPviiPv(heap, 0, 1, *(void**)(*(u8**)(self + 0x1ec) + 0x10));
        *(void**)(self + 0x21c) = mdl;
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(mdl, *(void**)(*(u8**)(self + 0x1f0) + 0x10), (const char*)(data + 0x280));
        getPos__8MuObjectFP5Vec3fPCc(*(void**)(self + 0x88), pos, (const char*)(data + 0x280));
        *(float*)(self + 0x6050) = pos[1];
    }
    {
        u8* p = *(u8**)(self + 0x48);
        *(u8**)(p + 0x40) = self + 0x54;
        *(s32*)(p + 0x44) = 0x70;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_118C {
typedef unsigned char u8;

struct Entry {
    const char* name;
    u8 begin;
    u8 end;
    u8 kind;
    u8 useResource : 1;
    u8 incrementKind : 1;
    u8 unused : 6;
};

struct Owner {
    char pad[0x54];
    void* objs[1];
};

extern "C" {
extern char lbl_11_data_288[];

void* create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
    void*, const char*, int, void*, int);
void changeAnimN__8MuObjectFPCc(void*, const char*);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
}

extern "C" void createObjResFile__17muSelectStageTaskFv(
    void* self, Entry* entries, int count, void* resFile,
    void* otherResFile, int argument)
{
    char buf[0x48];
    int n;
    int i;
    int j;
    Entry* entry = entries;
    void* obj;

    for (i = 0; i < count; i++, entry++) {
        n = 1;
        if (entry->begin < entry->end)
            n = entry->end - entry->begin;

        for (j = 0; j < n; j++) {
            void* resource = entry->useResource ? otherResFile : 0;
            int kind = entry->kind + argument;
            if (entry->incrementKind)
                kind += j;

            obj = create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
                resFile, entry->name, kind, resource, 0x2a);

            const char* name = entry->name;
            ((Owner*)self)->objs[entry->begin + j] = obj;
            strcpy(buf, name);
            strcat(buf, lbl_11_data_288);
            changeAnimN__8MuObjectFPCc(obj, buf);
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_1294 {

class Scene {
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
    virtual void f13(void*, void*);
    char pad[0xe0];
    void* fieldE4;
};
struct Stage {
    char pad0[0x40];
    int mode;
    char pad44[0xc];
    Scene* scene;
    MuObject* objects[116];
    char pad224[0x6074 - 0x224];
    void* field6074;
    void* field6078;
    int field607c;
    char pad6080[0x6150 - 0x6080];
    u8 field6150;
};
extern "C" {
struct Rodata { float zero; char pad[0x110 - 4]; u8 indices[4]; float one; float two; };
extern const Rodata lbl_11_rodata_0;
extern char lbl_11_data_280[];
void* fn_800DA15C(int, void*, void*, void*, int, int, int);
void setCounter__16muWifiCntWndTaskFii(void*, int, int);
}

extern "C" void dispSelStageBackGround__17muSelectStageTaskFv(Stage* self) {
    MuObject* obj;
    const u8* indices = lbl_11_rodata_0.indices;
    const Rodata* rodata = &lbl_11_rodata_0;
    for (int i = 0; i < 3; ++i) {
        self->scene->f13(self->scene->fieldE4, self->objects[*indices]->m_sceneModel);
        ++indices;
    }
    self->scene->f13(self->scene->fieldE4, self->objects[115]);
    obj = self->objects[2];
    obj->m_modelAnim->setUpdateRate(rodata->zero);
    float frame;
    if (self->field6150) {
        int flag = 0;
        unsigned int n = self->mode - 1;
        if (n <= 5 && ((1 << n) & 0x29)) flag = 1;
        if (flag) frame = rodata->one;
        else frame = rodata->zero;
    } else {
        frame = rodata->two;
    }
    obj->setFrameVisible(frame);
    int flag = 0;
    unsigned int n = self->mode - 1;
    if (n <= 5 && ((1 << n) & 0x29)) flag = 1;
    if (flag) {
        obj = self->objects[109];
        obj->setFrameVisible(rodata->zero);
        obj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(rodata->zero);
        self->scene->f13(self->scene->fieldE4, obj->m_sceneModel);
        self->field6078 = fn_800DA15C(2, self->field6074, self->objects[115], lbl_11_data_280, 0x22, 0x2a, 0x2a);
        if (self->field607c >= 0) {
            setCounter__16muWifiCntWndTaskFii(self->field6078, self->field607c, 0);
        }
    }
    self->objects[106]->m_modelAnim->setUpdateRate(rodata->zero);
}
}

// TODO: clean up, give variables real names
namespace jj_1468 {
extern "C" {
extern void* g_GameGlobal;
unsigned int* getStageAppFlag__10GameGlobalFv(void*);
void* alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(int, unsigned long);
int MuStageTblAcces_GetNumStageInPage__14MuStageTblPageFv(int);
int MuStageTblAcces_GetStageKind__14MuStageTblPageFv(int, int);
int MuStageTblAcces_GetForceExistFlag__14MuStageTblPageFv(int, int);
int exchangeMuStageKindToGmHideStageKind__6muMenuFiii(int);
}

struct Sel {
    unsigned char pad[0x40];
    int mode;
    unsigned char pad2[0x22C - 0x44];
    struct {
        int* entries;
        unsigned char count;
        unsigned char pad3[3];
    } groups[3];
};

extern "C" void makeStageThumbTbl__17muSelectStageTaskFv(Sel* arg) {
    Sel* self = arg;
    unsigned int* bits = getStageAppFlag__10GameGlobalFv(g_GameGlobal);
    char* p = (char*)self;
    for (int i = 0; i < 3; ++i) {
        if (self->mode != 2 || i != 2) {
            *(int**)(p + 0x22C) = (int*)alloc__13gfHeapManagerFQ25Heaps8HeapTypeUl(0x2a, 0x27);
            *(unsigned char*)(p + 0x230) = 0;
            int n = MuStageTblAcces_GetNumStageInPage__14MuStageTblPageFv(i);
            for (int j = 0; j < n; ++j) {
                if (self->mode == 2 && MuStageTblAcces_GetStageKind__14MuStageTblPageFv(i, j) == 0x1E)
                    continue;
                if (MuStageTblAcces_GetForceExistFlag__14MuStageTblPageFv(i, j) == 0) {
                    int id = exchangeMuStageKindToGmHideStageKind__6muMenuFiii(MuStageTblAcces_GetStageKind__14MuStageTblPageFv(i, j));
                    if (id != 0xC && !(bits[id / 32] & (1 << (id % 32))))
                        continue;
                }
                *(unsigned char*)(*(char**)(p + 0x22C) + *(unsigned char*)(p + 0x230)) = j;
                ++*(unsigned char*)(p + 0x230);
            }
            if (self->mode == 2 && i == 1) {
                *(unsigned char*)(*(char**)(p + 0x22C) + *(unsigned char*)(p + 0x230)) = 0xFF;
                ++*(unsigned char*)(p + 0x230);
            }
        }
        p += 8;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_19FC {
extern "C" {
extern const float lbl_11_rodata_0[];
void dispEditList__17muSelectStageTaskFv(void*, float);
void setFrameVisible__8MuObjectFf(void*, float);
void setFrameMatCol__8MuObjectFf(void*, float);
}

class Animation {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void setRate(float);
};

class Group {
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
    virtual void insert(int, void*);
    virtual void remove(int);
    char pad[0xe0];
    int count;
};

struct Animations {
    char pad0[8];
    Animation* visible;
    char pad1[0xc];
    Animation* matCol;
};

struct Object {
    char pad0[0x10];
    void* model;
    Animations* animations;
};

struct Menu {
    char pad0[0x50];
    Group* group;
    char pad1[0x170];
    Object* model;
    Object* objects[7];
    Object* matCol1;
    Object* matCol2;
    char pad2[4];
    Object* visible;
    char pad3[0x24];
    Group* children;
};

inline void resetVisible(Object* object) {
    setFrameVisible__8MuObjectFf(object, lbl_11_rodata_0[0]);
    object->animations->visible->setRate(lbl_11_rodata_0[0]);
}

inline void resetMatCol(Object* object) {
    setFrameMatCol__8MuObjectFf(object, lbl_11_rodata_0[0]);
    object->animations->matCol->setRate(lbl_11_rodata_0[0]);
}

extern "C" void dispEditPage__17muSelectStageTaskFv(Menu* self, float delta) {
    Group* group = self->children;
    while (group->count != 0) {
        if (group->count != 0) {
            group->remove(group->count - 1);
        }
    }
    dispEditList__17muSelectStageTaskFv(self, delta);
    self->group->insert(self->group->count, self->model->model);
    for (int i = 0; i < 7; ++i) {
        resetVisible(self->objects[i]);
    }
    resetMatCol(self->matCol1);
    resetMatCol(self->matCol2);
    resetVisible(self->visible);
}
}

// TODO: clean up, give variables real names
namespace jj_2F04 {
struct Entry {
    unsigned char flags : 1;
    unsigned char rest : 7;
    unsigned char pad[3];
    int index;
    int value;
};

struct Object {
    unsigned char pad[0x28C];
    Entry entries[2000];
    int count;
};

extern "C" {
extern void* lbl_11_bss_8;

void deleteAllFileData__21muSelectStageFileTaskFv(void*);
int getNumFile__21muSelectStageFileTaskFi(void*, int);
int editListCompare__17muSelectStageTaskFv(const void*, const void*);
void qsort(void*, unsigned int, unsigned int,
                 int (*)(const void*, const void*));

void makeEditList__17muSelectStageTaskFv(Object* self) {
    int i;
    int count = 0;
    deleteAllFileData__21muSelectStageFileTaskFv(lbl_11_bss_8);

    Entry* entry = self->entries;
    for (i = 0; i < getNumFile__21muSelectStageFileTaskFi(lbl_11_bss_8, 0) && count < 2000; ++i) {
        entry->flags = 0;
        ++count;
        entry->index = i;
        entry->value = -1;
        ++entry;
    }

    entry = &self->entries[count];
    for (i = 0; i < getNumFile__21muSelectStageFileTaskFi(lbl_11_bss_8, 1) && count < 2000; ++i) {
        entry->flags = 1;
        ++count;
        entry->index = i;
        entry->value = -1;
        ++entry;
    }

    self->count = count;
    qsort(self->entries, count, sizeof(Entry), editListCompare__17muSelectStageTaskFv);
}
}
}

// TODO: clean up, give variables real names
namespace jj_3004 {
extern "C" {
extern void* lbl_11_bss_8;

bool isDeliveryData__21muSelectStageFileTaskFii(char*, bool, int);
void* getFileName__21muSelectStageFileTaskFii(char*, bool, int);
int strcmp(const char*, const char*);

struct Entry {
    unsigned char flags : 1;
    unsigned char rest : 7;
    int index;
};

int editListCompare__17muSelectStageTaskFv(Entry* a, Entry* b) {
    bool fa = a->flags;
    bool fb = b->flags;
    int ia = a->index;
    int ib = b->index;

    if (isDeliveryData__21muSelectStageFileTaskFii((char*)lbl_11_bss_8, fa, ia)) {
        return -1;
    }

    return strcmp(
        (char*)getFileName__21muSelectStageFileTaskFii((char*)lbl_11_bss_8, fb, ib),
        (char*)getFileName__21muSelectStageFileTaskFii((char*)lbl_11_bss_8, fa, ia));
}
}
}

// TODO: clean up, give variables real names
namespace jj_4270 {
typedef unsigned long u32;

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
    virtual void v11();
    virtual void v12();
    virtual void set(void*);
};

struct Pane {
    char pad[0x10];
    void* data;
};

struct Result {
    char pad[0x120];
    bool enabled;
    char pad2[7];
    u32 flags;
};

struct Menu {
    char pad[0x50];
    Display* display;
    char pad2[0x200 - 0x54];
    Pane* pane;
    char pad3[0x6068 - 0x204];
    void* resource;
    Result* result;
    void* message;
};

extern "C" {
u32 writeTagClear__7MessageFPc(char*);
void getPrintIndexData__7MessageFPvUlPPcPUl(void*, u32, char**, u32*);
void utf16to8__7MessageFPcPCw(char*, u32);
void* memcpy(void*, const void*, u32);
u32 strlen(const char*);
Result* fn_800BF048(int, char*, u32, void*, Display*,
                   int, int, int, int, int, int, int);
void setYesButtonLabel__15muNoticeWndTaskFPvi(Result*, void*, int);
void setNoButtonLabel__15muNoticeWndTaskFPvi(Result*, void*, int);

void openJoinConfirmDlg__17muSelectStageTaskFv(Menu* self, u32 value) {
    char buffer[0x100];
    char number[0x18];
    char* text;
    u32 size;

    u32 pos = writeTagClear__7MessageFPc(buffer);
    getPrintIndexData__7MessageFPvUlPPcPUl(self->message, 31, &text, &size);
    memcpy(buffer + pos, text, size);
    pos += size;

    utf16to8__7MessageFPcPCw(number, value);
    u32 len = strlen(number);
    memcpy(buffer + pos, number, len);
    pos += len;

    getPrintIndexData__7MessageFPvUlPPcPUl(self->message, 32, &text, &size);
    memcpy(buffer + pos, text, size);
    pos += size;
    buffer[pos] = 0;

    self->display->set(self->pane->data);
    self->result = fn_800BF048(1, buffer, pos + 1, self->resource,
                              self->display, 23, 60, 42, 42, 0, 0, 240);
    self->result->enabled = true;
    self->result->flags = 0x2055;
    setYesButtonLabel__15muNoticeWndTaskFPvi(self->result, self->message, 33);
    setNoButtonLabel__15muNoticeWndTaskFPvi(self->result, self->message, 34);
}
}
}

// TODO: clean up, give variables real names
namespace jj_5F24 {
class Scene {
public:
 virtual void s08(); virtual void s0C(); virtual void s10(); virtual void s14();
 virtual void s18(); virtual void s1C(); virtual void s20(); virtual void s24();
 virtual void s28(); virtual void s2C(); virtual void s30(); virtual void s34();
 virtual void s38(); virtual void detach(void*);
};
struct Object { char pad[12]; void* model; void* resource; };
struct Menu {
 char pad0[0x50]; Scene* scene;
 Object* objects[109];
 char pad208[12]; Scene* otherScene;
 char pad218[0x2C]; int current; int pad248; int selected;
 char pad250[0x5E38]; float scroll;
 char pad608C[0xC0]; int base;
};
extern const float lbl_11_rodata_0[];
extern const float lbl_11_rodata_114[];
extern "C" {
 void setFrameVisible__8MuObjectFf(Object*,float);
 void SetPriorityDrawOpa__Q34nw4r3g3d6ScnObjFi(void*,int);
 void SetPriorityDrawXlu__Q34nw4r3g3d6ScnObjFi(void*,int);
 void dispEditPreview__17muSelectStageTaskFv(Menu*,int);
 void dispPreview__17muSelectStageTaskFv(Menu*,int);
}
inline void reset(Menu* self) {
 self->otherScene->detach(self->objects[108]->resource);
 for(int i=0;i<7;i++) {
  int value;
  Object* o=self->objects[i+93];
  if(o) {
   setFrameVisible__8MuObjectFf(o,lbl_11_rodata_0[0]);
   value=i+self->base+12;
   SetPriorityDrawOpa__Q34nw4r3g3d6ScnObjFi(o->model,value);
   SetPriorityDrawXlu__Q34nw4r3g3d6ScnObjFi(o->model,value);
  }
 }
}
inline void reset2(Menu* self) {
 self->otherScene->detach(self->objects[108]->resource);
 Object* o;
 int value;
 int i;
 for(i=0;i<7;i++) {
  o=self->objects[i+93];
  if(o) {
   setFrameVisible__8MuObjectFf(o,lbl_11_rodata_0[0]);
   value=i+self->base+12;
   SetPriorityDrawOpa__Q34nw4r3g3d6ScnObjFi(o->model,value);
   SetPriorityDrawXlu__Q34nw4r3g3d6ScnObjFi(o->model,value);
  }
 }
}
inline void highlight(Menu* self,int index) {
  int i=index-(int)self->scroll;
  if(i>=0 && i<7) {
   Object* o=self->objects[i+93];
   setFrameVisible__8MuObjectFf(o,lbl_11_rodata_114[0]);
   SetPriorityDrawOpa__Q34nw4r3g3d6ScnObjFi(o->model,19);
   SetPriorityDrawXlu__Q34nw4r3g3d6ScnObjFi(o->model,19);
  }
}
extern "C" void pointPointerEditList__17muSelectStageTaskFv(Menu* self,int stage) {
 if(stage==self->current) return;
 int index;
 if(stage>=41 && stage<48) index=stage+(int)self->scroll-41;
 else index=-1;
 if(index>=0) {
  reset2(self);
  highlight(self,index);
 } else reset(self);
 if(index>=0) dispEditPreview__17muSelectStageTaskFv(self,index);
 else if(stage!=0) dispPreview__17muSelectStageTaskFv(self,stage);
 else self->scene->detach(self->objects[11]->resource);
 self->selected=index;
}
}

// TODO: clean up, give variables real names
namespace jj_6384 {
class Scene {
public:
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
 virtual void v8(); virtual void v9(); virtual void v10();
 virtual void add(void*, void*); virtual void v14(); virtual void remove(void*);
 char pad[0xe0]; void* group;
};
class Anim {
public:
 virtual void v0(); virtual void v1(); virtual void v2(); virtual void v3();
 virtual void v4(); virtual void v5(); virtual void v6(); virtual void v7();
 virtual void frame(float);
};
struct Entry { int type; char pad[8]; unsigned char flag; char pad2[3]; unsigned int image; char pad3[12]; char name[1]; };
struct Item { int a; int b; int index; };
struct Self { char pad0[0x50]; Scene* scene; char pad1[0x2c]; MuObject* object; char pad2[0x208]; Item items[1]; };
extern "C" Entry* getFileData__21muSelectStageFileTaskFi(void*, int);
extern "C" void utf16to8__7MessageFPcPCw(char*, const void*);
extern "C" void GXInitTexObj(void*, unsigned int, unsigned short, unsigned short, unsigned int, unsigned int, unsigned int, unsigned char);
extern "C" void changeMaterialTex__8MuObjectFPCcP9_GXTexObj(void*, const char*, void*);
extern void* lbl_11_bss_8;
extern const char* lbl_11_data_414;
extern const float lbl_11_rodata_0[];
inline void setVis(MuObject* obj, Entry* entry, const float* rodata) {
 obj->setFrameVisible(entry->flag ? rodata[70] : rodata[69]);
}
extern "C" void dispEditPreview__17muSelectStageTaskFv(Self* self, int index) {
 const float* rodata = lbl_11_rodata_0;
 char name[0x50];
 char texture[0x20];
 self->scene->remove(self->object->m_sceneModel);
 Item* item = &self->items[index];
 int kind;
 if (item->index >= 0) {
  switch (getFileData__21muSelectStageFileTaskFi(lbl_11_bss_8, item->index)->type) {
  case 3: kind = 0; break;
  case 4: kind = 2; break;
  default: kind = 1; break;
  }
 } else { kind = 1; }
 if (kind == 0) {
  utf16to8__7MessageFPcPCw(name, getFileData__21muSelectStageFileTaskFi(lbl_11_bss_8, item->index)->name);
  (*(MuMsg**)((char*)self + 0x6064))->printf(0, name);
  MuObject* obj = self->object;
  GXInitTexObj(texture, getFileData__21muSelectStageFileTaskFi(lbl_11_bss_8, item->index)->image, 0xa0, 0x78, 6, 0, 0, 0);
  changeMaterialTex__8MuObjectFPCcP9_GXTexObj(obj, lbl_11_data_414, texture);
  setVis(obj, getFileData__21muSelectStageFileTaskFi(lbl_11_bss_8, item->index), rodata);
  (*(Anim**)((char*)obj->m_modelAnim + 8))->frame(rodata[0]);
  self->scene->add(self->scene->group, obj->m_sceneModel);
 }
}
}

// TODO: clean up, give variables real names
namespace jj_6898 {
extern const char lbl_11_data_0[];
extern "C" {
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
int sprintf(char*, const char*, ...);
void getGlobalPosition__8MuObjectFPCc(Vec*, void*, const char*);
}
inline nw4r::math::VEC3 getpos(void* a, const char* name) {
    Vec v;
    getGlobalPosition__8MuObjectFPCc(&v, a, name);
    return nw4r::math::VEC3(v);
}
extern "C" int isObjPointed__17muSelectStageTaskFv(void* unused, const Vec* pos, void* a, s32 idx) {
    char sp18[8];
    char sp10[8];
    char sp8[8];
    strcpy(sp18, lbl_11_data_0 + 0x428);
    strcpy(sp10, lbl_11_data_0 + 0x430);
    if (idx >= 0) {
        sprintf(sp8, lbl_11_data_0 + 0x438, idx);
        strcat(sp18, sp8);
        strcat(sp10, sp8);
    }
    nw4r::math::VEC3 v1 = getpos(a, sp18);
    nw4r::math::VEC3 v2 = getpos(a, sp10);
    if (pos->x >= v1.x && pos->x <= v2.x && pos->y >= v1.y && pos->y <= v2.y) return 1;
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_7590 {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__21muSelectStageFileTaskFv(void* p);
extern void* g_GameGlobal;
}

extern "C" void* fn_11_7590(void* arg) {
    unsigned char* obj = (unsigned char*)__nw__FUlQ25Heaps8HeapType(0x3a0, 0x2a);
    if (obj != 0) {
        obj = (unsigned char*)__ct__21muSelectStageFileTaskFv(obj);
    }
    unsigned char* gg = (unsigned char*)(*(void**)g_GameGlobal);
    if (gg[0x10] != 0) {
        obj[0x48] |= 0x80;
    }
    *(void**)(obj + 0x39c) = arg;
    *(unsigned int*)(obj + 0x40) = 0;
    return obj;
}
}

