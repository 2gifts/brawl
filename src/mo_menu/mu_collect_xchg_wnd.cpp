#include <gf/gf_task.h>
#include <gm/gm_common.h>
#include <gm/gm_syssave_task.h>
#include <memory.h>
#include <mu/mu_msg.h>
#include <mu/mu_notice_wnd.h>
#include <mu/mu_object.h>
#include <mu/wifi/mu_wifi_data_load_task.h>
#include <nt/nt_etc_nwc24.h>
#include <nt/nt_etc_xchg.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

// The window of the collection viewer that sends a collection through WiiConnect24 to a friend (module
// sora_menu_collect_viewer). The class is only described by the members the decompiled functions touch.
/// Function names come from the sora_menu_collect_viewer map.
// HYPOTHESIS: two settings of the mails that are sent (nteExchange 0x800: main.dol 0x8014E6A8 and 0x8014E6B4)
extern "C" void fn_8014E6A8(nteExchange* exchange, int a, int b);
extern "C" void fn_8014E6B4(nteExchange* exchange, int a);

class muCollectXchgWndTask : public gfTask {
public:
    MuObject* m_obj40;                      // 0x40
    u8 m_unk44[0x68 - 0x44];
    nw4r::g3d::ScnObj* m_unk68;             // 0x68
    u8 m_unk6C[0x70 - 0x6C];
    nw4r::g3d::ScnGroup* m_scnGroup;        // 0x70
    u8 m_unk74_[0x74 - 0x74];
    MuMsg* m_msg;                           // 0x74
    void* m_msgData;                        // 0x78
    u8 m_unk7C[0x124 - 0x7C];
    int m_stepResult;                       // 0x124 HYPOTHESIS: what the main steps return while they wait
    u8 m_unk128[0x72C - 0x128];
    int m_numFriend;                        // 0x72C
    nteExchange* m_exchange;                // 0x730
    u8 m_unk734[0x748 - 0x734];
    int m_unk748;                           // 0x748
    int m_unk74C;                           // 0x74C
    muNoticeWndTask* m_wnd;                 // 0x750
    nw4r::g3d::ResFile* m_resFile;          // 0x754
    int m_priority;                         // 0x758
    u8 m_unk75C[0x760 - 0x75C];
    int m_unk760;                           // 0x760
    int m_unk764;                           // 0x764
    int m_unk768;                           // 0x768
    u8 m_unk76C[0x770 - 0x76C];
    muWifiDataLoadTask* m_dataLoadTask;     // 0x770
    u8 m_unk774;                            // 0x774
    u8 m_unk775[0x778 - 0x775];
    int m_msgId;                            // 0x778
    int m_unk77C;                           // 0x77C
    int m_unk780;                           // 0x780
    int m_libState;                         // 0x784
    int m_statusWait;                       // 0x788

    void mainStepLibOpenInit();
    int mainStepLibOpenMain();
    void mainStepCheckStatusInit();
    int mainStepCheckStatusMain();
    void mainStepWrongDataInit();
    int mainStepWrongDataMain();
    void mainStepLoadWifiDataInit();
    void mainStepMakeFriendListInit();
    int mainStepAppearMain();
    int mainStepNoFriendMain();
    void mainStepSelectingInit();
    void mainStepCancelConfirmInit();
    int mainStepCancelConfirmMain();
    void mainStepNoFriendInit();
    void mainStepSendConfirmInit();
    void mainStepSendInit();
    void mainStepSendCompleteInit();
    void mainStepSendOverInit();
    int mainStepSendOverMain();
    int mainStepSendConfirmMain();
    int mainStepSendMain();
    int mainStepSendCompleteMain();
    int mainStepErrorMain();
    void mainStepLeaveInit();
    int mainStepLeaveMain();
};

// The callback that keeps the error of WiiConnect24 for the window.
class muCollectXchgWndCB : public IWiiConnect24_Error {
public:
    int m_kind; // 0x04
    int m_code; // 0x08

    virtual void notify_Error(int kind, int unk, int code);
    ~muCollectXchgWndCB();
};

void muCollectXchgWndTask::mainStepLibOpenInit() {
    if (m_exchange->_requestJob(4, 0)) {
        m_libState = 1;
    } else {
        m_libState = 2;
    }
}

