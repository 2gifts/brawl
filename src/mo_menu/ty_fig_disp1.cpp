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

#include <gf/gf_model.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <string.h>
// TODO: clean up, give variables real names
namespace jj_6470 {
extern "C" void __ct__14muFileIOHandleFv(int *ptr) {
    *ptr = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_64D4 {
typedef unsigned char u8;
typedef int s32;

class MenuObject {
    char pad[0x5C];
public:
    virtual void f0(int);
};

extern "C" {
extern u8 lbl_2_data_DB8[];
extern void* g_sndSystem;

void stopSE__9sndSystemFll(void*, int, int);
void __dt__14muFileIOHandleFv(void*, int);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void free__13gfHeapManagerFPv(void*);
void cancelRequest__14gfFileIOHandleFv(void*);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__6gfTaskFv(void*, int);
void __dt__9gfArchiveFv(void*, int);
void __dl__FPv(void*);

void* __dt__14tyFigDisp1TaskFv(u8* object, int arg1) {
    if (object) {
        s32 p0 = *(s32*)(object + 0x704);
        *(void**)(object + 0x3C) = lbl_2_data_DB8;
        if (p0 >= 0) {
            stopSE__9sndSystemFll(g_sndSystem, p0, 0);
        }

        if (*(void**)(object + 0x40)) {
            for (int i = 0; i < 2; ++i) {
                void* p = *(void**)(*(u8**)(object + 0x40) + i * 4 + 0x1578);
                if (p) {
                    Destroy__Q34nw4r3g3d6G3dObjFv(p);
                }
            }

            void* p = *(void**)(*(u8**)(object + 0x40) + 0x1630);
            if (p) {
                Destroy__Q34nw4r3g3d6G3dObjFv(p);
            }

            for (int i = 0; i < 0x519; ++i) {
                MenuObject* m = *(MenuObject**)(*(u8**)(object + 0x40) + i * 4 + 0x114);
                if (m && m) {
                    m->f0(1);
                }
            }

            void* q = *(void**)(*(u8**)(object + 0x40) + 4);
            if (q) free__13gfHeapManagerFPv(q);
            q = *(void**)(*(u8**)(object + 0x40) + 0x10C);
            if (q) free__13gfHeapManagerFPv(q);
            q = *(void**)(*(u8**)(object + 0x40) + 0x110);
            if (q) free__13gfHeapManagerFPv(q);
            free__13gfHeapManagerFPv(*(void**)(object + 0x40));

            int i;
            u8* cursor = object + 0x44;
            *(void**)(object + 0x40) = 0;
            i = 0;
            for (; i < 0x44; ++i, cursor += 4) {
                cancelRequest__14gfFileIOHandleFv(cursor);
            }
        }

        __dt__9gfArchiveFv(object + 0x154, -1);
        __destroy_arr(object + 0x44, __dt__14muFileIOHandleFv, 4, 0x44);
        __dt__6gfTaskFv(object, 0);
        if (arg1 > 0) {
            __dl__FPv(object);
        }
    }
    return object;
}
}
}

// TODO: clean up, give variables real names
namespace jj_668C {
extern "C" {
void readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(void* self, const char* name, int heap, int a, int b);
void* getGlobalRecordMenuDatap__10GameGlobalFv(void* global);
void fn_800151FC(void* self, const char* name, int heap, int a, int b, int c);
void* getInstance__9itManagerFv();
void loadMenuSound__6muMenuFv();

void fn_2_668C(u8* self);
}

extern void* g_GameGlobal;
extern u8 lbl_2_data_B98[];

void fn_2_668C(u8* self) {
    u8* base = lbl_2_data_B98;
    u32 r30 = 0;
    u8* rec;
    const char* name;

    readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(self + 0x148, (const char*)(base + 0x8), 0x2b, 0, 0);

    rec = (u8*)getGlobalRecordMenuDatap__10GameGlobalFv(g_GameGlobal);
    {
        u8* dst = self + 0x14c;
        name = (const char*)(base + 0x5c);
        if ((rec[0x28] >> 7) & 1) {
            name = (const char*)(base + 0x2c);
        }
        readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(dst, name, 0x2b, 0, 0);
    }

    rec = (u8*)getGlobalRecordMenuDatap__10GameGlobalFv(g_GameGlobal);
    {
        u8* dst = self + 0x150;
        name = (const char*)(base + 0xb8);
        if ((rec[0x28] >> 7) & 1) {
            name = (const char*)(base + 0x88);
        }
        readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(dst, name, 0x2b, 0, 0);
    }

    fn_800151FC(self + 0x154, (const char*)(base + 0xe4), 0x2b, 0, 0, 0xff);

    {
        u8* itm = (u8*)getInstance__9itManagerFv();
        u32* p = *(u32**)(itm + 0x14b8);
        if (p != nullptr) {
            r30 = p[1];
        }
    }

    *(u32*)(*(u8**)(self + 0x40) + 0x162c) = r30;
    loadMenuSound__6muMenuFv();
}
}

// TODO: clean up, give variables real names
namespace jj_6984 {
extern "C" void fn_800B96E8(void*, int);

struct Obj {
    u8 pad[0x40];
    u8* p;
};

extern "C" void initWork__14tyFigDisp1TaskFv(Obj* self) {
    u32 r7 = 0;
    u32 z = 0;
    u32 r6 = 0;
    int i;
    u32 off;

    *(u32*)(self->p + 0x0) = z;
    *(u32*)(self->p + 0x4) = z;
    *(u32*)(self->p + 0x10C) = z;
    *(u32*)(self->p + 0x110) = z;

    for (r7 = 0; r7 < 65; ++r7) {
        *(u32*)(self->p + 0x8 + r7 * 4) = z;
    }

    for (r7 = 0; r7 < 1305; ++r7) {
        *(u32*)(self->p + 0x114 + r7 * 4) = 0;
    }
    *(u32*)(self->p + 0x1578) = 0;
    *(u32*)(self->p + 0x157C) = 0;
    fn_800B96E8(self->p + 0x1580, 0xF0);
    *(u8*)(self->p + 0x1628) = 0;
    *(u16*)(self->p + 0x162A) = 0;
    *(u32*)(self->p + 0x162C) = 0;
    *(u32*)(self->p + 0x1630) = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_6B9C {

typedef struct Entry {
    const char* name;
    u16 idxBegin;
    u16 idxEnd;
    u8 drawPriority;
    u8 flags;
} Entry;

extern "C" {
extern char lbl_2_data_D0C[];
extern float lbl_2_rodata_18C;
MuObject* createAlt__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(nw4r::g3d::ResFile* modelSource, const char* modelNode, u8 drawPriority, nw4r::g3d::ResFile* textureSource, HeapType type);

void createObjResFile__14tyFigDisp1TaskFv(u8* arg0, Entry* arg1, s32 arg2, nw4r::g3d::ResFile* arg3) {
    char buf[0x40];
    s32 i;
    Entry* e;
    MuObject* obj;
    s32 j;
    s32 n;
    u8* slot;
    const char* name;
    e = arg1;
    for (i = 0; i < arg2; i++, e++) {
        n = (e->idxBegin < e->idxEnd) ? (e->idxEnd - e->idxBegin) : 1;
        for (j = 0; j < n; j++) {
            if ((e->flags >> 7) & 1) {
                obj = MuObject::create(arg3, e->name, e->drawPriority, 0, (HeapType)0x2a);
            } else {
                obj = createAlt__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(arg3, e->name, e->drawPriority, 0, (HeapType)0x2a);
            }
            name = e->name;
            slot = *(u8**)(arg0 + 0x40) + ((e->idxBegin + j) << 2);
            *(MuObject**)(slot + 0x114) = obj;
            strcpy(buf, name);
            strcat(buf, lbl_2_data_D0C);
            obj->changeAnimN(buf);
            if (obj->m_modelAnim != 0) {
                obj->m_modelAnim->setUpdateRate(lbl_2_rodata_18C);
            }
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_6CBC {
extern "C" {
extern const char lbl_2_data_D10[];
extern const char lbl_2_data_D1C[];
unsigned long getBilTexFileIdx__20tyFigListDataManagerFUl(void*, unsigned long);
void getBilTexName__20tyFigListDataManagerFUlPc(void*, unsigned long, char*);
void* GetResTex__Q34nw4r3g3d7ResFileCFPCc(void*, const char*);
void* GetResMat__Q34nw4r3g3d6ResMdlCFPCc(void*, const char*);
void changeMaterialTex__8MuObjectFUlPCcPQ34nw4r3g3d7ResFile(void*, unsigned long, const char*, void*);
void OSReport(const char*, ...);
}
struct Data {
char pad[8];
unsigned long textures[69];
void* models[1];
};
struct Obj { char pad[64]; Data* data; };
static inline void impl(void* selfArg, void* manager, unsigned short* input, unsigned int countArg) {
char name[32];
int loaded = 0;
const char* material = lbl_2_data_D10;
unsigned int count = countArg;
Obj* self = (Obj*)selfArg;
unsigned long id;
unsigned long* tex;
int i = 0;
int offset = 0;
unsigned short* ids = input;
for(; i<(int)count; ++i) {
id = *ids;
unsigned long idx = getBilTexFileIdx__20tyFigListDataManagerFUl(manager,id);
tex = &self->data->textures[idx];
if (*tex) {
getBilTexName__20tyFigListDataManagerFUlPc(manager,id,name);
if(GetResTex__Q34nw4r3g3d7ResFileCFPCc(tex,name)) {
void* model = *(void**)((char*)self->data + 0x11c + offset);
void* mat = GetResMat__Q34nw4r3g3d6ResMdlCFPCc((char*)model+8,material);
changeMaterialTex__8MuObjectFUlPCcPQ34nw4r3g3d7ResFile(model,*(unsigned long*)((char*)mat+12),name,tex);
++loaded;
}
}
++ids;
offset += 4;
}
OSReport(lbl_2_data_D1C,loaded);
}
extern "C" void resurfaceFigTex__14tyFigDisp1TaskFv(void* self, void* manager, unsigned short* ids, int count) {
impl(self,manager,ids,count);
}
}

// TODO: clean up, give variables real names
namespace jj_6DBC {
struct gfPadStatus {
    unsigned char data[0x38];
    signed char flag;
    int type;
};

extern "C" {
extern const float lbl_2_rodata_138[];
extern char lbl_2_data_D30[];
extern char lbl_2_data_D44[];
extern char lbl_2_data_D90[];
extern char __RTTI__18gfCameraController[];
extern void* g_gfPadSystem;

void fn_1_6ACC(void* a, int b);
void changeAnimN__8MuObjectFPCc(void* self, const char* name);
void setUpdateRate__16gfModelAnimationFf(void* self, float f);
void makeAndPutFigure__14tyFigDisp1TaskFv(void* a, int b, int c, float* d, float* e, void* f, int g);
void fn_800A97AC(float f);
void* setObjScale__8MuObjectFP5Vec3f(void* a, float* b);
void* getInstance__16CameraControllerFv(void);
void* __dynamic_cast(void* a, int b, void* c, void* d, int e);
void fn_1_6AE0(void* a, int b);
void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void* self, int idx, gfPadStatus* st);
void OSReport(const char* fmt, ...);
void* setFrameFlg__8MuObjectFfUc(void* a, int b);
}

#define OBJ (*(char**)(self + 0x40))

extern "C" void* fn_2_6DBC(char* self) {
    const float* c = lbl_2_rodata_138;
    gfPadStatus st;
    float local[3];
    float spC;
    float sp8;
    float k;
    void* ret;
    float f;

    fn_1_6ACC(*(void**)OBJ, *(int*)(*(char**)(OBJ + 0x156c) + 0x10));
    fn_1_6ACC(*(void**)OBJ, *(int*)(*(char**)(OBJ + 0x114) + 0x10));
    changeAnimN__8MuObjectFPCc(*(void**)(OBJ + 0x114), lbl_2_data_D30);
    setUpdateRate__16gfModelAnimationFf(*(void**)(*(char**)(OBJ + 0x114) + 0x14), c[22]);
    fn_1_6ACC(*(void**)OBJ, *(int*)(OBJ + 0x1578));
    fn_1_6ACC(*(void**)OBJ, *(int*)(OBJ + 0x157c));
    makeAndPutFigure__14tyFigDisp1TaskFv(self, *(int*)(OBJ + 0x162c), *(int*)(self + 0x6f8), &spC, &sp8, self + 0x1e0, *(int*)(self + 0x6f4));
    k = c[23];
    local[1] = c[22];
    local[2] = (k + sp8) / k;
    local[0] = (k + spC) / k;
    OSReport(lbl_2_data_D44, local[0], local[1], local[2], k);
    setObjScale__8MuObjectFP5Vec3f(*(void**)(OBJ + 0x156c), local);
    __dynamic_cast(*(void**)((char*)getInstance__16CameraControllerFv() + 0x48), 4, lbl_2_data_D90, __RTTI__18gfCameraController, 0);
    fn_800A97AC(spC);
    fn_1_6AE0(*(void**)OBJ, *(int*)(*(char**)(OBJ + 0x118) + 0x10));
    getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, *(int*)(self + 0x6fc), &st);

    ret = *(void**)(OBJ + 0x118);
    if (st.flag != 0) {
        f = c[24];
    } else {
        switch (st.type) {
        case 0: f = c[21]; break;
        case 1: f = c[25]; break;
        case 2: f = c[26]; break;
        case 3: f = c[22]; break;
        default: f = c[21]; break;
        }
    }
    if (f >= c[21]) {
        ret = setFrameFlg__8MuObjectFfUc(ret, 4);
    }
    return ret;
}
}

