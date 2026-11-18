#include <gf/gf_task.h>
#include <string.h>
#include <types.h>
#include <gm/gm_global.h>
#include <gm/gm_syssave_task.h>

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
extern unsigned char lbl_8042E8FC[0x64];
extern int lbl_805A00F0;
extern int lbl_805A00F4;
extern Data28** g_GameGlobal;

void __dl__FPv(void*);
void __dt__6gfTaskFv(void*, int);
}

struct Task {
    unsigned char padding[0x3C];
    void* unk3C;
};

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
Obj* fn_80030548(int);
int fn_800305B0(int);
void fn_800307B4();
int fn_80030DEC(Obj*, int);
int fn_80030F2C(Obj*);
void fn_8004EEC8(int, int);
}
extern int g_GameGlobal;
extern int lbl_8059CE48;
extern int lbl_805A00F0;
extern Obj* lbl_805A00F8;
extern int lbl_805A00FC;

extern "C" void fn_8005A238(gfTask* t) {
    if (lbl_805A00FC) {
        if (lbl_805A00F8) { fn_800307B4(); lbl_805A00F8 = 0; }
        lbl_805A00F0 = 0;
        exit__6gfTaskFv(t);
        return;
    }
    switch (t->s) {
    case 0: {
        if (!fn_800305B0(0)) { t->s = 3; return; }
        lbl_805A00F8 = fn_80030548(lbl_8059CE48);
        if (!lbl_805A00F8) { t->s = 3; return; }
        int r = fn_80030DEC(lbl_805A00F8, 0);
        if (!r) { t->s = 3; return; }
        fn_8004EEC8(g_GameGlobal, r);
        if (!fn_80030F2C(lbl_805A00F8)) { t->s = 3; return; }
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
        if (lbl_805A00F8) { fn_800307B4(); lbl_805A00F8 = 0; }
        exit__6gfTaskFv(t);
        break;
    }
}
}

namespace jj_8005A470 {

struct Obj { char pad[0xA0]; int a0; char pad2[0x4C]; int f0; };
struct Arg { char pad[0x8]; u32 state; int cnt; u8 flag; };

extern "C" {
void fn_800307B4();
int fn_800305B0(int);
Obj* fn_80030548(int);
int fn_80030DEC(Obj*, int);
int fn_80030F2C(Obj*);
int fn_80030F58(Obj*);
void fn_8004EEC8(int, int);
int fn_8014C61C();
int fn_8014C754();
int fn_8014C75C(int*);
}
extern int g_GameGlobal;
extern int lbl_8059CE48;
extern gfTask* lbl_805A00F0;
extern Obj* lbl_805A00F8;

extern "C" void fn_8005A470(Arg* arg) {
    gfTask* t = lbl_805A00F0;
    if (t != 0) {
        if (lbl_805A00F8) {
            fn_800307B4();
            lbl_805A00F8 = 0;
        }
        lbl_805A00F0 = 0;
        t->exit();
        return;
    }
    u32 state = arg->state;
    switch (state) {
    case 0: {
        if (!fn_800305B0(0)) {
            arg->state = 3;
            return;
        }
        lbl_805A00F8 = fn_80030548(lbl_8059CE48);
        if (!lbl_805A00F8) {
            arg->state = 3;
            return;
        }
        int r = fn_80030DEC(lbl_805A00F8, 0);
        if (!r) {
            arg->state = 3;
            return;
        }
        fn_8004EEC8(g_GameGlobal, r);
        if (!fn_80030F2C(lbl_805A00F8)) {
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
        if (fn_8014C61C() != 0) {
            if (lbl_805A00F8 == 0) {
                lbl_805A00F8 = fn_80030548(lbl_8059CE48);
                if (!lbl_805A00F8) {
                    arg->state = 6;
                    return;
                }
            }
            int p = fn_80030DEC(lbl_805A00F8, 2);
            if (!p) {
                arg->state = 6;
                return;
            }
            int size = fn_8014C754();
            int src = 0;
            if (!fn_8014C75C(&src)) {
                arg->state = 6;
                return;
            }
            memcpy((void*)p, (void*)src, size);
            if (!fn_80030F58(lbl_805A00F8)) {
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
            fn_800307B4();
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

