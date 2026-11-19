#include <mu/mu_adv_selchr_c.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <mu/mu_adv_selchr_b.h>

// muMenuController::getControllerID(): the pad of the controller (below 0: there is none).
extern "C" int fn_800BA3D8(muMenuController* controller);

extern const u8 lbl_40_rodata_930[4][2];

// The trainer and the three Pokemon, Zelda and Sheik, Samus and Zero Suit Samus each take one entry: the first of a
// group keeps the group's first kind, the next ones of the same group give CharKind_None.
int muAdvSelchrCTask::normalizeCharKind(int kind, bool* pokeTrainerSeen, bool* zeldaSeen, bool* samusSeen) {
    // BYTES: each range test is one unsigned compare of the distance to the group's first kind (subi/cmplwi).
    if (static_cast<u32>(kind - CharKind_PokeTrainer) <= 3) {
        if (*pokeTrainerSeen) {
            return CharKind_None;
        }
        *pokeTrainerSeen = true;
        return CharKind_PokeTrainer;
    }
    if (static_cast<u32>(kind - CharKind_Zelda) <= 1) {
        if (*zeldaSeen) {
            return CharKind_None;
        }
        *zeldaSeen = true;
        return CharKind_Zelda;
    }
    if (static_cast<u32>(kind - CharKind_Samus) <= 1) {
        if (*samusSeen) {
            return CharKind_None;
        }
        *samusSeen = true;
        return CharKind_Samus;
    }
    return kind;
}

// The team that has the character (the characters that change form are found by the group's first kind), and where
// in the team it is.
int muAdvSelchrCTask::getCharTeam(int* index, int kind) {
    // BYTES: the same unsigned range tests as normalizeCharKind.
    if (static_cast<u32>(kind - CharKind_PokeTrainer) <= 3) {
        kind = CharKind_PokeTrainer;
    }
    if (static_cast<u32>(kind - CharKind_Zelda) <= 1) {
        kind = CharKind_Zelda;
    }
    if (static_cast<u32>(kind - CharKind_Samus) <= 1) {
        kind = CharKind_Samus;
    }
    for (int team = 0; team < m_numTeams; team++) {
        for (int i = 0; i < m_teams[team].m_count; i++) {
            if (kind == m_teams[team].m_members[i]) {
                *index = i;
                return m_teams[team].m_id;
            }
        }
    }
    return 10;
}

int muAdvSelchrCTask::getTeamMember(int team, int index) {
    return m_teams[team].m_members[index];
}

// The kind of the member with the form that is shown for the characters that change form.
int muAdvSelchrCTask::getTeamMemberDetail(int team, int index) {
    int kind = m_teams[team].m_members[index];
    // BUG: the trainer is tested twice (the second test can never be true); the code without it does not match.
    // HYPOTHESIS: the original tested two names of the same kind.
    if (kind == CharKind_PokeTrainer) {
        return m_pokeTrainerKind;
    } else if (kind == CharKind_PokeTrainer) {
        return m_pokeTrainerKind;
    } else if (kind == CharKind_Zelda) {
        return m_zeldaKind;
    } else if (kind == CharKind_Samus) {
        return m_samusKind;
    }
    return kind;
}

int muAdvSelchrCTask::getNumTeamMember(int team) {
    return m_teams[team].m_count;
}

MuObject* muAdvSelchrCTask::getFaceObj(int team, int index) {
    return m_objects[m_teams[team].m_top + index + Obj_Face];
}

bool muAdvSelchrCTask::isPlayerJoined(int player) {
    return fn_800BA3D8(&m_players[player].m_controller) >= 0;
}

int muAdvSelchrCTask::getTeamCurPos() {
    return m_teamCurPos;
}

// How many lines of seven faces the team takes.
int muAdvSelchrCTask::getNumTeamLine(int team) {
    return (m_teams[team].m_count + 6) / 7;
}

