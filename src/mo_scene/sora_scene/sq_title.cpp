#include <gf/gf_application.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <mt/mt_prng.h>
#include <revolution/OS.h>
#include <sq/sq_title.h>
#include <sr/sr_common.h>
#include <types.h>


sqTitle* sqTitle::create() {
    return new (Heaps::GlobalMode) sqTitle("sqTitle");
}

void sqTitle::exit() { }

// TODO: clean up, give variables real names
namespace jj_22134 {
struct Scene {
    char pad0[8];
    int state;
    char padC[8];
    int mode;
    int value;
    bool flag;
};

struct Application {
    char pad0[0x110];
    unsigned char flag;
};

struct SceneManager {
    char pad0[0x31C];
    int layout;
};

extern "C" {
    unsigned long OSGetTick();
    void srandi__Fl(long);
    void initSelCharData__10GameGlobalFv(void*);
    SceneManager* getInstance__14gfSceneManagerFv();
    extern void* g_GameGlobal;
    extern Application* g_gfApplication;
    void fn_1_22134(Scene* self);
}

void fn_1_22134(Scene* self) {
    self->state = 2;
    self->value = 0;
    srandi__Fl(OSGetTick());
    initSelCharData__10GameGlobalFv(g_GameGlobal);
    self->flag = g_gfApplication->flag != 0;
    g_gfApplication->flag = 0;
    switch (getInstance__14gfSceneManagerFv()->layout) {
    case 0:
    default:
        self->mode = 0;
        break;
    case 1:
        self->mode = 1;
        break;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_221D0 {
struct LObj {
    char pad[0x5D];
    unsigned char flags;
};
struct LMgr {
    LObj* o[4];
};

extern "C" {
void* getInstance__14gfSceneManagerFv();
void setNextScene__14gfSceneManagerFv(void* mgr, const char* name, int arg);
void setNextSequence__14gfSceneManagerFv(void* mgr, const char* name, int arg);
void fn_1_2268C(void*);
void gmInitGlobalModeMelee__FP17gmGlobalModeMelee(void*);
extern const char lbl_1_data_6D60[];
extern LMgr* g_stLoaderManager;
extern void** g_GameGlobal;
extern char* g_gfApplication;

#define SETF(i) do { LObj* p = g_stLoaderManager->o[i]; if (p != 0) p->flags |= 0x20; } while (0)
#define CLRF(i) do { LObj* p = g_stLoaderManager->o[i]; if (p != 0) p->flags &= ~0x20; } while (0)
#define SETALL() do { SETF(0); SETF(1); SETF(2); SETF(3); } while (0)
#define CLRALL() do { CLRF(0); CLRF(1); CLRF(2); CLRF(3); } while (0)

void fn_1_221D0(char* self) {
    const char* str = lbl_1_data_6D60;
    char* mgr = (char*)getInstance__14gfSceneManagerFv();
    int cont = 1;
    while (cont) {
        cont = 0;
        switch (*(unsigned int*)(self + 8)) {
        default:
            *(int*)(self + 8) = 2;
            *(int*)(self + 0x14) = 1;
        case 0:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x10, *(unsigned char*)(self + 0x10));
            *(int*)(self + 8) = 1;
            break;
        case 1:
            cont = 1;
            *(int*)(self + 8) = *(int*)(self + 0xC);
            break;
        case 2:
            *(int*)(self + 8) = 0;
            cont = 1;
            *(int*)(self + 0xC) = 3;
            *(unsigned char*)(self + 0x10) = 0xD;
            break;
        case 3:
            fn_1_2268C(self);
            SETALL();
            setNextScene__14gfSceneManagerFv(mgr, str + 0x20, *(int*)(self + 0x14));
            *(int*)(self + 8) = 4;
            break;
        case 4:
            switch (*(int*)(mgr + 0x284)) {
            case 0:
                *(int*)(self + 0xC) = 0xC;
                *(unsigned char*)(self + 0x10) = 2;
                gmInitGlobalModeMelee__FP17gmGlobalModeMelee(g_GameGlobal[2]);
                break;
            case 1:
                switch (*(int*)(self + 0x18)) {
                case 0:
                    *(int*)(self + 0xC) = 5;
                    *(int*)(self + 0x18) = *(int*)(self + 0x18) + 1;
                    break;
                case 1:
                    *(int*)(self + 0xC) = 7;
                    *(int*)(self + 0x18) = 0;
                    break;
                }
                *(unsigned char*)(self + 0x10) = 0xD;
                break;
            }
            *(int*)(self + 8) = 0;
            cont = 1;
            CLRALL();
            break;
        case 5:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x28, 1);
            *(int*)(self + 8) = 6;
            break;
        case 6:
            switch (*(int*)(mgr + 0x284)) {
            default:
                *(int*)(self + 8) = 9;
                break;
            case 0:
                *(int*)(self + 8) = 0xC;
                break;
            case 1:
                *(int*)(self + 8) = 2;
                *(int*)(self + 0x14) = 1;
                break;
            }
            cont = 1;
            break;
        case 7:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x30, 0);
            *(int*)(self + 8) = 8;
            break;
        case 8:
            switch (*(int*)(mgr + 0x284)) {
            default:
                *(int*)(self + 8) = 9;
                break;
            case 0:
                *(int*)(self + 8) = 0xC;
                break;
            case 1:
                *(int*)(self + 8) = 2;
                *(int*)(self + 0x14) = 1;
                break;
            }
            cont = 1;
            break;
        case 9:
            *(int*)(self + 8) = 0;
            *(int*)(self + 0xC) = 0xA;
            *(unsigned char*)(self + 0x10) = 0;
            CLRALL();
            cont = 1;
            break;
        case 10:
            setNextScene__14gfSceneManagerFv(mgr, str + 0x40, 0);
            *(int*)(self + 8) = 0xB;
            break;
        case 11:
            switch (*(int*)(mgr + 0x284)) {
            case 2:
                break;
            case 3:
                *(int*)(self + 8) = 0xC;
                break;
            case 1:
                *(int*)(self + 8) = 2;
                *(int*)(self + 0x14) = 1;
                break;
            case 0:
                switch (*(int*)(self + 0x18)) {
                case 1:
                    *(int*)(self + 8) = 2;
                    *(int*)(self + 0x14) = 1;
                    break;
                default:
                    *(int*)(self + 8) = 2;
                    *(int*)(self + 0x14) = 0;
                    break;
                }
                break;
            }
            cont = 1;
            break;
        case 12:
            gmInitGlobalModeMelee__FP17gmGlobalModeMelee(g_GameGlobal[2]);
            CLRALL();
            g_gfApplication[0x110] = *(unsigned char*)(self + 0x1C);
            setNextSequence__14gfSceneManagerFv(mgr, str + 0x48, 0);
            *(int*)(self + 8) = 0xD;
            break;
        }
    }
}
}
}

