#include <gf/gf_pad_system.h>
#include <gf/gf_pad_status.h>
#include "gf/gf_pad_system.h"
#include <gm/gm_global.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

// TODO: clean up, give variables real names
namespace jj_80110310 {
extern "C" {
extern "C" int lbl_8045D908[4];
int fn_80110190(int*, int*, int*);

// TODO: clean up, give variables real names
int fn_80110310(int* a, int* b) {
    return fn_80110190(a, b, (int*)&lbl_8045D908);
}
}
}

namespace jj_8011031C {
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8011031C(void* arg0) {
    if (arg0 == 0) return;

    u8* p = (u8*)arg0;
    if (p[1] != 0) {
        p[0x18] = 0x78;
        *(u16*)(p + 0xC) = 0;
        return;
    }

    unsigned val = p[0x1C];
    if ((val >> 5) & 1) return;

    u16 hw = *(u16*)(p + 0xC);
    if (hw == 0) return;

    p[0x1C] = (u8)(val | 0x20);
}
}

namespace jj_8010FF20 {
extern "C" {
void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void*, int, void*);
extern "C" int lbl_8045D908[4];
extern "C" void* g_gfPadSystem;

struct gfPadStatus {
    unsigned int _0x0;
    unsigned int _0x4;
    unsigned int _0x8;
    unsigned int m_buttonsPressedThisFrame;
    unsigned int _0x10;
    unsigned int _0x14;
    unsigned int _0x18;
    unsigned int _0x1c;
    unsigned int _0x20;
    unsigned int _0x24;
    unsigned int _0x28;
    unsigned int _0x2c;
    unsigned int _0x30;
    unsigned int _0x34;
    unsigned int _0x38;
    unsigned int m_controllerType;
};

// TODO: clean up, give variables real names
int fn_8010FF20(unsigned int mask) {
    int i = 0;
    while (i < 8) {
        if (mask & (1 << i)) {
            gfPadStatus status;
            getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, i, &status);
            if (status.m_buttonsPressedThisFrame & lbl_8045D908[status.m_controllerType])
                return 1;
        }
        i++;
    }
    return 0;
}
}
}

namespace jj_8010FFBC {
extern "C" void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void*, int, void*);
extern "C" void* g_gfPadSystem;
extern "C" u32 lbl_8045D918[];

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" s32 fn_8010FFBC(s32 arg0) {
    u8 buf[0x40];
    s32 i;
    for (i = 0; i < 8; i++) {
        if (arg0 & (1 << i)) {
            getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, i, buf);
            if (*(u32*)(buf + 0x0C) & lbl_8045D918[*(u32*)(buf + 0x3C)]) {
                return 1;
            }
        }
    }
    return 0;
}
}

namespace jj_80110058 {
extern "C" {
void getSysPadStatus__11gfPadSystemFiP11gfPadStatus(void*, int, void*);
extern "C" void* g_gfPadSystem;
extern "C" unsigned int lbl_8045D928[];
}

struct PadStatusLocal {
    char pad0[0xC];
    unsigned int buttons;
    char pad1[0x2C];
    unsigned int index;
};

// TODO: clean up, give variables real names
extern "C" s32 fn_80110058(s32 arg0) {
    PadStatusLocal st;
    s32 i;

    for (i = 0; i < 8; i++) {
        if (arg0 & (1 << i)) {
            getSysPadStatus__11gfPadSystemFiP11gfPadStatus(g_gfPadSystem, i, &st);
            if (st.buttons & lbl_8045D928[st.index]) {
                return 1;
            }
        }
    }
    return 0;
}
}

namespace jj_801100F4 {

extern "C" {

extern "C" s32 lbl_8045D938[4];

// TODO: clean up, give variables real names
s32 fn_801100F4(s32 mask) {
    gfPadStatus sp8;
    for (s32 i = 0; i < 8; i++) {
        if (mask & (1 << i)) {
            g_gfPadSystem->getSysPadStatus(i, &sp8);
            if (sp8.m_buttonsPressedThisFrame.bits & lbl_8045D938[sp8.m_controllerType]) {
                return 1;
            }
        }
    }
    return 0;
}

}
}

