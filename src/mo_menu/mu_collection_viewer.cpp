#include <types.h>

#include <mu/mu_collection_viewer.h>
#include <mu/mu_notice_wnd.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <gf/gf_homemenu.h>
#include <gf/gf_shutdown_manager.h>
#include <snd/snd_system.h>

// The base of the menus that browse through the collection (module sora_menu_collect_viewer). Only a part of its functions
// is decompiled.
/// Function names come from the sora_menu_collect_viewer map.

// 0x10 bytes of the game data: two words that tell how the files of each medium are counted
extern int lbl_80423690[4];

u32 muCollectionViewer::getCurFileID() {
    Work* work = m_work;
    int numFiles;
    if (work->m_extraPanel) {
        numFiles = work->m_numFiles - 1;
    } else {
        numFiles = work->m_numFiles;
    }
    u32 numPic = getNumPic(work->m_curMedia);
    u32 id = 0x80000000;
    if ((u32)numFiles < numPic) {
        id = numFiles;
    }
    return id;
}

muCollectionFileName muCollectionViewer::getFileHandle(int index) {
    return muCollectionFileName(m_work->m_media[m_work->m_curMedia].m_names, index);
}

int muCollectionViewer::getNumPic(int media) {
    return m_work->m_media[media].m_numPic;
}

int muCollectionViewer::getNumPanel(int media) {
    Work* work = m_work;
    u8 extraPanel = work->m_extraPanel;
    return extraPanel + getNumPic(media);
}

void muCollectionViewer::getNewLabel() { }

int muCollectionViewer::dispThumbnail(MuObject* object, void* summary) {
    return 0;
}

void muCollectionViewer::dispPreviewCore(void* info) { }

void muCollectionViewer::erasePreviewCore() { }

int muCollectionViewer::unk_2FA0() {
    Work* work = m_work;
    int* table = lbl_80423690;
    if (work->m_curMedia == 0) {
        return table[3];
    }
    return table[2];
}

void muCollectionViewer::unk_3134() {
    addObjCore(m_work->m_objs[23]->m_sceneModel);
    addObjCore(m_work->m_objs[32]);
}

void muCollectionViewer::unk_3194(int cur, int count) {
    int left = cur > 0;
    int right;
    if (count != 0) {
        right = (u32)(count - 1) > (u32)cur;
    } else {
        right = 0;
    }
    unk_31D8(left, right);
}

void muCollectionViewer::unk_31D8(int left, int right) {
    int flags = 0;
    if (left) {
        flags |= 1;
    }
    if (right) {
        flags |= 2;
    }
    m_work->m_objs[23]->setFrameTex((float)flags);
}

void muCollectionViewer::unk_38F8(int flag) {
    float frame;
    if (flag == 0) {
        frame = 1.0f;
    } else {
        frame = 0.0f;
    }
    if (m_work->m_flag1C74 == 0) {
        frame += 2.0f;
    }
    m_work->m_objs[1]->setFrameVisible(frame);
}

void muCollectionViewer::onLoadFailed() { }

void muCollectionViewer::controlBrowsing() { }

void muCollectionViewer::controlBrowsingLoading() { }

void muCollectionViewer::controlPopUpStart() { }

void muCollectionViewer::controlPopUp() { }

void muCollectionViewer::controlPopUpCheckStart() { }

void muCollectionViewer::controlPopUpChecking() { }

void muCollectionViewer::controlNewDataStart() { }

void muCollectionViewer::controlNewDataChecking() { }

void muCollectionViewer::controlNewData() { }

void muCollectionViewer::controlNewDataExit() { }

void muCollectionViewer::controlScanStart(int drive, int unk, int unk2) {
    Work* work = m_work;
    muCollection* collection = (muCollection*)work->m_collection;
    if (collection->m_busy == 0) {
        if (unk2 == 0 && collection->m_result != 0) {
            work->m_exitRequest = 0;
        } else {
            collection->scan(drive, unk, unk2);
            m_work->m_exitRequest = 6;
        }
    }
}

void muCollectionViewer::controlScanAllStart(int unk) {
    muCollection* collection = (muCollection*)m_work->m_collection;
    if (collection->m_busy == 0) {
        collection->scanAll(1, unk);
        m_work->m_exitRequest = 6;
    }
}

void muCollectionViewer::controlScan() {
    Work* work = m_work;
    muCollection* collection = (muCollection*)work->m_collection;
    if (collection->m_busy == 0) {
        work->m_exitRequest = 0;
    }
}

void muCollectionViewer::controlWaitForDialog(int step) {
    Work* work = m_work;
    muNoticeWndTask* wnd = work->m_wnd;
    if (wnd == NULL || wnd->m_step == 3) {
        if (wnd != NULL) {
            wnd->exit();
        }
        work->m_wnd = NULL;
        m_work->m_state = step;
    }
}

void muCollectionViewer::closeWaitWnd() {
    m_work->m_objs[22]->m_scnMdl->DetachFromParent();
}

void muCollectionViewer::endErase() {
    closeEraseReportWnd();
    m_work->m_state = 7;
    addObjCore(m_work->m_objs[23]->m_sceneModel);
}

void muCollectionViewer::endCopy() {
    closeEraseReportWnd();
    m_work->m_state = 10;
}

void muCollectionViewer::openMediaMenu(int attach, int second) {
    int index = 2;
    if (second == 0) {
        index = 1;
    }
    if (attach != 0) {
        addObjCore(*(MuObject**)((char*)m_work + index * 4 + 0x184));
    } else {
        removeObjCore(*(MuObject**)((char*)m_work + index * 4 + 0x184));
    }
}

void muCollectionViewer::unk_64C8(int index) {
    m_work->m_msg->printIndex(0xF, index, m_work->m_msgFile[1].m_handle.getBuffer());
}

void muCollectionViewer::closeEraseReportWnd() {
    m_work->m_msg->printf(0xF, "");
    removeObjCore(m_work->m_objs[35]);
    g_sndSystem->playSE(static_cast<SndID>(6), -1, 0, 0, -1);
}

void muCollectionViewer::openEraseReportWnd() {
    m_work->m_objs[2]->setFrameNode(2.0f);
    addObjCore(m_work->m_objs[35]);
    g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
}

void muCollectionViewer::dispMediaMenuSelect(int selected) {
    float frame = (float)selected;
    m_work->m_objs[3]->setFrameNode(frame);
    m_work->m_objs[4]->setFrameNode(frame);
}

void muCollectionViewer::closeEraseConfirmWnd() {
    m_work->m_msg->printf(0xC, "");
    m_work->m_msg->printf(0xD, "");
    m_work->m_msg->printf(0xE, "");
    removeObjCore(m_work->m_objs[35]);
}

void muCollectionViewer::openWaitWnd() {
    MuObject* obj = m_work->m_objs[22];
    ((nw4r::g3d::ScnMdlExpand*)m_work->m_objs[35])->PushBack(obj->m_sceneModel, (u32)0);
    obj->setFrameNode(0.0f);
    obj->setFrameTexSrt(0.0f);
    Vec3f trans(0.0f, 7.0f, 0.0f);
    obj->setTrans(&trans);
}

void muCollectionViewer::beginCopy(int flag) {
    openWaitWnd();
    openEraseReportWnd();
    int index = 2;
    if (flag != 0) {
        index = 0;
    }
    unk_64C8(index);
    int state = 0xE;
    if (flag != 0) {
        state = 0xB;
    }
    m_work->m_state = state;
}

void muCollectionViewer::controlErasing() {
    muCollection* collection = (muCollection*)m_work->m_collection;
    if (collection->m_busy == 0) {
        g_gfHomeMenu->setBan(0, 5);
        g_gfShutdownManager.unkD = true;
        if (((muCollection*)m_work->m_collection)->m_result != 0) {
            endErase();
        } else {
            g_sndSystem->playSE(static_cast<SndID>(0x28), -1, 0, 0, -1);
            closeWaitWnd();
            unk_64C8(6);
            m_work->m_waitCount = 0;
            m_work->m_state = 0x14;
        }
    }
}

void muCollectionViewer::openEraseConfirmWnd(int selected) {
    addObjCore(m_work->m_objs[35]);
    closeWaitWnd();
    m_work->m_msg->printIndex(0xC, 0, NULL);
    dispEraseConfirmWndYesNo(selected);
    g_sndSystem->playSE(static_cast<SndID>(5), -1, 0, 0, -1);
}

