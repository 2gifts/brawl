#include <gf/gf_file_io_handle.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <gm/gm_global.h>
#include <types.h>
#include <if/if_stgedit_mngr.h>
#include <if/if_stgedit.h>
#include <gf/gf_3d_scene.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0350[0x8] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A0358[0x8] __attribute__((aligned(4))) = { 0 };

void IfStgeditMngr::setVisible(bool isVisible) {
    unk10->setVisible(isVisible);
}

void IfStgeditMngr::setVisibleZoomHelp(bool isVisible) {
    unk10->setVisibleZoomHelp(isVisible);
}

void IfStgeditMngr::destroyModel() {
    if (unk10 != NULL) {
        delete unk10;
        unk10 = NULL;
    }
    if (unk4 != NULL) {
        unk4->Destroy();
        unk4 = NULL;
    }
    g_gfSceneRoot->removeInfoCamResAnm();
}

void IfStgeditMngr::startMelee() {
    unk10->startMelee(unk4);
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, reinterpret_cast<nw4r::g3d::ScnMdl*>(unk4));
}

// TODO: clean up, give variables real names
extern "C" void main__9IfStgeditFv(void*);

extern "C" void fn_8010F940(void* self) {
    main__9IfStgeditFv(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x10));
}

extern "C" void setHelpControllerNo__9IfStgeditFUl(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8010F950(void* self) {
    setHelpControllerNo__9IfStgeditFUl(*reinterpret_cast<void**>(reinterpret_cast<u8*>(self) + 0x10));
}

// TODO: clean up, give variables real names
namespace jj_8010F310 {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void __construct_array(void*, void (*)(void*), void (*)(void*), unsigned long, int);
void fn_800FC3D8(void*);
void fn_800FC3E4(void*);
extern void* lbl_805A0350;
}

struct Flags8 {
    unsigned char b0 : 1;
    unsigned char rest : 7;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8010F310() {
    unsigned char* self = (unsigned char*)__nw__FUlQ25Heaps8HeapType(0x1C, 0x28);
    if (self != 0) {
        ((Flags8*)self)->b0 = 0;
        *(int*)(self + 0x8) = 0;
        *(int*)(self + 0xC) = 0;
        *(int*)(self + 0x10) = 0;
        __construct_array(self + 0x14, (void (*)(void*))fn_800FC3D8, (void (*)(void*))fn_800FC3E4, 4, 2);
        *(int*)(self + 0x8) = 0;
        *(int*)(self + 0xC) = 0;
        lbl_805A0350 = self;
    }
}
}

namespace jj_8010F754 {
typedef unsigned char u8;

struct Flags {
    unsigned char b0 : 1;
    unsigned char b1 : 1;
    unsigned char b2 : 1;
    unsigned char b3 : 1;
    unsigned char b4 : 1;
    unsigned char b5 : 1;
    unsigned char b6 : 1;
    unsigned char b7 : 1;
};

extern "C" {
void cancelRequest__14gfFileIOHandleFv(void*);
void free__13gfHeapManagerFPv(void*);

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_8010F754(void* self)
{
    u8* p = reinterpret_cast<u8*>(self);
    if (*reinterpret_cast<void**>(p + 0x8)) {
        free__13gfHeapManagerFPv(*reinterpret_cast<void**>(p + 0x8));
        *reinterpret_cast<void**>(p + 0x8) = 0;
    }
    if (*reinterpret_cast<void**>(p + 0xC)) {
        free__13gfHeapManagerFPv(*reinterpret_cast<void**>(p + 0xC));
        *reinterpret_cast<void**>(p + 0xC) = 0;
    }
    u8* cursor = p + 0x14;
    for (int i = 0; i < 2; ++i, cursor += 4) {
        cancelRequest__14gfFileIOHandleFv(cursor);
    }
    reinterpret_cast<Flags*>(p)->b0 = 0;
}
}
}

namespace jj_8010F7EC {
extern "C" {
void* getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(int);
void* fn_801AB6CC(void*, void*, int);
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__9IfStgeditFv(void*);
void createModel__9IfStgeditFPQ34nw4r3g3d7ResFile(void*, void*);
void* GetResAnmScn__Q34nw4r3g3d7ResFileFi(void*, int);
void fn_801971D0(void*, void*);
void fn_8000D6DC(void*, void*);
extern void* g_gfSceneRoot;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_8010F7EC(void* self) {
    char* p = (char*)self;
    void* spC = 0;
    void* sp8;
    void* allocator = getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(0x28);
    void* scene = fn_801AB6CC(allocator, 0, 1);
    *(void**)(p + 0x4) = scene;
    void* stgedit = __nw__FUlQ25Heaps8HeapType(0x14, 0x28);
    if (stgedit != 0) {
        stgedit = __ct__9IfStgeditFv(stgedit);
    }
    *(void**)(p + 0x10) = stgedit;
    createModel__9IfStgeditFPQ34nw4r3g3d7ResFile(stgedit, p + 0x8);
    spC = GetResAnmScn__Q34nw4r3g3d7ResFileFi(p + 0xc, 0);
    sp8 = spC;
    fn_801971D0(&spC, &sp8);
    fn_8000D6DC(g_gfSceneRoot, &spC);
}
}
}

// TODO: clean up, give variables real names
namespace jj_8010F39C {
struct Base10 {
    char pad[0x10];
};

class Obj : public Base10 {
public:
    virtual ~Obj();
};

struct Flags8 {
    unsigned char b0 : 1;
    unsigned char rest : 7;
};

extern "C" {
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void fn_8000D7B8(void*);
void cancelRequest__14gfFileIOHandleFv(void*);
void free__13gfHeapManagerFPv(void*);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dl__FPv(void*);
void fn_800FC3E4(void*, int);
extern void* g_gfSceneRoot;
}

struct Handle {
    int x;
};

struct Self {
    Flags8 flags;
    char pad1[3];
    void* g3d;
    void* buf8;
    void* bufC;
    Obj* obj;
    Handle handles[2];
    ~Self() {
        if (obj != 0) {
            delete obj;
            obj = 0;
        }
        if (g3d != 0) {
            Destroy__Q34nw4r3g3d6G3dObjFv(g3d);
            g3d = 0;
        }
        fn_8000D7B8(g_gfSceneRoot);
        if (buf8 != 0) {
            free__13gfHeapManagerFPv(buf8);
            buf8 = 0;
        }
        if (bufC != 0) {
            free__13gfHeapManagerFPv(bufC);
            bufC = 0;
        }
        for (int i = 0; i < 2; i++) {
            cancelRequest__14gfFileIOHandleFv(&handles[i]);
        }
        flags.b0 = 0;
        __destroy_arr(handles, fn_800FC3E4, 4, 2);
    }
};

extern "C" {
extern Self* lbl_805A0350;
}

// TODO: clean up, give variables real names
extern "C" void fn_8010F39C() {
    delete lbl_805A0350;
    lbl_805A0350 = 0;
}
}

namespace jj_8010F624 {

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" int fn_8010F624(void* obj) {
    char* self = (char*)obj;
    void* res2;
    void* res;
    void* buffer;
    if ((((u32)*(u8*)self >> 7) & 1) == 1) {
        return 1;
    }
    gfFileIOHandle* h = (gfFileIOHandle*)(self + 0x14);
    int i = 0;
    do {
        if (!h->isReady()) {
            return 0;
        }
        i++;
        h = (gfFileIOHandle*)((char*)h + 4);
    } while (i < 2);
    res = 0;
    if (((gfFileIOHandle*)(self + 0x14))->getReturnStatus() == 0x15)
        buffer = 0;
    else {
        buffer = ((gfFileIOHandle*)(self + 0x14))->getBuffer();
        ((gfFileIOHandle*)(self + 0x14))->release();
    }
    if (buffer) {
        res = buffer;
        nw4r::g3d::ResFile::Init(&res);
    }
    *(void**)(self + 8) = res;
    res2 = 0;
    if (((gfFileIOHandle*)(self + 0x18))->getReturnStatus() == 0x15)
        buffer = 0;
    else {
        buffer = ((gfFileIOHandle*)(self + 0x18))->getBuffer();
        ((gfFileIOHandle*)(self + 0x18))->release();
    }
    if (buffer) {
        res2 = buffer;
        nw4r::g3d::ResFile::Init(&res2);
    }
    *(void**)(self + 0xc) = res2;
    *(u8*)self |= 0x80;
    return 1;
}
}