namespace jj_80110368 {
extern "C" void* g_GameGlobal;
extern "C" int fn_8004D9A8(void* p, int i);
extern "C" void* getGlobalRecordMenuDatap__10GameGlobalFv(void* global);

struct Flags1B {
    unsigned char f7 : 1;
    unsigned char f6 : 1;
    unsigned char rest : 6;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_80110368(unsigned char* p) {
    if (p != 0) {
        if (p[1] != 0 || !((p[0x3E] >> 7) & 1) || p[0x3D] == 0) {
            p[0x1B] = (unsigned char)(p[0x1B] & ~0x40);
            return;
        }
        if (p[0x18] != 0x78) {
            int v = fn_8004D9A8(g_GameGlobal, p[0x18]);
            ((Flags1B*)(p + 0x1B))->f6 = (unsigned char)v;
            return;
        }
        {
            unsigned char* base =
                (unsigned char*)getGlobalRecordMenuDatap__10GameGlobalFv(g_GameGlobal);
            unsigned char idx = p[0x3D];
            int v = base[idx + 0xF];
            ((Flags1B*)(p + 0x1B))->f6 = (unsigned char)v;
        }
    }
}
}

namespace jj_80110410 {


extern "C" void maskMotor__11gfPadSystemFUs(gfPadSystem*, unsigned int);

static inline unsigned int getMask(unsigned char* p) {
    unsigned int mask = 0;

    for (int i = 0; i < 4; ++i) {
        if ((p[0xB3 + i * 0x5C] >> 6) & 1) {
            unsigned int port = p[0xD5 + i * 0x5C];
            if (port != 0) {
                mask |= 1U << (port - 1);
            }
        }
    }

    return mask;
}

// TODO: clean up, give variables real names
extern "C" void fn_80110410(unsigned char* p) {
    if (p != 0) {
        maskMotor__11gfPadSystemFUs(g_gfPadSystem, getMask(p) ^ 0xFF);
    } else {
        maskMotor__11gfPadSystemFUs(g_gfPadSystem, 0xFF);
    }
}
}

namespace jj_80110190 {

// TODO: clean up, give variables real names
extern "C" s32 fn_80110190(s32* arg0, s32* arg1, s32* arg2) {
    gfPadStatus status;
    s32 i;
    u8* p;
    s32 idx;
    s32 j;

    *arg0 = -1;
    *arg1 = -1;
    i = 0;
    while (i < 8) {
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_buttonsPressedThisFrame.bits & arg2[status.m_controllerType]) {
            *arg0 = i;
            break;
        }
        i++;
    }
    idx = *arg0;
    if (idx < 0) {
        return 0;
    }
    j = 0;
    p = (u8*)g_GameGlobal->m_modeMelee;
    while (j < 4) {
        u32 base = 0x99 + j * 0x5C;
        if (p[base] == 0 && ((p[base + 0x3D] >> 7) & 1) && idx == p[base + 0x3C] - 1) {
            *arg1 = j;
            break;
        }
        j++;
    }
    return 1;
}
}

