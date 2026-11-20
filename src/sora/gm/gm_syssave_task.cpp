#include <gf/gf_task.h>
#include <string.h>
#include <types.h>
#include <gm/gm_global.h>
#include <gm/gm_syssave_task.h>
#include <nt/nt_offline.h>

extern "C" void* lbl_8042E8D8[3];
extern "C" Heaps::HeapType lbl_8059CE48;
extern "C" gmSysSave* lbl_805A00FC;

// HYPOTHESIS: the 0x14-byte block is the gmSysSave instance; base link, vtable slot and status fields are set inline
struct gmSysSaveObj {
    gmSysSaveObj* unk0;
    void* unk4;
    s32 unk8;
    s32 unkC;
    u8 unk10;
};

gmSysSave* gmSysSave::create(Heaps::HeapType heap) {
    g_GameGlobal->m_appData->_0x0[0x28] |= 0x20;
    lbl_8059CE48 = heap;
    gmSysSaveObj* obj = new (heap) gmSysSaveObj;
    if (obj != NULL) {
        obj->unk0 = NULL;
        obj->unk4 = &lbl_8042E8D8;
        obj->unk8 = -1;
        obj->unkC = 0;
        obj->unk10 = 0;
    }
    lbl_805A00FC = (gmSysSave*) obj;
    obj->unk8 = 0;
    obj->unkC = 0;
    return lbl_805A00FC;
}

// TODO: clean up, give variables real names
extern "C" u8 fn_8005A744(void* self) {
    return *reinterpret_cast<u8*>(reinterpret_cast<u8*>(self) + 0x10);
}

// TODO: clean up, give variables real names
namespace jj_8005A228 {
extern "C" void fn_8005A228(int* p)
{
    p[0x40 / 4] = 0;
    p[0x44 / 4] = 0;
}
}

