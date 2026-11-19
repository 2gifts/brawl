#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scnmdlexpand.h>
#include <types.h>

// The screen that shows a Wi-Fi battle somebody else plays (module sora_menu_watch). The class is only described by the
// members the decompiled functions touch.
/// Function names come from the sora_menu_watch map.
// HYPOTHESIS: a pair that the menus keep two of in every unit (an id that is 0xFF while it is not used, and a state); the
// pairs are made when the unit starts.
struct muWatchPair {
    int m_id;
    int m_state;

    muWatchPair(int id, int state) : m_id(id), m_state(state) { }
};

static muWatchPair s_pair0(0xFF, 0);
static muWatchPair s_pair1(0xFF, 1);

class nteWatching {
public:
    void cancel();
};

// HYPOTHESIS: the task of the prize window (its state is read at 0x104).
class muWatchSubTask : public gfTask {
public:
    u8 m_unk40[0x104 - 0x40];
    int m_state; // 0x104
};

// HYPOTHESIS: the task of the prize window (its state is read at 0x10C, the flag at 0x3D0 lets it close).
class muWatchPrizeTask : public gfTask {
public:
    u8 m_unk40[0x10C - 0x40];
    int m_state; // 0x10C
    u8 m_unk110[0x3D0 - 0x110];
    u8 m_close; // 0x3D0
};

// HYPOTHESIS: the data of one player of the match that is watched (the sort functions read the team and the area).
struct muWifiWatchPlayerInfo {
    u8 m_unk0[0xC];
    int m_team;     // 0x0C
    u8 m_area;      // 0x10
    u8 m_unk11[3];
};

class muWifiWatchTask : public gfTask {
public:
    // One of the textures of the characters (the ctor clears the first members, the dtor frees the file handle).
    struct CharTex {
        int m_unk0;
        int m_unk4;
        gfFileIOHandle m_file; // 0x8

        CharTex();
        ~CharTex();
    };

    // The data that initStepNextBattle copies over the state of the current battle.
    struct BattleData {
        int m_unk0;
        int m_data[0x33];
    };

    u8 m_unk40[0x48 - 0x40];
    int m_nextStep;                     // 0x48 HYPOTHESIS: the step the main step jumps to
    MuObject* m_obj4C;                  // 0x4C
    u8 m_unk50[0x8C - 0x50];
    MuObject* m_obj8C;                  // 0x8C
    u8 m_unk90[0x108 - 0x90];
    MuObject* m_obj108;                 // 0x108
    u8 m_unk10C[0x18C - 0x10C];
    MuObject* m_obj18C;                 // 0x18C
    u8 m_unk190[0x194 - 0x190];
    nw4r::g3d::ScnMdlExpand* m_group194; // 0x194
    u8 m_unk198[0x1A0 - 0x198];
    nw4r::g3d::ScnMdlExpand* m_group1A0; // 0x1A0
    u8 m_unk1A4[0x4EC - 0x1A4];
    int m_unk4EC;                       // 0x4EC
    int m_unk4F0;                       // 0x4F0
    u8 m_unk4F4[0x504 - 0x4F4];
    int m_stepResult;                   // 0x504 HYPOTHESIS: what the main steps return while they wait
    u8 m_unk508[0x518 - 0x508];
    int m_unk518;                       // 0x518
    u8 m_unk51C[0x534 - 0x51C];
    muWatchPrizeTask* m_prizeTask;      // 0x534
    u8 m_unk538[0x540 - 0x538];
    muWatchSubTask* m_subTask;          // 0x540
    u8 m_unk544[0x548 - 0x544];
    BattleData m_battle;                // 0x548
    BattleData m_nextBattle;            // 0x618
    u8 m_unk6E8[0xD54 - 0x6E8];
    int m_unkD54;                       // 0xD54
    u8 m_unkD58;                        // 0xD58
    u8 m_downloaded;                    // 0xD59
    u8 m_unkD5A[0xD5C - 0xD5A];
    nteWatching* m_watching;            // 0xD5C
    u8 m_unkD60[0xD78 - 0xD60];

    int mainStepNetError();
    void initStepDownloadCancel();
    int mainStepDownloadCancel();
    void initStepNextBattle();
    void initStepPost();
    muWifiWatchTask();
    static muWifiWatchTask* create(void* param);
    void initProc(void* param);
    static int playerTeamSortCompare(const muWifiWatchPlayerInfo* a, const muWifiWatchPlayerInfo* b);
    static int playerInfoAreaSortCompare(const muWifiWatchPlayerInfo* a, const muWifiWatchPlayerInfo* b);
    void fn_4078();
    void initStepDownloadWait();
    int mainStepGetBonus();
    int mainStepPrizeWindow();
};

muWifiWatchTask::CharTex::CharTex() : m_unk4(0) { }

muWifiWatchTask::CharTex::~CharTex() { }

void muWifiWatchTask::initStepPost() { }

void muWifiWatchTask::initStepNextBattle() {
    m_unkD54 = 0x3C;
    m_battle = m_nextBattle;
}

void muWifiWatchTask::initStepDownloadCancel() {
    if (!m_downloaded) {
        m_watching->cancel();
    }
    m_unk4EC = 6;
}

int muWifiWatchTask::mainStepDownloadCancel() {
    if (m_unk4EC != 6) {
        m_nextStep = 2;
        return 0xB;
    }
    return m_stepResult;
}

