#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_obj.h>
#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <ty/ty_fig_listmng.h>
#include <gf/gf_file_io_handle.h>
#include <mu/menu.h>
#include <types.h>
#include <gf/gf_pad_system.h>
#include <mu/mu_prize_wnd.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <string.h>
#include <gm/gm_common.h>

extern "C" int exchangeMuSelchkindToMuCharKind__6muMenuFiii(int selchKind);
extern "C" int getInfoMsgID__6muMenuFiii(int info);
extern const char lbl_8059E0FC[4];
extern const char lbl_8059E100[6];
extern const u8 lbl_804080E8[16];

extern struct {
    const char* unk0;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
} lbl_804080C0[];

extern const struct {
    void (*unk0)(muPrizeWndTask*);
    int (*unk4)(muPrizeWndTask*);
} lbl_80407C28[];

extern const struct {
    int unk0;
    int unk4;
} lbl_80407C58[];

extern const char lbl_80456DD8[];
extern const char lbl_8059E0F8[4];

muPrizeWndTask::muPrizeWndTask()
    : gfTask(lbl_80456DD8, Category_Menu, 15, 8, true),
      unk58(NULL), unk5C(NULL), unk108(true), unk10C(5), unk110(6),
      unk114(0), unk118(0), unk21C(0), unk220(0), unk224(NULL), unk228(NULL),
      unk22C(0), unk230(0), unk2B4(false), unk2B8(NULL), unk2BC(NULL),
      unk2C0(NULL), unk344(NULL), unk348(NULL), unk3CC(-1), unk3D0(false) {
    unk2C_b1 = false;
    for (int i = 0; i < 5; i++) {
        unk40[i] = NULL;
    }
    unk54[0] = NULL;
    memset(unk11C, 0, sizeof(unk11C));
}

muPrizeWndTask7TexLoadFv::muPrizeWndTask7TexLoadFv() {}

muPrizeWndTask7TexLoadFv::~muPrizeWndTask7TexLoadFv() {}

void muPrizeWndTask::createObjResFile(const void* models, int count,
                                     nw4r::g3d::ResFile* resFile, int priority) {
    int num;
    int i;
    int j;
    const __typeof__(lbl_804080C0[0])* model =
        static_cast<const __typeof__(lbl_804080C0[0])*>(models);
    for (i = 0; i < count; i++, model++) {
        num = 1;
        if (model->unk4 < model->unk5) {
            num = model->unk5 - model->unk4;
        }
        for (j = 0; j < num; j++) {
            MuObject* object = MuObject::create(resFile, model->unk0,
                model->unk6 + priority, NULL, Heaps::MenuInstance);
            unk40[model->unk4 + j] = object;
            char animName[0x40];
            strcpy(animName, model->unk0);
            strcat(animName, lbl_8059E0F8);
            object->changeAnimN(animName);
            if (object->m_modelAnim != NULL) {
                object->m_modelAnim->setUpdateRate(0.0f);
            }
        }
    }
}

void muPrizeWndTask::processDefault() {
    fn_800B9984(&unk60);
    if (unk110 != unk10C) {
        if (lbl_80407C28[unk10C].unk0 != NULL) {
            lbl_80407C28[unk10C].unk0(this);
        }
        unk110 = unk10C;
    }
    if (lbl_80407C28[unk10C].unk4 != NULL) {
        unk10C = lbl_80407C28[unk10C].unk4(this);
    }
}

bool muPrizeWndTask::appendPrizeList(const muPrizeWndEntry* entry) {
    if (unk21C >= 16) {
        return false;
    }
    prizeList[unk21C++] = *entry;
    return true;
}

int muPrizeWndTask::mainStepAppearMain(muPrizeWndTask* task) {
    if (task->unk40[0]->isNodeAnimFinished()) {
        return 3;
    }
    return task->unk10C;
}

void muPrizeWndTask::mainStepSelectingInit(muPrizeWndTask* task) {
    task->unk118 = 0;
    task->unk3D0 = false;
}