int muAdvSelchrCTask::getCharCursorPos(int player) {
    return m_players[player].m_cursor;
}

void muAdvSelchrCTask::setCharCursorPos(int player, int pos) {
    m_players[player].m_cursor = pos;
}

// Where the character at the position is in the picks of the player that picked it (-1: no player picked it).
int muAdvSelchrCTask::getMemberNumber(int pos) {
    // BYTES: the counter is declared before the pointer to the player's data (the register order needs it).
    int player = 0;
    PlayerWork* work = m_players;
    for (; player < 2; player++, work++) {
        if (isPlayerJoined(player)) {
            for (int i = 0; i < work->m_count; i++) {
                if (pos == work->m_members[i]) {
                    return i;
                }
            }
        }
    }
    return -1;
}

// Whether the cursor of the player can be on the position: nobody picked the character, and the cursors of the players
// before it are not on it.
bool muAdvSelchrCTask::canLocateCursor(int player, int pos) {
    if (getMemberNumber(pos) >= 0) {
        return false;
    }
    for (int i = player - 1; i >= 0; i--) {
        if (m_players[i].m_cursor == pos) {
            return false;
        }
    }
    return true;
}

// How many characters of the current team nobody picked.
int muAdvSelchrCTask::getNumFreeChar() {
    int count = 0;
    for (int pos = 0; pos < m_teams[m_teamCurPos].m_count; pos++) {
        if (getMemberNumber(pos) == -1) {
            count++;
        }
    }
    return count;
}

// Puts the cursor of the player on the face at the position.
void muAdvSelchrCTask::moveCharCursor(int player, int pos) {
    nw4r::g3d::ScnMdlExpand* expand = m_expands[m_teamCurPos + 2];
    int index = Obj_Cursor + 1;
    if (player == 0) {
        index = Obj_Cursor;
    }
    MuObject* cursor = m_objects[index];
    expand->Remove(cursor->m_sceneModel);
    char nodeName[8];
    sprintf(nodeName, "pos%02d", pos);
    expand->PushBack(cursor->m_sceneModel, nodeName);
}

// Shows the number of the pick of the player on the face at the position.
void muAdvSelchrCTask::dispNumber(int player, int pos, int number) {
    int base = player == 0 ? Obj_Number1 : Obj_Number2;
    MuObject* numberObj = m_objects[base + number - 1];
    numberObj->setFrameTex(number);
    nw4r::g3d::ScnMdlExpand* expand = m_expands[m_teamCurPos + 2];
    char nodeName[8];
    sprintf(nodeName, "pos%02d", pos);
    expand->PushBack(numberObj->m_sceneModel, nodeName);
}

void muAdvSelchrCTask::removeNumber(int player, int number) {
    int base = Obj_Number2;
    if (player == 0) {
        base = Obj_Number1;
    }
    m_expands[m_teamCurPos + 2]->Remove(m_objects[base + number - 1]->m_sceneModel);
}

int muAdvSelchrCTask::mainStepSelectedMain(muAdvSelchrCTask* task) {
    if (++task->m_timer >= 2) {
        task->m_state = 2;
        return Step_Zombie;
    }
    return task->m_step;
}

void muAdvSelchrCTask::mainStepZombieInit(muAdvSelchrCTask* task) { }

int muAdvSelchrCTask::mainStepZombieMain(muAdvSelchrCTask* task) {
    return task->m_step;
}

bool muAdvSelchrCTask::isSelected() {
    if (m_step >= Step_Selected && m_stepPrev >= Step_Selected) {
        return true;
    }
    return false;
}

muAdvSelchrCTask::PlayerWork::PlayerWork() { }

muAdvSelchrCTask::PlayerWork::~PlayerWork() { }

int muAdvSelchrCTask::getDirButtonRepeat(int player) {
    return m_players[player].m_controller.m_trigger;
}