namespace jj_8005A6E8 {
extern "C" void __dl__FPv(void*);

extern "C" char lbl_8042E8D8[];
extern "C" void* g_GameGlobal;
extern "C" int lbl_805A00FC;

// TODO: clean up, give variables real names
extern "C" void fn_8005A6E8(void) {
    void* temp_r3 = (void*)lbl_805A00FC;

    if (temp_r3 != 0) {
        ((int*)temp_r3)[1] = (int)lbl_8042E8D8;
        lbl_805A00FC = 0;
        unsigned char* appData = (unsigned char*)*(int*)g_GameGlobal;
        appData[0x28] &= ~0x20;
        __dl__FPv(temp_r3);
    }
    lbl_805A00FC = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_8005A1A4 {
struct Data28 {
    unsigned char padding[0x28];
    unsigned char flags;
};

extern "C" {
extern "C" unsigned char lbl_8042E8FC[0x64];
extern "C" int lbl_805A00F0;
extern "C" int lbl_805A00F4;
extern "C" Data28** g_GameGlobal;

void __dl__FPv(void*);
void __dt__6gfTaskFv(void*, int);
}

struct Task {
    unsigned char padding[0x3C];
    void* unk3C;
};

// TODO: clean up, give variables real names
extern "C" Task* fn_8005A1A4(Task* p, int flag) {
    if (p) {
        p->unk3C = lbl_8042E8FC;
        lbl_805A00F4 = 0;
        (*g_GameGlobal)->flags &= ~0x20;
        lbl_805A00F0 = 0;
        __dt__6gfTaskFv(p, 0);
        if (flag > 0) {
            __dl__FPv(p);
        }
    }
    return p;
}
}

// TODO: clean up, give variables real names
namespace jj_8005A238 {
struct Obj { char pad[0xA0]; int a0; char pad2[0x4C]; int f0; };
struct gfTask { char pad[0x40]; int s; int c; };
extern "C" {
void exit__6gfTaskFv(gfTask*);
Obj* createInstance__14gfSysRecorder2FQ25Heaps8HeapType(int);
int isEnable__14gfSysRecorder2Fi(int);
void dropInstance__14gfSysRecorder2Fv();
int getBuffer__14gfSysRecorder2Fi(Obj*, int);
int saveSystem__14gfSysRecorder2Fv(Obj*);
void getSaveData__10GameGlobalFPv(int, int);
}
extern "C" int g_GameGlobal;
extern "C" int lbl_8059CE48;
extern "C" int lbl_805A00F0;
extern "C" Obj* lbl_805A00F8;
extern "C" int lbl_805A00FC;

// TODO: clean up, give variables real names
extern "C" void fn_8005A238(gfTask* t) {
    if (lbl_805A00FC) {
        if (lbl_805A00F8) { dropInstance__14gfSysRecorder2Fv(); lbl_805A00F8 = 0; }
        lbl_805A00F0 = 0;
        exit__6gfTaskFv(t);
        return;
    }
    switch (t->s) {
    case 0: {
        if (!isEnable__14gfSysRecorder2Fi(0)) { t->s = 3; return; }
        lbl_805A00F8 = createInstance__14gfSysRecorder2FQ25Heaps8HeapType(lbl_8059CE48);
        if (!lbl_805A00F8) { t->s = 3; return; }
        int r = getBuffer__14gfSysRecorder2Fi(lbl_805A00F8, 0);
        if (!r) { t->s = 3; return; }
        getSaveData__10GameGlobalFPv(g_GameGlobal, r);
        if (!saveSystem__14gfSysRecorder2Fv(lbl_805A00F8)) { t->s = 3; return; }
        t->s = 1; t->c = 0;
        break;
    }
    case 1:
        t->c++;
        if (!lbl_805A00F8->f0) t->s = 2;
        break;
    case 2:
        switch (lbl_805A00F8->a0) {
        case 3:
        default: t->s = 3; break;
        case 1: t->s = 3; break;
        case 2: t->s = 3; break;
        case 0: t->s = 3; break;
        }
        break;
    case 3:
        if (lbl_805A00F8) { dropInstance__14gfSysRecorder2Fv(); lbl_805A00F8 = 0; }
        exit__6gfTaskFv(t);
        break;
    }
}
}

namespace jj_8005A470 {

struct Obj { char pad[0xA0]; int a0; char pad2[0x4C]; int f0; };
struct Arg { char pad[0x8]; u32 state; int cnt; u8 flag; };

extern "C" {
void dropInstance__14gfSysRecorder2Fv();
int isEnable__14gfSysRecorder2Fi(int);
Obj* createInstance__14gfSysRecorder2FQ25Heaps8HeapType(int);
int getBuffer__14gfSysRecorder2Fi(Obj*, int);
int saveSystem__14gfSysRecorder2Fv(Obj*);
int saveWifi__14gfSysRecorder2Fv(Obj*);
void getSaveData__10GameGlobalFPv(int, int);
}
extern "C" int g_GameGlobal;
extern "C" int lbl_8059CE48;
extern "C" gfTask* lbl_805A00F0;
extern "C" Obj* lbl_805A00F8;

// TODO: clean up, give variables real names
extern "C" void fn_8005A470(Arg* arg) {
    gfTask* t = lbl_805A00F0;
    if (t != 0) {
        if (lbl_805A00F8) {
            dropInstance__14gfSysRecorder2Fv();
            lbl_805A00F8 = 0;
        }
        lbl_805A00F0 = 0;
        t->exit();
        return;
    }
    u32 state = arg->state;
    switch (state) {
    case 0: {
        if (!isEnable__14gfSysRecorder2Fi(0)) {
            arg->state = 3;
            return;
        }
        lbl_805A00F8 = createInstance__14gfSysRecorder2FQ25Heaps8HeapType(lbl_8059CE48);
        if (!lbl_805A00F8) {
            arg->state = 3;
            return;
        }
        int r = getBuffer__14gfSysRecorder2Fi(lbl_805A00F8, 0);
        if (!r) {
            arg->state = 3;
            return;
        }
        getSaveData__10GameGlobalFPv(g_GameGlobal, r);
        if (!saveSystem__14gfSysRecorder2Fv(lbl_805A00F8)) {
            arg->state = 3;
            return;
        }
        arg->state = 1;
        arg->cnt = 0;
        return;
    }
    case 1:
    case 4:
        arg->cnt++;
        if (lbl_805A00F8->f0 == 0) {
            if ((s32)state == 1) {
                arg->state = 2;
            } else {
                arg->state = 5;
            }
        }
        return;
    case 2:
    case 5:
        if ((s32)state == 2) {
            arg->state = 3;
        } else {
            arg->state = 6;
        }
        return;
    case 3:
        if (ntFriendInfo::isSaveNeeded() != 0) {
            if (lbl_805A00F8 == 0) {
                lbl_805A00F8 = createInstance__14gfSysRecorder2FQ25Heaps8HeapType(lbl_8059CE48);
                if (!lbl_805A00F8) {
                    arg->state = 6;
                    return;
                }
            }
            int p = getBuffer__14gfSysRecorder2Fi(lbl_805A00F8, 2);
            if (!p) {
                arg->state = 6;
                return;
            }
            int size = ntFriendInfo::getSaveSize();
            void* src = 0;
            if (!ntFriendInfo::getSaveHeader(&src)) {
                arg->state = 6;
                return;
            }
            memcpy((void*)p, src, size);
            if (!saveWifi__14gfSysRecorder2Fv(lbl_805A00F8)) {
                arg->state = 6;
                return;
            }
            arg->cnt = 0;
            arg->state = 4;
            return;
        }
        /* fall through */
    case 6:
        if (lbl_805A00F8 != 0) {
            dropInstance__14gfSysRecorder2Fv();
            lbl_805A00F8 = 0;
        }
        arg->state = 7;
        return;
    case 7:
        arg->flag = 1;
        return;
    default:
        return;
    }
}
}

#include <sr/sr_common.h>
// TODO: clean up, give variables real names
namespace jj_8005A0E4 {

extern "C" void __ct__6gfTaskFPCcQ26gfTask8Categoryiib(void*, const char*, int, int, int, bool);
extern "C" char lbl_8042E8A8[];
extern "C" char lbl_8042E8FC[];
extern "C" int lbl_8059CE48;
extern "C" gfTask* lbl_805A00F0;
extern "C" gfTask* lbl_805A00F4;

extern "C" gfTask* create__13gmSysSaveTaskFi(int heap) {
    if (lbl_805A00F4) return 0;
    ((unsigned char*)g_GameGlobal->m_appData)[0x28] |= 0x20;
    lbl_8059CE48 = heap;
    gfTask* t = (gfTask*)operator new(0x48, (Heaps::HeapType)heap);
    if (t) {
        __ct__6gfTaskFPCcQ26gfTask8Categoryiib(t, lbl_8042E8A8, 0, 14, 6, true);
        *(char**)((char*)t + 0x3c) = lbl_8042E8FC;
        *(int*)((char*)t + 0x40) = -1;
        *(int*)((char*)t + 0x44) = 0;
        lbl_805A00F0 = t;
    }
    lbl_805A00F4 = t;
    t->init();
    return lbl_805A00F4;
}
}