int muPrizeWndTask::mainStepSelectingMain(muPrizeWndTask* task) {
    bool isYes = true;
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    if (task->unk114 < 0) {
        return task->unk10C;
    }
    if (++task->unk118 < task->unk114) {
        return task->unk10C;
    }
    muPrizeWndEntry* entry = &task->prizeList[task->unk220];
    if (entry->unk0 == 2 && entry->unkC != 0) {
        bool old = task->unk108;
        bool selected = old;
        if (task->unk60.m_pressed & 1) {
            selected = true;
        }
        if (task->unk60.m_pressed & 2) {
            selected = false;
        }
        if (selected != old) {
            task->unk40[0]->setFrameMatCol(selected ? 1.0f : 2.0f);
            task->unk108 = selected;
            g_sndSystem->playSE(static_cast<SndID>(0), -1, 0, 0, -1);
        }
        isYes = !task->unk108;
    }
    if ((pad.m_buttonsPressedThisFrame.bits & 0x100) || (pad.m_buttonsPressedThisFrame.bits & 0x1000) || task->unk3D0) {
        if (isYes || task->unk3D0) {
            if (task->unk3CC != g_sndSystem->getBGMId()) {
                g_sndSystem->stopBGM(0);
            }
            return 4;
        }
        g_sndSystem->stopBGM(0);
        g_sndSystem->playBGM(entry->unk8, 0, true);
    }
    return task->unk10C;
}

void muPrizeWndTask::mainStepLeaveInit(muPrizeWndTask* task) {
    MuObject* object = task->unk40[0];
    char animName[0x40];
    const nw4r::g3d::ResMdlData& data = object->m_resMdl.ref();
    sprintf(animName, "%s__1", data.m_toName != 0 ? reinterpret_cast<const char*>(&data) + data.m_toName : NULL);
    object->changeNodeAnimN(animName);
    object->setFrameNode(0.0f);
    object->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
}

void muPrizeWndTask::setBgmID(int index, muPrizeWndEntry* entry) {
    entry->unk4 = lbl_80407C58[index].unk4;
}

