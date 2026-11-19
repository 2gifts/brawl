#include <gf/gf_application.h>
#include <gf/gf_archive.h>
#include <gf/gf_scene.h>
#include <gf/gf_task.h>
#include <mu/mu_boot_pad.h>
#include <mu/mu_controller_operator.h>
#include <mu/mu_esrb.h>
#include <mu/mu_now_loading.h>
#include <mu/mu_strap.h>
#include <revolution/sc.h>
#include <sr/sr_common.h>
#include <types.h>

// The strap scene of the boot (health and safety screens, controller notice, rating notice).
class scStrap : public gfScene {
    gfArchive* m_archive;
    muStrapTask* m_strapTask;
    muControllerOperatorTask* m_controllerTask;
    muESRBTask* m_esrbTask;
    muNowLoadingTask* m_nowLoadingTask;
    muBootPad* m_pad;
    u8 m_language;
    s32 m_0x24;
    s32 m_0x28;
    u8 m_0x2c;

public:
    static scStrap* create();
    scStrap(const char* name) : gfScene(name), m_archive(nullptr), m_strapTask(nullptr), m_controllerTask(nullptr), m_esrbTask(nullptr), m_nowLoadingTask(nullptr), m_pad(nullptr) { }
    virtual void start();
    virtual s32 process();
    virtual s32 exit();
    virtual void setModule();
    void initialize();
    void release();
};

scStrap* scStrap::create() {
    return new (Heaps::GlobalMode) scStrap("scStrap");
}

s32 scStrap::exit() {
    return 0;
}

void scStrap::setModule() {
    setLoadModule("sora_menu_title.rel", false);
}

void scStrap::release() {
    delete m_archive;
    m_archive = nullptr;
    if (m_strapTask) {
        m_strapTask->release();
        m_strapTask->exit();
        m_strapTask = nullptr;
    }
    if (m_controllerTask) {
        m_controllerTask->release();
        m_controllerTask->exit();
        m_controllerTask = nullptr;
    }
    if (m_esrbTask) {
        m_esrbTask->release();
        m_esrbTask->exit();
        m_esrbTask = nullptr;
    }
    if (m_nowLoadingTask) {
        m_nowLoadingTask->release();
        m_nowLoadingTask->exit();
        m_nowLoadingTask = nullptr;
    }
    delete m_pad;
    m_pad = nullptr;
}