int muWifiWatchTask::mainStepNetError() {
    if (m_subTask->m_state == 3) {
        m_subTask->exit();
        m_subTask = NULL;
        if (m_unk4F0 != 0) {
            m_nextStep = 3;
        } else {
            m_nextStep = 4;
        }
        return 0xB;
    }
    return m_stepResult;
}

void muWifiWatchTask::fn_4078() {
    m_obj8C->changeAnimN(1, 0x13, true, false);
}

void muWifiWatchTask::initStepDownloadWait() {
    m_obj4C->changeAnimN(1, 1, true, false);
    m_unk518 = 0x1E;
    nw4r::g3d::ScnMdlExpand* group = m_group194;
    MuObject* obj = m_obj18C;
    group->Remove(obj->m_sceneModel);
    group->PushBack(obj->m_sceneModel, "pos00");
}

int muWifiWatchTask::mainStepGetBonus() {
    if (m_obj108->isNodeAnimFinished()) {
        m_group1A0->Remove(m_obj108->m_sceneModel);
        int step;
        if (m_unkD58) {
            if (m_battle.m_unk0 != 0) {
                step = 4;
            } else {
                step = 6;
            }
        } else {
            step = 6;
        }
        return step;
    }
    return m_stepResult;
}

int muWifiWatchTask::mainStepPrizeWindow() {
    if (m_unkD54 > 0) {
        m_unkD54--;
    }
    if (m_prizeTask != NULL) {
        if (m_prizeTask->m_state == 3 && m_unkD54 == 0) {
            m_prizeTask->m_close = 1;
            m_unkD54 = -1;
        }
        if (m_prizeTask->m_state == 5 || m_unkD54 == 0) {
            m_prizeTask->exit();
            m_prizeTask = NULL;
            m_group1A0->Remove(m_obj8C->m_sceneModel);
            return 6;
        }
    }
    return m_stepResult;
}

muWifiWatchTask* muWifiWatchTask::create(void* param) {
    muWifiWatchTask* task = new (Heaps::MenuInstance) muWifiWatchTask();
    task->initProc(param);
    return task;
}

int muWifiWatchTask::playerTeamSortCompare(const muWifiWatchPlayerInfo* a, const muWifiWatchPlayerInfo* b) {
    return a->m_team - b->m_team;
}

int muWifiWatchTask::playerInfoAreaSortCompare(const muWifiWatchPlayerInfo* a, const muWifiWatchPlayerInfo* b) {
    return a->m_area - b->m_area;
}

#include <mt/mt_vector.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <stdio.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_pad_status.h>
#include <snd/snd_system.h>
#include <string.h>
#include <ms/ms_message.h>
// TODO: clean up, give variables real names
namespace jj_B1C {
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

extern "C" void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
extern "C" void* __ct__21tySealListDataManagerFb(void*, int);
extern "C" void setData__21tySealListDataManagerFPUcUl(void*, u32, u32);

struct WatchEntry {
    int data[3];
    int id;
    u8 kind;
    u8 padding[3];
};

struct WatchEntries {
    WatchEntry entries[4];
};

struct WatchView {
    u8 padding[0x440];
    WatchEntry entries[4];
};

struct WatchText {
    u16 text[100];
};

extern "C" void fn_15_B1C(unsigned char* self, unsigned char* source) {
    *reinterpret_cast<int*>(self + 0x434) =
        *reinterpret_cast<int*>(source + 0xF4);
    *reinterpret_cast<int*>(self + 0x438) =
        *reinterpret_cast<int*>(source + 0xF8);
    self[0x43C] = source[0xFC];

    *reinterpret_cast<WatchEntries*>(self + 0x440) =
        *reinterpret_cast<WatchEntries*>(source + 0x100);

    if (*reinterpret_cast<int*>(self + 0x434) == 0) {
        int i;
        for (i = 0; i < self[0x43C]; ++i) {
            if (source[0x221] ==
                reinterpret_cast<WatchView*>(self)->entries[i].kind) {
                break;
            }
        }
        *reinterpret_cast<int*>(self + 0x514) = i + 1;
    } else {
        int i;
        for (i = 0; i < self[0x43C]; ++i) {
            if (*reinterpret_cast<int*>(source + 0x224) ==
                reinterpret_cast<WatchView*>(self)->entries[i].id) {
                break;
            }
        }
        *reinterpret_cast<int*>(self + 0x514) = i / 2 + 5;
    }

    *reinterpret_cast<int*>(self + 0x520) =
        *reinterpret_cast<int*>(source + 0x228) / 10;

    *reinterpret_cast<WatchText*>(self + 0x6E8) =
        *reinterpret_cast<WatchText*>(source + 0x234);
    *reinterpret_cast<u16*>(self + 0x7B0) =
        *reinterpret_cast<u16*>(source + 0x2FC);

    self[0xD58] = source[0x220];

    void* object = __nw__FUlQ25Heaps8HeapType(0x3EB4, 0x2A);
    if (object != 0) {
        object = __ct__21tySealListDataManagerFb(object, 0);
    }
    *reinterpret_cast<void**>(self + 0x978) = object;

    u32 first = *reinterpret_cast<u32*>(source + 0x304);
    u32 second = *reinterpret_cast<u32*>(source + 0x308);
    setData__21tySealListDataManagerFPUcUl(object, first, second);
}
}