// TODO: clean up, give variables real names
namespace jj_229A4 {
extern "C" {

extern int lbl_1_bss_17A0[35];
extern int lbl_1_data_6DE8[35];
extern int lbl_1_bss_182C;

int randi__Fl(long);
bool gmCheckChKindUseEnable__F15gmCharacterKind(int);

inline int resolve(int value) {
    switch (value) {
    default:
        return value;
    case 72:
        return randi__Fl(3) * 2 + 29;
    case 73:
        return randi__Fl(2) + 3;
    case 74:
        return randi__Fl(2) + 14;
    }
}

int fn_1_229A4() {
    int result = 62;

    while (result == 62) {
        if (lbl_1_bss_182C >= 35) {
            for (int i = 0; i < 4; ++i) {
                lbl_1_bss_17A0[i] = lbl_1_bss_17A0[i + 31];
            }
            lbl_1_bss_182C = 5;
        }

        int index = randi__Fl(35);
        bool retry = true;
        int value = 0;

        while (retry == true) {
            value = lbl_1_data_6DE8[index];
            retry = false;

            for (int i = 0; i < lbl_1_bss_182C; ++i) {
                if (value == lbl_1_bss_17A0[i]) {
                    retry = true;
                    index = (index + 1) % 35;
                }
            }
        }

        lbl_1_bss_17A0[lbl_1_bss_182C] = value;
        ++lbl_1_bss_182C;

        value = resolve(value);
        if (gmCheckChKindUseEnable__F15gmCharacterKind(value) == true) {
            result = value;
        }
    }

    return result;
}

}
}

// TODO: clean up, give variables real names
namespace jj_22B28 {
extern "C" {
int gmCheckSrStageKind__FQ26Stages11srStageKind(int);
int randi__Fl(long);
extern int lbl_1_bss_1830[31];
extern int lbl_1_bss_18AC;
extern int lbl_1_data_6E74[31];

int fn_1_22B28() {
    int result = 0xFF;

    while (result == 0xFF) {
        if (lbl_1_bss_18AC >= 0x1F) {
            lbl_1_bss_1830[0] = lbl_1_bss_1830[30];
            lbl_1_bss_18AC = 1;
        }

        int index = randi__Fl(0x1F);
        int count = lbl_1_bss_18AC;
        bool retry = true;
        int value = 0;

        while (retry == true) {
            value = lbl_1_data_6E74[index];
            retry = false;

            for (int i = 0; i < count; ++i) {
                if (value == lbl_1_bss_1830[i]) {
                    index = (index + 1) % 31;
                    retry = true;
                }
            }
        }

        lbl_1_bss_1830[count] = value;
        lbl_1_bss_18AC += 1;

        if ((unsigned int)gmCheckSrStageKind__FQ26Stages11srStageKind(value) == 1) {
            result = value;
        }
    }

    return result;
}
}
}