int muCollectXchgWndTask::mainStepLibOpenMain() {
    if (m_libState == 2) {
        m_unk77C = 3;
        m_unk780 = 0;
        m_unk774 = 1;
        return 0xD;
    }
    nteExchange* exchange = m_exchange;
    if (!exchange->m_jobDone) {
        return m_stepResult;
    }
    if (exchange->m_result != 0) {
        return m_stepResult;
    }
    return 1;
}

void muCollectXchgWndTask::mainStepCheckStatusInit() {
    m_exchange->check_getStatus(1);
    m_statusWait = 2;
}

int muCollectXchgWndTask::mainStepCheckStatusMain() {
    m_statusWait--;
    if (m_statusWait >= 0) {
        return m_stepResult;
    }
    return 3;
}

void muCollectXchgWndTask::mainStepWrongDataInit() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), m_msgId, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectXchgWndTask::mainStepWrongDataMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xE;
    }
    return m_stepResult;
}

void muCollectXchgWndTask::mainStepLoadWifiDataInit() {
    m_dataLoadTask = muWifiDataLoadTask::create(m_resFile, m_scnGroup, m_msgData, m_priority + 3);
}

void muCollectXchgWndTask::mainStepMakeFriendListInit() {
    m_exchange->_requestJob(1, 0);
}

int muCollectXchgWndTask::mainStepAppearMain() {
    if (m_obj40->isNodeAnimFinished()) {
        int step = 7;
        if (m_numFriend <= 0) {
            step = 6;
        }
        return step;
    }
    return m_stepResult;
}

int muCollectXchgWndTask::mainStepNoFriendMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 7;
    }
    return m_stepResult;
}

void muCollectXchgWndTask::mainStepSelectingInit() {
    if (m_numFriend > 0) {
        return;
    }
    m_unk748 = 1;
    m_unk74C = 1;
}

void muCollectXchgWndTask::mainStepCancelConfirmInit() {
    m_wnd = fn_800BEEB8(1, m_msg->getBinary(), 6, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectXchgWndTask::mainStepCancelConfirmMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        u8 selected = wnd->m_selected;
        wnd->exit();
        m_wnd = NULL;
        int step = 7;
        if (selected != 0) {
            step = 0xE;
        }
        return step;
    }
    return m_stepResult;
}

void muCollectXchgWndTask::mainStepNoFriendInit() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 0xB, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

void muCollectXchgWndTask::mainStepSendOverInit() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 8, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

int muCollectXchgWndTask::mainStepSendOverMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 7;
    }
    return m_stepResult;
}

int muCollectXchgWndTask::mainStepSendConfirmMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        u8 selected = wnd->m_selected;
        wnd->exit();
        m_wnd = NULL;
        int step = 7;
        if (selected != 0) {
            step = 0xB;
        }
        return step;
    }
    return m_stepResult;
}

int muCollectXchgWndTask::mainStepSendMain() {
    nteExchange* exchange = m_exchange;
    if (!exchange->m_jobDone) {
        return m_stepResult;
    }
    if (exchange->m_result == 0) {
        return 0xC;
    }
    m_msgId = 9;
    return 2;
}

int muCollectXchgWndTask::mainStepSendCompleteMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xE;
    }
    return m_stepResult;
}

int muCollectXchgWndTask::mainStepErrorMain() {
    muNoticeWndTask* wnd = m_wnd;
    if (wnd->m_step == 3) {
        wnd->exit();
        m_wnd = NULL;
        return 0xE;
    }
    return m_stepResult;
}

void muCollectXchgWndTask::mainStepLeaveInit() {
    m_scnGroup->Remove(m_unk68);
}

int muCollectXchgWndTask::mainStepLeaveMain() {
    return 0xF;
}

muCollectXchgWndCB::~muCollectXchgWndCB() { }

void muCollectXchgWndCB::notify_Error(int kind, int unk, int code) {
    m_kind = kind;
    m_code = code;
}