// TODO: clean up, give variables real names
namespace jj_CA0 {
typedef unsigned short u16;
typedef unsigned char u8;

struct Entry {
    const char* name;
    u16 begin;
    u16 end;
    u8 kind;
    u8 pad;
};

struct Owner {
    char pad[0x4c];
    void* objs[1];
};

extern "C" {
extern char lbl_15_data_14C[];
extern const float lbl_15_rodata_0;

void* create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(void*, const char*, int, void*, int);
void changeAnimN__8MuObjectFPCc(void*, const char*);
void setUpdateRate__16gfModelAnimationFf(void*, float);
char* strcpy(char*, const char*);
char* strcat(char*, const char*);
}

extern "C" void fn_15_CA0(void* self, Entry* entries, int count, void* resFile, int kindOffset) {
    char buf[64];
    int n;
    int i;
    int j;
    Entry* entry = entries;
    void* obj;

    for (i = 0; i < count; i++, entry++) {
        n = 1;
        if (entry->begin < entry->end) {
            n = entry->end - entry->begin;
        }
        for (j = 0; j < n; j++) {
            obj = create__8MuObjectFPQ34nw4r3g3d7ResFilePCciPQ34nw4r3g3d7ResFileQ25Heaps8HeapType(
                resFile, entry->name, entry->kind + kindOffset, 0, 0x2a);
            const char* name = entry->name;
            ((Owner*)self)->objs[entry->begin + j] = obj;
            strcpy(buf, name);
            strcat(buf, lbl_15_data_14C);
            changeAnimN__8MuObjectFPCc(obj, buf);
            if (*(void**)((char*)obj + 0x14) != 0) {
                setUpdateRate__16gfModelAnimationFf(*(void**)((char*)obj + 0x14), lbl_15_rodata_0);
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_D98 {
typedef unsigned char u8;
typedef unsigned int u32;
extern "C" {
void* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32, u32, u32);
void allocMsgBuf__5MuMsgFUlUl(void*, u32, u32);
void setMsgData__5MuMsgFPv(void*, void*);
void attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(void*, u32, void*, u32, float);
void setFontColor__5MuMsgFUlUcUcUcUc(void*, u32, u8, u8, u8, u8);
void setAlignMode__5MuMsgFUlQ25MuMsg9AlignMode(void*, u32, u32);
void setFontType__5MuMsgFUlUl(void*, u32, u32);
extern const float lbl_15_rodata_D0[];
}
struct Obj { char pad[0x10]; void* mdl; };
struct Self {
    char pad0[0x4c]; Obj* a;
    char pad1[0x64 - 0x50]; Obj* b[6];
    Obj* c;
    char pad2[0x24c - 0x80]; void* msg;
};

extern "C" void initMsg__15muWifiWatchTaskFv(Self* self, void* data) {
    Obj* o;
    u32 id;
    int i;
    self->msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(3, 0x2a, 0x2b);
    void* msg = self->msg;
    allocMsgBuf__5MuMsgFUlUl(msg, 0x100, 8);
    setMsgData__5MuMsgFPv(msg, data);
    o = self->a;
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 0, o->mdl, 0, lbl_15_rodata_D0[0]);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 1, o->mdl, 1, lbl_15_rodata_D0[0]);
    setFontColor__5MuMsgFUlUcUcUcUc(msg, 1, 0xff, 0xff, 0xff, 0xff);
    setAlignMode__5MuMsgFUlQ25MuMsg9AlignMode(msg, 1, 1);
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 2, o->mdl, 2, lbl_15_rodata_D0[0]);
    for (i = 0; i < 4; i++) {
        id = i + 3;
        attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, id, self->b[i]->mdl, 0, lbl_15_rodata_D0[0]);
        setFontColor__5MuMsgFUlUcUcUcUc(msg, id, 0xff, 0xff, 0xff, 0xff);
        setFontType__5MuMsgFUlUl(msg, id, 1);
    }
    attachScnMdlSimple__5MuMsgFUlPQ34nw4r3g3d12ScnMdlSimpleUlf(msg, 7, self->c->mdl, 0, lbl_15_rodata_D0[0]);
    setFontColor__5MuMsgFUlUcUcUcUc(msg, 7, 0, 0, 0, 0xff);
}
}