namespace jj_8010F4AC {

extern char lbl_8045D840[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8010F4AC(void* obj, int async) {
        void* res2;
    char* self = (char*)obj;
    const char* data = "/info/info_stgedit.brres";
    if (async)
        ((gfFileIOHandle*)(self + 0x14))->readRequest(data, (HeapType)9, 0, 0);
    else
        ((gfFileIOHandle*)(self + 0x14))->read(data, (HeapType)9, 0);
    u8 flag = *(u8*)((char*)g_GameGlobal->getGlobalRecordMenuDatap() + 0x28);
    const char* path = ((flag >> 7) & 1) ? "/info/defaultcamera/InfoWide.brres" : "/info/defaultcamera/Info.brres";
    if (async)
        ((gfFileIOHandle*)(self + 0x18))->readRequest(path, (HeapType)9, 0, 0);
    else
        ((gfFileIOHandle*)(self + 0x18))->read(path, (HeapType)9, 0);
    if (!async) {
        void* res = 0;
        void* buffer;
        if (((gfFileIOHandle*)(self + 0x14))->getReturnStatus() == 0x15)
            buffer = 0;
        else {
            buffer = ((gfFileIOHandle*)(self + 0x14))->getBuffer();
            ((gfFileIOHandle*)(self + 0x14))->release();
        }
        if (buffer) {
            res = buffer;
            nw4r::g3d::ResFile::Init(&res);
        }
        *(void**)(self + 8) = res;
        res2 = 0;
        if (((gfFileIOHandle*)(self + 0x18))->getReturnStatus() == 0x15)
            buffer = 0;
        else {
            buffer = ((gfFileIOHandle*)(self + 0x18))->getBuffer();
            ((gfFileIOHandle*)(self + 0x18))->release();
        }
        if (buffer) {
            res2 = buffer;
            nw4r::g3d::ResFile::Init(&res2);
        }
        *(void**)(self + 0xc) = res2;
        *(u8*)self |= 0x80;
    }
}
}