void muPrizeWndTask::openWnd(int index) {
    nw4r::g3d::ScnMdlExpand* group = unk54[0];
    muPrizeWndEntry& entry = prizeList[index];
    MuObject* window = unk40[0];
    int frameIndex;
    window->setFrameVisible(0.0f);
    window->setFrameMatCol(0.0f);
    group->Remove(unk40[1]->m_sceneModel);
    group->Remove(unk40[2]->m_sceneModel);
    group->Remove(unk40[3]->m_sceneModel);
    group->Remove(unk40[4]->m_sceneModel);
    unk5C->printf(0, lbl_8059E0FC);
    unk5C->printf(1, lbl_8059E0FC);
    unk5C->printf(2, lbl_8059E0FC);
    unk5C->printf(3, lbl_8059E0FC);
    switch (entry.unk0) {
    case 0:
        setWndFigurePrize(index);
        break;
    case 1:
        setWndSealPrize(index);
        break;
    case 2:
        setWndBGMPrize(index);
        break;
    case 3: {
        muPrizeWndTask7TexLoadFv& texture = unk34C[index];
        int characterKind;
        MuObject* character = unk40[4];
        characterKind = entry.unk8;
        nw4r::g3d::ScnMdlExpand* characterGroup = unk54[0];
        character->changeMaterialTex("Character_dammy_Mat", 0, &texture.unk4);
        character->changeMaterialTex("Character_dammy_shadow_Mat", 0, &texture.unk4);
        characterGroup->PushBack(character->m_sceneModel, lbl_8059E100);
        int menuKind = exchangeMuSelchkindToMuCharKind__6muMenuFiii(characterKind);
        int message = getInfoMsgID__6muMenuFiii(static_cast<u8>(gmRetIfKindMenukind(menuKind)));
        unk5C->printIndex(0, message, NULL);
        unk40[0]->setFrameVisible(6.0f);
        break;
    }
    case 4: {
        frameIndex = entry.unkC_word;
        unk5C->printIndex(0, entry.unk8, NULL);
        unk40[0]->setFrameVisible(lbl_804080E8[frameIndex]);
        break;
    }
    }
    if (entry.unk4 > 0) {
        g_sndSystem->stopBGM(0);
        g_sndSystem->playBGM(entry.unk4, 0, true);
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_800C11F0(void) {}

extern "C" void fn_800C137C(void) {}

// TODO: clean up, give variables real names
namespace jj_800C1B20 {
extern "C" {
extern int lbl_80407C58[][2];

struct Entry {
    int pad[2];
    int key;   // 0x8
    int value; // 0xC
};

void fn_800C1B20(void* unused, Entry* p) {
    p->value = lbl_80407C58[p->key][0];
}
}
}

namespace jj_800C15AC {

extern "C" void fn_800C09FC(void* self, int v);
extern "C" int fn_800B5854(void* p, int a, int b, int c, int d);

class VObj {
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
    virtual void target(void* a, void* b);
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800C15AC(void* self) {
    char* s = (char*)self;
    *(int*)(s + 0x3cc) = g_sndSystem->getBGMId();
    fn_800C09FC(self, *(int*)(s + 0x220));
    {
        VObj* o = *(VObj**)(s + 0x58);
        o->target(*(void**)((char*)o + 0xe4), *(void**)(s + 0x54));
    }
    fn_800B5854(*(void**)(s + 0x40), 0, 1, 1, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_800C001C {

extern "C" {
void* fn_800C0164(void*);
void fn_800B96E8(void*, int);
void fn_800C0580(void*, void*, int, void*, int);
void fn_800C0670(void*, void*);
int fn_800B5854(void*, int, int, int, int);
extern char lbl_804080C0[];
extern unsigned char lbl_805A22D0;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void* fn_800C001C(void* a0, void* a1, void* a2, int a3, void* a4, int a5)
{
    void* heap;
    unsigned char* tbl;
    int count;
    int i;
    char* obj = (char*)operator new(0x3D4, (Heaps::HeapType)0x2A);

    if (obj != 0) {
        obj = (char*)fn_800C0164(obj);
    }
    *(int*)(obj + 0x114) = a5;
    *(void**)(obj + 0x58) = a2;
    *(void**)(obj + 0x224) = a1;
    *(void**)(obj + 0x228) = a4;
    if (a4 != 0) {
        obj[0x2B4] = 1;
    }
    fn_800B96E8(obj + 0x60, 0xF0);
    fn_800C0580(obj, lbl_804080C0, 5, a0, a3);
    tbl = (unsigned char*)&lbl_805A22D0;
    heap = gfHeapManager::getMEMAllocator((Heaps::HeapType)0x2A);
    count = tbl[1] - tbl[0];
    if (count <= 0) {
        count = 1;
    }
    for (i = 0; i < count; i++) {
        void* model = ((void**)(obj + 0x40))[tbl[2] + i];
        void* r = nw4r::g3d::ScnMdl::Construct(heap, 0, tbl[3], *(void**)((char*)model + 0x10));
        ((void**)(obj + 0x54))[tbl[0] + i] = r;
    }
    fn_800C0670(obj, a1);
    fn_800B5854(*(void**)(obj + 0x40), 2, 2, 0, 0);
    return obj;
}
}

namespace jj_800C0670 {
typedef unsigned char u8;
typedef unsigned int u32;

struct Inner { char pad[0x10]; void* m10; };
struct Self { char pad0[0x40]; Inner* p40; char pad1[0x18]; void* msg; };

extern "C" {
extern float lbl_805A22D8;
extern float lbl_805A22DC;
void* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32, u32, u32);
void allocMsgBuf__5MuMsgFUlUl(void*, u32, u32);
void setMsgData__5MuMsgFPv(void*, void*);
void attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(void*, u32, void*, u32, float);
void getWScale__5MuMsgFUlf(void*, u32, float);
void setFontColor__5MuMsgFUlUcUcUcUc(void*, u32, u8, u8, u8, u8);

// TODO: clean up, give variables real names
void fn_800C0670(Self* self, void* data) {
    Inner* p;
    void* msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, 0x2a, 0x2b);
    setMsgData__5MuMsgFPv(msg, data);
    self->msg = msg;
    allocMsgBuf__5MuMsgFUlUl(msg, 0x168, 4);
    p = self->p40;
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 0, p->m10, 0, lbl_805A22D8);
    getWScale__5MuMsgFUlf(msg, 0, lbl_805A22DC);
    setFontColor__5MuMsgFUlUcUcUcUc(msg, 0, 0, 0, 0, 0xff);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 1, p->m10, 1, lbl_805A22D8);
    getWScale__5MuMsgFUlf(msg, 1, lbl_805A22DC);
    setFontColor__5MuMsgFUlUcUcUcUc(msg, 1, 0, 0, 0, 0xff);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 2, p->m10, 2, lbl_805A22D8);
    setFontColor__5MuMsgFUlUcUcUcUc(msg, 2, 0, 0, 0, 0xff);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 3, p->m10, 3, lbl_805A22D8);
    setFontColor__5MuMsgFUlUcUcUcUc(msg, 3, 0, 0, 0, 0xff);
}
}
}

namespace jj_800C11F4 {

extern "C" {
int fn_800AD954(void*);
char* fn_800AE578(void*, unsigned short);
extern char lbl_80456EC0[];
extern char lbl_80456ED0[];
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_800C11F4(char* self)
{
    if (*(tyFigListDataManager**)(self + 0x228) &&
        !(*(tyFigListDataManager**)(self + 0x228))->isLoadFinish()) {
        return *(int*)(self + 0x10c);
    }
    if (*(void**)(self + 0x2b8) &&
        !fn_800AD954(*(void**)(self + 0x2b8))) {
        return *(int*)(self + 0x10c);
    }

    char names[0xa0];
    int i;
    char* entry = self + 0x11c;
    char* figHandle = self + 0x234;
    char* sealHandle = self + 0x2c4;
    char* faceHandle = self + 0x34c;
    i = 0;
    for (; i < *(int*)(self + 0x21c); ++i) {
        switch (*(int*)entry) {
        case 0: {
            (*(tyFigListDataManager**)(self + 0x228))->getBilTexFileNameFigId(
                *(unsigned short*)(entry + 8), names);
            ((gfFileIOHandle*)figHandle)->readRequest(names, (HeapType)0x2b, 0, 0);
            break;
        }
        case 1: {
            sprintf(names + 0x20, lbl_80456EC0,
                *(char**)(fn_800AE578(*(void**)(self + 0x2b8), *(unsigned short*)(entry + 8)) + 8));
            ((gfFileIOHandle*)sealHandle)->readRequest(names + 0x20, (HeapType)0x2b, 0, 0);
            break;
        }
        case 2:
            break;
        case 3: {
            sprintf(names + 0x60, lbl_80456ED0, muMenu::exchangeGmCharacterKind2Something(
                muMenu::exchangeMuSelchkind2MuStockchkind(*(int*)(entry + 8))));
            ((gfFileIOHandle*)faceHandle)->readRequest(names + 0x60, (HeapType)0x2b, 0, 0);
            break;
        }
        }
        entry += 0x10;
        figHandle += 8;
        sealHandle += 8;
        faceHandle += 8;
    }
    return 1;
}
}

namespace jj_800C07D0 {

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__20tyFigListDataManagerFb(void*, int);
void loadRequest__20tyFigListDataManagerFv(void*);
void readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(void* self, const char* name, int heap, int a, int b);
void* fn_800AD808(void*, int);
void fn_800AD8F8(void*);
extern const char lbl_80456DD8[];
}

struct Elem {
    s32 id;
    u8 pad[0xC];
};

struct Self {
    u8 pad0[0x10C];
    s32 f10C;        // 0x10C
    u8 pad110[0xC];  // 0x110
    Elem elems[16];  // 0x11C
    s32 count;       // 0x21C
    u8 pad220[8];    // 0x220
    void* figManager; // 0x228
    gfFileIOHandle fh1; // 0x22C
    u8 pad230[0x84];    // 0x230
    u8 isLoaded;        // 0x2B4
    u8 pad2B5[3];
    void* obj2;         // 0x2B8
    gfFileIOHandle fh2; // 0x2BC
    u8 pad2C0[0x84];    // 0x2C0
    gfFileIOHandle fh3; // 0x344
};

static s32 hasId(const Self* s, s32 val) {
    for (s32 i = 0; i < s->count; i++) {
        if (s->elems[i].id == val) {
            return 1;
        }
    }
    return 0;
}

// TODO: clean up, give variables real names
extern "C" void fn_800C07D0(void* self) {
    const char* base = lbl_80456DD8;
    Self* s = (Self*)self;

    if (hasId(s, 0)) {
        if (s->isLoaded == 0) {
            void* p = __nw__FUlQ25Heaps8HeapType(0x1468, 0x2A);
            if (p != 0) {
                p = __ct__20tyFigListDataManagerFb(p, 0);
            }
            s->figManager = p;
            loadRequest__20tyFigListDataManagerFv(p);
        }
        readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(&s->fh1, base + 0x88, 0x2B, 0, 0);
    }

    if (hasId(s, 1)) {
        void* p = __nw__FUlQ25Heaps8HeapType(0x3EB4, 0x2A);
        if (p != 0) {
            p = fn_800AD808(p, 0);
        }
        s->obj2 = p;
        fn_800AD8F8(p);
        readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(&s->fh2, base + 0xA8, 0x2B, 0, 0);
    }

    if (hasId(s, 2)) {
        readRequest__14gfFileIOHandleFPCcQ25Heaps8HeapTypeii(&s->fh3, base + 0xCC, 0x2B, 0, 0);
    }

    s->f10C = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_800C1380 {

struct IOPair {
    gfFileIOHandle handle;
    void* buffer;
};

struct Entry {
    int type;
    char pad[0xC];
};

struct Self {
    char pad0[0x10C];
    int f10C;
    char pad110[0x11C - 0x110];
    Entry entries[16];
    int count21C;
    int index220;
    char pad224[0x22C - 0x224];
    IOPair single22C;
    IOPair arr234[17];
    IOPair single2BC;
    IOPair arr2C4[16];
    IOPair single344;
    IOPair arr34C[4];
};

// TODO: clean up, give variables real names
extern "C" int fn_800C1380(Self* self) {
    IOPair* p;
    int r0;
    int idx = self->index220;
    switch (self->entries[idx].type) {
    case 0:
        p = &self->arr234[idx];
        if (self->single22C.buffer == 0) {
            if (!self->single22C.handle.isReady()) { r0 = 0; goto done; }
            self->single22C.buffer = self->single22C.handle.getBuffer();
            self->single22C.handle.release();
        }
        if (p->buffer == 0) {
            if (!p->handle.isReady()) { r0 = 0; goto done; }
            p->buffer = p->handle.getBuffer();
            nw4r::g3d::ResFile::Init(&p->buffer);
            p->handle.release();
        }
        r0 = 1;
        break;
    case 1:
        p = &self->arr2C4[idx];
        if (self->single2BC.buffer == 0) {
            if (!self->single2BC.handle.isReady()) { r0 = 0; goto done; }
            self->single2BC.buffer = self->single2BC.handle.getBuffer();
            self->single2BC.handle.release();
        }
        if (p->buffer == 0) {
            if (!p->handle.isReady()) { r0 = 0; goto done; }
            p->buffer = p->handle.getBuffer();
            nw4r::g3d::ResFile::Init(&p->buffer);
            p->handle.release();
        }
        r0 = 1;
        break;
    case 2:
        if (self->single344.buffer == 0) {
            if (!self->single344.handle.isReady()) { r0 = 0; goto done; }
            self->single344.buffer = self->single344.handle.getBuffer();
            self->single344.handle.release();
        }
        r0 = 1;
        break;
    case 3:
        p = &self->arr34C[idx];
        if (p->buffer == 0) {
            if (!p->handle.isReady()) { r0 = 0; goto done; }
            p->buffer = p->handle.getBuffer();
            nw4r::g3d::ResFile::Init(&p->buffer);
            p->handle.release();
        }
        r0 = 1;
        break;
    default:
        r0 = 1;
        break;
    }
done:
    if (r0 != 0) {
        return 2;
    }
    return self->f10C;
}
}

namespace jj_800C0330 {
class MenuObject { char pad[0x5c]; public: virtual void method(int); };
extern "C" {
extern char lbl_80456F5C[];
void fn_800AD87C(void*, int);
void fn_800B8A0C(void*, int);
void fn_800C02D8(void*, int);
void __destroy_arr(void*, void (*)(void*, int), int, int);
void __dt__14gfFileIOHandleFv(void*, int);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void* fn_800C0330(unsigned char* self, int flag) {
 if (self) {
  *(void**)(self + 0x3c) = lbl_80456F5C;
  if (!self[0x2b4]) {
   tyFigListDataManager* p = *(tyFigListDataManager**)(self + 0x228);
   if (p) delete p;
  }
  ((gfFileIOHandle*)(self + 0x22c))->cancelRequest();
  if (*(void**)(self + 0x230)) gfHeapManager::free(*(void**)(self + 0x230));
  int n;
  gfFileIOHandle* handle = (gfFileIOHandle*)(self + 0x234);
  unsigned char* cursor = self;
  for (int i = 0; i < 16; ++i, handle = (gfFileIOHandle*)((char*)handle + 8), cursor += 8) {
   handle->cancelRequest();
   if (*(void**)(cursor + 0x238)) gfHeapManager::free(*(void**)(cursor + 0x238));
  }
  if (*(void**)(self + 0x2b8)) fn_800AD87C(*(void**)(self + 0x2b8), 1);
  ((gfFileIOHandle*)(self + 0x2bc))->cancelRequest();
  if (*(void**)(self + 0x2c0)) gfHeapManager::free(*(void**)(self + 0x2c0));
  cursor = self + 0x2c4;
  handle = (gfFileIOHandle*)self;
  for (int j = 0; j < 16; ++j, cursor += 8, handle = (gfFileIOHandle*)((char*)handle + 8)) {
   ((gfFileIOHandle*)cursor)->cancelRequest();
   if (*(void**)((char*)handle + 0x2c8)) gfHeapManager::free(*(void**)((char*)handle + 0x2c8));
  }
  ((gfFileIOHandle*)(self + 0x344))->cancelRequest();
  if (*(void**)(self + 0x348)) gfHeapManager::free(*(void**)(self + 0x348));
  cursor = self;
  handle = (gfFileIOHandle*)(self + 0x34c);
  for (int k = 0; k < 16; ++k, handle = (gfFileIOHandle*)((char*)handle + 8), cursor += 8) {
   handle->cancelRequest();
   if (*(void**)(cursor + 0x350)) gfHeapManager::free(*(void**)(cursor + 0x350));
  }
  if (*(void**)(self + 0x5c)) fn_800B8A0C(*(void**)(self + 0x5c), 1);
  nw4r::g3d::G3dObj* obj = *(nw4r::g3d::G3dObj**)(self + 0x54);
  if (obj) obj->Destroy();
  cursor = self;
  for (n = 0; n < 5; ++n, cursor += 4) {
   MenuObject* entry = *(MenuObject**)(cursor + 0x40);
   if (entry) { if (entry) entry->method(1); }
  }
  __destroy_arr(self + 0x34c, fn_800C02D8, 8, 16);
  __dt__14gfFileIOHandleFv(self + 0x344, -1);
  __destroy_arr(self + 0x2c4, fn_800C02D8, 8, 16);
  __dt__14gfFileIOHandleFv(self + 0x2bc, -1);
  __destroy_arr(self + 0x234, fn_800C02D8, 8, 16);
  __dt__14gfFileIOHandleFv(self + 0x22c, -1);
  __dt__6gfTaskFv(self, 0);
  if (flag > 0) __dl__FPv(self);
 }
 return self;
}
}
}

namespace jj_800C1890 {

extern "C" int fn_800B61C0(void*);

class Object_vptr_0 {
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
    virtual void slot13(void* p);
};

struct IOPair {
    gfFileIOHandle handle;
    void* buffer;
};

struct Entry {
    int type;
    char pad[0xC];
};

struct Self {
    char pad0[0x40];
    void* p40;
    char pad44[0x10];
    void* p54;
    Object_vptr_0* obj58;
    char pad5C[0x10C - 0x5C];
    int f10C;
    char pad110[0x11C - 0x110];
    Entry entries[16];
    int count21C;
    int index220;
    char pad224[0x22C - 0x224];
    IOPair single22C;
    IOPair arr234[17];
    IOPair single2BC;
    IOPair arr2C4[16];
    IOPair single344;
    IOPair arr34C[4];
};

// TODO: clean up, give variables real names
extern "C" int fn_800C1890(Self* self) {
    IOPair* p;
    int r0;

    if (fn_800B61C0(self->p40) != 0) {
        self->obj58->slot13(self->p54);
        int idx = self->index220 + 1;
        self->index220 = idx;
        if (idx < self->count21C) {
            switch (self->entries[idx].type) {
            case 0:
                p = &self->arr234[idx];
                if (self->single22C.buffer == 0) {
                    if (!self->single22C.handle.isReady()) {
                        r0 = 0;
                        goto done;
                    }
                    self->single22C.buffer = self->single22C.handle.getBuffer();
                    self->single22C.handle.release();
                }
                if (p->buffer == 0) {
                    if (!p->handle.isReady()) {
                        r0 = 0;
                        goto done;
                    }
                    p->buffer = p->handle.getBuffer();
                    nw4r::g3d::ResFile::Init(&p->buffer);
                    p->handle.release();
                }
                r0 = 1;
                break;
            case 1:
                p = &self->arr2C4[idx];
                if (self->single2BC.buffer == 0) {
                    if (!self->single2BC.handle.isReady()) {
                        r0 = 0;
                        goto done;
                    }
                    self->single2BC.buffer = self->single2BC.handle.getBuffer();
                    self->single2BC.handle.release();
                }
                if (p->buffer == 0) {
                    if (!p->handle.isReady()) {
                        r0 = 0;
                        goto done;
                    }
                    p->buffer = p->handle.getBuffer();
                    nw4r::g3d::ResFile::Init(&p->buffer);
                    p->handle.release();
                }
                r0 = 1;
                break;
            case 2:
                if (self->single344.buffer == 0) {
                    if (!self->single344.handle.isReady()) {
                        r0 = 0;
                        goto done;
                    }
                    self->single344.buffer = self->single344.handle.getBuffer();
                    self->single344.handle.release();
                }
                r0 = 1;
                break;
            case 3:
                p = &self->arr34C[idx];
                if (p->buffer == 0) {
                    if (!p->handle.isReady()) {
                        r0 = 0;
                        goto done;
                    }
                    p->buffer = p->handle.getBuffer();
                    nw4r::g3d::ResFile::Init(&p->buffer);
                    p->handle.release();
                }
                r0 = 1;
                break;
            default:
                r0 = 1;
                break;
            }
done:
            int ret = 1;
            if (r0 != 0) {
                ret = 2;
            }
            return ret;
        } else {
            return 5;
        }
    } else {
        return self->f10C;
    }
}
}

