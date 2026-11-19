#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <sq/sq_targetbreak.h>
#include <sr/sr_common.h>
#include <types.h>


sqTargetBreak* sqTargetBreak::create() {
    return new (Heaps::GlobalMode) sqTargetBreak("sqTargetBreak");
}

void sqTargetBreak::start() {
    unk14 = gfSceneManager::getInstance()->unk31C;
    if (unk14 == 0xff) {
        unk8 = 9;
        unk14 = 0;
    } else {
        unk8 = 0;
        g_GameGlobal->initSelCharData();
        unk18 = g_gfApplication->unk110[0] != 0;
    }
    unk19 = 0;
}

void sqTargetBreak::exit() { }

// TODO: clean up, give variables real names
namespace jj_34398 {
struct SixShorts { unsigned short a[6]; };
struct ThreeWords { int a[3]; };
extern "C" {
void setNextScene__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void setNextSequence__14gfSceneManagerFv(gfSceneManager*, const char*, int);
void fn_1_3D2D8();
void fn_1_347F0(void*);
int gmGetMenuDecisionPad__Fv();
void gmAddSingleTimeAll__FUlUl(unsigned int, int);
unsigned int gmCheckTargetBreakHideElement__Fv();
extern char lbl_1_data_85E8[];
extern char* g_GameGlobal;
extern char* g_gfApplication;
}
extern "C" void fn_1_34398(char* self) {
    const char* str = lbl_1_data_85E8;
    char* p;
    gfSceneManager* mgr = gfSceneManager::getInstance();
    int cont = 1;
    while (cont) {
        cont = 0;
        switch (*(unsigned int*)(self + 8)) {
        default:
            *(int*)(self + 8) = 0;
            cont = 1;
            break;
        case 0:
            g_gfApplication[0x110] = 0;
            *(int*)(self + 8) = 1;
            *(int*)(self + 0xc) = 3;
            self[0x10] = 13;
            cont = 1;
            break;
        case 1:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x10, *(unsigned char*)(self + 0x10));
            *(int*)(self + 8) = 2;
            break;
        case 2:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 0xc);
            break;
        case 3: {
            fn_1_3D2D8();
            setNextScene__14gfSceneManagerFv(mgr, str + 0x20, 0);
            (*(char**)(g_GameGlobal + 0x10))[8] = 1;
            (*(char**)(g_GameGlobal + 0x10))[4] = 7;
            p = *(char**)(g_GameGlobal + 0x10);
            p[0] = gmGetMenuDecisionPad__Fv() + 1;
            *(int*)(self + 0x1c) = gmGetMenuDecisionPad__Fv();
            *(int*)(self + 8) = 4;
            break;
        }
        case 4:
            switch (*(int*)((char*)mgr + 0x284)) {
            case 0: *(int*)(self + 8) = 10; break;
            case 1: *(int*)(self + 8) = 5; break;
            }
            cont = 1;
            break;
        case 5:
            fn_1_347F0(self);
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 0xc) = 6;
            self[0x10] = 0;
            break;
        case 6:
            g_gfApplication[0x110] = self[0x18];
            setNextScene__14gfSceneManagerFv(mgr, str + 0x38, 0);
            *(int*)(self + 8) = 7;
            break;
        case 7: {
            g_gfApplication[0x110] = 0;
            gmAddSingleTimeAll__FUlUl(*(unsigned int*)(*(char**)(g_GameGlobal + 0x18) + 0x1384) / 60,
                (*(unsigned char*)(*(char**)(g_GameGlobal + 8) + 9) >> 2) & 7);
            switch (*(int*)((char*)mgr + 0x284)) {
            case 2:
                self[0x19] = 1;
                cont = 1;
                *(int*)(self + 8) = 5;
                break;
            default:
                *(int*)(self + 8) = 1;
                cont = 1;
                *(int*)(self + 0xc) = 8;
                self[0x10] = 13;
                break;
            }
            break;
        }
        case 8: {
            if (*(unsigned char*)(*(char**)(g_GameGlobal + 0x18) + 0x1378) == 5 &&
                gmCheckTargetBreakHideElement__Fv() == 1) {
                char* dst = *(char**)(g_GameGlobal + 0xc);
                char* src = *(char**)(g_GameGlobal + 8) +
                    (((*(unsigned char*)(*(char**)(g_GameGlobal + 0x18) + 0x1f) - 1) == 0) * 0x5c);
                dst[0x00] = src[0x98];
                dst[0x01] = src[0x99];
                dst[0x02] = src[0x9a];
                dst[0x03] = src[0x9b];
                dst[0x04] = src[0x9c];
                dst[0x05] = src[0x9d];
                dst[0x06] = src[0x9e];
                dst[0x07] = src[0x9f];
                dst[0x08] = src[0xa0];
                dst[0x09] = src[0xa1];
                dst[0x0a] = src[0xa2];
                dst[0x0b] = src[0xa3];
                *(SixShorts*)(dst + 0x0c) = *(SixShorts*)(src + 0xa4);
                dst[0x18] = src[0xb0];
                dst[0x19] = src[0xb1];
                dst[0x1a] = src[0xb2];
                dst[0x1b] = src[0xb3];
                dst[0x1c] = src[0xb4];
                dst[0x1d] = src[0xb5];
                dst[0x1e] = src[0xb6];
                dst[0x1f] = src[0xb7];
                *(unsigned short*)(dst + 0x20) = *(unsigned short*)(src + 0xb8);
                *(unsigned short*)(dst + 0x22) = *(unsigned short*)(src + 0xba);
                *(unsigned short*)(dst + 0x24) = *(unsigned short*)(src + 0xbc);
                *(unsigned short*)(dst + 0x26) = *(unsigned short*)(src + 0xbe);
                *(unsigned short*)(dst + 0x28) = *(unsigned short*)(src + 0xc0);
                *(unsigned short*)(dst + 0x2a) = *(unsigned short*)(src + 0xc2);
                *(float*)(dst + 0x2c) = *(float*)(src + 0xc4);
                *(float*)(dst + 0x30) = *(float*)(src + 0xc8);
                *(float*)(dst + 0x34) = *(float*)(src + 0xcc);
                *(float*)(dst + 0x38) = *(float*)(src + 0xd0);
                dst[0x3c] = src[0xd4];
                dst[0x3d] = src[0xd5];
                dst[0x3e] = src[0xd6];
                dst[0x3f] = src[0xd7];
                *(float*)(dst + 0x40) = *(float*)(src + 0xd8);
                *(float*)(dst + 0x44) = *(float*)(src + 0xdc);
                *(float*)(dst + 0x48) = *(float*)(src + 0xe0);
                dst[0x4c] = src[0xe4];
                *(ThreeWords*)(dst + 0x50) = *(ThreeWords*)(src + 0xe8);
                *(int*)(*(char**)(g_GameGlobal + 0x1c) + 0x24) =
                    (int)(*(unsigned char*)(src + 0x9f) - 1);
            }
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x40, 0x10);
            break;
        }
        case 9:
            *(int*)(*(char**)(g_GameGlobal + 0x1c) + 0x24) = *(int*)(self + 0x1c);
            *(int*)(self + 8) = 1;
            cont = 1;
            *(int*)(self + 0xc) = 3;
            self[0x10] = 13;
            break;
        case 10:
            g_gfApplication[0x110] = self[0x18];
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x50, 9);
            *(int*)(self + 8) = 11;
            break;
        }
    }
}
}