void muAdvSelchrCTask::mainStepSelectedInit(muAdvSelchrCTask* task) {
    int anim = 3;
    MuObject* object = task->m_objects[1];
    if (task->m_unkC28[1]) {
        anim = 1;
    }
    task->storeResult();
    object->changeAnimN(anim, 0x11, true, false);
    task->m_timer = 0;
}

int muAdvSelchrCTask::mainStepAppearMain(muAdvSelchrCTask* task) {
    MuObject* object = task->m_objects[1];
    if (object->isNodeAnimFinished() && object->isClrAnimFinished()) {
        if (task->m_numTeams <= 1 || task->m_unkC28[2]) {
            return 2;
        }
        return 1;
    }
    return task->m_step;
}

// HYPOTHESIS: the table has the kind of each of the four forms and the bit of m_unkC28[4] that says it is unlocked.
int muAdvSelchrCTask::getNextPoke(int kind, bool forward) {
    int index;
    for (index = 0; index < 4; index++) {
        if (kind == lbl_40_rodata_930[index][0]) {
            break;
        }
    }
    if (forward) {
        do {
            index++;
            if (index >= 4) {
                index = 0;
            }
        } while (kind != lbl_40_rodata_930[index][0] && (m_unkC28[4] & lbl_40_rodata_930[index][1]) == 0);
    } else {
        do {
            index--;
            if (index < 0) {
                index = 3;
            }
        } while (kind != lbl_40_rodata_930[index][0] && (m_unkC28[4] & lbl_40_rodata_930[index][1]) == 0);
    }
    return lbl_40_rodata_930[index][0];
}

gfPadStatus muAdvSelchrCTask::getPadStatus(int player) {
    gfPadStatus status;
    int padNo = fn_800BA3D8(&m_players[player].m_controller);
    if (padNo >= 0) {
        g_gfPadSystem->getSysPadStatus(padNo, &status);
    } else {
        memset(&status, 0, sizeof(status));
    }
    return status;
}

int muAdvSelchrCTask::findNextCharCursorPos(int player, int pos) {
    int next = pos;
    int count = m_teams[m_teamCurPos].m_count;
    do {
        next++;
        if (next >= count) {
            next = 0;
        }
        if (canLocateCursor(player, next)) {
            return next;
        }
    } while (next != pos);
    return -1;
}

int muAdvSelchrCTask::findLocateCursorPosX(int player, int team, int row, int column) {
    int count = m_teams[team].m_count;
    int lines = count / 7;
    int remainder = count % 7;
    bool full = remainder == 0;
    if (full) remainder = 7;
    if (!full) lines++;
    int width = row < lines - 1 ? 7 : remainder;
    int left;
    int right = column + 1;
    left = column - 1;
    while (right < width || left >= 0) {
        if (right < width && row * 7 + right < m_teams[team].m_count && canLocateCursor(player, row * 7 + right)) {
            return right;
        }
        if (left >= 0 && row * 7 + left < m_teams[team].m_count && canLocateCursor(player, row * 7 + left)) {
            return left;
        }
        right++;
        left--;
    }
    return -1;
}

muAdvSelchrCTask::muAdvSelchrCTask()
    : gfTask("muAdvSelchrC", Category_Menu, 15, 8, true),
      m_unk700(), m_unk964(0), m_step(Step_Appear), m_stepPrev(Step_Count),
      m_timer(0), m_teamCurPos(0), m_unkC20(true), m_state(0),
      unkC28(false), unkC29(false), unkC2A(false), unkC2B(false),
      unkC2C(false), unkC2D(false), unkC2E(false),
      m_pokeTrainerKind(CharKind_PokeTrainer), m_zeldaKind(CharKind_Zelda),
      m_samusKind(CharKind_Samus), m_unkC3C(0), m_loaded(false),
      m_instanceHeap(static_cast<Heaps::HeapType>(42)), m_resourceHeap(static_cast<Heaps::HeapType>(43)) {
    unk2C_b1 = false;
    memset(m_teams, 0, sizeof(m_teams) + sizeof(m_numTeams) + sizeof(m_unk6FC));
    for (int index = 0; index < 4; index++) m_unk700[index] = 0;
    for (int index = 0; index < 137; index++) m_objects[index] = 0;
    for (int index = 0; index < 12; index++) m_expands[index] = 0;
    for (int player = 0; player < 2; player++) {
        for (int index = 0; index < 40; index++) m_players[player].m_members[index] = 0;
        m_players[player].m_count = 0;
        m_players[player].m_cursor = 0;
    }
    m_unkC10[0] = 0;
    m_unkC10[1] = 0;
}