// TODO: clean up, give variables real names
namespace jj_F61C {
extern "C" int SCGetLanguage();
extern void* g_gfApplication;
extern void* lbl_1_bss_20;
extern void* lbl_1_data_4B08;

extern "C" void fn_1_F61C(void* arg0) {
    int zero = 0;
    *(int*)((char*)arg0 + 0xC) = zero;
    *(int*)((char*)arg0 + 0x10) = zero;
    *(int*)((char*)arg0 + 0x14) = zero;
    *(int*)((char*)arg0 + 0x18) = zero;
    *(int*)((char*)arg0 + 0x1C) = zero;
    *(char*)((char*)arg0 + 0x20) = (char)SCGetLanguage();
    *(int*)((char*)arg0 + 0x24) = 2;
    *(int*)((char*)arg0 + 0x28) = zero;
    int v = *(int*)((char*)g_gfApplication + 0xE4) >> 24;
    if (v == 1) {
        *(char*)((char*)arg0 + 0x2C) = 1;
    } else {
        *(char*)((char*)arg0 + 0x2C) = 3;
    }
    char* p = (char*)lbl_1_bss_20;
    *(char*)(p + 0x30) = 0;
    *(void**)(p + 0x0) = lbl_1_data_4B08;
    *(int*)(p + 0x4) = 0;
    *(void**)(p + 0x8) = lbl_1_data_4B08;
    *(int*)(p + 0xC) = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_F6D0 {
struct Self {
    char pad0[8];
    char* p8;
    char* pC;
    char* p10;
    char* p14;
    char* p18;
    char* p1C;
    char pad20[4];
    int f24;
    unsigned int state;
    unsigned char b2C;
};

extern "C" {
void update__9muBootPadFv(void*);
void fn_1_FD24(void*);
void kill__10gfHomeMenuFv(void*);
void createData__11muStrapTaskFP9gfArchive(void*, void*);
void createData__24muControllerOperatorTaskFP9gfArchive(void*, void*);
void createData__10muESRBTaskFP9gfArchive(void*, void*);
void createData__16muNowLoadingTaskFP9gfArchive(void*, void*);
void* getInstance__16CameraControllerFv(void);
void changeInput__16CameraControllerFQ216CameraController4Kind(void*, int);
void endKeepScreen__17gfKeepFrameBufferFv(void*);
void removeModel__11muStrapTaskFv(void*);
void removeModel__24muControllerOperatorTaskFv(void*);
void removeModel__10muESRBTaskFv(void*);
void removeModel__16muNowLoadingTaskFv(void*);
void addModel__11muStrapTaskFv(void*);
unsigned int isNextMode__11muStrapTaskFv(void*);
unsigned int isPressStrapSkipButton__9muBootPadFv(void*);
unsigned int isPressStartButton__9muBootPadFv(void*);
void setNextMode__11muStrapTaskFv(void*);
void addModel__24muControllerOperatorTaskFv(void*);
unsigned int isNextMode__24muControllerOperatorTaskFv(void*);
unsigned int isPressBootSkipButton__9muBootPadFv(void*);
void setLastFrame__24muControllerOperatorTaskFv(void*);
void setNextMode__24muControllerOperatorTaskFv(void*);
void addModel__10muESRBTaskFv(void*);
unsigned int isNextMode__10muESRBTaskFv(void*);
void setNextMode__10muESRBTaskFv(void*);
void addModel__16muNowLoadingTaskFv(void*);
void start__10gfHomeMenuFv(void*);
unsigned int setBan__10gfHomeMenuFUlUl(void*, unsigned int, unsigned int);
void release__7scStrapFv(void*);
void fn_1_4A44(void*, int, int, int);
void* fn_80024790(void*);
void* getInstance__14gfSceneManagerFv(void);
extern char* g_gfApplication;
extern void* g_gfHomeMenu;
extern char* lbl_1_bss_20;
extern char lbl_804221E8;
}

extern "C" int fn_1_F6D0(Self* self) {
    if (self->p1C != 0) {
        update__9muBootPadFv(self->p1C);
    }
    switch (self->state) {
    case 0:
        fn_1_FD24(self);
        if (self->p10 == 0) {
            kill__10gfHomeMenuFv(g_gfHomeMenu);
        }
        self->state = 1;
        break;
    case 1:
        if ((((unsigned char*)self->p8)[9] >> 7) & 1) {
            self->state = 2;
        }
        break;
    case 2:
        if (self->pC != 0) {
            createData__11muStrapTaskFP9gfArchive(self->pC, self->p8);
        }
        if (self->p10 != 0) {
            createData__24muControllerOperatorTaskFP9gfArchive(self->p10, self->p8);
        }
        if (self->p14 != 0) {
            createData__10muESRBTaskFP9gfArchive(self->p14, self->p8);
        }
        if (self->p18 != 0) {
            createData__16muNowLoadingTaskFP9gfArchive(self->p18, self->p8);
        }
        changeInput__16CameraControllerFQ216CameraController4Kind(getInstance__16CameraControllerFv(), 9);
        if ((*(int*)(g_gfApplication + 0xE4) >> 24) == 1) {
            self->state = 0xF;
        } else {
            self->state = 3;
        }
        break;
    case 3:
        if (self->pC != 0) {
            self->state = 4;
        } else {
            self->state = 7;
        }
        break;
    case 4:
        endKeepScreen__17gfKeepFrameBufferFv(g_gfApplication + 0xD0);
        if (self->pC != 0) {
            removeModel__11muStrapTaskFv(self->pC);
        }
        if (self->p10 != 0) {
            removeModel__24muControllerOperatorTaskFv(self->p10);
        }
        if (self->p14 != 0) {
            removeModel__10muESRBTaskFv(self->p14);
        }
        if (self->p18 != 0) {
            removeModel__16muNowLoadingTaskFv(self->p18);
        }
        addModel__11muStrapTaskFv(self->pC);
        *(int*)((char*)self->pC + 0x1D4) = 1;
        self->state = 5;
        break;
    case 5:
        if (isNextMode__11muStrapTaskFv(self->pC) == 1 && isPressStrapSkipButton__9muBootPadFv(self->p1C) == 1) {
            if (self->f24 != 0) {
                if (isPressStartButton__9muBootPadFv(self->p1C) == 1) {
                    self->f24 = 0;
                } else {
                    self->f24 = 1;
                }
            }
            setNextMode__11muStrapTaskFv(self->pC);
        }
        if (*(int*)((char*)self->pC + 0x1D4) == 0xB) {
            self->state = 6;
        }
        break;
    case 6:
        self->state = 7;
        break;
    case 7:
        if (self->p10 != 0) {
            self->state = 8;
        } else {
            self->state = 0xB;
        }
        break;
    case 8:
        endKeepScreen__17gfKeepFrameBufferFv(g_gfApplication + 0xD0);
        if (self->pC != 0) {
            removeModel__11muStrapTaskFv(self->pC);
        }
        if (self->p10 != 0) {
            removeModel__24muControllerOperatorTaskFv(self->p10);
        }
        if (self->p14 != 0) {
            removeModel__10muESRBTaskFv(self->p14);
        }
        if (self->p18 != 0) {
            removeModel__16muNowLoadingTaskFv(self->p18);
        }
        addModel__24muControllerOperatorTaskFv(self->p10);
        *(int*)((char*)self->p10 + 0x4C) = 1;
        self->state = 9;
        break;
    case 9:
        if (isNextMode__24muControllerOperatorTaskFv(self->p10) == 1 && isPressBootSkipButton__9muBootPadFv(self->p1C) == 1) {
            if (self->f24 != 0) {
                if (isPressStartButton__9muBootPadFv(self->p1C) == 1) {
                    self->f24 = 0;
                } else {
                    self->f24 = 1;
                }
            }
            setLastFrame__24muControllerOperatorTaskFv(self->p10);
            setNextMode__24muControllerOperatorTaskFv(self->p10);
        }
        if (*(int*)((char*)self->p10 + 0x4C) == 6) {
            self->state = 0xA;
        }
        break;
    case 10:
        self->state = 0xB;
        break;
    case 11:
        if (self->p14 != 0) {
            self->state = 0xC;
        } else {
            self->state = 0xF;
        }
        break;
    case 12:
        endKeepScreen__17gfKeepFrameBufferFv(g_gfApplication + 0xD0);
        if (self->pC != 0) {
            removeModel__11muStrapTaskFv(self->pC);
        }
        if (self->p10 != 0) {
            removeModel__24muControllerOperatorTaskFv(self->p10);
        }
        if (self->p14 != 0) {
            removeModel__10muESRBTaskFv(self->p14);
        }
        if (self->p18 != 0) {
            removeModel__16muNowLoadingTaskFv(self->p18);
        }
        addModel__10muESRBTaskFv(self->p14);
        *(int*)((char*)self->p14 + 0x58) = 1;
        self->state = 0xD;
        break;
    case 13:
        if (isNextMode__10muESRBTaskFv(self->p14) == 1 && isPressBootSkipButton__9muBootPadFv(self->p1C) == 1) {
            if (self->f24 != 0) {
                if (isPressStartButton__9muBootPadFv(self->p1C) == 1) {
                    self->f24 = 0;
                } else {
                    self->f24 = 1;
                }
            }
            setNextMode__10muESRBTaskFv(self->p14);
        }
        if (*(int*)((char*)self->p14 + 0x58) == 0xB) {
            self->state = 0xE;
        }
        break;
    case 14:
        self->state = 0xF;
        break;
    case 15:
        if (self->p18 != 0) {
            self->state = 0x10;
        } else {
            self->state = 0x13;
        }
        break;
    case 16:
        if (self->pC != 0) {
            removeModel__11muStrapTaskFv(self->pC);
        }
        if (self->p10 != 0) {
            removeModel__24muControllerOperatorTaskFv(self->p10);
        }
        if (self->p14 != 0) {
            removeModel__10muESRBTaskFv(self->p14);
        }
        if (self->p18 != 0) {
            removeModel__16muNowLoadingTaskFv(self->p18);
        }
        addModel__16muNowLoadingTaskFv(self->p18);
        *(int*)((char*)self->p18 + 0x4C) = 1;
        self->state = 0x11;
        start__10gfHomeMenuFv(g_gfHomeMenu);
        setBan__10gfHomeMenuFUlUl(g_gfHomeMenu, 1, 0);
        break;
    case 17:
        setBan__10gfHomeMenuFUlUl(g_gfHomeMenu, 1, 0);
        if (((unsigned char*)lbl_1_bss_20)[self->b2C + 0xF] != 0) {
            self->state = 0x12;
        }
        break;
    case 18:
        if (setBan__10gfHomeMenuFUlUl(g_gfHomeMenu, 0, 0) == 1) {
            self->state = 0x13;
        }
        break;
    case 19:
        if (self->pC != 0) {
            removeModel__11muStrapTaskFv(self->pC);
        }
        if (self->p10 != 0) {
            removeModel__24muControllerOperatorTaskFv(self->p10);
        }
        if (self->p14 != 0) {
            removeModel__10muESRBTaskFv(self->p14);
        }
        if (self->p18 != 0) {
            removeModel__16muNowLoadingTaskFv(self->p18);
        }
        release__7scStrapFv(self);
        endKeepScreen__17gfKeepFrameBufferFv(g_gfApplication + 0xD0);
        self->state = 0x14;
        break;
    case 20: {
        fn_80024790(&lbl_804221E8);
        int temp = self->f24;
        void* mgr = getInstance__14gfSceneManagerFv();
        *(int*)((char*)mgr + 0x284) = temp;
        *(int*)((char*)mgr + 0x288) = 2;
        break;
    }
    }
    if ((int)self->state >= 4 && self->p10 == 0) {
        fn_1_4A44(lbl_1_bss_20, self->b2C, 0x29, 0x2B);
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_FD24 {
extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__9gfArchiveFv(void*);
void* getInstance__14gfSceneManagerFv();
void* create__11muStrapTaskFv();
void initialize__11muStrapTaskFi(void*, unsigned char);
void* create__24muControllerOperatorTaskFv();
void initialize__24muControllerOperatorTaskFv(void*);
void* create__10muESRBTaskFv();
void initialize__10muESRBTaskFUc(void*, unsigned char);
void* create__16muNowLoadingTaskFv();
void initialize__16muNowLoadingTaskFv(void*);
void* __ct__9muBootPadFv(void*);
void fn_800151FC(void*, const char*, int, int, int, int);
extern char lbl_1_data_4B08[];
}

struct Scene {
    char pad[8];
    void* archive;
    void* taskC;
    void* task10;
    void* task14;
    void* task18;
    void* task1C;
    unsigned char mode;
};

extern "C" void fn_1_FD24(Scene* self) {
    const char* data = lbl_1_data_4B08;
    void* p = __nw__FUlQ25Heaps8HeapType(0x80, 0x2A);
    if (p)
        p = __ct__9gfArchiveFv(p);
    self->archive = p;

    if (*(int*)((char*)getInstance__14gfSceneManagerFv() + 0x280) == 0 ||
        *(int*)((char*)getInstance__14gfSceneManagerFv() + 0x280) == -1) {
        self->taskC = create__11muStrapTaskFv();
        initialize__11muStrapTaskFi(self->taskC, self->mode);
    }

    if (*(int*)((char*)getInstance__14gfSceneManagerFv() + 0x280) == 1 ||
        *(int*)((char*)getInstance__14gfSceneManagerFv() + 0x280) == -2) {
        self->task10 = create__24muControllerOperatorTaskFv();
        initialize__24muControllerOperatorTaskFv(self->task10);
    }

    if (*(int*)((char*)getInstance__14gfSceneManagerFv() + 0x280) == 0 ||
        *(int*)((char*)getInstance__14gfSceneManagerFv() + 0x280) == -1) {
        self->task14 = create__10muESRBTaskFv();
        initialize__10muESRBTaskFUc(self->task14, self->mode);
    }

    if (*(int*)((char*)getInstance__14gfSceneManagerFv() + 0x280) == 0 ||
        *(int*)((char*)getInstance__14gfSceneManagerFv() + 0x280) == -1) {
        self->task18 = create__16muNowLoadingTaskFv();
        initialize__16muNowLoadingTaskFv(self->task18);
    }

    p = __nw__FUlQ25Heaps8HeapType(0x204, 0x2A);
    if (p)
        p = __ct__9muBootPadFv(p);
    self->task1C = p;

    if (self->taskC) {
        switch (self->mode) {
        case 0:
            fn_800151FC(self->archive, data + 0x78, 0x2B, 0, 0, 0xFF);
            break;
        case 2:
        default:
            fn_800151FC(self->archive, data + 0x90, 0x2B, 0, 0, 0xFF);
            break;
        case 3:
            fn_800151FC(self->archive, data + 0xA8, 0x2B, 0, 0, 0xFF);
            break;
        case 4:
            fn_800151FC(self->archive, data + 0xC0, 0x2B, 0, 0, 0xFF);
            break;
        }
    } else {
        fn_800151FC(self->archive, data + 0xD8, 0x2B, 0, 0, 0xFF);
    }
}
}