// TODO: clean up, give variables real names
namespace jj_347F0 {
struct Mode {
    char pad[8];
    u8 f8 : 6; u8 f8b : 2;
    u8 f9a : 3; u8 f9cnt : 3; u8 f9b : 1; u8 f9c : 1;
    u8 fa0 : 1; u8 fa1 : 5; u8 fa2 : 1; u8 fa3 : 1;
    u8 fb0 : 5; u8 fb1 : 1; u8 fb2 : 2;
    u8 fc0 : 1; u8 fc1 : 6; u8 fc2 : 1;
    u8 pad0d;
    u8 fe0 : 3; u8 fe1 : 5;
    char pad0f[4];
    u8 f13; u8 f14; u8 pad15; u8 f16; u8 pad17[2];
    u16 f1a; u8 f1c; char pad1d[3];
    int f20; char pad24[12];
    int f30; int f34; int f38; int f3c;
};
struct Sel {
    char pad[0x2e];
    u8 e0 : 3; u8 e1 : 5;
};

extern "C" {
struct GG { int a; int b; Mode* mode; int c; Sel* sel; };
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
int exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(int);
int getFighterColorFileNo__6muMenuFiii(int, int);
void sqWriteMetamorphosisHumanFighter__Fv();
void fn_8010F960(u16, void*, void*, int);
extern GG* g_GameGlobal;

void fn_1_347F0(void* arg0) {
    Mode* self = g_GameGlobal->mode;
    Sel* input = g_GameGlobal->sel;
    u8* q = (u8*)input;
    gmInitGlobalModeMelee__FP17gmGlobalModeMelee(self);
    self->f8 = 8;
    self->f9a = 1;
    self->f13 = 0;
    self->fe0 = 0;
    self->fc2 = 1;
    self->fa0 = 1;
    self->f20 = 0;
    self->f9b = 0; self->f9c = 1;
    self->fa3 = 0; self->fa2 = 1;
    self->fc0 = 0;
    self->fb1 = 0;
    self->f16 = 0;
    self->f34 = 0; self->f30 = 0; self->f38 = 0; self->f3c = 0;
    self->f1a = 0x38;
    self->f1c = input->e0;
    self->fe0 = input->e0;
    self->f14 = 3;
    u8* p = (u8*)self;
    int count = 0;
    int n = 0;
    do {
        if (q[0xB9] == 0) {
            p[0x99] = 0;
            u8 k = q[0xB8];
            p[0x98] = k;
            p[0x9C] = 1;
            p[0xA3] = 0;
            int color = q[0xBD];
            int kind = exchangeGmCharacterKind2MuSelchkind__6muMenuFiii(k);
            p[0x9D] = kind == 0x28 ? 0 : getFighterColorFileNo__6muMenuFiii(kind, color);
            ++count;
            p[0x9E] = q[0xBD];
            p[0xB0] = q[0xD0];
            p[0x9F] = q[0xBF];
            p[0x9A] = n;
        } else {
            p[0x99] = 3;
        }
        ++n;
        q += 0x5C;
        p += 0x5C;
    } while (n < 2);
    self->f9cnt = count;
    ((u8*)self)[0x151] = 3;
    ((u8*)self)[0x1AD] = 3;
    if (((u8*)arg0)[0x19]) {
        ((u8*)arg0)[0x19] = 0;
        sqWriteMetamorphosisHumanFighter__Fv();
    }
    fn_8010F960(self->f1a, (u8*)self + 0x1c, (u8*)self + 0x5c, 0);
}
}
}