void muAdvSelchrCTask::createObjResFile(const void* modelData, int count, nw4r::g3d::ResFile* resFile) {
    int num;
    int model;
    int index;
    const muAdvSelchrBModel* models = static_cast<const muAdvSelchrBModel*>(modelData);
    MuObject* object;
    for (model = 0; model < count; model++, models++) {
        num = 1;
        if (models->m_start < models->m_end) num = models->m_end - models->m_start;
        for (index = 0; index < num; index++) {
            object = MuObject::create(resFile, models->m_name, models->m_priority, NULL, m_resourceHeap);
            m_objects[models->m_start + index] = object;
            char animName[0x40];
            strcpy(animName, models->m_name);
            strcat(animName, "__0");
            object->changeAnimN(animName);
            if (object->m_modelAnim) object->m_modelAnim->setUpdateRate(0.0f);
        }
    }
}

#include <gf/gf_pad_system.h>
#include <gf/gf_pad_status.h>
#include <mu/mu_object.h>
// TODO: clean up, give variables real names
namespace jj_3DDEC {
extern "C" {

void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__16muAdvSelchrCTaskFv(void*);
void  fn_40_3E418(void*, int, int, int);

void* create__16muAdvSelchrCTaskFPCUlQ25Heaps8HeapTypeUl(int a, int b, int c) {
    void* obj = __nw__FUlQ25Heaps8HeapType(0xc60, b);
    if (obj) {
        obj = __ct__16muAdvSelchrCTaskFv(obj);
    }
    fn_40_3E418(obj, a, b, c);
    return obj;
}

}
}