namespace jj_8011058C {


extern "C" int fn_8004D9A8(GameGlobal*, int);
extern "C" void fn_801104C4(void*);
extern "C" void maskMotor__11gfPadSystemFUs(gfPadSystem*, unsigned int);

struct Entry {
    unsigned char pad[0x1b];
    unsigned char a : 1;
    unsigned char enabled : 1;
    unsigned char rest : 6;
    unsigned char pad2[0x21];
    unsigned char port;
    unsigned char active;
    unsigned char pad3[0x1d];
};

static inline unsigned int getMask(Entry* p)
{
    unsigned int mask = 0;

#pragma unroll 4
    for (int i = 0; i < 4; i++) {
        if (p[i].enabled && p[i].port)
            mask |= 1 << (p[i].port - 1);
    }

    return mask;
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8011058C(unsigned char* self)
{
    Entry* d;
    unsigned char* c;
    int i;
    unsigned char* original = self;
    Entry* base = (Entry*)(self + 0x98);

    d = base;
    i = 0;
    c = self;
    do {
        if (d != 0) {
            if (c[0x99] != 0 || !((d->active >> 7) & 1) || c[0xd5] == 0) {
                d->enabled = 0;
            } else if (c[0xb0] != 0x78) {
                d->enabled = fn_8004D9A8(g_GameGlobal, c[0xb0]);
            } else {
                unsigned char* rec =
                    (unsigned char*)g_GameGlobal->getGlobalRecordMenuDatap();
                d->enabled = rec[c[0xd5] + 0xf];
            }
        }
        i++;
        d++;
        c += 0x5c;
    } while (i < 4);

    if (original)
        maskMotor__11gfPadSystemFUs(g_gfPadSystem, getMask(base) ^ 0xff);
    else
        maskMotor__11gfPadSystemFUs(g_gfPadSystem, 0xff);

    fn_801104C4(original);
}
}

namespace jj_8010F960 {

extern "C" {
extern "C" void* lbl_805A01D8;
extern "C" u8 lbl_8045D8A0[];
extern "C" f32 lbl_805A28E8;
extern "C" f32 lbl_805A28EC;
extern "C" f32 lbl_805A28F0;
extern "C" f32 lbl_805A28F4;
extern "C" f32 lbl_805A28F8;
extern "C" f32 lbl_805A28FC;
extern "C" f32 lbl_805A2900;
s32 fn_8007923C(void*, s32, s32, s32);
void fn_801E1D80(s64, OSCalendarTime*);
f32 randf__Fv();
}

struct MeleeView {
    char pad[8];
    u8 gameMode : 6;
    u8 rest : 2;
};

static inline void pick3(u8* mode, f32 p0, f32 p1, f32 p2) {
    f32 total = p2 + (p0 + p1);
    f32 r = total * randf__Fv();
    if (r < p0) {
        *mode = 0;
    } else if (r < (p0 + p1)) {
        *mode = 1;
    } else {
        *mode = 2;
    }
}

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8010F960(s32 kind, u8* mode, s32* id, s32 force) {
    if (force == 0) {
        *id = 0;
        for (s32 i = 0; i < 0x33; i++) {
            if (kind == (s32)lbl_8045D8A0[i * 2]) {
                u8 k = lbl_8045D8A0[i * 2 + 1];
                s32 r = fn_8007923C(lbl_805A01D8, k, 0, 0);
                if (r >= 0) {
                    *id = (k << 24) | r;
                }
                break;
            }
        }
    }

    switch (kind) {
    case 0x15: {
        f32 b;
        f32 a = lbl_805A28E8;
        f32 c = lbl_805A28EC;
        b = c + (a + a);
        f32 r = b * randf__Fv();
        if (r < a) {
            *mode = 0;
        } else if (r < (a + a)) {
            *mode = 1;
        } else {
            *mode = 2;
        }
        break;
    }
    case 0x5: {
        if (g_GameGlobal->m_modeMelee != 0 && ((MeleeView*)g_GameGlobal->m_modeMelee)->gameMode == 7) {
            *mode = 0;
        } else {
            u16 stage = *(u16*)g_GameGlobal->m_stageData;
            u16 newStage;
            switch (stage) {
            case 2:
                newStage = 3;
                *mode = 1;
                break;
            case 3:
                newStage = 1;
                *mode = 0;
                break;
            default:
                if (randf__Fv() < lbl_805A28F0) {
                    *mode = 0;
                    if (stage == 1) {
                        newStage = 2;
                    } else {
                        newStage = 1;
                    }
                } else {
                    newStage = 3;
                    *mode = 1;
                }
                break;
            }
            u8 sub;
            switch (*mode) {
            case 0:
                sub = 4;
                break;
            case 1:
                sub = 5;
                break;
            }
            s32 r = fn_8007923C(lbl_805A01D8, sub, 0, 0);
            if (r >= 0) {
                *id = (sub << 24) | r;
            }
            *(u16*)g_GameGlobal->m_stageData = newStage;
        }
        break;
    }
    case 0x21: {
        if (*mode == 0x10) {
            *mode = 5;
            break;
        }
        OSCalendarTime cal;
        fn_801E1D80(OSGetTime(), &cal);
        s32 hour = cal.hour;
        s32 min = cal.min;
        s32 wday = cal.wday;
        if (hour >= 0x17 || hour <= 3) {
            *mode = 6;
        } else if (hour >= 4 && hour <= 7) {
            *mode = 0;
        } else if (hour >= 8 && hour <= 0xF) {
            *mode = 1;
        } else if (hour >= 0x10 && hour <= 0x12) {
            *mode = 2;
        } else {
            *mode = 3;
        }
        if (wday == 6) {
            if (hour == 0x13 && min >= 0x1E) {
                *mode = 4;
            } else if (hour >= 0x14 && hour <= 0x18) {
                if (hour < 0x17) {
                    *mode = 5;
                } else {
                    *mode = 7;
                }
            }
        }
        u8 t = *mode;
        if (t == 5 || t == 7) {
            f32 r = randf__Fv();
            if (r < lbl_805A28F4) {
                *id = 0xFF0027B9;
            } else if (r < lbl_805A28F8) {
                *id = 0xFF0027BA;
            } else if (r < lbl_805A28F0) {
                *id = 0xFF0027BB;
            } else if (r < lbl_805A28FC) {
                *id = 0xFF0027BC;
            } else if (r < lbl_805A2900) {
                *id = 0xFF0027BD;
            } else {
                *id = 0xFF0027BE;
            }
        }
        break;
    }
    case 0x22: {
        f32 b;
        f32 a = lbl_805A28F0;
        b = a + (a + a);
        f32 r = b * randf__Fv();
        if (r < a) {
            *mode = 0;
        } else if (r < (a + a)) {
            *mode = 1;
        } else {
            *mode = 2;
        }
        break;
    }
    case 0x34:
        *id = 0xFF002708;
        break;
    case 0x35: {
        void* t = g_GameGlobal->m_stageEditData;
        *mode = ((u8*)t)[0x14];
        *id = *(u16*)((u8*)t + 0x18) | 0xFF000000;
        break;
    }
    case 0x36:
        *id = 0xFF002707;
        break;
    case 0x37:
        *id = 0xFF0026FF;
        *mode = 0;
        break;
    case 0x38:
        *id = 0xFF002712;
        break;
    }

    if (force != 0) {
        *id = force | 0xFF000000;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_801104C4 {
extern "C" {

void buttonConfigInit__FP14ipButtonConfig(void*);
void* getButtonConfig__10GameGlobalFi(void*, int);
void reset__11ipPadConfigFv(void*);
void* getInstance__11ipPadConfigFv();
void setButtonConfig__11ipPadConfigFiUlPC14ipButtonConfig(void*, int, unsigned int, void*);
void createPadNo2PlayerNoTable__11ipPadConfigFv(void*);
extern "C" void* g_GameGlobal;

struct PadElem {
    unsigned char pad0[7];
    unsigned char flag;     // 0x07
    unsigned char pad1[0x10];
    unsigned char charId;   // 0x18
    unsigned char pad2[0x24];
    unsigned char count;    // 0x3D
    unsigned char pad3[0x1E];
};

void stSetBottonConfig__FP17gmGlobalModeMelee(void* arg0) {
    unsigned char sp8[0x2D];
    PadElem* p;
    int i;
    int val;
    void* src;

    buttonConfigInit__FP14ipButtonConfig(sp8);
    reset__11ipPadConfigFv(getInstance__11ipPadConfigFv());

    p = (PadElem*)((unsigned char*)arg0 + 0x98);

    for (i = 0; i < 4; i++) {
        if (p->flag == 0) {
            val = -1;
        } else {
            val = p->count - 1;
            if (val < 0) {
                val = i | 0x10;
            }
        }

        if (p->charId != 0x78) {
            src = getButtonConfig__10GameGlobalFi(g_GameGlobal, p->charId);
        } else {
            src = sp8;
        }

        setButtonConfig__11ipPadConfigFiUlPC14ipButtonConfig(getInstance__11ipPadConfigFv(), i, (unsigned int)val, src);
        p++;
    }

    createPadNo2PlayerNoTable__11ipPadConfigFv(getInstance__11ipPadConfigFv());
}

}
}