// TODO: clean up, give variables real names
namespace jj_F18 {
struct WatchNode {
    s32 index;
    u8 pad0[0x14];
    WatchNode* next;
    WatchNode* prev;
};

struct WatchEntry {
    u8 pad0[0x10];
    void* obj;
};

struct WatchSub {
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
    virtual void vf11(void* a, void* b) = 0;
    virtual void vf12() = 0;
    virtual void vf13(void* a) = 0;
    u8 pad0[0xE0];
    void* fieldE4;
};

struct WatchOwner {
    u8 pad0[0x4C];
    WatchEntry* entries[85];
    WatchSub* sub;
    u8 pad1[0xD3C - 0x1A4];
    WatchNode* listA;
    WatchNode* listB;
    s32 count;
};

extern "C" {
s32 dropCoinMainOne__15muWifiWatchTaskFv(WatchOwner* self, WatchNode* node);
void dropCoinStartOne__15muWifiWatchTaskFv(WatchOwner* self, WatchNode* node);
s32 randi__Fl(s32);
}

extern "C" void dropCoinMain__15muWifiWatchTaskFv(WatchOwner* self) {
    WatchNode* sel = 0;
    WatchNode* node = self->listB;
    while (node != 0) {
        WatchNode* next = node->next;
        if (dropCoinMainOne__15muWifiWatchTaskFv(self, node) == 0) {
            WatchNode* n = node->next;
            WatchNode* prev = node->prev;
            if (prev != 0) {
                prev->next = n;
            } else {
                self->listB = n;
            }
            if (n != 0) {
                n->prev = prev;
            }
            if (self->listA != 0) {
                self->listA->prev = node;
            }
            node->prev = 0;
            node->next = self->listA;
            self->listA = node;
            self->sub->vf13(self->entries[node->index]->obj);
        }
        node = next;
    }
    if (self->count > 0) {
        if (randi__Fl(1) == 0) {
            sel = self->listA;
            if (sel != 0) {
                WatchNode* n = sel->next;
                WatchNode* prev = sel->prev;
                if (prev != 0) {
                    prev->next = n;
                } else {
                    self->listA = n;
                }
                if (n != 0) {
                    n->prev = prev;
                }
                if (self->listB != 0) {
                    self->listB->prev = sel;
                }
                sel->prev = 0;
                sel->next = self->listB;
                self->listB = sel;
                dropCoinStartOne__15muWifiWatchTaskFv(self, sel);
                self->sub->vf11(self->sub->fieldE4, self->entries[sel->index]->obj);
                self->count = self->count - 1;
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_10B0 {

#pragma fp_contract off
#pragma opt_propagation off

extern const float lbl_15_rodata_0[];
extern "C" void setTrans__8MuObjectFP5Vec3f(void*, const Vec3f*);
extern "C" void setObjRotate__8MuObjectFP5Vec3f(void*, const Vec3f*);

struct WatchState {
    int index;
    float speed;
    float time;
    float x;
    float y;
    float z;
};

extern "C" void dropCoinStartOne__15muWifiWatchTaskFv(void* self, WatchState* state) {
    const float* c = lbl_15_rodata_0;
    void* object = ((void**)((char*)self + 0x4C))[state->index];
    Vec3f pos;
    pos.m_x = c[0xD4 / 4] + c[0xD8 / 4] * randf();
    pos.m_y = c[0xDC / 4];
    pos.m_z = c[0xE0 / 4];
    setTrans__8MuObjectFP5Vec3f(object, &pos);
    state->time = c[0];
    state->speed = c[0xE4 / 4] + randf();
    Vec3f rot;
    rot.m_x = c[0xE8 / 4] * (c[0xEC / 4] * randf()) - c[0xEC / 4];
    rot.m_y = c[0xE8 / 4] * (c[0xEC / 4] * randf()) - c[0xEC / 4];
    rot.m_z = c[0xE8 / 4] * (c[0xEC / 4] * randf()) - c[0xEC / 4];
    setObjRotate__8MuObjectFP5Vec3f(object, &rot);
    state->x = c[0xF0 / 4] + c[0xF4 / 4] * randf();
    state->y = c[0xF0 / 4] + c[0xF4 / 4] * randf();
    state->z = c[0xF0 / 4] + c[0xF4 / 4] * randf();
}
}

// TODO: clean up, give variables real names
namespace jj_11F4 {

#pragma fp_contract off
#pragma opt_propagation off

extern const float lbl_15_rodata_0[];
extern "C" void setTrans__8MuObjectFP5Vec3f(void*, const Vec3f*);
extern "C" void setObjRotate__8MuObjectFP5Vec3f(void*, const Vec3f*);
extern "C" void stopSE__9sndSystemFll(void*, unsigned int, int);
extern void* g_sndSystem;

struct WatchState {
    int index;
    float speed;
    float time;
    float x;
    float y;
    float z;
};

extern "C" int dropCoinMainOne__15muWifiWatchTaskFv(void* self, WatchState* state) {
    const float* c = lbl_15_rodata_0;
    void* object = ((void**)((char*)self + 0x4C))[state->index];
    state->time -= c[0xF8 / 4];
    Vec3f pos;
    pos = *(Vec3f*)((char*)object + 0x3C);
    pos.m_x += state->speed;
    pos.m_y += state->time;
    setTrans__8MuObjectFP5Vec3f(object, &pos);
    Vec3f rot;
    rot = *(Vec3f*)((char*)object + 0x48);
    rot.m_x += state->x;
    while (rot.m_x > c[0xEC / 4]) rot.m_x -= c[0xFC / 4];
    while (rot.m_x < c[0x100 / 4]) rot.m_x += c[0xFC / 4];
    rot.m_y += state->y;
    while (rot.m_y > c[0xEC / 4]) rot.m_y -= c[0xFC / 4];
    while (rot.m_y < c[0x100 / 4]) rot.m_y += c[0xFC / 4];
    rot.m_z += state->z;
    while (rot.m_z > c[0xEC / 4]) rot.m_z -= c[0xFC / 4];
    while (rot.m_z < c[0x100 / 4]) rot.m_z += c[0xFC / 4];
    setObjRotate__8MuObjectFP5Vec3f(object, &rot);
    if (*(int*)((char*)self + 0xD44) <= 0 && pos.m_y < c[0x104 / 4]) {
        unsigned int sound = *(unsigned int*)((char*)self + 0xD6C);
        if (sound != -1U) {
            stopSE__9sndSystemFll(g_sndSystem, sound, 0);
            *(unsigned int*)((char*)self + 0xD6C) = -1U;
        }
    }
    return !(pos.m_y < c[0x108 / 4]);
}
}

// TODO: clean up, give variables real names
namespace jj_1B48 {
struct Target {
    char pad0[0x348];
    unsigned char ready;
    char pad349[3];
    int result;
    char pad350[0x34];
    unsigned short time;
    char pad386[6];
    int value;
};
struct Object {
    char pad0[0x490];
    char area[0x5c];
    int state;
    int status;
    int value;
    int timer;
    int timer2;
    char pad500[0x2c];
    int duration;
    char pad530[0x829];
    unsigned char flag;
    char padD5A[2];
    Target* target;
};
extern "C" void download__11nteWatchingFv(Target*);
extern "C" void updateNextReplayDataProperty__15muWifiWatchTaskFv(Object*);
extern "C" void preLoadFightersByReplayDataProperty__15muWifiWatchTaskFv(Object*, void*);
extern "C" int fn_800CBCB0(void*, int*);
extern "C" void OSReport(const char*, ...);
extern void* g_muWifiInterfaceTask;
extern int s_savingCount__16ntFriendInfoSave;
extern char lbl_15_data_1D0[];
extern char lbl_15_data_1F8[];
extern "C" void downloadMain__15muWifiWatchTaskFv(Object* self) {
    if (self->state == 6) {
        unsigned char ready;
        if (self->flag) ready = 1;
        else ready = self->target->ready;
        if (ready) self->state = 0;
    } else if (self->state == 1) {
        if (--self->timer <= 0) {
            self->state = 2;
            if (self->flag) self->timer2 = 180;
            else download__11nteWatchingFv(self->target);
        }
    } else if (self->state == 2) {
        int ready;
        if (self->flag) {
            if (--self->timer2 <= 0) ready = 1;
            else goto not_ready;
        } else if (self->target->ready) ready = 1;
        else {
        not_ready:
            ready = 0;
        }
        if (ready) {
            int next;
            if (self->flag) next = 3;
            else {
                switch (self->target->result) {
                case 0: next = 3; break;
                case 2: next = 5; break;
                case 3: next = 4; break;
                default: next = 4; break;
                }
            }
            switch (next) {
            case 3:
                updateNextReplayDataProperty__15muWifiWatchTaskFv(self);
                int duration;
                if (self->flag) duration = 900;
                else duration = self->target->time * 60;
                self->duration = duration;
                preLoadFightersByReplayDataProperty__15muWifiWatchTaskFv(self, self->area);
                break;
            case 5:
                break;
            case 4:
                self->status = 0;
                self->value = self->target->value;
                break;
            }
            self->state = next;
            return;
        }
        goto check5;
    } else {
    check5:
      if (self->state == 5) {
        if (s_savingCount__16ntFriendInfoSave == 0) {
            OSReport(lbl_15_data_1D0);
            self->status = fn_800CBCB0(g_muWifiInterfaceTask, &self->value);
            if (self->status) self->state = 4;
        } else OSReport(lbl_15_data_1F8);
      }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_1D7C {
struct Entry {
    unsigned char a;
    int kind;
    unsigned char col;
    unsigned char pad[0xb];
};

extern "C" int exchangeMuSelchkind2GmCharacterKind__6muMenuFiii(int);
extern "C" int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
extern "C" int getNumCharColor__6muMenuFiii(int);
extern "C" int getFighterColorFileNo__6muMenuFiii(int, int);
extern "C" void fn_1_379C(int, int, int);

inline int colorCount(int kind) {
    int m = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
    return m == 0x28 ? 1 : getNumCharColor__6muMenuFiii(m);
}

inline int colorFile(int kind, int col) {
    int m = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
    return m == 0x28 ? 0 : getFighterColorFileNo__6muMenuFiii(m, col);
}

extern "C" void preLoadFightersByReplayDataProperty__15muWifiWatchTaskFv(unsigned char* self, unsigned char* p) {
    if (self[0xd59] == 0) {
        Entry* e = (Entry*)(p + 0xc);
        int kind;
        int col;
        int i;
        for (i = 0; i < p[8]; i++, e++) {
            kind = exchangeMuSelchkind2GmCharacterKind__6muMenuFiii(e->kind);
            if (kind != 0x3e) {
                col = e->col;
                int c;
                if (col < colorCount(kind)) {
                    c = colorFile(kind, col);
                } else {
                    c = 0;
                }
                fn_1_379C(e->a, (unsigned char)kind, (unsigned char)c);
            }
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_1E4C {
extern "C" {
int exchangeSrStageKindToMuStageKind__6muMenuFiii(unsigned int);
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(unsigned int);
int playerTeamSortCompare__15muWifiWatchTaskFPC21muWifiWatchPlayerInfoPC21muWifiWatchPlayerInfo(void*, void*);
int playerInfoAreaSortCompare__15muWifiWatchTaskFPC21muWifiWatchPlayerInfoPC21muWifiWatchPlayerInfo(void*, void*);
void qsort(void*, unsigned int, unsigned int, int (*)(void*, void*));
}

extern "C" void updateNextReplayDataProperty__15muWifiWatchTaskFv(void* self) {
    unsigned char* object = (unsigned char*)self;
    if (object[0xD59] == 0) {
        unsigned char* menuData = (unsigned char*)*(void**)(object + 0xD5C);
        unsigned char* stageData = menuData + 0x35C;
        if (menuData[0x386] == 1)
            *(unsigned int*)(object + 0x490) = 1;
        else
            *(unsigned int*)(object + 0x490) = 0;
        *(unsigned int*)(object + 0x494) =
            exchangeSrStageKindToMuStageKind__6muMenuFiii(
                *(unsigned short*)(stageData + 2));

        unsigned char* entry;
        unsigned char* output;
        int i;
        int count;
        output = object + 0x49C;
        count = 0;
        i = 0;
        while (i < 4) {
            entry = stageData + i;
            if (entry[0x19] != 3) {
                output[0] = (unsigned char)i;
                *(unsigned int*)(output + 4) =
                    exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(entry[4]);
                ++count;
                output[8] = entry[0x0B];
                *(unsigned int*)(output + 0x0C) = entry[0x12];
                output[0x10] = entry[0x20];
                output += 0x14;
            }
            ++i;
        }

        object[0x498] = (unsigned char)count;
        if (*(int*)(object + 0x490) == 1)
            qsort(object + 0x49C, count, 0x14, playerTeamSortCompare__15muWifiWatchTaskFPC21muWifiWatchPlayerInfoPC21muWifiWatchPlayerInfo);
        else
            qsort(object + 0x49C, count, 0x14, playerInfoAreaSortCompare__15muWifiWatchTaskFPC21muWifiWatchPlayerInfoPC21muWifiWatchPlayerInfo);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_26E8 {
#pragma opt_propagation off
extern "C" {
struct Data { char a[12]; char b[0x274]; char c[8]; char d[4]; char e[4]; };
extern Data lbl_15_data_0;
extern void* lbl_805A01D8;
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
void changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);
void dispAllPlayerPanel__15muWifiWatchTaskFv(void*, int);
void* GetResMat__Q34nw4r3g3d6ResMdlCFPCc(void*, const char*);
void changeMaterialTex__8MuObjectFUlPCcPQ34nw4r3g3d7ResFile(void*, unsigned int, const char*, int);
void printf__5MuMsgFUlPCce(void*, unsigned long, const char*, ...);
void playBGM__16sndBgmRateSystemFiUli(void*, int, int, int);
}
struct Obj {
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
virtual void call(void*);
};
extern "C" void initStepAppear__15muWifiWatchTaskFv(char* self) {
Data* data=&lbl_15_data_0;
Obj* obj;
char* resource;
char* mdl;
char* name;
int mode;
if (*(int*)(self+0x44)==0) {
mode=0;
obj=*(Obj**)(self+0x194);
resource=*(char**)(self+0x18c);
obj->call(*(void**)(resource+0x10));
Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(obj, *(void**)(resource+0x10), data->b);
} else mode=3;
changeAnimN__8MuObjectFiUlbb(*(void**)(self+0x4c),mode,1,1,0);
dispAllPlayerPanel__15muWifiWatchTaskFv(self,0);
mdl=*(char**)(self+0x4c);
name=data->e;
void* mat=GetResMat__Q34nw4r3g3d6ResMdlCFPCc(mdl+8,data->c);
changeMaterialTex__8MuObjectFUlPCcPQ34nw4r3g3d7ResFile(mdl,*(unsigned int*)((char*)mat+0xc),name,0);
printf__5MuMsgFUlPCce(*(void**)(self+0x24c),1,data->d);
*(int*)(self+0x514)=0;
playBGM__16sndBgmRateSystemFiUli(lbl_805A01D8,0x2a,0,0);
}
}

// TODO: clean up, give variables real names
namespace jj_27EC {
extern "C" int isNodeAnimFinished__8MuObjectFv(void*);
extern "C" void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
extern const float lbl_15_rodata_0[];
extern const char lbl_15_data_150[];
struct MenuWatch {
 char pad0[0x4c]; void* m4c;
 char pad1[0x74 - 0x50]; MuObject* m74;
 char pad2[0x194 - 0x78]; nw4r::g3d::ScnGroup* m194;
 char pad3[0x4ec - 0x198]; int m4ec;
 char pad4[0x504 - 0x4f0]; int m504;
 char pad5[0x514 - 0x508]; int m514;
};
extern "C" int mainStepAppear__15muWifiWatchTaskFv(MenuWatch* self) {
 const float* c = lbl_15_rodata_0;
 int idx;
 MuObject* obj;
 nw4r::g3d::ScnGroup* mdl;
 int ret = self->m504;
 if (isNodeAnimFinished__8MuObjectFv(self->m4c)) {
  switch (self->m4ec) {
  case 0: break;
  case 1: case 2: case 5: ret = 1; break;
  case 4: ret = 9; break;
  case 3: ret = 2; break;
  }
  idx = self->m514;
  obj = self->m74;
  mdl = self->m194;
  float f;
  switch (idx) {
  case 0: f = c[0]; break;
  case 1: case 2: case 3: case 4: f = c[1]; break;
  case 5: case 6: f = c[0x3a]; break;
  }
  obj->setFrameVisible(f);
  mdl->Remove(obj->m_sceneModel);
  char buf[16];
  sprintf(buf, lbl_15_data_150, idx);
  Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(mdl, obj->m_sceneModel, buf);
 }
 return ret;
}
}

// TODO: clean up, give variables real names
namespace jj_2990 {

extern "C" void stopSE__9sndSystemFll(void*, u32, int);

struct Vec2 {
    float x, y;
    Vec2() {}
    Vec2(const Vec2& o) { x = o.x; y = o.y; }
};

struct PadStatus {
    int w[6];
    Vec2 v[3];
    unsigned char b0, b1, b2, b3, b4, b5, b6, b7, b8;
    int last;
};

struct Raw {
    int w[16];
};

inline Raw getRaw() {
    Raw s;
    g_gfPadSystem->getSysPadStatus(0xF0, (gfPadStatus*)&s);
    return s;
}

extern "C" int mainStepDownloadWait__15muWifiWatchTaskFv(char* self) {
    int ret = *(int*)(self + 0x504);
    int state = *(int*)(self + 0x4ec);

    if (state == 3)
        goto state3;
    if (state >= 3)
        goto state_ge3;
    if (state >= 2)
        goto state2;
    goto state_done;

state_ge3:
    if (state == 5)
        goto state_done;
    if (state >= 5)
        goto state_done;
    ret = 9;
    goto state_done;

state3:
    if (*(u32*)(self + 0xd68) + 0x10000 != 0xffff) {
        stopSE__9sndSystemFll(g_sndSystem, *(u32*)(self + 0xd68), 0);
        *(int*)(self + 0xd68) = -1;
    }
    g_sndSystem->playSE((SndID)0x1EF3, -1, 0, 0, -1);
    ret = 2;
    goto state_done;

state2:
    if (*(u32*)(self + 0xd68) + 0x10000 == 0xffff) {
        *(int*)(self + 0xd68) = g_sndSystem->playSE((SndID)0x1EF2, -1, 0, 0, -1);
    }

state_done:
    int t = *(int*)(self + 0x518) - 1;
    *(int*)(self + 0x518) = t;
    if (t < 0) {
        PadStatus b(*(PadStatus*)&getRaw());
        if ((b.w[3] & 0x200) || (b.w[3] & 0x100) || (b.w[3] & 0x1000)) {
            g_sndSystem->playSE((SndID)2, -1, 0, 0, -1);
            if (state == 2) {
                return 10;
            }
            *(int*)(self + 0x48) = 2;
            return 11;
        }
    }
    return ret;
}
}

// TODO: clean up, give variables real names
namespace jj_4090 {
class Base {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual float getFrame();
};
struct Inner { char pad[0xc]; Base* base; };
struct Obj { char pad[0x14]; Inner* inner; };
struct Self { char pad[0x8c]; Obj* obj; char pad2[0x504-0x90]; int result; };
extern "C" int getNodeAnimLength__8MuObjectFv(Obj*);
extern "C" float lbl_15_rodata_1C8;
extern "C" int initStepPrizeWindow__15muWifiWatchTaskFv(Self* self) {
    Obj* obj = self->obj;
    if (lbl_15_rodata_1C8 == obj->inner->base->getFrame()) {
        g_sndSystem->playSE((SndID)0x1efb, -1, 0, 0, -1);
    }
    float frame = obj->inner->base->getFrame();
    if (frame >= (float)(getNodeAnimLength__8MuObjectFv(obj) - 15)) {
        return 5;
    }
    return self->result;
}
}

// TODO: clean up, give variables real names
namespace jj_464C {

struct Obj {
    char pad0[0x78];
    MuObject* object;
    void* other;
    char pad1[0x504 - 0x80];
    int state;
    char pad2[0x6e8 - 0x508];
    unsigned short values[100];
    unsigned short length;
    unsigned short ring[200];
    char pad3[2];
    int write;
    int read;
    int count;
    char pad4[0xd48 - 0x950];
    int timer;
    char pad5[0xd58 - 0xd4c];
    unsigned char flag;
};
extern "C" void dispNumBetCoin__15muWifiWatchTaskFv(Obj*, int);
extern "C" int isNodeAnimFinished__8MuObjectFv(void*);
inline void push(Obj* self, unsigned short value) {
    if (self->count < 200) {
        self->ring[self->write] = value;
        if (++self->write >= 200) self->write = 0;
        ++self->count;
    }
}

extern "C" int mainStepMultiplyBetCoin__15muWifiWatchTaskFv(Obj* self) {
    if (!self->flag && self->timer > 0) {
        dispNumBetCoin__15muWifiWatchTaskFv(self, --self->timer);
        int frame = self->timer;
        self->object->setFrameVisible((float)frame);
        self->object->setFrameNode((float)frame);
        if (self->timer % 10 == 0) {
            g_sndSystem->playSE((SndID)0x1ef9, -1, 0, 0, -1);
        }
    }
    if (isNodeAnimFinished__8MuObjectFv(self->other)) {
        if (!self->flag) {
            if (self->timer <= 0) {
                for (int i = 0; i < self->length; ++i) {
                    push(self, self->values[i]);
                }
                return 8;
            }
        } else {
            return 7;
        }
    }
    return self->state;
}
}

// TODO: clean up, give variables real names
namespace jj_47E8 {
struct WatchMenu {
    u8 pad0[0x50C];
    s32 unk50C;
    u8 pad510[4];
    s32 unk514;
    u8 pad518[8];
    s32 unk520;
    u8 pad524[0x6E8 - 0x524];
    u16 src6E8[0x64];
    u16 count7B0;
    u16 buf7B2[0xC8];
    s32 writeIdx944;
    u8 pad948[4];
    s32 count94C;
    u8 pad950[0xD44 - 0x950];
    s32 unkD44;
    s32 unkD48;
    s32 unkD4C;
    f32 unkD50;
    u8 padD54[4];
    u8 unkD58;
    u8 padD59[0xD6C - 0xD59];
    s32 unkD6C;
    s32 unkD70;
};

extern "C" {

s32 playSE__9sndSystemF5SndIDiiii(void* arg, s32 id, s32 a, s32 b, s32 c, s32 d);
extern void* g_sndSystem;
extern f32 lbl_15_rodata_210;
extern f32 lbl_15_rodata_4;

void initStepCoinInc__15muWifiWatchTaskFv(WatchMenu* self) {
    u16 v;
    u16* p;
    int i;
    s32 val;

    if (self->unkD58) {
        val = self->unkD48 + self->unk50C;
        self->unkD4C = val;
        if (val > 0x270F) {
            self->unkD4C = 0x270F;
        }

        p = (u16*)self;
        for (i = 0; i < self->count7B0; i++) {
            v = p[0x374];
            if (self->count94C < 0xC8) {
                self->buf7B2[self->writeIdx944] = v;
                self->writeIdx944 = self->writeIdx944 + 1;
                if (self->writeIdx944 >= 0xC8) {
                    self->writeIdx944 = 0;
                }
                self->count94C = self->count94C + 1;
            }
            p += 1;
        }

        val = self->unkD48;
        if (val > 0x64) {
            val = 0x64;
        }
        if (val > 0 && (u32)(self->unkD6C + 0x10000) == 0xFFFF) {
            self->unkD6C = playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1EF8, -1, 0, 0, -1);
        }
        self->unkD44 = val;
    } else {
        s32 r4;
        if (self->unk514 == 0) {
            r4 = 0;
        } else {
            r4 = self->unk520;
        }
        val = self->unk50C - r4;
        self->unkD4C = val;
        if (val < 0) {
            self->unkD4C = 0;
        }
        if ((u32)(self->unkD70 + 0x10000) == 0xFFFF) {
            self->unkD70 = playSE__9sndSystemF5SndIDiiii(g_sndSystem, 0x1EFA, -1, 0, 0, -1);
        }
    }

    s32 diff = self->unkD4C - self->unk50C;
    s32 s = diff >> 31;
    diff = (s ^ diff) - s;
    if (diff > 0x78) {
        self->unkD50 = (f32)diff / lbl_15_rodata_210;
    } else {
        self->unkD50 = lbl_15_rodata_4;
    }
}

}
}

// TODO: clean up, give variables real names
namespace jj_49C4 {
struct Obj {
    char pad0[0x504];
    int result;
    char pad508[4];
    int value;
    char pad510[0xd4c-0x510];
    int target;
    float step;
    char padD54[0x18];
    unsigned int sound1;
    unsigned int sound2;
};
extern "C" void fn_15_1424(Obj*, int);
extern "C" void stopSE__9sndSystemFll(void*, unsigned int, int);
extern void* g_sndSystem;
extern "C" int mainStepCoinInc__15muWifiWatchTaskFv(Obj* self) {
    int value = self->value;
    int target = self->target;
    if (target != value) {
        if (target > value) {
            int next = (int)((float)value + self->step);
            self->value = next;
            if (next > target) self->value = target;
        } else {
            int next = (int)((float)value - self->step);
            self->value = next;
            if (next < target) self->value = target;
        }
        fn_15_1424(self, self->value);
        return self->result;
    }
    if (self->sound1 != 0xffffffff) {
        stopSE__9sndSystemFll(g_sndSystem, self->sound1, 0);
        self->sound1 = 0xffffffff;
    }
    if (self->sound2 != 0xffffffff) {
        stopSE__9sndSystemFll(g_sndSystem, self->sound2, 0);
        self->sound2 = 0xffffffff;
    }
    return 8;
}
}

// TODO: clean up, give variables real names
namespace jj_4C7C {

extern "C" gfTask* fn_800BEEB8(int, void*, int, int, int, int, int, int, int, int, int, int);
extern "C" int getErrorMsgID__19muWifiInterfaceTaskFi(int);
extern "C" int haveErrorCode__19muWifiInterfaceTaskFi(int);
extern "C" void dispSubMsg__15muNoticeWndTaskFPCc(gfTask*, const char*);
extern "C" char lbl_15_data_2AC[];

extern "C" void initStepNetError__15muWifiWatchTaskFv(char* self) {
    unsigned long size;
    char* data;
    int a, b;
    
    if (*(int*)(self + 0x4F0) == 0) {
        a = 4;
        b = 1;
        data = (char*)*(void**)(*(char**)(self + 0x24C) + 4);
    } else {
        data = (char*)(*(void**)(self + 0x544));
        a = getErrorMsgID__19muWifiInterfaceTaskFi(*(int*)(self + 0x4F0));
        b = haveErrorCode__19muWifiInterfaceTaskFi(*(int*)(self + 0x4F0));
    }
    
    *(gfTask**)(self + 0x540) = fn_800BEEB8(0, (void*)data, a, *(int*)(self + 0x53C), *(int*)(self + 0x1A0), 0x1a, 0x1e, 0x2a, 0x2b, 0, 0, 0xf0);
    
    if (b != 0) {
        char buf[0x80];
        Message::getPrintIndexData(*(void**)(self + 0x544), 0x13, &data, &size);
        memcpy(buf, data, size);
        size += sprintf(buf + size, lbl_15_data_2AC, *(int*)(self + 0x4F4));
        dispSubMsg__15muNoticeWndTaskFPCc(*(gfTask**)(self + 0x540), buf);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_4E80 {
extern "C" int isReady__14gfFileIOHandleCFv(const void*);

extern "C" int mainStepPost__15muWifiWatchTaskFv(char* self) {
    int i = 0;
    int st;
    int ready;
    char* p = self + 0x254;

    for (; i < 0x28; i++) {
        if (*(int*)(p + 4) != 0) {
            st = 2;
        } else if (!((((unsigned char)p[0]) >> 7) & 1)) {
            st = 0;
        } else {
            int r = isReady__14gfFileIOHandleCFv(p + 8);
            st = 1;
            if (r != 0) {
                st = 2;
            }
        }

        if (st == 1) {
            ready = 0;
            goto done;
        }
        p += 0xc;
    }
    ready = 1;

done:
    if (ready != 0) {
        return 0xc;
    }
    return *(int*)(self + 0x504);
}
}