#include <mu/mu_msg.h>
#include <gf/gf_file_io_handle.h>
#include <mu/mu_object.h>
#include <string.h>
// TODO: clean up, give variables real names
namespace jj_180 {
struct Object {
    unsigned char pad00[0x2C];
    unsigned char flags2C;
    unsigned char pad2D[0xF];
    void* table;
    unsigned char pad40[2];
    unsigned char flags42a : 2;
    unsigned char flag42 : 1;
    unsigned char flags42b : 5;
    unsigned char pad43[0x169];
    void* field1AC;
};

extern "C" void __ct__Q27utility12StandardMenuFPCciQ25Heaps8HeapType(Object*, void*, int, int);
extern "C" unsigned char lbl_6_data_270[];

extern "C" Object* __ct__18muCollectionViewerFPCc(Object* self, void* arg) {
    __ct__Q27utility12StandardMenuFPCciQ25Heaps8HeapType(self, arg, 0xF0, 0xFF);
    self->table = lbl_6_data_270;
    self->field1AC = 0;
    self->flag42 = 1;
    self->flags2C &= ~2;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_1E4 {
extern "C" {
extern unsigned char lbl_6_data_270[];
void __dt__Q27utility12StandardTaskFv(void*, int);
void __dl__FPv(void*);
}

struct ChildBase {
    unsigned char pad[0x1D08];
};

struct Child : ChildBase {
    virtual ~Child();
};

struct Object {
    unsigned char pad0[0x3C];
    void* table;
    unsigned char pad1[0x16C];
    Child* child;
};

extern "C" void* __dt__18muCollectionViewerFv(Object* self, int flag) {
    if (self) {
        self->table = lbl_6_data_270;
        delete self->child;

        if (self) {
            __dt__Q27utility12StandardTaskFv(self, 0);
        }

        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_804 {
typedef unsigned char u8;

struct gfFileIOHandle {
    void release();
    ~gfFileIOHandle();
};

struct CollectViewerObject {
    u8 pad0[8];
    void* node;
    void* vtable;
    gfFileIOHandle handle;
};

extern "C" void clear__Q27utility8AutoFileFv(void*);
extern "C" void removeExist__6utListFP10utListNode(void*, void*);
extern "C" void __dl__FPv(void*);
extern "C" u8 lbl_6_data_510[];
extern "C" u8 lbl_8049E550[];

extern "C" CollectViewerObject* fn_6_804(CollectViewerObject* self, int deleting) {
    if (self != 0) {
        self->vtable = lbl_6_data_510;
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
namespace jj_8C4 {
extern "C" void fn_6_8C4(u32 *arg0) {
    arg0[2] = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_8D0 {
extern "C" void reset__Q210muRecorder4DataFPv(void*, void*);
extern "C" void __dl__FPv(void*);

extern "C" void* checkLoadFinish__18muCollectionViewerFv(void* ptr, int shouldFree) {
    if (ptr) {
        if (static_cast<char*>(ptr) + 8)
            reset__Q210muRecorder4DataFPv(static_cast<char*>(ptr) + 8, 0);

        if (shouldFree > 0)
            __dl__FPv(ptr);
    }

    return ptr;
}
}

// TODO: clean up, give variables real names
namespace jj_1134 {
#pragma opt_propagation off

extern "C" void clear__Q27utility8AutoFileFv(void*);
extern "C" void request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(void*, const void*, int, int);
extern "C" void loadMenuSound__6muMenuFv(void);
extern char lbl_6_data_0[];

struct Viewer {
    char pad[0x1AC];
    char* data;
};

extern "C" void initProc__18muCollectionViewerFv(Viewer* self)
{
    int zero;
    unsigned long base = (unsigned long)lbl_6_data_0;
    char* data = self->data;
    const void* p = (void*)(base + 4);

    clear__Q27utility8AutoFileFv(data + 0xE4);
    zero = 0;
    *(int*)(data + 0xD0) = zero;
    data[0xFC] = zero;
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(data + 0xE4, p, 0x2B, 0);

    data = self->data;
    const void* p2 = (void*)(base + 0x2C);
    clear__Q27utility8AutoFileFv(data + 0x48);
    *(int*)(data + 0x34) = zero;
    data[0x60] = zero;
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(data + 0x48, p2, 0x2B, 0);

    data = self->data;
    const void* p3 = (void*)(base + 0x50);
    clear__Q27utility8AutoFileFv(data + 0x7C);
    *(int*)(data + 0x68) = zero;
    data[0x94] = zero;
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(data + 0x7C, p3, 0x2B, 0);

    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(self->data + 0x1CB0, (void*)(base + 0x70), 0x2B, 0);
    request__Q27utility8AutoFileFPCcQ25Heaps8HeapTypei(self->data + 0x1CC8, (void*)(base + 0x8C), 0x2B, 0);
    loadMenuSound__6muMenuFv();
}
}

// TODO: clean up, give variables real names
namespace jj_1228 {
struct Viewer {
    unsigned char padding[0x3C];

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
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
    virtual void slot50();
    virtual void slot51();
    virtual void slot52();
    virtual void slot53();
    virtual void slot54();
    virtual int query();
};

extern "C" bool fn_6_1228(Viewer* object) {
    return object->query() != 0;
}
}

// TODO: clean up, give variables real names
namespace jj_1A38 {
typedef unsigned char u8;

extern "C" {
void* copyFileName__Q212muCollection8FileInfoCFQ25Heaps8HeapType(void*, int);
void reset__Q210muRecorder4DataFPv(void*, void*);
void scanAll__12muCollectionFii(void*, int, int);
void updateButtonCondition__9nteGlobalFv(void);
unsigned long isEnableButton__9nteGlobalFUs(unsigned long);
}

struct Unk104 {
    char pad[0x10];
    void* f10;
};

struct ViewerBase {
    char pad[0x3C];
};

struct Viewer : ViewerBase {
#define SLOT(n) virtual void unused##n();
    SLOT(0) SLOT(1) SLOT(2) SLOT(3) SLOT(4) SLOT(5) SLOT(6)
    SLOT(7) SLOT(8) SLOT(9) SLOT(10) SLOT(11) SLOT(12) SLOT(13)
    SLOT(14) SLOT(15) SLOT(16) SLOT(17) SLOT(18) SLOT(19) SLOT(20)
    SLOT(21) SLOT(22) SLOT(23) SLOT(24) SLOT(25) SLOT(26) SLOT(27)
    SLOT(28) SLOT(29) SLOT(30) SLOT(31)
#undef SLOT
    virtual void f88();
    virtual void f8C(int);
    virtual void f90();
    virtual void f94();
    virtual void f98();
    virtual void f9C();
    virtual void fA0();
    virtual void fA4();
    virtual void fA8();
    virtual void fAC();
    virtual void fB0(void*);
    virtual void fB4();
    virtual void fB8();
    virtual void fBC();
    virtual void fC0(int, int);

    char pad40[0x16C];
    struct Data* data;
};

struct Data {
    char pad0[0x104];
    Unk104* field104;
    char pad1[0x1A0 - 0x108];
    int field1A0;
    char pad2[0x1BF4 - 0x1A4];
    int field1BF4;
    char pad3[0x1C74 - 0x1BF8];
    u8 field1C74;
    char pad4[0x1CE4 - 0x1C75];
    char* database;
};

extern "C" void expandPic__18muCollectionViewerFv(Viewer* self) {
    self->f94();
    self->f88();
    self->f8C(1);

    char* d;
    char* src;
    char* base;
    int offB;
    int offD;
    int i;
    i = 0;
    offB = 0;
    offD = 0;
    do {
        d = (char*)self->data + offD;
        base = self->data->database + offB;
        *(u8*)(d + 0x1A4) = *(u8*)(base + 0x358);
        src = base + 0x358;
        *(int*)(d + 0x1A8) = *(int*)(src + 8);
        reset__Q210muRecorder4DataFPv(d + 0x1AC, copyFileName__Q212muCollection8FileInfoCFQ25Heaps8HeapType(src, 0x2B));
        *(int*)(d + 0x1B0) = *(int*)(src + 4);
        i++;
        offB += 0x14;
        offD += 0x10;
    } while (i < 2);

    self->fB0(self->data->field104->f10);
    self->data->field1A0 = 0;
    self->fC0(self->data->field1A0, 0);
    scanAll__12muCollectionFii(self->data->database, 1, 0);
    self->data->field1BF4 = 2;

    int v;
    switch (*(int*)(self->data->database + 0x118)) {
    case 1:
        v = 1;
        break;
    case 2:
        v = 2;
        break;
    case 3:
        v = 4;
        break;
    }
    updateButtonCondition__9nteGlobalFv();
    self->data->field1C74 = (u8)isEnableButton__9nteGlobalFUs(v);
}
}

// TODO: clean up, give variables real names
namespace jj_1BCC {
struct MuObject {
    char pad[0x14];
    void* animation;
};

struct Entry {
    char name[0x41];
    unsigned char first;
    unsigned char last;
    unsigned char argument;
};

struct ObjectList {
    char pad[0x104];
    MuObject* objects[1];
};

struct Menu {
    char pad[0x1ac];
    ObjectList* list;
};

extern "C" {
extern char lbl_6_data_130[];
extern const float lbl_6_rodata_1C4;

void* create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
    void*, const char*, int, void*, int);
void changeAnimN__8MuObjectFPCc(void*, const char*);
void setUpdateRate__16gfModelAnimationFf(void*, float);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
}

extern "C" void changePicAnim__18muCollectionViewerFv(Menu* self, Entry* entries, int count, void* resourceFile) {
    int num;
    int i = 0;
    int j;
    Entry* entry = entries;
    MuObject* object;
    char animationName[0x40];

    while (i < count) {
        num = 1;
        if (entry->first < entry->last)
            num = entry->last - entry->first;

        j = 0;
        while (j < num) {
            object = (MuObject*)create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
                resourceFile, entry->name, entry->argument, 0, 42);
            self->list->objects[entry->first + j] = object;
            strcpy(animationName, entry->name);
            strcat(animationName, lbl_6_data_130);
            changeAnimN__8MuObjectFPCc(object, animationName);
            if (object->animation)
                setUpdateRate__16gfModelAnimationFf(
                    object->animation, lbl_6_rodata_1C4);
            ++j;
        }
        ++i;
        ++entry;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_1CC0 {
typedef unsigned char u8;

extern "C" {
void* getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(int);
void* Construct__Q34nw4r3g3d6ScnMdlFPviiPv(void*, int, int, void*);
}

extern "C" void getNumPanel__14muSnapShotTaskFi(u8* self, u8* data, int count) {
    void* heap = getMEMAllocator__13gfHeapManagerFQ25Heaps8HeapType(0x2b);
    u8* cur = data;
    for (int i = 0; i < count; i++) {
        u8* p = *(u8**)(self + 0x1ac) + (cur[1] << 2);
        void* model = *(void**)(*(u8**)(p + 0x104) + 0x10);
        void* r = Construct__Q34nw4r3g3d6ScnMdlFPviiPv(heap, 0, cur[2], model);
        u8* q = *(u8**)(self + 0x1ac) + (cur[0] << 2);
        *(void**)(q + 0x184) = r;
        cur += 3;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_1ECC {
extern "C" {
void* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32, u32, u32);
void FUN_800b9448__5MuMsgFUlb(void*, u32, bool);
void FUN_800b9460__5MuMsgFUlUcUcUcf(void*, u32, u8, u8, u8, float);
}
struct ViewerModel {
    char pad[0x10];
    nw4r::g3d::ScnMdlSimple* mdl;
};
struct ViewerData {
    char pad0[0x108];
    ViewerModel* a;
    ViewerModel* b;
    char pad1[0x11c - 0x110];
    ViewerModel* c;
    char pad2[0x1c7c - 0x120];
    MuMsg* msg;
    char pad3[0x1c90 - 0x1c80];
    gfFileIOHandle handle;
};
struct ViewerObj {
    char pad[0x1ac];
    ViewerData* data;
};
extern "C" void fn_6_1ECC(ViewerObj* self, u32 flags) {
    self->data->msg = (MuMsg*)create__5MuMsgFUlQ25Heaps8HeapTypeUl(flags, 0x2a, 0x2b);
    self->data->msg->allocMsgBuf(0x100, 0x16);
    self->data->msg->setMsgData(self->data->handle.getBuffer());

    ViewerModel* a = self->data->a;
    self->data->msg->attachScnMdlSimple(0, a->mdl, 0, 0.0625f);
    self->data->msg->setFontColor(0, 0xED, 0xED, 0xED, 0xFF);
    self->data->msg->attachScnMdlSimple(1, a->mdl, 1, 0.0625f);
    self->data->msg->setFontColor(1, 0, 0, 0, 0xFF);
    self->data->msg->attachScnMdlSimple(2, a->mdl, 0xA, 0.0625f);
    self->data->msg->setFontColor(2, 0xFF, 0xFF, 0xFF, 0xFF);

    ViewerModel* b;
    for (int i = 0; i < 7; i++) {
        u32 id = i + 3;
        self->data->msg->attachScnMdlSimple(id, a->mdl, i + 2, 0.0625f);
        self->data->msg->setFontColor(id, 0xB7, 0xB7, 0xB7, 0xFF);
        FUN_800b9448__5MuMsgFUlb(self->data->msg, id, true);
        FUN_800b9460__5MuMsgFUlUcUcUcf(self->data->msg, id, 0x20, 0x20, 0x20, 3.0f);
    }

    self->data->msg->attachScnMdlSimple(0xB, a->mdl, 9, 0.0625f);
    self->data->msg->setFontColor(0xB, 0x4D, 0x4D, 0x4D, 0xFF);

    b = self->data->b;
    self->data->msg->attachScnMdlSimple(0xC, b->mdl, 0, 0.0625f);
    self->data->msg->setFontColor(0xC, 0x4F, 0x1E, 0x20, 0xFF);
    self->data->msg->attachScnMdlSimple(0xD, b->mdl, 1, 0.0625f);
    self->data->msg->attachScnMdlSimple(0xE, b->mdl, 2, 0.0625f);
    self->data->msg->attachScnMdlSimple(0xF, b->mdl, 3, 0.0625f);
    self->data->msg->setFontColor(0xF, 0x4F, 0x1E, 0x20, 0xFF);

    self->data->msg->attachScnMdlSimple(0x10, self->data->c->mdl, 0, 0.0625f);
    self->data->msg->setFontColor(0x10, 0, 0, 0, 0xFF);
}
}

// TODO: clean up, give variables real names
namespace jj_2EC0 {
extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* getBuffer__14gfFileIOHandleCFv(const void*);
extern "C" void* fn_800C35CC(void*, int, int*, int*, void*, int, int, int, int, int, int);

struct Obj {
    char pad[0x3c];
    virtual void v0(void*);
    virtual void v1(void*);
    virtual void v2(void*);
    virtual void v3(void*);
    virtual void v4(void*);
    virtual void v5(void*);
    virtual void v6(void*);
    virtual void v7(void*);
    virtual void v8(void*);
    virtual void v9(void*);
    virtual void v10(void*);
    virtual void v11(void*);
    virtual void v12(void*);
    virtual void v13(void*);
    virtual void v14(void*);
    virtual void v15(void*);
    virtual void v16(void*);
    virtual void v17(void*);
    virtual void v18(void*);
    virtual void v19(void*);
    virtual void v20(void*);
    virtual void v21(void*);
    virtual void v22(void*);
    virtual void v23(void*);
    virtual void v24(void*);
    virtual void v25(void*);
    virtual void v26(void*);
    virtual void v27(void*);
    virtual void v28(void*);
    virtual void v29(void*);
    virtual void v30(void*);
    virtual void v31(void*);
    virtual void v32(void*);
    virtual void v33(void*);
    virtual void v34(void*);
    virtual void v35(void*);
    virtual void v36(void*);
    virtual void v37(void*);
    virtual void v38(void*);
    virtual void v39(void*);
    virtual void v40(void*);
    virtual void v41(void*);
    virtual void v42(void*);
    virtual void v43(void*);
    virtual void v44(void*);
    virtual void v45(void*);
    virtual void v46(void*);
    virtual void v47(void*);
    virtual void v48(void*);
    virtual void v49(void*);
    virtual void v50(void*);
    virtual void v51(void*);
    virtual void v52(void*);
    virtual void v53(void*);
    virtual void v54(void*);
    virtual int v55(void*);
    virtual void v56(void*);
    virtual void v57(void*);
};

extern "C" void fn_6_2EC0(void* self, int arg1)
{
    int vd0;
    int v9c;
    int r8;
    void* obj = __nw__FUlQ25Heaps8HeapType(0x394, 0x2a);
    if (obj != 0) {
        void* buf = getBuffer__14gfFileIOHandleCFv((char*)*(void**)((char*)self + 0x1ac) + 0x1cc0);
        vd0 = *(int*)((char*)*(void**)((char*)self + 0x1ac) + 0xd0);
        void* p = *(void**)((char*)self + 0x1ac);
        v9c = *(int*)((char*)p + 0x9c);
        r8 = ((Obj*)self)->v55(p);
        obj = fn_800C35CC(obj, arg1, &v9c, &vd0, buf, r8, 0x1a, 0x2a, 0x2b, 0, 0xf0);
    }
    *(void**)((char*)*(void**)((char*)self + 0x1ac) + 0x1ce4) = obj;
}
}

// TODO: clean up, give variables real names
namespace jj_2FC8 {
struct Copy { unsigned char data[0x41]; };
class Obj {
public:
 unsigned char pad[0x3C];
 virtual void vf0() = 0;
 virtual void vf1() = 0;
 virtual void vf2() = 0;
 virtual void vf3() = 0;
 virtual void vf4() = 0;
 virtual void vf5() = 0;
 virtual void vf6() = 0;
 virtual void vf7() = 0;
 virtual void vf8() = 0;
 virtual void vf9() = 0;
 virtual void vf10() = 0;
 virtual void vf11() = 0;
 virtual void vf12() = 0;
 virtual void vf13() = 0;
 virtual void vf14() = 0;
 virtual void vf15() = 0;
 virtual void vf16() = 0;
 virtual void vf17() = 0;
 virtual void vf18() = 0;
 virtual void vf19() = 0;
 virtual void vf20() = 0;
 virtual void vf21() = 0;
 virtual void vf22() = 0;
 virtual void vf23() = 0;
 virtual void vf24() = 0;
 virtual void vf25() = 0;
 virtual void vf26() = 0;
 virtual void vf27() = 0;
 virtual void vf28() = 0;
 virtual void vf29() = 0;
 virtual void vf30() = 0;
 virtual void vf31() = 0;
 virtual void vf32() = 0;
 virtual void vf33() = 0;
 virtual void vf34() = 0;
 virtual void vf35() = 0;
 virtual void vf36() = 0;
 virtual void vf37() = 0;
 virtual void vf38() = 0;
 virtual s32 getIdx() = 0;
 virtual Copy copyTo(s32 idx) = 0;
};
extern "C" {
extern unsigned char lbl_6_data_178[0x40];
void* __ct__Q212muCollection10FileHandleFPCc(void*, void*);
}
extern "C" void fn_6_2FC8(void* dst, Obj* obj) {
 unsigned char buf4C[0x44];
 *(Copy*)dst = obj->getIdx() < 0 ? *(Copy*)__ct__Q212muCollection10FileHandleFPCc(buf4C, lbl_6_data_178) : obj->copyTo(obj->getIdx());
}
}

// TODO: clean up, give variables real names
namespace jj_3784 {
typedef unsigned char u8;

extern "C" {
void* copyFileName__Q212muCollection8FileInfoCFQ25Heaps8HeapType(void*, int);
void reset__Q210muRecorder4DataFPv(void*, void*);
void printf__5MuMsgFUlPCce(void*, unsigned long, const char*, ...);
extern char lbl_6_data_0[];
}

struct Node {
    virtual void v8();
    virtual void vc();
    virtual void v10();
    virtual void v14();
    virtual void v18();
    virtual void v1c();
    virtual void v20();
    virtual void v24();
    virtual void v28();
    virtual void v2c();
    virtual void v30();
    virtual void v34();
    virtual void remove(int);
    char pad[0xE0];
    int count;
};

struct Record {
    u8 kind;
    char pad[3];
    int value;
    int extra;
    char tail[8];
};

struct Entry {
    u8 kind;
    char pad[3];
    int extra;
    void* handle;
    int value;
};

struct Database {
    char pad[0x358];
    Record records[1];
};

struct Data {
    char pad0[0x184];
    Node* node;
    char pad188[0x1A4 - 0x188];
    Entry entries[1];
    char pad1B4[0x1B40 - 0x1B4];
    int value;
    char pad1B44[0x1C7C - 0x1B44];
    void* msg;
    char pad1C80[0x1CE4 - 0x1C80];
    Database* database;
};

struct ViewerBase {
    char pad[0x3C];
};

struct Viewer : ViewerBase {
    virtual void at8();
    virtual void atC();
    virtual void at10();
    virtual void at14();
    virtual void at18();
    virtual void at1C();
    virtual void at20();
    virtual void at24();
    virtual void at28();
    virtual void at2C();
    virtual void at30();
    virtual void at34();
    virtual void at38();
    virtual void at3C();
    virtual void at40();
    virtual void at44();
    virtual void at48();
    virtual void at4C();
    virtual void at50();
    virtual void at54();
    virtual void at58();
    virtual void at5C();
    virtual void at60();
    virtual void at64();
    virtual void at68();
    virtual void at6C();
    virtual void at70();
    virtual void at74();
    virtual void at78();
    virtual void at7C();
    virtual void at80();
    virtual void at84();
    virtual void at88();
    virtual void at8C();
    virtual void at90();
    virtual void at94();
    virtual void at98();
    virtual void at9C();
    virtual void atA0();
    virtual void atA4();
    virtual void atA8();
    virtual void atAC();
    virtual void atB0();
    virtual void atB4();
    virtual void atB8();
    virtual void atBC();
    virtual void atC0();
    virtual void atC4(int);
    virtual void atC8(int, int);
    virtual void atCC();
    virtual void atD0();
    virtual void atD4();
    virtual void atD8();
    virtual void atDC();
    virtual void atE0();
    virtual void atE4();
    virtual void atE8(int, int);

    char pad40[0x16C];
    Data* data;
};

extern "C" void fn_6_3784(Viewer* self, int index, int flag) {
    Record* src = &self->data->database->records[index];
    Entry* dest = &self->data->entries[index];

    dest->kind = src->kind;
    dest->extra = src->extra;
    reset__Q210muRecorder4DataFPv(&dest->handle, copyFileName__Q212muCollection8FileInfoCFQ25Heaps8HeapType(src, 0x2B));
    dest->value = src->value;
    self->data->value = 0;

    dest = (Entry*)self->data->node;
    while (((Node*)dest)->count != 0) {
        if (((Node*)dest)->count != 0)
            ((Node*)dest)->remove(((Node*)dest)->count - 1);
    }

    if (flag) {
        self->atE8(2, 1);
        printf__5MuMsgFUlPCce(self->data->msg, 0, lbl_6_data_0);
        printf__5MuMsgFUlPCce(self->data->msg, 2, lbl_6_data_0);
        self->atD8();
    }

    if (!flag) {
        self->atC4(index);
        self->atC8(self->data->value, index);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_3B1C {
struct Vt {
    virtual void s0();
    virtual void s1();
    virtual void s2();
    virtual void s3();
    virtual void s4();
    virtual void s5();
    virtual void s6();
    virtual void s7();
    virtual void s8();
    virtual float s9();
};

struct C { char pad[0xc]; Vt* vt; };
struct B { char pad[0x14]; C* c; };
struct A { char pad[0x108]; B* b; };
struct Root { char pad[0x1ac]; A* a; };

extern const float lbl_6_rodata_1C4;

extern "C" bool fn_6_3B1C(Root* r) {
    return r->a->b->c->vt->s9() != lbl_6_rodata_1C4;
}
}

// TODO: clean up, give variables real names
namespace jj_49E4 {
struct B { char pad[0x50]; };
struct VObj : B { virtual ~VObj(); };

struct Out {
    int a;
    int b;
    int c;
    VObj* d;
    int e;
};
struct In {
    char pad[0x1ec];
    int a;
    int b;
    int c;
    VObj* d;
    int e;
};

extern "C" void reset__Q210muRecorder4DataFPv(void*, int);

extern "C" void fn_6_49E4(Out* out, In* in) {
    out->a = 0;
    out->d = 0;
    int t = in->a;
    in->a = 0;
    reset__Q210muRecorder4DataFPv(out, t);
    out->b = in->b;
    out->c = in->c;
    VObj* n = in->d;
    in->d = 0;
    if (out->d != n) {
        delete out->d;
        out->d = n;
    }
    out->e = in->e;
}
}

// TODO: clean up, give variables real names
namespace jj_535C {
extern "C" {
void exit__6gfTaskFv(void*);
void* fn_800BF048(int, void*, void*, void*, void*, int, int, int,
                 int, int, int, int);
}

struct Slot {
    void* task;

    void set(void* value) {
        if (task)
            exit__6gfTaskFv(task);
        task = value;
    }
};

struct State {
    char pad[0x1bfc];
    int active;
    char pad2[0x1cfc - 0x1c00];
    Slot slot;
};

struct Viewer {
    char pad[0x3c];

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
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual void v42();
    virtual void v43();
    virtual void v44();
    virtual void v45();
    virtual void v46();
    virtual void v47();
    virtual void v48();
    virtual void v49();
    virtual void v50();
    virtual void v51();
    virtual void v52();
    virtual void v53();
    virtual void v54();
    virtual void* getDisplay();
};

extern "C" void createNoticeWindow__18muCollectionViewerFv(Viewer* self, void* arg1, void* arg2) {
    void* display = self->getDisplay();

    (*(State**)((char*)self + 0x1ac))->slot.set(
        fn_800BF048(
            0, arg1, arg2,
            (char*)*(State**)((char*)self + 0x1ac) + 0xd0,
            display, 0x1a, 0, 0x2a, 0x2b, 0, 0, 0xf0));

    (*(State**)((char*)self + 0x1ac))->active = 1;
}
}

// TODO: clean up, give variables real names
namespace jj_5A90 {
typedef unsigned int u32;
typedef unsigned char u8;

extern "C" {
extern void* g_sndSystem;
extern const u8 lbl_6_rodata_5B0[];
extern char lbl_6_data_0[];

void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);
int getAButtonPushedControllerID__16muMenuControllerFv();
void printf__5MuMsgFUlPCce(void*, unsigned long, const char*, ...);
}

struct ViewerBase {
    char pad0[0x3C];
};

struct Viewer : ViewerBase {
    virtual void at8();
    virtual void atC();
    virtual void at10();
    virtual void at14();
    virtual void at18();
    virtual void at1C();
    virtual void at20();
    virtual void at24();
    virtual void at28();
    virtual void at2C();
    virtual void at30();
    virtual void at34();
    virtual void at38();
    virtual void at3C();
    virtual void at40();
    virtual void at44();
    virtual void at48();
    virtual void at4C();
    virtual void at50();
    virtual void at54();
    virtual void at58();
    virtual void at5C();
    virtual void at60();
    virtual void at64();
    virtual void at68();
    virtual void at6C();
    virtual void at70();
    virtual void at74();
    virtual void at78();
    virtual void at7C();
    virtual void at80();
    virtual void at84();
    virtual void at88();
    virtual void at8C();
    virtual void at90();
    virtual void at94();
    virtual void at98();
    virtual void at9C();
    virtual void atA0();
    virtual void atA4();
    virtual void atA8();
    virtual void atAC();
    virtual void atB0();
    virtual void atB4();
    virtual void atB8();
    virtual int atBC(int);
    virtual void atC0();
    virtual void atC4();
    virtual void atC8();
    virtual void atCC();
    virtual void atD0();
    virtual void atD4();
    virtual void atD8();
    virtual void atDC();
    virtual void atE0();
    virtual void atE4();
    virtual void atE8(int, int);
    virtual void atEC();
    virtual void atF0();
    virtual void atF4();
    virtual void atF8();
    virtual void atFC();
    virtual void at100(int);
    virtual void at104();
    virtual void at108();
    virtual void at10C();
    virtual void at110();
    virtual void at114();
    virtual void at118();
    virtual void at11C();
    virtual void at120(int, int);
    virtual void at124(int, int);

    char pad40[0x168];
    u32 flags;
    char* data;
};

extern "C" void controlPicSelect__18muCollectionViewerFv(Viewer* self) {
    if (self->flags & 4) {
        self->atF4();
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 2, -1, 0, 0, -1);
    } else if (self->flags & 2) {
        if (self->atBC(*(int*)(self->data + 0x1A0))) {
            self->at100(getAButtonPushedControllerID__16muMenuControllerFv());
        }
    } else if (*(u32*)(self->data + 0x1B50) & 8) {
        self->at124(1, 5);
    } else if (*(u32*)(self->data + 0x1B50) & 4) {
        *(int*)(self->data + 0x1BFC) = *(int*)(self->data + 0x1C00);

        int next;
        switch (*(int*)(self->data + 0x1C00)) {
        case 7:
            next = 6;
            break;
        case 8:
            next = 7;
            break;
        case 30:
            next = 8;
            break;
        case 34:
            next = 9;
            break;
        }

        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
        self->atE8(next, 1);

        if (lbl_6_rodata_5B0[next]) {
            printf__5MuMsgFUlPCce(
                *(void**)(self->data + 0x1C7C), 0, lbl_6_data_0);
            printf__5MuMsgFUlPCce(
                *(void**)(self->data + 0x1C7C), 2, lbl_6_data_0);
            self->atD8();
        }
    } else {
        self->at120(1, 5);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_5C8C {
struct Input {
    char pad[0x1B54];
    unsigned int flags;
};

struct ViewerBase {
    char pad[0x3C];

    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual void unused41();
    virtual void unused42();
    virtual void unused43();
    virtual void unused44();
    virtual void unused45();
    virtual void unused46();
    virtual void unused47();
    virtual void unused48();
    virtual void unused49();
    virtual void unused50();
    virtual void unused51();
    virtual void unused52();
    virtual void unused53();
    virtual void unused54();
    virtual void unused55();
    virtual void unused56();
    virtual void unused57();
    virtual void unused58();
    virtual void unused59();
    virtual void unused60();
    virtual void unused61();
    virtual void unused62();
    virtual void unused63();
    virtual void unused64();
    virtual void unused65();
    virtual void unused66();
    virtual void unused67();
    virtual void unused68();
    virtual void unused69();
    virtual void unused70();
    virtual void move(int, int);
};

struct Viewer : ViewerBase {
    char pad1[0x168];
    unsigned int flags;
    Input* input;
};

extern "C" int slidePic__18muCollectionViewerFv(Viewer* self, int enabled, int step) {
    int direction = 0;

    if (enabled) {
        unsigned int flags = self->input->flags;
        if (flags & 1)
            direction = -1;
        else if (flags & 2)
            direction = 1;
    }

    if (direction == 0) {
        unsigned int flags = self->flags;
        if (flags & 0x80)
            direction = -step;
        else if (flags & 0x100)
            direction = step;
        else if (flags & 0x20)
            direction = -1;
        else if (flags & 0x40)
            direction = 1;
        else
            return 0;
    }

    self->move(enabled, direction);
    return direction;
}
}

// TODO: clean up, give variables real names
namespace jj_5F24 {
struct Result {
    unsigned char data[0x48];
};

struct Target {
    unsigned char padding[0x350];
    int status;
};

struct State {
    unsigned char padding0[0x1A0];
    int value;
    unsigned char padding1[0x1BFC - 0x1A4];
    int mode;
    unsigned char padding2[0x1CE4 - 0x1C00];
    Target* target;
};

struct ViewerBase {
    unsigned char padding[0x3C];

    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual Result getResult();
};

struct Viewer : ViewerBase {
    unsigned char padding1[0x1AC - 0x40];
    State* state;
};

extern "C" void read__12muCollectionFiQ212muCollection8FileName(Target*, int, const Result*);

extern "C" void controlLoadStart__18muCollectionViewerFv(Viewer* self) {
    State* state = self->state;
    if (state->target->status == 0) {
        read__12muCollectionFiQ212muCollection8FileName(state->target, state->value, &self->getResult());
        self->state->mode = 5;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_5F9C {
struct Pad {
    char pad[0x3c];
};

class Viewer : public Pad {
public:
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
    virtual void v36(); virtual void v37(); virtual void v38(); virtual void v39();
    virtual void v40(); virtual void v41(); virtual void v42(); virtual void v43();
    virtual void v44(); virtual void v45(); virtual void v46(); virtual void v47();
    virtual void v48(); virtual void v49(); virtual void v50(); virtual void v51();
    virtual void v52(); virtual void v53(); virtual void v54(); virtual void v55();
    virtual void v56(); virtual void v57(); virtual void v58(); virtual void v59();
    virtual void v60(); virtual void v61(); virtual void v62(); virtual void v63();
    virtual void v64(); virtual void v65(); virtual void v66(); virtual void v67();
    virtual void v68(); virtual void v69(); virtual void v70(); virtual void v71();
    virtual void v72(); virtual void v73(); virtual void v74(); virtual void v75();
};

extern "C" {
void getData__12muCollectionFv(int* p);
void reset__Q210muRecorder4DataFPv(void* p, int v);

void controlLoading__18muCollectionViewerFv(unsigned char* self) {
    unsigned char* obj;
    unsigned char* inner;
    int sp8;

    obj = *(unsigned char**)(self + 0x1ac);
    inner = *(unsigned char**)(obj + 0x1ce4);
    if (*(int*)(inner + 0x350) == 0) {
        if (*(int*)(inner + 0x354) == 0) {
            int v;
            unsigned char* p;
            getData__12muCollectionFv(&sp8);
            p = *(unsigned char**)(self + 0x1ac) + 0x1cf0;
            v = sp8;
            sp8 = 0;
            reset__Q210muRecorder4DataFPv(p, v);
            reset__Q210muRecorder4DataFPv(&sp8, 0);
            obj = *(unsigned char**)(self + 0x1ac);
            *(int*)(obj + 0x1cf4) =
                *(int*)(*(unsigned char**)(obj + 0x1ce4) + 0x1a4);
            obj = *(unsigned char**)(self + 0x1ac);
            *(int*)(obj + 0x1bfc) = 6;
            return;
        } else {
            ((Viewer*)self)->v75();
            obj = *(unsigned char**)(self + 0x1ac);
            *(int*)(obj + 0x1bfc) = 0;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_604C {
extern "C" {
void reset__Q210muRecorder4DataFPv(void* a, void* b);

void getData__12muCollectionFv(int* a, char* b) {
    *a = 0;
    void* x = *(void**)(b + 0x1A0);
    *(int*)(b + 0x1A0) = 0;
    reset__Q210muRecorder4DataFPv(a, x);
}
}
}

// TODO: clean up, give variables real names
namespace jj_6068 {
typedef unsigned int u32;
typedef unsigned char u8;

struct Resource {
    u8 pad0[0x10];
    void* value;
};

struct ViewerData {
    u8 pad0[0x160];
    Resource* resource;
    u8 pad164[0x1B50 - 0x164];
    u32 input;
    u8 pad1B54[0x1BFC - 0x1B54];
    u32 state;
    u32 previousState;
    u8 flag1;
    u8 pad1C05[0x1C75 - 0x1C05];
    u8 flag2;
};

struct ViewerBase {
    u8 pad0[0x3C];
};

struct ViewerInterface : ViewerBase {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual void unused41();
    virtual void unused42();
    virtual void setResource(void*);
    virtual void unused44();
    virtual void unused45();
    virtual void unused46();
    virtual void unused47();
    virtual void unused48();
    virtual void unused49();
    virtual void unused50();
    virtual void unused51();
    virtual void unused52();
    virtual void unused53();
    virtual void unused54();
    virtual void unused55();
    virtual void changeState(int, int);
    virtual void unused57();
    virtual void unused58();
    virtual void unused59();
    virtual void unused60();
    virtual void unused61();
    virtual void unused62();
    virtual void unused63();
    virtual void unused64();
    virtual void unused65();
    virtual void unused66();
    virtual void unused67();
    virtual void unused68();
    virtual void unused69();
    virtual void update(int, int);
    virtual void unused71();
    virtual void unused72();
    virtual void unused73();
    virtual void unused74();
    virtual void unused75();
    virtual void unused76();
    virtual void unused79(int);
};

struct Viewer : ViewerInterface {
    u8 pad40[0x1A8 - 0x40];
    u32 flags;
    ViewerData* data;
};

extern "C" {
extern void* g_sndSystem;
void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);

void controlErase__18muCollectionViewerFv(Viewer* self) {
    if (self->flags & 2) {
        if ((self->data->flag1 >> 7) & 1) {
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 3, -1, 0, 0, -1);
            return;
        }
        self->data->flag2 = 0;
        self->unused79(self->data->flag2);
        self->data->state = 9;
        self->setResource(self->data->resource->value);
    } else if ((self->data->input & 8) || (self->flags & 4)) {
        self->data->previousState = self->data->state;
        self->data->state = 0;
        self->changeState(1, 1);
        playSE__9sndSystemF5SndIDiiii(
            g_sndSystem, (-((self->flags >> 2) & 1)) & 2, -1, 0, 0, -1);
    } else if (self->data->input & 2) {
        self->data->state = 8;
        self->changeState(7, 1);
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
    } else {
        self->update(0, 5);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_6344 {
extern "C" {
extern char lbl_6_rodata_0[];

void setFrameNode__8MuObjectFf(void*, float);
void setFontColor__5MuMsgFUlUcUcUcUc(
    void*, unsigned long, unsigned char, unsigned char,
    unsigned char, unsigned char);
void printIndex__5MuMsgFUlUlPv(void*, unsigned long, unsigned long, void*);
}

struct Data {
    char pad0[0x10c];
    void* object;
    char pad1[0x1c7c - 0x110];
    void* msg;
};

struct Self {
    char pad[0x1ac];
    Data* data;
};

extern "C" void dispEraseConfirmWndYesNo__18muCollectionViewerFv(Self* self, int arg) {
    char* base = lbl_6_rodata_0;

    setFrameNode__8MuObjectFf(
        self->data->object,
        arg ? *(float*)(base + 0x590) : *(float*)(base + 0x1c4));

    int first, second;
    if (arg) {
        first = 0;
        second = 1;
    } else {
        first = 1;
        second = 0;
    }

    int offset = first * 3;
    unsigned char* colors = (unsigned char*)(base + 0x5bc);
    unsigned char* color = colors + offset;

    setFontColor__5MuMsgFUlUcUcUcUc(
        self->data->msg, 13, colors[offset], color[1], color[2], 255);
    printIndex__5MuMsgFUlUlPv(self->data->msg, 13, 1, 0);

    offset = second * 3;
    color = colors + offset;

    setFontColor__5MuMsgFUlUcUcUcUc(
        self->data->msg, 14, colors[offset], color[1], color[2], 255);
    printIndex__5MuMsgFUlUlPv(self->data->msg, 14, 2, 0);
}
}

// TODO: clean up, give variables real names
namespace jj_684C {
typedef unsigned int u32;

struct Result {
    unsigned char data[0x48];
};

struct Manager {
    unsigned char pad[0x350];
    int state;
};

struct Data {
    unsigned char pad0[0x1A0];
    u32 id;
    unsigned char pad1[0x1BF4 - 0x1A4];
    int state;
    unsigned char pad2[4];
    int nextState;
    unsigned char pad3[0x1CE4 - 0x1C00];
    Manager* manager;
};

struct Base {
    unsigned char pad[0x3C];
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual Result collect(Data* p1);
};

struct Viewer : Base {
    unsigned char pad1[0x1AC - 0x40];
    Data* data;
};

extern "C" {
extern void* g_gfHomeMenu;
extern unsigned char g_gfShutdownManager[];

int setBan__10gfHomeMenuFUlUl(void*, u32, u32);
void erase__12muCollectionFiQ212muCollection8FileName(Manager*, u32, const Result&);

void controlEraseReady__18muCollectionViewerFv(Viewer* self) {
    if (setBan__10gfHomeMenuFUlUl(g_gfHomeMenu, 1, 5) &&
        self->data->manager->state == 0) {
        g_gfShutdownManager[0xD] = 0;
        Data* data = self->data;
        data->nextState = 0x13;
        erase__12muCollectionFiQ212muCollection8FileName(self->data->manager, self->data->id, self->collect(data));
        self->data->state = 3;
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_69F0 {
extern "C" void fn_800B9F90(void*, void*, int);

struct State {
    unsigned char pad0[0x1B4C];
    int flag;
    unsigned char pad1[0x190];
    int timer;
};

struct Self {
    unsigned char pad0[0x3C];
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
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual void v37();
    virtual void v38();
    virtual void v39();
    virtual void v40();
    virtual void v41();
    virtual void v42();
    virtual void v43();
    virtual void v44();
    virtual void v45();
    virtual void v46();
    virtual void v47();
    virtual void v48();
    virtual void v49();
    virtual void v50();
    virtual void v51();
    virtual void v52();
    virtual void v53();
    virtual void v54();
    virtual void v55();
    virtual void v56();
    virtual void v57();
    virtual void v58();
    virtual void v59();
    virtual void v60();
    virtual void v61();
    virtual void v62();
    virtual void v63();
    virtual void v64();
    virtual void v65();
    virtual void v66();
    virtual void v67();
    virtual void v68();
    virtual void v69();
    virtual void v70();
    virtual void v71();
    virtual void v72();
    virtual void v73();
    virtual void v74();
    virtual void v75();
    virtual void v76();
    virtual void v77();
    virtual void v78();
    virtual void v79();
    virtual void v80();
    virtual void v81();
    virtual void v82();
    virtual void v83();
    virtual void v84();
    virtual void v85();
    virtual void v86();
    virtual void v87();
    virtual void v88();
    virtual void callback();
    unsigned char pad1[0x16C];
    State* state;
};

inline int ready(Self* self) {
    int result[16];
    fn_800B9F90(result, (unsigned char*)self + 0x100, 0);
    if (result[0] != 0 || self->state->flag != 0)
        return 1;
    else
        return 0;
}

extern "C" void controlEraseReport__18muCollectionViewerFv(Self* self) {
    if (++self->state->timer >= 120 || ready(self)) {
        self->callback();
    }
}
}

// TODO: clean up, give variables real names
namespace jj_6AE8 {
extern "C" {
void DetachFromParent__Q34nw4r3g3d6G3dObjFv(void*);
extern const float lbl_6_rodata_1C4;
extern const float lbl_6_rodata_1C8;
extern char lbl_6_data_0[];
}
struct ViewerData {
    char pad0[0x160];
    MuObject* object160;
    char pad164[0x1A0 - 0x164];
    int value;
    char pad1A4[0x1C0 - 0x1A4];
    int mode;
    char pad1C4[0x1B50 - 0x1C4];
    u32 input;
    char pad1B54[0x1BFC - 0x1B54];
    int state;
    int previousState;
    char pad1C04[0x1C14 - 0x1C04];
    bool blocked : 1;
    unsigned int timer : 16;
    unsigned int rest : 15;
    MuObject* object;
    MuMsg* currentMessage;
    u32 messageIndex;
    char pad1C24[0x1C7C - 0x1C24];
    MuMsg* message;
    char pad1C80[0x1CA8 - 0x1C80];
    gfFileIOHandle handle;
    char pad1CAC[0x1CC0 - 0x1CAC];
    gfFileIOHandle handle2;
    void show(u32 index, void* buffer) {
        MuMsg* msg = message;
        object->setFrameVisible(lbl_6_rodata_1C8);
        timer = 120;
        currentMessage = msg;
        messageIndex = 16;
        msg->printIndex(16, index, buffer);
    }
};
struct ViewerBase { char pad0[0x3C]; };
struct Viewer : ViewerBase {
    virtual void at8();
    virtual void atC();
    virtual void at10();
    virtual void at14();
    virtual void at18();
    virtual void at1C();
    virtual void at20();
    virtual void at24();
    virtual void at28();
    virtual void at2C();
    virtual void at30();
    virtual void at34();
    virtual void at38();
    virtual void at3C();
    virtual void at40();
    virtual void at44();
    virtual void at48();
    virtual void at4C();
    virtual void at50();
    virtual void at54();
    virtual void at58();
    virtual void at5C();
    virtual void at60();
    virtual void at64();
    virtual void at68();
    virtual void at6C();
    virtual void at70();
    virtual void at74();
    virtual void at78();
    virtual void at7C();
    virtual void at80();
    virtual void at84();
    virtual void at88();
    virtual void at8C();
    virtual void at90();
    virtual void at94();
    virtual void at98();
    virtual void at9C();
    virtual void atA0();
    virtual void atA4();
    virtual void atA8();
    virtual void atAC();
    virtual void atB0();
    virtual void atB4(void*);
    virtual void atB8();
    virtual void atBC();
    virtual void atC0();
    virtual void atC4();
    virtual void atC8();
    virtual void atCC();
    virtual void atD0();
    virtual void atD4();
    virtual void atD8();
    virtual void atDC();
    virtual void atE0();
    virtual void atE4();
    virtual void atE8(int, int);
    virtual void atEC();
    virtual void atF0();
    virtual void atF4();
    virtual void atF8();
    virtual void atFC();
    virtual void at100();
    virtual void at104();
    virtual void at108();
    virtual void at10C();
    virtual void at110();
    virtual void at114();
    virtual void at118();
    virtual void at11C();
    virtual void at120(int, int);
    virtual void at124();
    virtual void at128();
    virtual void at12C();
    virtual void at130();
    virtual void at134();
    virtual void at138();
    virtual void at13C();
    virtual void at140();
    virtual void at144();
    virtual void at148();
    virtual void at14C();
    virtual void at150();
    virtual void at154();
    virtual void at158();
    virtual void at15C();
    virtual void at160();
    virtual void at164();
    virtual void at168();
    virtual void at16C();
    virtual void at170();
    virtual void at174(int, int);

    char pad40[0x1A8 - 0x40];
    u32 flags;
    ViewerData* data;
};
extern "C" void fn_6_6AE8(Viewer* self) {
    if (self->flags & 2) {
        ViewerData* data = self->data;
        if (data->blocked) {
            g_sndSystem->playSE((SndID)3, -1, 0, 0, -1);
            ViewerData* d = self->data;
            if ((int)(d->value == 0) == 1) {
                if (d->mode == 3) {
                    MuMsg* msg;
                    void* buffer = d->handle2.getBuffer();
                    ViewerData* p = self->data;
                    msg = p->message;
                    p->object->setFrameVisible(lbl_6_rodata_1C8);
                    p->timer = 120;
                    p->currentMessage = msg;
                    p->messageIndex = 16;
                    msg->printIndex(16, 0x51, buffer);
                } else {
                    ViewerData* p;
                    MuMsg* msg;
                    void* buffer = d->handle.getBuffer();
                    p = self->data;
                    msg = p->message;
                    p->object->setFrameVisible(lbl_6_rodata_1C8);
                    p->timer = 120;
                    p->currentMessage = msg;
                    p->messageIndex = 16;
                    msg->printIndex(16, 7, buffer);
                }
            }
        } else {
            if (data->currentMessage) {
                data->timer = 0;
                data->object->setFrameVisible(lbl_6_rodata_1C4);
                data->currentMessage->printf(data->messageIndex, lbl_6_data_0);
                data->currentMessage = 0;
                if (!data->blocked) {
                    DetachFromParent__Q34nw4r3g3d6G3dObjFv(data->object->m_sceneModel);
                }
            }
            self->at174(1, self->data->value);
            self->data->state = 10;
            self->atB4(self->data->object160->m_sceneModel);
            g_sndSystem->playSE((SndID)1, -1, 0, 0, -1);
        }
    } else if ((self->data->input & 8) || (self->flags & 4)) {
        self->data->previousState = self->data->state;
        self->data->state = 0;
        self->atE8(1, 1);
        g_sndSystem->playSE((SndID)((self->flags & 4) ? 2 : 0), -1, 0, 0, -1);
    } else if (self->data->input & 1) {
        self->data->state = 7;
        self->atE8(6, 1);
        g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
    } else if (self->data->input & 2) {
        self->data->state = 30;
        self->atE8(8, 1);
        g_sndSystem->playSE((SndID)0, -1, 0, 0, -1);
    } else {
        self->at120(0, 5);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_6EF8 {
typedef unsigned int u32; typedef unsigned char u8;
struct Resource { u8 pad0[0x10]; void* value; };
struct ViewerData { u8 pad0[0x160]; Resource* resource; u8 pad164[0x3C]; u32 unk1A0; u8 pad1A4[0x19B0]; u32 input; u8 pad1B58[0x9C]; u32 unk1BF4; u8 pad1BF8[4]; u32 state; u8 pad1C00[0x24]; u8 flags1; u8 pad1C25[0xF]; u8 flags2; u8 pad1C35[0xF]; u8 flags3; u8 pad1C45[0x33]; int counter; };
struct ViewerBase { u8 pad0[0x3C]; virtual void at8(); virtual void atC(); virtual void at10(); virtual void at14(); virtual void at18(); virtual void at1C(); virtual void at20(); virtual void at24(); virtual void at28(); virtual void at2C(); virtual void at30(); virtual void at34(); virtual void at38(); virtual void at3C(); virtual void at40(); virtual void at44(); virtual void at48(); virtual void at4C(); virtual void at50(); virtual void at54(); virtual void at58(); virtual void at5C(); virtual void at60(); virtual void at64(); virtual void at68(); virtual void at6C(); virtual void at70(); virtual void at74(); virtual void at78(); virtual void at7C(); virtual void at80(); virtual void at84(); virtual void at88(); virtual void at8C(); virtual void at90(); virtual void at94(); virtual void at98(); virtual void at9C(); virtual void atA0(); virtual void atA4(); virtual void atA8(); virtual void atAC(); virtual void atB0(void*); virtual void atB4(); virtual void atB8(); virtual void atBC(); virtual void atC0(); virtual void atC4(); virtual void atC8(); virtual void atCC(); virtual void atD0(); virtual void atD4(); virtual void atD8(); virtual void atDC(); virtual void atE0(); virtual void atE4(); virtual void atE8(); virtual void atEC(); virtual void atF0(); virtual void atF4(); virtual void atF8(); virtual void atFC(); virtual void at100(); virtual void at104(); virtual void at108(); virtual void at10C(); virtual void at110(); virtual void at114(); virtual void at118(); virtual void at11C(); virtual void at120(int,int); virtual void at124(); virtual void at128(); virtual void at12C(); virtual void at130(); virtual void at134(); virtual void at138(); virtual void at13C(); virtual void at140(); virtual void at144(); virtual void at148(); virtual void at14C(); virtual void at150(); virtual void at154(); virtual void at158(); virtual void at15C(); virtual void at160(); virtual void at164(); virtual void at168(); virtual void at16C(); virtual void at170(); virtual void at174(int,int); virtual void at178(int); virtual void at17C(); virtual void at180(); virtual void at184(); virtual void at188(); virtual void at18C(int); };
struct Viewer : ViewerBase { u8 pad40[0x168]; u32 flags; ViewerData* data; };
extern "C" { extern void* g_sndSystem; void playSE__9sndSystemF5SndIDiiii(void*,int,int,int,int,int);
void controlMediaMenu__18muCollectionViewerFv(Viewer* self) { u32 flags=self->flags; if(flags&4) { self->at174(0,self->data->unk1A0); self->data->state=8; self->atB0(self->data->resource->value); playSE__9sndSystemF5SndIDiiii(g_sndSystem,2,-1,0,0,-1); } else if(flags&2) { int val=self->data->counter; switch(val) { case 1: if((self->data->flags1>>7)&1) playSE__9sndSystemF5SndIDiiii(g_sndSystem,3,-1,0,0,-1); else { playSE__9sndSystemF5SndIDiiii(g_sndSystem,1,-1,0,0,-1); self->at178(1); } break; case 2: if((self->data->flags2>>7)&1) playSE__9sndSystemF5SndIDiiii(g_sndSystem,3,-1,0,0,-1); else { playSE__9sndSystemF5SndIDiiii(g_sndSystem,1,-1,0,0,-1); self->at178(0); } break; case 0: if((self->data->flags3>>7)&1) playSE__9sndSystemF5SndIDiiii(g_sndSystem,3,-1,0,0,-1); else { playSE__9sndSystemF5SndIDiiii(g_sndSystem,1,-1,0,0,-1); self->data->state=17; self->data->unk1BF4=5; } break; } } else { u32 input=self->data->input; if(input&2) { if(self->data->counter<2) { self->data->counter++; self->at18C(self->data->counter); playSE__9sndSystemF5SndIDiiii(g_sndSystem,0,-1,0,0,-1); } } else if(input&1) { if(self->data->counter>0) { self->data->counter--; self->at18C(self->data->counter); playSE__9sndSystemF5SndIDiiii(g_sndSystem,0,-1,0,0,-1); } } else self->at120(0,5); } }
}
}

// TODO: clean up, give variables real names
namespace jj_72B8 {
typedef unsigned int u32;

struct MenuResult {
    unsigned char data[0x44];
};

struct ViewerBase {
    unsigned char pad[0x3C];
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual MenuResult getResult();
};

struct Manager {
    unsigned char pad[0x350];
    int state;
};

struct ViewerData {
    unsigned char pad0[0x1A0];
    u32 id;
    unsigned char pad1[0x1BF4 - 0x1A4];
    int state;
    unsigned char pad2[4];
    int nextState;
    unsigned char pad3[0x1CE4 - 0x1C00];
    Manager* manager;
};

struct Viewer : ViewerBase {
    unsigned char pad1[0x1AC - 0x40];
    ViewerData* data;
};

extern "C" {
extern void* g_gfHomeMenu;
extern unsigned char g_gfShutdownManager[];

int setBan__10gfHomeMenuFUlUl(void*, u32, u32);
void copy__12muCollectionFiQ212muCollection8FileName(Manager*, u32, const MenuResult&);
void move__12muCollectionFiQ212muCollection8FileName(Manager*, u32, const MenuResult&);

void controlCopyReady__18muCollectionViewerFv(Viewer* self, int flag) {
    if (setBan__10gfHomeMenuFUlUl(g_gfHomeMenu, 1, 5) &&
        self->data->manager->state == 0) {
        g_gfShutdownManager[0xD] = 0;
        if (flag) {
            ViewerData* data = self->data;
            copy__12muCollectionFiQ212muCollection8FileName(data->manager, data->id, self->getResult());
            self->data->nextState = 12;
        } else {
            ViewerData* data = self->data;
            move__12muCollectionFiQ212muCollection8FileName(data->manager, data->id, self->getResult());
            self->data->nextState = 15;
        }
        self->data->state = 4;
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_73B4 {
typedef unsigned int u32;

struct Pre {
    unsigned char pad[0x3C];
};

struct Base : Pre {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual void unused41();
    virtual void unused42();
    virtual void unused43();
    virtual void unused44();
    virtual void unused45();
    virtual void unused46();
    virtual void unused47();
    virtual void unused48();
    virtual void unused49();
    virtual void unused50();
    virtual void unused51();
    virtual void unused52();
    virtual void unused53();
    virtual void unused54();
    virtual void unused55();
    virtual void unused56();
    virtual void unused57();
    virtual void unused58();
    virtual void unused59();
    virtual void unused60();
    virtual void unused61();
    virtual void unused62();
    virtual void unused63();
    virtual void unused64();
    virtual void unused65();
    virtual void unused66();
    virtual void unused67();
    virtual void unused68();
    virtual void unused69();
    virtual void unused70();
    virtual void unused71();
    virtual void unused72();
    virtual void unused73();
    virtual void unused74();
    virtual void unused75();
    virtual void unused76();
    virtual void unused77();
    virtual void unused78();
    virtual void unused79();
    virtual void unused80();
    virtual void vf81(int);
    virtual void unused82();
    virtual void unused83();
    virtual void unused84();
    virtual void vf85();
    virtual void unused86();
    virtual void unused87();
    virtual void unused88();
    virtual void unused89();
    virtual void unused90();
    virtual void unused91();
    virtual void unused92();
    virtual void unused93();
    virtual void unused94();
    virtual void unused95();
    virtual void vf96();
};

struct Manager {
    unsigned char pad[0x350];
    int state;
    int unk354;
};

struct Data {
    unsigned char pad0[0x1BFC];
    int nextState;
    unsigned char pad1[0x1CE0 - 0x1C00];
    int unk1CE0;
    Manager* manager;
};

struct Viewer : Base {
    unsigned char pad1[0x1AC - 0x40];
    Data* data;
};

extern "C" {
extern void* g_gfHomeMenu;
extern unsigned char g_gfShutdownManager[];
extern void* g_sndSystem;

int setBan__10gfHomeMenuFUlUl(void*, u32, u32);
void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);

void controlCopying__18muCollectionViewerFv(Viewer* self, int flag) {
    if (self->data->manager->state != 0) {
        return;
    }
    setBan__10gfHomeMenuFUlUl(g_gfHomeMenu, 0, 5);
    g_gfShutdownManager[0xD] = 1;
    if (self->data->manager->unk354 != 0) {
        self->vf96();
        return;
    }
    self->vf85();
    self->vf81(flag ? 1 : 3);
    self->data->unk1CE0 = 0;
    self->data->nextState = flag ? 0xD : 0x10;
    playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x27, -1, 0, 0, -1);
}
}
}

// TODO: clean up, give variables real names
namespace jj_74D0 {
struct Result {
    int value;
    char rest[0x3C];
};

extern "C" void fn_800B9F90(Result*, void*, int);

#define SLOT(n) virtual void v##n();

struct Self {
    char padding[0x3C];
    SLOT(0) SLOT(1) SLOT(2) SLOT(3) SLOT(4) SLOT(5) SLOT(6) SLOT(7) SLOT(8) SLOT(9)
    SLOT(10) SLOT(11) SLOT(12) SLOT(13) SLOT(14) SLOT(15) SLOT(16) SLOT(17) SLOT(18) SLOT(19)
    SLOT(20) SLOT(21) SLOT(22) SLOT(23) SLOT(24) SLOT(25) SLOT(26) SLOT(27) SLOT(28) SLOT(29)
    SLOT(30) SLOT(31) SLOT(32) SLOT(33) SLOT(34) SLOT(35) SLOT(36) SLOT(37) SLOT(38) SLOT(39)
    SLOT(40) SLOT(41) SLOT(42) SLOT(43) SLOT(44) SLOT(45) SLOT(46) SLOT(47) SLOT(48) SLOT(49)
    SLOT(50) SLOT(51) SLOT(52) SLOT(53) SLOT(54) SLOT(55) SLOT(56) SLOT(57) SLOT(58) SLOT(59)
    SLOT(60) SLOT(61) SLOT(62) SLOT(63) SLOT(64) SLOT(65) SLOT(66) SLOT(67) SLOT(68) SLOT(69)
    SLOT(70) SLOT(71) SLOT(72) SLOT(73) SLOT(74) SLOT(75) SLOT(76) SLOT(77) SLOT(78) SLOT(79)
    SLOT(80) SLOT(81) SLOT(82) SLOT(83) SLOT(84) SLOT(85) SLOT(86) SLOT(87) SLOT(88) SLOT(89)
    SLOT(90) SLOT(91) SLOT(92) SLOT(93) SLOT(94) SLOT(95)
    virtual void callback();
};

#undef SLOT

inline bool isActive(Self* self) {
    char* p = (char*)self;
    Result result;
    fn_800B9F90(&result, p + 0x100, 0);
    if (result.value != 0 ||
        *(int*)(*(char**)(p + 0x1AC) + 0x1B4C) != 0) {
        return true;
    }
    return false;
}

extern "C" void controlCopyReport__18muCollectionViewerFv(Self* self) {
    char* p = (char*)self;
    char* owner = *(char**)(p + 0x1AC);
    int value = *(int*)(owner + 0x1CE0) + 1;
    *(int*)(owner + 0x1CE0) = value;

    if (value >= 0x78 || isActive(self)) {
        self->callback();
    }
}
}

// TODO: clean up, give variables real names
namespace jj_7564 {
extern "C" {
void reset__Q210muRecorder4DataFPv(void*, int);
extern char lbl_6_data_0[];
extern char lbl_6_data_658[];
}
struct ViewerBase { char pad[0x3C]; };
struct Viewer : ViewerBase {
#define SLOT(n) virtual void unused##n();
    SLOT(1);
    SLOT(2);
    SLOT(3);
    SLOT(4);
    SLOT(5);
    SLOT(6);
    SLOT(7);
    SLOT(8);
    SLOT(9);
    SLOT(10);
    SLOT(11);
    SLOT(12);
    SLOT(13);
    SLOT(14);
    SLOT(15);
    SLOT(16);
    SLOT(17);
    SLOT(18);
    SLOT(19);
    SLOT(20);
    SLOT(21);
    SLOT(22);
    SLOT(23);
    SLOT(24);
    SLOT(25);
    SLOT(26);
    SLOT(27);
    SLOT(28);
    SLOT(29);
    SLOT(30);
    SLOT(31);
    SLOT(32);
    SLOT(33);
    SLOT(34);
    SLOT(35);
    SLOT(36);
    SLOT(37);
    SLOT(38);
    SLOT(39);
    SLOT(40);
    SLOT(41);
    SLOT(42);
    virtual void fB0(void*);
    SLOT(44); SLOT(45);
    virtual int fBC(int);
    virtual void fC0(int, int);
    SLOT(48);
    SLOT(49);
    SLOT(50);
    SLOT(51);
    SLOT(52);
    SLOT(53);
    SLOT(54);
    SLOT(55);
    SLOT(56);
    SLOT(57);
    SLOT(58);
    SLOT(59);
    SLOT(60);
    SLOT(61);
    SLOT(62);
    SLOT(63);
    SLOT(64);
    SLOT(65);
    SLOT(66);
    SLOT(67);
    SLOT(68);
    SLOT(69);
    SLOT(70);
    SLOT(71);
    SLOT(72);
    SLOT(73);
    SLOT(74);
    SLOT(75);
    SLOT(76);
    SLOT(77);
    SLOT(78);
    SLOT(79);
    SLOT(80);
    SLOT(81);
    SLOT(82);
    SLOT(83);
    SLOT(84);
    SLOT(85);
    SLOT(86);
    SLOT(87);
    SLOT(88);
    SLOT(89);
    SLOT(90);
    SLOT(91);
    virtual void f174(int, int);
#undef SLOT
};
struct ResourceBase { char pad[0x50]; };
struct Resource : ResourceBase { virtual void f0(int); };
extern "C" void controlMediaCheck__18muCollectionViewerFv(char* self) {
    int var_r27;
    char* sub;
    char* temp_r26;
    char* name;
    char* fmt;
    unsigned int var_r24;
    Resource* q;
    var_r27 = 0;
    unsigned int idx = (*(unsigned int*)(*(char**)(self + 0x1ac) + 0x1a0) == 0);
    sub = *(char**)(*(char**)(self + 0x1ac) + 0x1ce4) + idx * 0x14;
    if (((Viewer*)self)->fBC(idx) != 0) {
        if (((unsigned char*)sub)[0x358] != 0) var_r27 = 1;
    }
    if (var_r27 != 0) {
        if (*(int*)(*(char**)(*(char**)(self + 0x1ac) + 0x1ce4) + 0x350) == 0) {
            char* temp_r23;
            char* var_r22;
            temp_r26 = *(char**)(self + 0x1ac) + 0x1c8;
            name = lbl_6_data_0;
            var_r22 = temp_r26;
            var_r24 = 0;
            do {
                bool ne = strcmp(var_r22, name) != 0;
                if (ne) {
                    var_r22[0] = 0;
                    strcat(var_r22, lbl_6_data_658);
                    temp_r23 = (char*)(((unsigned long)(var_r22 + 0x4f)) & ~7UL);
                    if (temp_r23 != 0 && temp_r23 != 0) {
                        if (temp_r23 + 0xc != 0) {
                            q = *(Resource**)(temp_r23 + 0xc);
                            if (q != 0) q->f0(1);
                        }
                        if (temp_r23 != 0) {
                            if (temp_r23 != 0) reset__Q210muRecorder4DataFPv(temp_r23, 0);
                        }
                    }
                    *(int*)(var_r22 + 0x44) = 0x3d;
                }
                ++var_r24;
                var_r22 += 0x68;
            } while (var_r24 < 0x3e);
            *(int*)(temp_r26 + 0x1930) = 0;
            ((Viewer*)self)->fC0(idx, 1);
            ((Viewer*)self)->f174(0, *(int*)(*(char**)(self + 0x1ac) + 0x1a0));
            *(int*)(*(char**)(self + 0x1ac) + 0x1a0) = idx;
            *(int*)(*(char**)(self + 0x1ac) + 0x1bfc) = 0;
            ((Viewer*)self)->fB0(*(void**)(*(char**)(*(char**)(self + 0x1ac) + 0x160) + 0x10));
        }
    } else {
        *(int*)(*(char**)(self + 0x1ac) + 0x1bfc) = 10;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_7730 {
typedef unsigned int u32;
typedef unsigned char u8;

struct CollectData {
    u8 pad0[0x1B50];
    u32 input;
    u8 pad1B54[0x1BFC - 0x1B54];
    u32 state;
    u32 previousState;
    u8 pad1C04[0x1C54 - 0x1C04];
    u8 flag1;
    u8 pad1C55[0x1C74 - 0x1C55];
    u8 flag2;
};

struct CollectBase {
    u8 pad0[0x3C];
};

struct CollectInterface : CollectBase {
    virtual void v0();   virtual void v1();
    virtual void v2();   virtual void v3();
    virtual void v4();   virtual void v5();
    virtual void v6();   virtual void v7();
    virtual void v8();   virtual void v9();
    virtual void v10();  virtual void v11();
    virtual void v12();  virtual void v13();
    virtual void v14();  virtual void v15();
    virtual void v16();  virtual void v17();
    virtual void v18();  virtual void v19();
    virtual void v20();  virtual void v21();
    virtual void v22();  virtual void v23();
    virtual void v24();  virtual void v25();
    virtual void v26();  virtual void v27();
    virtual void v28();  virtual void v29();
    virtual void v30();  virtual void v31();
    virtual void v32();  virtual void v33();
    virtual void v34();  virtual void v35();
    virtual void v36();  virtual void v37();
    virtual void v38();  virtual void v39();
    virtual void v40();  virtual void v41();
    virtual void v42();  virtual void v43();
    virtual void v44();  virtual void v45();
    virtual void v46();  virtual void v47();
    virtual void v48();  virtual void v49();
    virtual void v50();  virtual void v51();
    virtual void v52();  virtual void v53();
    virtual void v54();  virtual void v55();
    virtual void changeState(int, int);
    virtual void v59();  virtual void v60();
    virtual void v61();  virtual void v62();
    virtual void v63();  virtual void v64();
    virtual void v65();  virtual void v66();
    virtual void v67();  virtual void v68();
    virtual void v69();  virtual void v70();
    virtual void v71();
    virtual void update(int, int);
};

struct Collect : CollectInterface {
    u8 pad40[0x1A8 - 0x40];
    u32 flags;
    CollectData* data;
};

extern "C" {
extern void* g_sndSystem;
void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);

void controlSend__18muCollectionViewerFv(Collect* self) {
    if (self->flags & 2) {
        if ((self->data->flag1 >> 7) & 1) {
            playSE__9sndSystemF5SndIDiiii(
                g_sndSystem, 3, -1, 0, 0, -1);
        } else {
            self->data->state = 0x1F;
            self->changeState(0xA, 1);
            playSE__9sndSystemF5SndIDiiii(
                g_sndSystem, 1, -1, 0, 0, -1);
        }
    } else if ((self->data->input & 8) || (self->flags & 4)) {
        self->data->previousState = self->data->state;
        self->data->state = 0;
        self->changeState(1, 1);
        playSE__9sndSystemF5SndIDiiii(
            g_sndSystem, (self->flags & 4) ? 2 : 0, -1, 0, 0, -1);
    } else if (self->data->input & 1) {
        self->data->state = 8;
        self->changeState(7, 1);
        playSE__9sndSystemF5SndIDiiii(
            g_sndSystem, 0, -1, 0, 0, -1);
    } else if (self->data->flag2 != 0 && (self->data->input & 2)) {
        self->data->state = 0x22;
        self->changeState(9, 1);
        playSE__9sndSystemF5SndIDiiii(
            g_sndSystem, 0, -1, 0, 0, -1);
    } else {
        self->update(0, 5);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_7C90 {
typedef unsigned int u32;
typedef unsigned char u8;

struct ViewerData {
    u8 pad0[0x1B50];
    u32 input;
    u8 pad1B54[0x1BFC - 0x1B54];
    u32 state;
    u32 previousState;
    u8 pad1C04[0x1C64 - 0x1C04];
    u8 flags;
};

struct ViewerBase {
    u8 pad0[0x3C];
};

struct ViewerInterface : ViewerBase {
    virtual void unused0();
    virtual void unused1();
    virtual void unused2();
    virtual void unused3();
    virtual void unused4();
    virtual void unused5();
    virtual void unused6();
    virtual void unused7();
    virtual void unused8();
    virtual void unused9();
    virtual void unused10();
    virtual void unused11();
    virtual void unused12();
    virtual void unused13();
    virtual void unused14();
    virtual void unused15();
    virtual void unused16();
    virtual void unused17();
    virtual void unused18();
    virtual void unused19();
    virtual void unused20();
    virtual void unused21();
    virtual void unused22();
    virtual void unused23();
    virtual void unused24();
    virtual void unused25();
    virtual void unused26();
    virtual void unused27();
    virtual void unused28();
    virtual void unused29();
    virtual void unused30();
    virtual void unused31();
    virtual void unused32();
    virtual void unused33();
    virtual void unused34();
    virtual void unused35();
    virtual void unused36();
    virtual void unused37();
    virtual void unused38();
    virtual void unused39();
    virtual void unused40();
    virtual void unused41();
    virtual void unused42();
    virtual void unused43();
    virtual void unused44();
    virtual void unused45();
    virtual void unused46();
    virtual void unused47();
    virtual void unused48();
    virtual void unused49();
    virtual void unused50();
    virtual void unused51();
    virtual void unused52();
    virtual void unused53();
    virtual void unused54();
    virtual void unused55();
    virtual void changeState(int, int);
    virtual void unused57();
    virtual void unused58();
    virtual void unused59();
    virtual void unused60();
    virtual void unused61();
    virtual void unused62();
    virtual void unused63();
    virtual void unused64();
    virtual void unused65();
    virtual void unused66();
    virtual void unused67();
    virtual void unused68();
    virtual void unused69();
    virtual void update(int, int);
};

struct Viewer : ViewerInterface {
    u8 pad40[0x1A8 - 0x40];
    u32 flags;
    ViewerData* data;
};

extern "C" {
extern void* g_sndSystem;
void playSE__9sndSystemF5SndIDiiii(void*, int, int, int, int, int);

void controlContribute__18muCollectionViewerFv(Viewer* self) {
    if (self->flags & 2) {
        if ((self->data->flags >> 7) & 1) {
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 3, -1, 0, 0, -1);
        } else {
            self->data->state = 0x23;
            self->changeState(10, 1);
            playSE__9sndSystemF5SndIDiiii(g_sndSystem, 1, -1, 0, 0, -1);
        }
    } else if ((self->data->input & 8) || (self->flags & 4)) {
        self->data->previousState = self->data->state;
        self->data->state = 0;
        self->changeState(1, 1);
        playSE__9sndSystemF5SndIDiiii(
            g_sndSystem, (self->flags & 4) ? 2 : 0, -1, 0, 0, -1);
    } else if (self->data->input & 1) {
        self->data->state = 30;
        self->changeState(8, 1);
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
    } else {
        self->update(0, 5);
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_8174 {
extern "C" s32 fn_6_8174(void) {
    return 0x1;
}
}