void muCollectXchgWndTask::mainStepSendConfirmInit() {
    m_wnd = fn_800BEEB8(1, m_msg->getBinary(), 7, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
}

void muCollectXchgWndTask::mainStepSendCompleteInit() {
    m_wnd = fn_800BEEB8(0, m_msg->getBinary(), 0xA, m_resFile, m_scnGroup, m_priority + 3, 0, 0x2A, 0x2B, 0, 0, 0xF0);
    m_wnd->unk128 = 0x1EF3;
    gmSysSaveTask::create(0x2E);
    gmResetAutosaveDirtyFlag();
}

void muCollectXchgWndTask::mainStepSendInit() {
    u8* entry;
    nteExchange* exchange = m_exchange;
    exchange->mail_ClearSendTo();
    fn_8014E6A8(exchange, m_unk760, m_unk764);
    fn_8014E6B4(exchange, m_unk768);
    entry = (u8*)this;
    for (int i = 0; i < m_numFriend; i++) {
        if (entry[0x142] != 0) {
            exchange->mail_AddSendTo(exchange->friend_getWiiID(i));
        }
        entry += 0x18;
    }
    exchange->_requestJob(2, 0);
}

#include <nt/nt_offline.h>
#include <gf/gf_heap_manager.h>
#include <string.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_pad_status.h>
#include <snd/snd_system.h>
#include <ms/ms_message.h>
#include <stdio.h>
// TODO: clean up, give variables real names
namespace jj_8728 {
typedef unsigned char u8;

class VtblObj_5C {
    u8 pad[0x5C];
public:
    virtual void fn(int);
};

class VtblObj_3C {
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
extern u8 lbl_6_data_6EC[];
void exit__6gfTaskFv(void*);
void free__13gfHeapManagerFPv(void*);
void __dt__11nteExchangeFv(void*, int);
void __dt__5MuMsgFv(void*, int);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void fn_8014C960(void);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);

void* __dt__20muCollectXchgWndTaskFv(void* self, int deleting)
{
    if (self) {
        u8* obj = (u8*)self;

        void* prev = *reinterpret_cast<void**>(obj + 0x750);
        *reinterpret_cast<void**>(obj + 0x3C) = (void*)lbl_6_data_6EC;
        if (prev) {
            exit__6gfTaskFv(prev);
        }

        void* heap = *reinterpret_cast<void**>(obj + 0x760);
        if (heap) {
            free__13gfHeapManagerFPv(heap);
        }

        void* a = *reinterpret_cast<void**>(obj + 0x730);
        if (a) {
            __dt__11nteExchangeFv(a, 1);
        }

        void* b = *reinterpret_cast<void**>(obj + 0x74);
        if (b) {
            __dt__5MuMsgFv(b, 1);
        }

        u8* cursor = obj;
        for (int i = 0; i < 2; ++i, cursor += 4) {
            void* entry = *reinterpret_cast<void**>(cursor + 0x68);
            if (entry) {
                Destroy__Q34nw4r3g3d6G3dObjFv(entry);
            }
        }

        cursor = obj;
        for (int i = 0; i < 10; ++i, cursor += 4) {
            VtblObj_5C* entry = *reinterpret_cast<VtblObj_5C**>(cursor + 0x40);
            if (entry && entry) {
                entry->fn(1);
            }
        }

        VtblObj_3C* d = *reinterpret_cast<VtblObj_3C**>(obj + 0x770);
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
namespace jj_8870 {

struct SwitchObject {
    int type;
    int pad4;
    virtual void destroy(int);
    void* data;
};

extern "C" {
void fn_80153488(void*);
void fn_80153498(void*, int);
int fn_80153504(void*, int);
void fn_80153610(void*, void*, u64, int, int, int);
int fn_80153810(void*);
SwitchObject* fn_80153818(void*, int);
int getStateType__10nteSectionFv(void*);
u64 getCreated__10nteSectionFv(void*);
void* getSize__10nteSectionFv(void*);
void __ct__7nteFileFQ25Heaps8HeapType(void*, int);
void __dt__7nteFileFv(void*, int);
void fn_80153EC4(void*, void*);
int getDataFileSize__7nteFileFv(void*);
int addSection__7nteFileFP14nteSectionData(void*, int);
s16 fn_8014E708(void*);
}

extern "C" void* makeAttachData__20muCollectXchgWndTaskFv(void*, s16* outId, int* outSize, void** outType, void* input) {
    char source[8];
    char target[8];
    char writer[0xC4];
    SwitchObject* object;
    s16 id;
    int size;

    fn_80153488(source);
    fn_80153488(target);
    __ct__7nteFileFQ25Heaps8HeapType(writer, 0x2B);
    if (fn_80153504(source, *(int*)((char*)input + 0xE4)) == 0) {
        __dt__7nteFileFv(writer, -1);
        fn_80153498(target, -1);
        fn_80153498(source, -1);
        return 0;
    }
    if (getStateType__10nteSectionFv(source) != 0 && getStateType__10nteSectionFv(source) != 1) {
        __dt__7nteFileFv(writer, -1);
        fn_80153498(target, -1);
        fn_80153498(source, -1);
        return 0;
    }
    object = fn_80153818(source, 0x2B);
    switch (object->type) {
    case 1:
        {
            ntFriendInfo* info = ntFriendInfo::getInstance();
            void* destination = object->data;
            void* name = *(void**)info;
            memcpy(destination, (char*)name + 0x4F0, 0xA);
        }
        input = (void*)1;
        break;
    case 2:
        {
            ntFriendInfo* info = ntFriendInfo::getInstance();
            void* destination = object->data;
            void* name = *(void**)info;
            memcpy(destination, (char*)name + 0x4F0, 0xA);
        }
        input = (void*)2;
        break;
    case 3:
        {
            ntFriendInfo* info = ntFriendInfo::getInstance();
            void* destination = object->data;
            void* name = *(void**)info;
            memcpy(destination, (char*)name + 0x4F0, 0xA);
        }
        input = (void*)3;
        break;
    }
    fn_80153610(target, object, getCreated__10nteSectionFv(source), 0x2B, 1, 0);
    id = fn_8014E708(getSize__10nteSectionFv(target));
    if (object != 0) {
        object->destroy(1);
    }
    addSection__7nteFileFP14nteSectionData(writer, fn_80153810(target));
    size = getDataFileSize__7nteFileFv(writer);
    object = (SwitchObject*)gfHeapManager::alloc((Heaps::HeapType)0x2B, size, 0x20);
    fn_80153EC4(writer, object);
    *outId = id;
    *outSize = size;
    *outType = input;
    __dt__7nteFileFv(writer, -1);
    fn_80153498(target, -1);
    fn_80153498(source, -1);
    return object;
}
}

// TODO: clean up, give variables real names
namespace jj_8AC0 {
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
extern char lbl_6_data_6DC[];
extern const float lbl_6_rodata_608;

void* create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
    void*, const char*, int, void*, int);
void changeAnimN__8MuObjectFPCc(void*, const char*);
void setUpdateRate__16gfModelAnimationFf(void*, float);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
}

extern "C" void createObjResFile__20muCollectXchgWndTaskFv(Menu* self, Entry* entries, int count, void* resourceFile,
                          int idBase, int heapType) {
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
                resourceFile, entry->name, entry->argument + idBase, 0, heapType);
            name = entry->name;
            self->objects[entry->first + j] = object;
            strcpy(animationName, name);
            strcat(animationName, lbl_6_data_6DC);
            changeAnimN__8MuObjectFPCc(object, animationName);
            if (object->animation)
                setUpdateRate__16gfModelAnimationFf(object->animation, lbl_6_rodata_608);
            ++j;
        }
        ++i;
        ++entry;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8BBC {
extern "C" {
extern float lbl_6_rodata_60C;
void* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32, u32, u32);
void allocMsgBuf__5MuMsgFUlUl(void*, u32, u32);
void setMsgData__5MuMsgFPv(void*, void*);
void attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(void*, u32, void*, u32, float);
void printIndex__5MuMsgFUlUlPv(void*, u32, u32, void*);
}

struct Obj {
    char pad[0x10];
    void* model;
};

struct Self {
    char pad[0x40];
    Obj* a;
    char pad2[4];
    Obj* b[7];
};

extern "C" void initMsg__20muCollectXchgWndTaskFv(Self* self, void* data, u32 a, u32 b) {
    int i;
    Obj* inner;
    void* msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(1, a, b);
    *(void**)((char*)self + 0x74) = msg;
    allocMsgBuf__5MuMsgFUlUl(msg, 0x100, 0xB);
    setMsgData__5MuMsgFPv(msg, data);

    inner = self->a;
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(
        msg, 0, inner->model, 0, lbl_6_rodata_60C);
    printIndex__5MuMsgFUlUlPv(msg, 0, 0, 0);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(
        msg, 1, inner->model, 1, lbl_6_rodata_60C);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(
        msg, 2, inner->model, 2, lbl_6_rodata_60C);
    printIndex__5MuMsgFUlUlPv(msg, 2, 4, 0);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(
        msg, 3, inner->model, 3, lbl_6_rodata_60C);
    printIndex__5MuMsgFUlUlPv(msg, 3, 5, 0);

    for (i = 0; i < 7; i++) {
        attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(
            msg, i + 4, self->b[0]->model, 0, lbl_6_rodata_60C);
        self = (Self*)((char*)self + 4);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8D04 {
extern "C" {

typedef int (*StateFunc)(void*);

struct StateTable {
    void (*enter)(void*);
    StateFunc update;
};

extern StateTable lbl_6_rodata_610[];
void fn_800B9984(void*);

struct Viewer {
    char pad[0x124];
    union {
        int state;
        unsigned int stateIndex;
    };
    int previousState;
    char pad2[0x60C];
    int fieldA;
    int fieldB;
    char pad3[0x34];
    unsigned char initialized;
    char pad4[7];
    int fieldA2;
    int fieldB2;
};

void processDefault__20muCollectXchgWndTaskFv(Viewer* self) {
    if (self->initialized == 0) {
        if (self->fieldA != 0) {
            self->fieldA2 = self->fieldA;
            self->fieldB2 = self->fieldB;
            self->initialized = 1;
            self->state = 0xd;
        }
    }

    fn_800B9984((char*)self + 0x7C);

    if (self->previousState != self->state) {
        StateFunc enter = (StateFunc)lbl_6_rodata_610[self->stateIndex].enter;
        if (enter)
            enter(self);
        self->previousState = self->state;
    }

    StateFunc update = lbl_6_rodata_610[self->stateIndex].update;
    if (update)
        self->state = update(self);
}

}
}

// TODO: clean up, give variables real names
namespace jj_8DD4 {
extern "C" {
unsigned long writeTagClear__7MessageFPc(char*);
void getPrintIndexData__7MessageFPvUlPPcPUl(
    void*, unsigned long, char**, unsigned long*);
void* memcpy(void*, const void*, unsigned long);
int sprintf(char*, const char*, ...);
extern char lbl_6_data_6E0[];
void printData__5MuMsgFUlPCvi(void*, unsigned long, char*, unsigned long);
}

struct MessageHolder {
    char pad[4];
    void* data;
};

struct Viewer {
    char pad[0x74];
    MessageHolder* message;
};

extern "C" void dispDataPoint__20muCollectXchgWndTaskFv(
    Viewer* self, unsigned long a, unsigned long b)
{
    char buffer[256];
    char number[24];
    char* data;
    unsigned long size;

    unsigned long pos = writeTagClear__7MessageFPc(buffer);
    getPrintIndexData__7MessageFPvUlPPcPUl(
        self->message->data, 1, &data, &size);
    memcpy(buffer + pos, data, size);
    pos += size;

    if (a > 99)
        a = 99;

    int len = sprintf(number, lbl_6_data_6E0, a);
    memcpy(buffer + pos, number, len);
    pos += len;

    getPrintIndexData__7MessageFPvUlPPcPUl(
        self->message->data, 2, &data, &size);
    memcpy(buffer + pos, data, size);
    pos += size;

    if (b > 99)
        b = 99;

    len = sprintf(number, lbl_6_data_6E0, b);
    memcpy(buffer + pos, number, len);
    pos += len;

    getPrintIndexData__7MessageFPvUlPPcPUl(
        self->message->data, 3, &data, &size);
    memcpy(buffer + pos, data, size);
    pos += size;

    buffer[pos] = 0;
    printData__5MuMsgFUlPCvi(self->message, 1, buffer, pos);
}
}

// TODO: clean up, give variables real names
namespace jj_93B0 {

extern "C" {
extern gfPadSystem* g_gfPadSystem;
extern sndSystem* g_sndSystem;
extern const float lbl_6_rodata_5D8;
extern const float lbl_6_rodata_690;

int getSysPadStatus__11gfPadSystemFiP11gfPadStatus(gfPadSystem*, int, gfPadStatus*);
void setFrameNode__8MuObjectFf(void*, float);
void setFrameVisible__8MuObjectFf(void*, float);
int playSE__9sndSystemF5SndIDiiii(sndSystem*, int, int, int, int, int);

struct Menu {
    char pad0[0x64];
    void* object;                        /* 0x64 */
    char pad68[0x8c - 0x68];
    u32 buttons;                         /* 0x8c */
    char pad90[0x72c - 0x90];
    int count;                           /* 0x72c */
    char pad730[0x744 - 0x730];
    int frameBase;                       /* 0x744 */
    int row;                             /* 0x748 */
    int col;                             /* 0x74c */
};

int selectCommand__20muCollectXchgWndTaskFv(Menu* self) {
    gfPadStatus status;
    int row = self->row;
    int col = self->col;
    int action = 0;
    u32 buttons = self->buttons;

    getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, 0xF0, &status);

    if (buttons & 8) {
        if (--col < 0)
            col = 0;
        int c = 0;
        char* p = (char*)self;
        for (int i = 0; i < self->count; i++) {
            if (*(unsigned char*)(p + 0x142) != 0)
                c++;
            p += 0x18;
        }
        if (c <= 0)
            col = 1;
    }
    if (buttons & 4) {
        if (++col >= 2)
            col = 1;
    }
    if (buttons & 1) {
        row = 0;
        if (self->count <= 0)
            row = 1;
    }
    u32 pressed = status.m_buttonsPressedThisFrame.bits;
    if (pressed & 0x100) {
        if (col == 0)
            action = 1;
        if (col == 1)
            action = 2;
    }
    if (pressed & 0x200) {
        action = 2;
    }
    if (row != self->row || col != self->col) {
        int base = self->frameBase;
        void* object = self->object;
        float frame;
        if (row == 0) {
            frame = lbl_6_rodata_5D8 + (float)base;
        } else {
            frame = lbl_6_rodata_690 + (float)col;
        }
        setFrameNode__8MuObjectFf(object, frame);
        setFrameVisible__8MuObjectFf(object, frame);
        self->row = row;
        self->col = col;
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0, -1, 0, 0, -1);
    }
    switch (action) {
    case 1:
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x13, -1, 0, 0, -1);
        break;
    case 2:
        playSE__9sndSystemF5SndIDiiii(g_sndSystem, 2, -1, 0, 0, -1);
        break;
    }
    return action;
}
}
}

// TODO: clean up, give variables real names
namespace jj_9808 {
struct Class_27C_base { char pad[0x3c]; };
class Class_27C : public Class_27C_base {
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
    virtual void slot22();
    virtual void initProc__18muCollectionViewerFv();
    char padding[8];
    int result;
    char padding2[4];
    int state;
};

struct Viewer {
    char pad0[0x124];
    int state;
    char pad1[0x634];
    int value75c;
    int value760;
    int value764;
    int value768;
    unsigned short value76c;
    char pad2[2];
    Class_27C* dialog;
    char pad3[4];
    int value778;
};

extern "C" {
int makeAttachData__20muCollectXchgWndTaskFv(Viewer*, unsigned short*, int*, int*, int);
void dispDataPoint__20muCollectXchgWndTaskFv(Viewer*, unsigned short, unsigned short);
unsigned short getRemainSendPoint__11nteExchangeFv();

int mainStepLoadWifiDataMain__20muCollectXchgWndTaskFv(Viewer* self) {
    Class_27C* dialog = self->dialog;
    if (dialog->state == 5) {
        int result = dialog->result;
        dialog->initProc__18muCollectionViewerFv();
        self->dialog = 0;
        if (result == 1)
            return 14;
        int value = makeAttachData__20muCollectXchgWndTaskFv(self, &self->value76c, &self->value764, &self->value768, self->value75c);
        self->value760 = value;
        if (value == 0) {
            self->value778 = 9;
            return 2;
        }
        dispDataPoint__20muCollectXchgWndTaskFv(self, self->value76c, getRemainSendPoint__11nteExchangeFv());
        if (getRemainSendPoint__11nteExchangeFv() < self->value76c) {
            self->value778 = 12;
            return 2;
        }
        return 4;
    }
    return self->state;
}
}
}

// TODO: clean up, give variables real names
namespace jj_9904 {
typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;

extern "C" {
s32 friend_getName__15nteWiiConnect24FUl(void*, u32);
void* memset(void*, int, u32);
void* memcpy(void*, const void*, u32);

s32 mainStepMakeFriendListMain__20muCollectXchgWndTaskFv(void* self)
{
    u8* item = (u8*)self;
    if (*(u8*)(*(u8**)(item + 0x730) + 0x8C)) {
        u8* dst = item + 0x12C;
        u32 i = 0;
        while (i < *(u32*)(*(u8**)(item + 0x730) + 0x10) && (s32)i < 0x40) {
            memset(dst, 0, 0x16);
            memcpy(dst, (const void*)friend_getName__15nteWiiConnect24FUl(*(void**)(item + 0x730), i), 0x14);
            ++i;
            dst += 0x18;
        }
        *(u32*)(item + 0x72C) = i;
        return 5;
    }
    return *(s32*)(item + 0x124);
}
}
}

// TODO: clean up, give variables real names
namespace jj_9CA8 {

extern "C" {
extern gfPadSystem* g_gfPadSystem;
int selectFriendList__20muCollectXchgWndTaskFv(void*);
int selectCommand__20muCollectXchgWndTaskFv(void*);
int getSysPadStatus__11gfPadSystemFiP11gfPadStatus(gfPadSystem*, int, gfPadStatus*);
}

struct Self {
    char pad0[0x124];
    int f124;
    char pad128[0x748 - 0x128];
    int state;
};

extern "C" int mainStepSelectingMain__20muCollectXchgWndTaskFv(Self* self) {
    gfPadStatus status;
    int r;

    r = getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, 0xF0, &status);

    switch (self->state) {
    case 0:
        r = selectFriendList__20muCollectXchgWndTaskFv(self);
        break;
    case 1:
        r = selectCommand__20muCollectXchgWndTaskFv(self);
        break;
    }

    switch (r) {
    case 1:
        return 10;
    case 2:
        return 8;
    case 3:
        return 9;
    default:
        return self->f124;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_A1B4 {

struct Entry6A0 {
    unsigned char key;
    int value;
};

extern "C" gfTask* fn_800BEEB8(int, void*, int, int, int, int, int, int, int, int, int, int);
extern "C" void dispSubMsg__15muNoticeWndTaskFPCc(gfTask*, const char*);
extern "C" const Entry6A0 lbl_6_rodata_6A0[5];
extern "C" char lbl_6_data_6E4[];

static inline int lookupId(int kind) {
    int id = 0x59;
    for (int i = 0; i < 5; i++) {
        if (kind == lbl_6_rodata_6A0[i].key) {
            id = lbl_6_rodata_6A0[i].value;
            break;
        }
    }
    return id;
}

extern "C" void mainStepErrorInit__20muCollectXchgWndTaskFv(void* p) {
    char* self = (char*)p;
    gfTask* task = *(gfTask**)(self + 0x750);
    if (task) {
        task->exit();
        *(gfTask**)(self + 0x750) = 0;
    }
    *(gfTask**)(self + 0x750) = fn_800BEEB8(0, *(void**)(self + 0x78), lookupId(*(int*)(self + 0x77c)), *(int*)(self + 0x754), *(int*)(self + 0x70), *(int*)(self + 0x758) + 3, 0, 0x2a, 0x2b, 0, 0, 0xf0);
    if (*(int*)(self + 0x780)) {
        unsigned long size;
        char* data;
        char buffer[128];
        Message::getPrintIndexData(*(void**)(self + 0x78), 0x13, &data, &size);
        memcpy(buffer, data, size);
        size += sprintf(buffer + size, lbl_6_data_6E4, *(int*)(self + 0x780));
        dispSubMsg__15muNoticeWndTaskFPCc(*(gfTask**)(self + 0x750), buffer);
    }
}
}

