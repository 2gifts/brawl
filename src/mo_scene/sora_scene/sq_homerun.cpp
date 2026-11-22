#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_homerun.h>
#include <sr/sr_common.h>
#include <types.h>


sqHomerun* sqHomerun::create() {
    return new (Heaps::GlobalMode) sqHomerun("sqHomerun");
}

void sqHomerun::start() {
    if (gfSceneManager::getInstance()->unk31C == 0xff) {
        unk8 = 7;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        unk11 = g_gfApplication->unk110[0] != 0;
    }
    unk12 = 0;
}

void sqHomerun::exit() { }

#include <revolution/GX/GXTypes.h>
// TODO: clean up, give variables real names
namespace jj_34A80 {
struct Class_8758_base { char pad[0x4]; };
class Class_8758 : public Class_8758_base {
public:
    virtual void start__9sqHomerunFv();  // offset 0x8
    virtual void fn_1_34A80();  // offset 0xc
    virtual void exit__9sqHomerunFv();  // offset 0x10
    virtual void fn_1_34DB8();  // offset 0x14
    u32 step;   // 0x08
    s32 step2;  // 0x0C
    u8 b10;     // 0x10
    u8 b11;     // 0x11
    u8 b12;     // 0x12
    u8 b13;     // 0x13
    u8 b14;     // 0x14
    s32 s18;    // 0x18
    s32 s1C;    // 0x1C
    s32 s20;    // 0x20
};

extern "C" {
void fn_1_3D2D8();
s32 gmGetMenuDecisionPad__Fv();
void fn_1_34FA0(Class_8758* self);
void* getInstance__14gfSceneManagerFv();
void setNextScene__14gfSceneManagerFv(void* mgr, const char* name, s32 arg);
void setNextSequence__14gfSceneManagerFv(void* mgr, const char* name, s32 arg);
void gmAddSingleTimeAll__FUlUl(u32 a, s32 b);
extern const char lbl_1_data_86D0[];
extern char* g_gfApplication;
extern char* g_GameGlobal;
}

extern "C" void fn_1_34A80(Class_8758* self) {
    const char* str = lbl_1_data_86D0;
    char* p;
    void* mgr = getInstance__14gfSceneManagerFv();
    s32 cont = 1;
    while (cont) {
        cont = 0;
        switch (self->step) {
        case 0:
            g_gfApplication[0x110] = 0;
            self->step = 8;
            self->step2 = 1;
            self->b10 = 13;
            cont = 1;
            break;
        case 1:
            fn_1_3D2D8();
            g_gfApplication[0x110] = 0;
            setNextScene__14gfSceneManagerFv(mgr, str + 12, 0);
            *(u8*)(*(char**)(g_GameGlobal + 0x10) + 8) = 1;
            *(u8*)(*(char**)(g_GameGlobal + 0x10) + 4) = 8;
            p = *(char**)(g_GameGlobal + 0x10);
            *(u8*)p = gmGetMenuDecisionPad__Fv() + 1;
            self->step = 2;
            break;
        case 2:
            switch (*(s32*)((char*)mgr + 0x284)) {
            case 0:
                g_gfApplication[0x110] = self->b11;
                setNextSequence__14gfSceneManagerFv(mgr, str + 0x20, 0xa);
                self->step = 0xa;
                break;
            case 1:
                self->step = 3;
                cont = 1;
                break;
            }
            break;
        case 3:
            self->fn_1_34DB8();
            self->step = 8;
            cont = 1;
            self->step2 = 4;
            self->b10 = 10;
            break;
        case 4:
            g_gfApplication[0x110] = self->b11;
            setNextScene__14gfSceneManagerFv(mgr, str + 0x30, 0);
            self->step = 5;
            break;
        case 5: {
            g_gfApplication[0x110] = 0;
            s32 n = (self->b14 != 0) + 1;
            gmAddSingleTimeAll__FUlUl(*(u32*)(*(char**)(g_GameGlobal + 0x18) + 0x1384) / 60, n);
            switch (*(s32*)((char*)mgr + 0x284)) {
            case 2:
            default:
                self->b12 = 1;
                self->fn_1_34DB8();
                self->step = 4;
                break;
            case 3:
                self->step2 = 6;
                break;
            case 1:
                if (*(u8*)(*(char**)(g_GameGlobal + 0x18) + 0x1378) == 9) {
                    self->step2 = 1;
                    break;
                }
                {
                    s32 t = self->s18 + 1;
                    self->s18 = t;
                    if (self->b13 == 0 && self->b14 == 0 && self->s1C > t) goto d1c;
                    if (self->b13 != 0 || self->b14 != 0 || self->s20 >= 1) goto d14;
                    self->s18 = 0;
                    self->s20 = self->s20 + 1;
                    goto d1c;
                d14:
                    self->step2 = 6;
                    goto d28;
                d1c:
                    self->step = 4;
                    fn_1_34FA0(self);
                }
                break;
            }
        d28:
            if ((s32)self->step != 4) {
                self->step = 8;
                self->b10 = 13;
            }
            cont = 1;
            break;
        }
        case 6:
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x38, 0xd);
            break;
        case 7:
            self->step = 8;
            self->step2 = 1;
            self->b10 = 13;
            cont = 1;
            break;
        case 8:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x48, self->b10);
            self->step = 9;
            break;
        case 9:
            cont = 1;
            self->step = self->step2;
            break;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_34DB8 {
struct Mode {
 char pad[8];
 u8 f8:6; u8 f8b:2;
 u8 f9:3; u8 f9b:5;
 u8 fa0:1; u8 fa1:1; u8 fa2:1; u8 fa3:1; u8 fa4:4;
 u8 fb0:5; u8 fb1:1; u8 fb2:2;
 u8 fc:1; u8 fc1:7;
 char pad0[6]; u8 f13; char pad1[2]; u8 f16; u8 pad17; u8 f18; u8 pad19; u16 f1a;
 char f1c[4]; int f20; char pad24[12]; int f30; int f34; int f38; int f3c; char pad40[28]; char f5c[4]; char pad60[3]; u8 f63; u8 f64; char pad65[3]; int f68; int f6c; int f70; int f74; int f78; int f7c;
};
extern "C" {
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
void fn_1_350F8(void*);
void fn_1_34FA0(void*);
void sqWriteMetamorphosisHumanFighter__Fv();
void fn_8010F960(u16, void*, void*, int);
extern void* g_GameGlobal;
void fn_1_34DB8(void* arg) {
 Mode* a = *(Mode**)((u8*)g_GameGlobal + 8);
 u8* self = (u8*)arg;
 u8* b = *(u8**)((u8*)g_GameGlobal + 16);
 gmInitGlobalModeMelee__FP17gmGlobalModeMelee(a);
 int count = 0;
 *(u32*)(self + 0x18) = count;
 *(u32*)(self + 0x1C) = count;
 *(u32*)(self + 0x20) = count;
 u8* p = b;
 for (int i = 0; i < 4; i++, p += 0x5c) {
  if (p[0xb9] != 3) ++*(u32*)(self + 0x1c);
 }
 int one = (*(u32*)(self + 0x1C) == 1);
 self[0x13] = one;
 int flag = !one && b[1] == 0;
 self[0x14] = flag;
 if (flag) fn_1_350F8(self); else fn_1_34FA0(self);
 a->f8 = 9; a->f1a = 0x34; a->f13 = self[0x14] != 0;
 a->f9 = 1; a->f20 = 600; a->fc = 0; a->fb1 = 0;
 a->f18 = 0; a->f63 = self[0x13]; a->f64 = self[0x14];
 a->f68 = *(u32*)(self + 0x18); a->f6c = *(u32*)(self + 0x20);
 a->fa3 = 1; a->f70 = 0; a->f74 = 0; a->f78 = 0; a->f7c = 0;
 a->f34 = 0; a->f30 = 0; a->f38 = 0; a->f3c = 0; a->f16 = 0;
 if (self[0x12]) { self[0x12] = 0; sqWriteMetamorphosisHumanFighter__Fv(); }
 fn_8010F960(a->f1a, a->f1c, a->f5c, 0);
}
}
}

// TODO: clean up, give variables real names
namespace jj_350F8 {
extern "C" {
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
int getFighterColorFileNo__6muMenuFii(int, int);
void fn_1_350F8() {
    int costume;
    GameGlobal* gg = g_GameGlobal;
    unsigned char* dest = (unsigned char*)gg->m_modeMelee;
    unsigned char* src = (unsigned char*)gg->m_selCharData;
    unsigned char* d = dest;
    int i = 0;
    int minusone = -1;
    int zero = 0;
    for (; i < 4; i++) {
        int kind = src[0xb8];
        d[0x98] = kind;
        d[0x99] = src[0xb9];
        d[0xa3] = zero;
        costume = src[0xbd];
        int m = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(kind);
        int color;
        if (m == 0x28) {
            color = 0;
        } else {
            color = getFighterColorFileNo__6muMenuFii(m, costume);
        }
        d[0x9d] = color;
        d[0x9e] = src[0xbd];
        d[0xb0] = src[0xd0];
        d[0x9f] = src[0xbf];
        d[0x9a] = i;
        *((signed char*)(d + 0x9c)) = -1;
        src += 0x5c;
        d += 0x5c;
    }
    ((gmGlobalModeMelee*)dest)->m_meleeInitData.m_isTeamAttack = false;
    ((gmGlobalModeMelee*)dest)->m_meleeInitData.m_numPlayers = 3;
}
}
}