// TODO: clean up, give variables real names
namespace jj_3E188 {
extern "C" void fn_40_3E188(unsigned int *ptr)
{
    *ptr = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_3E194 {
class gfFileIOHandle;

extern "C" {
void __dt__14gfFileIOHandleFv(void* p, int flag);
void __dl__FPv(void* p);

void* fn_40_3E194(void* p, int flag) {
    if (p != 0) {
        __dt__14gfFileIOHandleFv(p, 0);
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}
}

// TODO: clean up, give variables real names
namespace jj_3E260 {
typedef unsigned char u8;

class AdvStageObj {
    u8 pad[0x5C];
public:
    virtual void f0(int);
};

extern "C" {
extern u8 lbl_40_data_17CFC[];
extern void* g_gfSceneRoot;

void setClearDepthBufferState__11gfSceneRootFQ211gfSceneRoot9LayerTypeb(u8* base, int index, unsigned val);
void __dt__5MuMsgFv(void*, int);
void Destroy__Q34nw4r3g3d6G3dObjFv(void*);
void removeInfoCamResAnm__11gfSceneRootFv(void*);
void cancelRequest__14gfFileIOHandleFv(void*);
void free__13gfHeapManagerFPv(void*);
void fn_40_3E194(void*, int);
void __dt__Q216muAdvSelchrCTask10PlayerWorkFv(void*, int);
void __destroy_arr(void*, void (*)(void*, int), unsigned long, unsigned long);
void __dt__6gfTaskFv(void*, int);
void __dl__FPv(void*);

void* fn_40_3E260(u8* object, int deleting) {
    if (object) {
        *reinterpret_cast<void**>(object + 0x3C) = lbl_40_data_17CFC;
        setClearDepthBufferState__11gfSceneRootFQ211gfSceneRoot9LayerTypeb((u8*)g_gfSceneRoot, 8, 0);

        u8* cursor = object;
        for (int i = 0; i < 0xC; ++i, cursor += 4) {
            void* entry = *reinterpret_cast<void**>(cursor + 0x934);
            if (entry) {
                Destroy__Q34nw4r3g3d6G3dObjFv(entry);
            }
        }

        cursor = object;
        for (int i = 0; i < 0x89; ++i, cursor += 4) {
            AdvStageObj* entry = *reinterpret_cast<AdvStageObj**>(cursor + 0x710);
            if (entry) {
                if (entry) {
                    entry->f0(1);
                }
            }
        }

        if (*(void**)(object + 0x964)) {
            free__13gfHeapManagerFPv(*(void**)((u8*)*(void**)(object + 0x964) + 4));
            __dt__5MuMsgFv(*(void**)(object + 0x964), 1);
        }

        if (*(void**)(object + 0x700)) {
            free__13gfHeapManagerFPv(*(void**)(object + 0x700));
        }
        if (*(void**)(object + 0x704)) {
            free__13gfHeapManagerFPv(*(void**)(object + 0x704));
        }
        if (*(void**)(object + 0x708)) {
            free__13gfHeapManagerFPv(*(void**)(object + 0x708));
        }

        if (*(u8*)(object + 0xC2B)) {
            removeInfoCamResAnm__11gfSceneRootFv(g_gfSceneRoot);
        }

        if (*(void**)(object + 0x70C)) {
            free__13gfHeapManagerFPv(*(void**)(object + 0x70C));
        }

        cursor = object + 0xC40;
        for (int i = 0; i < 5; ++i, cursor += 4) {
            cancelRequest__14gfFileIOHandleFv(cursor);
        }

        __destroy_arr(object + 0xC40, fn_40_3E194, 4, 5);
        __destroy_arr(object + 0x970, __dt__Q216muAdvSelchrCTask10PlayerWorkFv, 0x150, 2);
        __dt__6gfTaskFv(object, 0);

        if (deleting > 0) {
            __dl__FPv(object);
        }
    }

    return object;
}
}
}

// TODO: clean up, give variables real names
namespace jj_3E58C {
extern "C" int getCharTeam__16muAdvSelchrCTaskFPii(void*, int*, int);

struct Data {
    int values[20];
    int count;
    int value54;
    int value58;
    int count2;
    int value60;
    int value64;
};

struct Entry {
    char pad[0x40];
    int value;
    char rest[0x68];
};

struct Sub {
    char pad[0xa8];
    int out[40];
    int count;
    int value;
};

struct Obj {
    Entry entries[10];
    char pad0[0x40];
    int entryCount;
    char pad1[0x274];
    Sub sub;
    char pad2[0xa8];
    int out2[40];
    int outCount2;
    int outValue2;
    char pad3[0xc];
    int index;
    char pad4[0x1c];
    Data* data;
};

inline int findEntry(int result, int index, Entry* entry, int count)
{
    for (int n = count; n > 0; n--) {
        if (result == entry->value)
            return index;
        entry++;
        index++;
    }
    return -1;
}

extern "C" void fn_40_3E58C(Obj* self)
{
    int value;
    int offset;
    Sub* p;
    Sub* sub;
    int i;

    sub = &self->sub;
    p = sub;
    i = 0;
    offset = 0;

    while (i < self->data->count) {
        getCharTeam__16muAdvSelchrCTaskFPii(self, &value, *(int*)((char*)self->data + offset));
        offset += 4;
        i++;
        p->out[0] = value;
        p = (Sub*)((char*)p + 4);
    }

    sub->count = self->data->count;
    int result = getCharTeam__16muAdvSelchrCTaskFPii(self, &value, self->data->value60);
    sub->value = value;

    self->index = findEntry(result, 0, self->entries, self->entryCount);
    if (self->data->count2 > 0) {
        getCharTeam__16muAdvSelchrCTaskFPii(self, &value, self->data->value54);
        self->out2[0] = value;
    }

    self->outCount2 = self->data->count2;
    getCharTeam__16muAdvSelchrCTaskFPii(self, &value, self->data->value64);
    self->outValue2 = value;
}
}

// TODO: clean up, give variables real names
namespace jj_3EEBC {
#pragma fp_contract on
#pragma strict_ieee_fp off
#pragma opt_loop_invariants off

extern "C" {
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
int exchangeGmCharacterKind2Something__6muMenuFi(int);
int exchangeMuSelchkind2MuStockchkind__6muMenuFi(int);
void setFrameTex__8MuObjectFf(void*, float);
int sprintf(char*, const char*, ...);
extern const char lbl_40_data_17C50[];
}

extern "C" void fn_40_3EEBC(void* arg0) {
    char* obj;
    char* sp = (char*)arg0;
    char* ep = (char*)arg0;
    char* kp = (char*)arg0;
    char* ip;
    int k = 0;
    int i = 0;

    goto outer_test;

outer_body:
    {
        void* scn = *(void**)(sp + 0x93C);
        ip = ep;
        int j = 0;
        *(int*)(ep + 0xE8) = k;
        goto inner_test;

inner_body:
        {
            int sel = *(int*)(ip + 0x44);
            obj = *(char**)(kp + 0x744);

            if (sel == 0x1B) {
                sel = *(int*)((char*)arg0 + 0xC30);
            } else if (sel == 0x1B) {
                sel = *(int*)((char*)arg0 + 0xC30);
            } else if (sel == 0xE) {
                sel = *(int*)((char*)arg0 + 0xC34);
            } else if (sel == 3) {
                sel = *(int*)((char*)arg0 + 0xC38);
            }

            int mu = exchangeMuSelchkind2MuStockchkind__6muMenuFi(sel);
            int g = exchangeGmCharacterKind2Something__6muMenuFi(mu);
            setFrameTex__8MuObjectFf(obj, (float)(g * 10 + 1));

            char buf[8];
            sprintf(buf, lbl_40_data_17C50, j);
            Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(
                scn, *(void**)(obj + 0x10), buf);

            kp += 4;
            k++;
            ip += 4;
            j++;
        }

inner_test:
        if (j < *(int*)(ep + 0xE4))
            goto inner_body;

        sp += 4;
        ep += 0xAC;
        i++;
    }

outer_test:
    if (i < *(int*)((char*)arg0 + 0x6F8))
        goto outer_body;
}
}

// TODO: clean up, give variables real names
namespace jj_3EFFC {
extern "C" {
int exchangeMuSelchkind2MuStockchkind__6muMenuFi(int);
int exchangeGmCharacterKind2Something__6muMenuFi(int);

float fn_40_3EFFC(int kind) {
    int sel = exchangeMuSelchkind2MuStockchkind__6muMenuFi(kind);
    int k = exchangeGmCharacterKind2Something__6muMenuFi(sel);
    return k * 10 + 1;
}
}
}

// TODO: clean up, give variables real names
namespace jj_3F07C {
typedef float f32;

struct ScnMdl {
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
    virtual void slot12(int n);
    char pad[0xE4 - 4];
    int count;
};

extern "C" {
void setFrameNode__8MuObjectFf(void*, f32);
void setFrameTex__8MuObjectFf(void*, f32);
void Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(void*, void*, char*);
int sprintf(char*, const char*, ...);
extern const char lbl_40_data_17C50[];
extern const double lbl_40_rodata_9A0;
}

extern "C" void fn_40_3F07C(char* self) {
    void* scn;
    void* obj;
    char* p28;
    char* p27;
    int i26;
    ScnMdl* mdl = *(ScnMdl**)(self + 0x938);
    int i24 = 0;
    char buf[8];
    while (mdl->count != 0) {
        if (mdl->count != 0) {
            mdl->slot12(mdl->count - 1);
        }
    }
    p28 = self;
    p27 = self;
    i26 = 0;
    while (i26 < *(int*)(self + 0x6f8) && i24 < 0xC) {
        int n = (*(int*)(p27 + 0xE4) + 6) / 7;
        obj = *(void**)(p28 + 0x71c);
        scn = *(void**)(p28 + 0x93c);
        setFrameNode__8MuObjectFf(obj, (f32)(n - 1));
        setFrameTex__8MuObjectFf(obj, (f32)*(int*)(p27 + 0x40));
        sprintf(buf, lbl_40_data_17C50, i24);
        Pushback__Q34nw4r3g3d6ScnMdlFPvPvPc(mdl, scn, buf);
        i24 += n;
        p28 += 4;
        p27 += 0xAC;
        i26++;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_4068C {

extern "C" int fn_800BA3D8(void*);
struct Masks { unsigned int a, b; };
extern Masks lbl_40_rodata_9D8[];
extern Masks lbl_40_rodata_9F8[];

static inline gfPadStatus getStatus(void* p) {
    gfPadStatus status;
    int index = fn_800BA3D8(p);
    if (index >= 0)
        g_gfPadSystem->getSysPadStatus(index, &status);
    else
        memset(&status, 0, sizeof(status));
    return status;
}

struct Entry { char bytes[0x150]; };
struct Self { char pad[0x970]; Entry entries[1]; };
extern "C" int fn_40_4068C(Self* self, int index, int flag) {
    char* p = self->entries[index].bytes;
    gfPadStatus status = getStatus(p);
    if (status.m_error != 0)
        return 0;
    Masks* masks = &lbl_40_rodata_9F8[status.m_controllerType];
    if (flag)
        masks = &lbl_40_rodata_9D8[status.m_controllerType];
    if (masks->a & *(unsigned int*)(p + 0x18))
        return 1;
    return (status.m_buttonsPressedThisFrame.bits & masks->b) != 0;
}
}

// TODO: clean up, give variables real names
namespace jj_407E8 {
struct FlagObj {
    int unk0;
    int unk4;
    int unk8;
    unsigned int flags;
};

extern "C" int bitcheckEdgeOn__11gfPadStatusCFUl(FlagObj* obj, unsigned int mask) {
    return (obj->flags & mask) != 0;
}
}

// TODO: clean up, give variables real names
namespace jj_40AA0 {
extern "C" {
int fn_40_40AA0(int base, int idx) {
    return base + idx * 7;
}
}
}

// TODO: clean up, give variables real names
namespace jj_41270 {
extern "C" void* g_sndSystem;

extern "C" void* fn_40_41270() {
    return g_sndSystem;
}
}

// TODO: clean up, give variables real names
namespace jj_4128C {
struct Data {
    unsigned int w[6];
    float a, b, c, d, e, f;
    unsigned char bytes[9];
    unsigned int end;
};
extern "C" void fn_40_4128C(Data* dst, const Data* src) {
    *dst = *src;
}
}

// TODO: clean up, give variables real names
namespace jj_41368 {
extern "C" {
void* getMsgData__5MuMsgFUlPPv(void* self, unsigned long index, void** size);
void* memcpy(void* dest, const void* src, unsigned long size);
int sprintf(char* str, const char* format, ...);
void printData__5MuMsgFUlPCvi(void* msg, unsigned long index, char* buffer, unsigned long len);
extern char lbl_40_data_17C6C[];
}

extern "C" void fn_40_41368(void* self, int arg1) {
    char buf[0x100];
    unsigned long size;
    unsigned long pos;
    unsigned long m1;
    int total = *(int*)((char*)self + 0xC10) - *(int*)((char*)self + 0xAB8);
    int c14 = *(int*)((char*)self + 0xC14);
    if (c14 > 0) {
        total += c14 - *(int*)((char*)self + 0xC08);
    }
    if (*(void**)((char*)self + 0x964) != 0) {
        unsigned long m0 = 2;
        if (arg1 != 0) {
            m0 = 0;
        }
        m1 = 3;
        if (arg1 != 0) {
            m1 = 1;
        }
        void* src = getMsgData__5MuMsgFUlPPv(*(void**)((char*)self + 0x964), m0, (void**)&size);
        memcpy(buf, src, size);
        pos = size;
        int len = sprintf(buf + pos, lbl_40_data_17C6C, total);
        size = len;
        pos += len;
        src = getMsgData__5MuMsgFUlPPv(*(void**)((char*)self + 0x964), m1, (void**)&size);
        memcpy(buf + pos, src, size);
        printData__5MuMsgFUlPCvi(*(void**)((char*)self + 0x964), 0, buf, pos + size);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_41834 {
extern "C" void OSReport(const char* fmt, ...);

extern char lbl_40_data_17A88[];

struct Inner {
    char _00[0x50];
    int count;   // 0x50
    int unk54;   // 0x54
    char _58[8];
    int unk60;   // 0x60
};

struct Obj {
    char _00[0xc3c];
    Inner* inner; // 0xc3c
};

extern "C" void initProc__16muAdvSelchrCTaskFv(Obj* arg0) {
    char* base = lbl_40_data_17A88;
    if (arg0->inner != 0) {
        OSReport(base + 0x1e8);
        OSReport(base + 0x210);
        int off;
        int i = 0;
        off = 0;
        while (i < arg0->inner->count) {
            OSReport(base + 0x228, i + 1, *(int*)((char*)arg0->inner + off));
            off += 4;
            i += 1;
        }
        OSReport(base + 0x234);
        OSReport(base + 0x24c, arg0->inner->unk54);
        OSReport(base + 0x258);
        OSReport(base + 0x268, arg0->inner->unk60);
    }
}
}

// TODO: clean up, give variables real names
namespace jj_41918 {


extern "C" {
int changeAnimN__8MuObjectFiUlbb(void*, int, int, int, int);
void SetPriorityDrawOpa__Q34nw4r3g3d6ScnObjFi(void*, int);
void SetPriorityDrawXlu__Q34nw4r3g3d6ScnObjFi(void*, int);
int fn_800BA3D8(void*);
extern unsigned char lbl_40_rodata_9B0[];
}

struct Self {
    char _0[0x714];
    MuObject* obj;
    char _718[4];
    MuObject* entries[10];
    char _744[0x970 - 0x744];
    char pad[0x2b9];
    unsigned char flag;
};

extern "C" void fn_40_41918(Self* self) {
    changeAnimN__8MuObjectFiUlbb(self->obj, self->flag ? 0 : 2, 0x11, 1, 0);

    int i;
    MuObject* entry;
    char* p = (char*)self;
    for (i = 0; i < 10; i++, p += 4) {
        entry = *(MuObject**)(p + 0x71c);
        if (i == 0) {
            SetPriorityDrawOpa__Q34nw4r3g3d6ScnObjFi(entry->m_scnMdl, 0x4c);
            SetPriorityDrawXlu__Q34nw4r3g3d6ScnObjFi(entry->m_scnMdl, 0x4c);
        } else {
            SetPriorityDrawOpa__Q34nw4r3g3d6ScnObjFi(entry->m_scnMdl, i + 0x43);
            SetPriorityDrawXlu__Q34nw4r3g3d6ScnObjFi(entry->m_scnMdl, i + 0x43);
        }
    }

    if (self->flag) {
        gfPadStatus status;
        int index = fn_800BA3D8((char*)self + 0x970);
        if (index >= 0)
            g_gfPadSystem->getSysPadStatus(index, &status);
        else
            memset(&status, 0, sizeof(status));

        gfPadStatus copy = status;
        self->obj->setFrameTex((float)lbl_40_rodata_9B0[copy.m_controllerType]);
    }
}
}

