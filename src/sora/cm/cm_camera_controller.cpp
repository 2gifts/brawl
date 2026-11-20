#include <gf/gf_application.h>
#include <gm/gm_global.h>
#include <types.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_adventure.h>
#include <cm/cm_controller_menu_pad.h>
#include <cm/cm_controller_demo.h>
#include <cm/cm_controller_fig_disp1.h>
#include <cm/cm_controller_fighter_target.h>
#include <cm/cm_controller_photo.h>
#include <cm/cm_controller_tech.h>
#include <gf/gf_camera.h>
#include <sr/sr_common.h>

extern "C" void fn_800A6BF0(gfCameraController* controller);
extern "C" void fn_800A6B78(gfCameraController* controller);
extern "C" void fn_8009FCF4(gfCameraController* controller, float value);

class cmMenuFixedController : public gfCameraController {
public:
    void init();
};

void CameraController::changeInput(Kind cameraControllerKind)
{
    if (m_currentControllerKind == AnimationLocked && *(int*)((char*)m_animationController + 0x20) == 0) {
        return;
    }
    if (m_currentControllerKind == Photo && cameraControllerKind != Photo) {
        fn_800A6BF0(m_photoController);
    }

    bool flag = true;
    if (m_currentControllerKind == Demo || cameraControllerKind == Demo) {
        if (*(int*)&unk168[0x20] > 0) {
            flag = false;
        }
    }
    if (flag && cameraControllerKind != Tech && cameraControllerKind != Demo) {
        gfCameraManager* manager = gfCameraManager::getManager();
        if (manager != nullptr) {
            gfCamera& camera = manager->m_cameras[0];
            Vec3f zero(0.0f, 0.0f, 0.0f);
            camera.m_0x9C = zero;
            camera.m_rotationOffset = zero;
        }
    }
    if (flag && (cameraControllerKind == AI || cameraControllerKind == Demo)) {
        if (m_currentControllerKind == Demo || m_currentControllerKind == Tech) {
            fn_8009FCF4(m_aiController, 10.0f);
        }
    }

    m_currentControllerKind = cameraControllerKind;
    *(float*)&unk168[0x1C] = 0.0f;
    m_currentController = getCameraController(cameraControllerKind);

    // MATCH-ONLY: empty cases list every kind so the compiler emits the 0-14 jump table
    switch (m_currentControllerKind) {
    case None:
    case Animation:
    case AI:
        break;
    case Adventure:
        static_cast<cmAdventureController*>(m_adventureController)->initialize();
        break;
    case AdventureFree:
    case AnimationLocked:
        break;
    case FighterTarget:
        static_cast<cmFighterTargetController*>(m_fighterTargetController)->resetTarget();
        break;
    case MenuPad:
        static_cast<cmMenuPadController*>(m_menuPadController)->init();
        break;
    case MenuFixed:
        static_cast<cmMenuFixedController*>(m_menuFixedController)->init();
        break;
    case Photo:
        fn_800A6B78(m_photoController);
        break;
    case Tech:
    case MeleeFixed:
    case Demo:
    case Default:
        break;
    }
}
// MATCH-ONLY: changeInput calls this with bl instead of inlining it
#pragma dont_inline on
gfCameraController* CameraController::getCameraController(Kind kind)
{
    switch (kind) {
    case Animation:
    case AnimationLocked:
        return m_animationController;
    case AI:
        return m_aiController;
    case Adventure:
        return m_adventureController;
    case AdventureFree:
        return m_adventureFreeController;
    case FighterTarget:
        return m_fighterTargetController;
    case Photo:
        return m_photoController;
    case MenuPad:
        return m_menuPadController;
    case MenuFixed:
        return m_menuFixedController;
    case FigDisp1:
        return m_figDisp1Controller;
    case Tech:
        return m_techController;
    case MeleeFixed:
        return m_meleeFixedController;
    case Demo:
        return m_demoController;
    case Default:
        return m_defaultController;
    }
}
#pragma dont_inline reset

gfCamera* CameraController::getCameraInternal()
{
    switch (m_currentControllerKind) {
    case Adventure:
    case AdventureFree:
    case FighterTarget:
        return gfCameraManager::getManager()->getCamera(0);
    default:
        return gfCameraManager::getManager()->getCamera(1);
    }
}

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
// HYPOTHESIS: the CameraController instance pointer (the map leaves this .sbss pair unnamed).
CameraController* lbl_805A0278[2] __attribute__((aligned(4))) = { 0 };

CameraController* CameraController::create()
{
    if (!lbl_805A0278[0]) {
        lbl_805A0278[0] = new (Heaps::System) CameraController();
        lbl_805A0278[0]->init();
    }
    return lbl_805A0278[0];
}

cmPhotoController::~cmPhotoController()
{
}

cmFigDisp1Controller::~cmFigDisp1Controller()
{
}

bool CameraController::isForceEnableCamControll3D()
{
    if (m_currentControllerKind == AI || m_currentControllerKind == MeleeFixed) {
        return false;
    }
    if (m_currentControllerKind == Demo) {
        return static_cast<cmDemoController*>(m_demoController)->isForceCamControl3D();
    }
    return true;
}

unsigned int CameraController::isSubjectSettingFlags()
{
    unsigned int flags = 0;
    if (m_currentControllerKind == Demo && m_demoController != nullptr) {
        switch (static_cast<cmDemoController*>(m_demoController)->m_targetType) {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            flags = 1;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
            flags = 4;
            break;
        case 9:
            flags = 3;
            break;
        }
    }
    return flags | 1;
}

void CameraController::clearDisablePlayerNo(int playerNo)
{
    if ((u32)playerNo < 7) {
        unk45 &= ~(u8)(1 << playerNo);
    }
}

void CameraController::startZoomIn(cmSubject* subject, u16 frame, const Vec2f& rotation, const Vec2f& targetRotation, float zoom, float distance)
{
    cmTechController* controller = static_cast<cmTechController*>(lbl_805A0278[0]->m_techController);
    controller->start(subject, frame, rotation, targetRotation, zoom, distance);
    controller->unk10 = m_currentControllerKind;
    changeInput(Tech);
}

void CameraController::setDisableZoomStart()
{
    if (unk188 == 0 && m_currentControllerKind == Demo && m_aiController != nullptr) {
        fn_8009FCF4(m_aiController, 10.0f);
    }
    ++unk188;
}

void CameraController::setDisableZoomEnd()
{
    if (unk188 > 0) {
        --unk188;
    }
}

bool CameraController::isDisableZoom()
{
    return unk188 > 0;
}

void CameraController::cancelZoomIn(cmSubject* subject)
{
    if (m_currentControllerKind == Tech) {
        cmTechController* controller = static_cast<cmTechController*>(lbl_805A0278[0]->m_techController);
        if (!subject || subject == controller->m_target) {
            controller->resetTarget();
            changeInput(static_cast<Kind>(controller->unk10));
        }
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_8009C618(void) {}

extern "C" void fn_8009CC20(void) {}

extern "C" void fn_8009CC24(void) {}

extern "C" void fn_8009CC28(void) {}

// TODO: clean up, give variables real names
namespace jj_8009D21C {
struct FnSrc {
    u32 a;
    u32 b;
    u32 c;
    f32 d;
    u32 e;
    u32 f;
};

struct FnDst {
    u8 pad[0x110];
    u32 a;
    u32 b;
    u32 c;
    f32 d;
    u32 e;
    u32 f;
};

extern "C" void fn_8009D21C(FnDst* dst, const FnSrc* src)
{
    dst->a = src->a;
    dst->b = src->b;
    dst->c = src->c;
    dst->d = src->d;
    dst->e = src->e;
    dst->f = src->f;
}
}

namespace jj_8009C968 {
typedef unsigned char u8;
typedef int s32;

extern "C" void fn_8009E3D0(void* p);

struct Obj {
    u8 _pad[0x44];
    u8 m_hi : 4;
    u8 m_bit : 1;
    u8 m_lo : 3;
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8009C968(Obj* self, s32 flag) {
    if (flag != 0) {
        fn_8009E3D0((u8*)self + 0x194);
    }
    self->m_bit = flag;
}
}

namespace jj_8009CD94 {
struct CameraController {
    char pad0[0x40];
    int unk40;
};

extern "C" void fn_800A9E58(void*, int, int, int);
extern "C" void changeInput__16CameraControllerFQ216CameraController4Kind(CameraController*, int);
extern "C" void* lbl_805A0278;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8009CD94(CameraController* arg0, int a, int b, int c) {
    char* target = *(char**)((char*)lbl_805A0278 + 0x78);
    fn_800A9E58(target, a, b, c);
    target[0x10] = (char)arg0->unk40;
    changeInput__16CameraControllerFQ216CameraController4Kind(arg0, 0xB);
}
}

namespace jj_8009C61C {
typedef unsigned char u8;
typedef signed char s8;
typedef int s32;
typedef float f32;

struct CameraController {
    char pad0[0x44];
    u8 upperFlags : 4;
    u8 lowerFlags : 4;
    s8 unk45;
    char pad46[0x12];
    void* unk58;
    char pad5C[0x12C];
    s32 unk188;
    f32 unk18C;
    f32 unk190;
};

extern "C" void reset__14cmAIControllerFv(void*);
extern "C" void changeInput__16CameraControllerFQ216CameraController4Kind(CameraController*, int);
extern "C" const f32 lbl_805A1DA0;
extern "C" const f32 lbl_805A1DB8;

// TODO: clean up, give variables real names
extern "C" void fn_8009C61C(CameraController* self) {
    self->lowerFlags = 0;
    self->unk45 = 0;
    void* controller = self->unk58;
    self->unk188 = 0;
    self->unk190 = lbl_805A1DB8;
    self->unk18C = lbl_805A1DA0;
    if (controller != 0) {
        reset__14cmAIControllerFv(controller);
        u8* p = (u8*)self->unk58;
        p[0x94] |= 8;
    }
    changeInput__16CameraControllerFQ216CameraController4Kind(self, 0xE);
}
}

// TODO: clean up, give variables real names
namespace jj_8009C8C0 {
extern "C" s32 fn_8009C8C0(void* arg0) {
    u8* self = (u8*)arg0;
    u8* ptr;
    s32 type;
    s32 result;
    s32 flag;

    type = *(s32*)(self + 0x40);
    result = 1;
    switch (type) {
    case 0xB:
        ptr = *(u8**)(self + 0x78);
        if (ptr != 0) {
            result = !((ptr[0x11] >> 6) & 1);
        }
        break;
    }
    if (result != 0) {
        if (!((self[0x44] >> 3) & 1)) {
            flag = 0;
        } else {
            switch (type) {
            case 2:
            case 0xC:
                ptr = *(u8**)(self + 0x58);
                if (ptr == 0) {
                    flag = 0;
                } else {
                    flag = (ptr[0x94] >> 4) & 1;
                }
                break;
            default:
                flag = 0;
                break;
            }
        }
        if (flag != 0 && (*(u32*)(self + 0x194) & 7)) {
            result = 0;
        }
    }
    return result;
}
}

namespace jj_8009D158 {
struct Block {
    float f;
    unsigned int w[18];
};

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8009D158(char* dst, const Block* src) {
    Block* d = (Block*)(dst + 0xC4);
    d->f = src->f;
    d->w[0] = src->w[0];
    d->w[1] = src->w[1];
    d->w[2] = src->w[2];
    d->w[3] = src->w[3];
    d->w[4] = src->w[4];
    d->w[5] = src->w[5];
    d->w[6] = src->w[6];
    d->w[7] = src->w[7];
    d->w[8] = src->w[8];
    d->w[9] = src->w[9];
    d->w[10] = src->w[10];
    d->w[11] = src->w[11];
    d->w[12] = src->w[12];
    d->w[13] = src->w[13];
    d->w[14] = src->w[14];
    d->w[15] = src->w[15];
    d->w[16] = src->w[16];
    d->w[17] = src->w[17];
}
}

// TODO: clean up, give variables real names
namespace jj_8009CA88 {
class gfCameraController_base { public: char pad[0x4]; };
class gfCameraController : public gfCameraController_base { public: virtual void slot0(float f); };
struct CameraSelf {
    char pad0[0x40]; s32 mode40; u8 flags44; char pad1[0x3];
    gfCameraController* ctrl48; char pad2[0xC]; void* cam58;
    char pad3[0x20]; void* ptr7C; char pad4[0x104]; f32 f184; s32 cnt188;
};
extern "C" {
float getGameFrameDelta__10GameGlobalCFv(void*);
void fn_8009D744(void*, float);
void update__13cmSubjectListFf(float);
void* fn_8009E3F4(void*, float);
void fn_8009EA18(void*, void*);
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8009CA88(CameraSelf* self) {
    s32 r30 = (*(u32*)((char*)g_gfApplication + 0xEC) >> 27) != 0;
    if ((self->flags44 >> 7) & 1) {
        f32 f31 = getGameFrameDelta__10GameGlobalCFv(g_GameGlobal);
        if (r30 == 0) update__13cmSubjectListFf(f31);
        if (self->ptr7C != 0) fn_8009D744(self->ptr7C, f31);
        if (self->mode40 == 7) self->ctrl48->slot0(f31);
        else if (r30 == 0) self->ctrl48->slot0(f31);
        else if (r30 != 0 && (*(u8*)((char*)g_gfApplication + 0xED) & 1))
            self->ctrl48->slot0(f31);
        s32 var;
        if (!((self->flags44 >> 3) & 1)) var = 0;
        else {
            switch (self->mode40) {
            case 12:
            case 2:
                if (!self->cam58) var = 0;
                else var = (((u8*)self->cam58)[0x94] >> 4) & 1;
                break;
            default: var = 0; break;
            }
        }
        if (var != 0) {
            fn_8009E3F4((char*)self + 0x194, f31);
            if (self->cnt188 <= 0)
                fn_8009EA18((void*)((u32)self + 0x194), gfCameraManager::getManager());
        }
        self->f184 = self->f184 + f31;
    }
}
}

namespace jj_8009C38C {
class gfCameraController_base {
public:
    char pad[0x4];
};

class gfCameraController : public gfCameraController_base {
public:
    virtual void slot0();
    virtual void draw();
    virtual void slot2(int);
};

extern "C" {
extern "C" char lbl_80454BF0[];
extern "C" char lbl_8049DEDC[];
extern "C" void* lbl_805A0278;
void __dl__FPv(void*);
void __dt__6gfTaskFv(void*, int);
void __dt__22cmMeleeFixedControllerFv(void*, int);
void* __dt__16cmTechControllerFv(void*, int);
void* __dt__16cmDemoControllerFv(void*, int);
void Clear__Q44nw4r2ut6detail12LinkListImplFv(void*);
}

struct gfTaskObj {
    char pad0[0x3C];
    char* vtbl;
    char pad40[8];
    int unk48;
    void* unk4C;
    void* unk50;
    void* unk54;
    gfCameraController* unk58;
    void* unk5C;
    void* unk60;
    void* unk64;
    gfCameraController* unk68;
    void* unk6C;
    void* unk70;
    gfCameraController* unk74;
    void* unk78;
    char pad7C[4];
    void* unk80;
};

// TODO: clean up, give variables real names
extern "C" gfTaskObj* fn_8009C38C(gfTaskObj* self, int flag) {
    if (self != 0) {
        self->vtbl = lbl_80454BF0;
        self->unk48 = 0;
        if (self->unk4C != 0) {
            void* p = self->unk4C;
            if (p != 0) {
                __dl__FPv(p);
            }
            self->unk4C = 0;
        }
        if (self->unk54 != 0) {
            __dl__FPv(self->unk54);
            self->unk54 = 0;
        }
        if (self->unk58 != 0) {
            gfCameraController* p = self->unk58;
            if (p != 0) {
                p->slot2(1);
            }
            self->unk58 = 0;
        }
        if (self->unk50 != 0) {
            __dl__FPv(self->unk50);
            self->unk50 = 0;
        }
        if (self->unk60 != 0) {
            __dl__FPv(self->unk60);
            self->unk60 = 0;
        }
        if (self->unk64 != 0) {
            __dl__FPv(self->unk64);
            self->unk64 = 0;
        }
        if (self->unk68 != 0) {
            gfCameraController* p = self->unk68;
            if (p != 0) {
                p->slot2(1);
            }
            self->unk68 = 0;
        }
        if (self->unk6C != 0) {
            __dl__FPv(self->unk6C);
            self->unk6C = 0;
        }
        if (self->unk70 != 0) {
            __dl__FPv(self->unk70);
            self->unk70 = 0;
        }
        if (self->unk74 != 0) {
            gfCameraController* p = self->unk74;
            if (p != 0) {
                p->slot2(1);
            }
            self->unk74 = 0;
        }
        if (self->unk78 != 0) {
            __dt__16cmTechControllerFv(self->unk78, 1);
            self->unk78 = 0;
        }
        if (self->unk5C != 0) {
            __dt__22cmMeleeFixedControllerFv(self->unk5C, 1);
            self->unk5C = 0;
        }
        if (self->unk80 != 0) {
            __dt__16cmDemoControllerFv(self->unk80, 1);
            self->unk80 = 0;
        }
        Clear__Q44nw4r2ut6detail12LinkListImplFv(lbl_8049DEDC);
        lbl_805A0278 = 0;
        __dt__6gfTaskFv(self, 0);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

#include <gf/gf_task.h>
#include <cm/cm_stage_param.h>
#include <cm/cm_controller_default.h>
#include <cm/cm_controller_melee_fixed.h>
// TODO: clean up, give variables real names
namespace jj_8009BE7C {

extern "C" {
void __ct__6gfTaskFPCcQ26gfTask8Categoryiib(gfTask*, const char*, gfTask::Category, int, int, bool);
void __ct__29cmStageParamPausedOffsetRangeFv(void*);
void* fn_8009D2DC();
void* create__14cmAIControllerFv();
void* __ct__25cmFighterTargetControllerFv(void*);
void* __ct__21cmAdventureControllerFv(void*);
void* __ct__25cmAdventureFreeControllerFv(void*);
void* fn_800A6AD4(void*);
void* __ct__19cmMenuPadControllerFv(void*);
void* __ct__21cmMenuFixedControllerFv(void*);
void* __ct__20cmFigDisp1ControllerFv(void*);
void* __ct__16cmTechControllerFv(void*);
void* __ct__22cmMeleeFixedControllerFv(void*);
void* __ct__16cmDemoControllerFv(void*);
extern "C" char lbl_8059DC20;
extern "C" char lbl_80454BF0[];
extern "C" char lbl_80454E70[];
extern "C" cmStageParam lbl_80455078;
extern "C" cmStageParamPaused lbl_804550B8;
extern "C" cmStageParamFixed lbl_80455108;
extern "C" char __vt__19cmDefaultController[];
extern "C" const f32 lbl_805A1DA0, lbl_805A1DA4, lbl_805A1DA8, lbl_805A1DAC, lbl_805A1DB0, lbl_805A1DB4, lbl_805A1DB8;
}
struct Flags { u8 a:1; u8 b:1; u8 c:1; u8 d:5; };
struct Pair { u32 a, b; };
struct Triple { u32 a, b, c; };
struct FixedCopy { Triple center; f32 fov; Pair angles; };
struct Words11 { Pair a,b,c,d,e; u32 f; };
struct Quad { Pair a, b; };
struct Words18 { Pair a; u32 b; Pair c,d,e; u32 f; Quad range, tail; };
#define BYTE(o) (*(u8*)((char*)self + (o)))
#define S32(o) (*(s32*)((char*)self + (o)))
#define F32(o) (*(f32*)((char*)self + (o)))
#define PTR(o) (*(void**)((char*)self + (o)))

static inline void init_values(gfTask* self) {
    f32 y;
    f32 z = lbl_805A1DA0;
    F32(0x168) = z;
    F32(0x16C) = z;
    BYTE(0x44) &= ~0x20;
    *(Words11*)((char*)self + 0x84) = *(Words11*)&lbl_80455078;
    *(f32*)((char*)self + 0xb0) = *(f32*)((char*)&lbl_80455078 + 0x2c);
    *(f32*)((char*)self + 0xb4) = *(f32*)((char*)&lbl_80455078 + 0x30);
    *(f32*)((char*)self + 0xb8) = *(f32*)((char*)&lbl_80455078 + 0x34);
    *(f32*)((char*)self + 0xbc) = *(f32*)((char*)&lbl_80455078 + 0x38);
    *(f32*)((char*)self + 0xc0) = *(f32*)((char*)&lbl_80455078 + 0x3c);
    F32(0xc4) = *(f32*)&lbl_804550B8;
    *(Words18*)((char*)self + 0xc8) = *(Words18*)((char*)&lbl_804550B8 + 4);
    f32 lo = lbl_805A1DA4;
    f32 hi = lbl_805A1DA8;
    f32 deadLo = lbl_805A1DAC;
    f32 deadHi = lbl_805A1DB0;
    f32 one = lbl_805A1DB4;
    f32 maximum = lbl_805A1DB8;
    f32 x = F32(0xA4) + z;
    y = F32(0xA8) + z;
    f32 f7 = lo + x;
    f32 f6 = hi + x;
    f32 f5 = hi + y;
    f32 f4 = z + y;
    f32 f3 = deadLo + x;
    f32 f2 = deadHi + x;
    f32 f1 = deadHi + y;
    f32 f0 = lo + y;
    *(FixedCopy*)((char*)self + 0x110) = *(FixedCopy*)&lbl_80455108;
    F32(0x128) = lo;
    F32(0x12C) = hi;
    F32(0x130) = hi;
    F32(0x134) = z;
    F32(0x138) = deadLo;
    F32(0x13C) = deadHi;
    F32(0x140) = deadHi;
    F32(0x144) = lo;
    F32(0x170) = z;
    F32(0x174) = z;
    F32(0x178) = z;
    F32(0x17C) = z;
    F32(0x180) = one;
    F32(0x184) = z;
    S32(0x188) = 0;
    F32(0x190) = maximum;
    F32(0x18C) = z;
    F32(0x148) = f7;
    F32(0x14C) = f6;
    F32(0x150) = f5;
    F32(0x154) = f4;
    F32(0x158) = f3;
    F32(0x15C) = f2;
    F32(0x160) = f1;
    F32(0x164) = f0;
}

extern "C" gfTask* __ct__16CameraControllerFv(gfTask* self) {
    __ct__6gfTaskFPCcQ26gfTask8Categoryiib(self, &lbl_8059DC20, gfTask::Category_Camera, 14, 11, true);
    PTR(0x3C) = lbl_80454BF0;
    S32(0x40) = 2;
    BYTE(0x44) = BYTE(0x44) | 0x80;
    BYTE(0x44) &= ~0x40;
    ((Flags*)((char*)self + 0x44))->d = 0;
    BYTE(0x45) = 0;
    __ct__29cmStageParamPausedOffsetRangeFv((char*)self + 0xF0);
    init_values(self);
    PTR(0x7C) = fn_8009D2DC();
    void* p = operator new(8, Heaps::System);
    if (p) {
        *(s32*)p = 0;
        *(void**)((char*)p + 4) = __vt__19cmDefaultController;
    }
    PTR(0x4C) = p;
    *(void**)PTR(0x4C) = gfCameraManager::getManager();
    p = operator new(0x94, Heaps::System);
    if (p) p = __ct__25cmFighterTargetControllerFv(p);
    PTR(0x54) = p;
    *(void**)PTR(0x54) = gfCameraManager::getManager();
    PTR(0x58) = create__14cmAIControllerFv();
    *(void**)PTR(0x58) = gfCameraManager::getManager();
    p = operator new(0x24, Heaps::System);
    if (p) {
        *(s32*)p = 0;
        *(void**)((char*)p + 4) = lbl_80454E70;
        *(s32*)((char*)p + 8) = 0;
        *(s32*)((char*)p + 0xC) = 0;
        *(s32*)((char*)p + 0x10) = 0;
        *(s32*)((char*)p + 0x14) = 0;
        *(s32*)((char*)p + 0x18) = 0;
        *(s32*)((char*)p + 0x20) = 1;
    }
    PTR(0x50) = p;
    *(void**)PTR(0x50) = gfCameraManager::getManager();
    p = operator new(0x148, Heaps::System);
    if (p) p = __ct__21cmAdventureControllerFv(p);
    PTR(0x60) = p;
    *(void**)PTR(0x60) = gfCameraManager::getManager();
    p = operator new(0x148, Heaps::System);
    if (p) p = __ct__25cmAdventureFreeControllerFv(p);
    PTR(0x64) = p;
    *(void**)PTR(0x64) = gfCameraManager::getManager();
    p = operator new(0x104, Heaps::System);
    if (p) p = fn_800A6AD4(p);
    PTR(0x68) = p;
    *(void**)PTR(0x68) = gfCameraManager::getManager();
    p = operator new(0x28, Heaps::System);
    if (p) p = __ct__19cmMenuPadControllerFv(p);
    PTR(0x6C) = p;
    *(void**)PTR(0x6C) = gfCameraManager::getManager();
    p = operator new(0x20, Heaps::System);
    if (p) p = __ct__21cmMenuFixedControllerFv(p);
    PTR(0x70) = p;
    *(void**)PTR(0x70) = gfCameraManager::getManager();
    p = operator new(0x40, Heaps::System);
    if (p) p = __ct__20cmFigDisp1ControllerFv(p);
    PTR(0x74) = p;
    *(void**)PTR(0x74) = gfCameraManager::getManager();
    p = operator new(0x58, Heaps::System);
    if (p) p = __ct__16cmTechControllerFv(p);
    PTR(0x78) = p;
    *(void**)PTR(0x78) = gfCameraManager::getManager();
    p = operator new(0x8, Heaps::System);
    if (p) p = __ct__22cmMeleeFixedControllerFv(p);
    PTR(0x5C) = p;
    *(void**)PTR(0x5C) = gfCameraManager::getManager();
    p = operator new(0x5C, Heaps::System);
    if (p) p = __ct__16cmDemoControllerFv(p);
    PTR(0x80) = p;
    *(void**)PTR(0x80) = gfCameraManager::getManager();
    S32(0x40) = 14;
    PTR(0x48) = PTR(0x4C);
    BYTE(0x2C) &= ~2;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_8009C9B8 {
typedef unsigned char u8;
typedef unsigned int u32;

extern "C" void setDisablePlayerNo__16CameraControllerFi(u8* p, u32 bit) {
    if (bit >= 7) {
        return;
    }
    p[0x45] = (u8)(p[0x45] | (u8)(1 << bit));
}
}

// TODO: clean up, give variables real names
namespace jj_8009CEA8 {
typedef float f32;
typedef unsigned char u8;

extern "C" void setCameraRange__16CameraControllerFP6Rect2D(u8* p, const f32* s) {
    f32 o3, o2, o1, o0;
    f32 s3, s2, s1, s0;
    f32 y, x;

    x = *(f32*)(p + 0xA4) + *(f32*)(p + 0x168);
    y = *(f32*)(p + 0xA8) + *(f32*)(p + 0x16C);
    s0 = s[0];
    s1 = s[1];
    s2 = s[2];
    s3 = s[3];
    o0 = s0 + x;
    o1 = s1 + x;
    o2 = s2 + y;
    o3 = s3 + y;
    *(f32*)(p + 0x128) = s0;
    *(f32*)(p + 0x12C) = s1;
    *(f32*)(p + 0x130) = s2;
    *(f32*)(p + 0x134) = s3;
    *(f32*)(p + 0x148) = o0;
    *(f32*)(p + 0x14C) = o1;
    *(f32*)(p + 0x150) = o2;
    *(f32*)(p + 0x154) = o3;
}
}

// TODO: clean up, give variables real names
namespace jj_8009CF04 {
typedef unsigned char u8;

extern "C" void setDeadRange__16CameraControllerFP6Rect2D(u8* p, const float* s) {
    float out3, out2, out1, out0;
    float x = *(float*)(p + 0xA4) + *(float*)(p + 0x168);
    float y = *(float*)(p + 0xA8) + *(float*)(p + 0x16C);
    float v3, v2, v1, v0;

    v0 = s[0];
    v1 = s[1];
    v2 = s[2];
    v3 = s[3];

    out0 = v0 + x;
    out1 = v1 + x;
    out2 = v2 + y;
    out3 = v3 + y;

    *(float*)(p + 0x138) = v0;
    *(float*)(p + 0x13C) = v1;
    *(float*)(p + 0x140) = v2;
    *(float*)(p + 0x144) = v3;
    *(float*)(p + 0x158) = out0;
    *(float*)(p + 0x15C) = out1;
    *(float*)(p + 0x160) = out2;
    *(float*)(p + 0x164) = out3;
}
}

// TODO: clean up, give variables real names
namespace jj_8009CF60 {
typedef float f32;

extern "C" void setRangeOffset__16CameraControllerFP5Vec2f(void* arg0, void* arg1) {
    u8* a = (u8*)arg0;
    f32* b = (f32*)arg1;
    f32 f8;
    f32 f9;
    f32 f10;
    f32 f11;
    int r0 = 0;
    if (*(f32*)(a + 0x168) != b[0] || *(f32*)(a + 0x16C) != b[1]) {
        r0 = 1;
    }
    if (r0 != 0) {
        f9 = b[0];
        f8 = b[1];
        f11 = *(f32*)(a + 0xA4) + f9;
        f10 = *(f32*)(a + 0xA8) + f8;
        *(f32*)(a + 0x168) = f9;
        *(f32*)(a + 0x16C) = f8;
        *(f32*)(a + 0x148) = *(f32*)(a + 0x128) + f11;
        *(f32*)(a + 0x14C) = *(f32*)(a + 0x12C) + f11;
        *(f32*)(a + 0x150) = *(f32*)(a + 0x130) + f10;
        *(f32*)(a + 0x154) = *(f32*)(a + 0x134) + f10;
        *(f32*)(a + 0x158) = *(f32*)(a + 0x138) + f11;
        *(f32*)(a + 0x15C) = *(f32*)(a + 0x13C) + f11;
        *(f32*)(a + 0x160) = *(f32*)(a + 0x140) + f10;
        *(f32*)(a + 0x164) = *(f32*)(a + 0x144) + f10;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_8009D014 {
struct Block { unsigned int w[11]; float f[5]; };
struct Obj { char pad[0x84]; Block b; char padc4[0x64]; float a[8]; float out[8]; float x, y; };
extern "C" void setStageParam__16CameraControllerFP12cmStageParam(Obj* p, const Block* q) {
    int changed = 0;
    if (*(float*)&p->b.w[8] != *(const float*)&q->w[8] || *(float*)&p->b.w[9] != *(const float*)&q->w[9]) changed = 1;
    p->b.w[0] = q->w[0]; p->b.w[1] = q->w[1]; p->b.w[2] = q->w[2]; p->b.w[3] = q->w[3];
    p->b.w[4] = q->w[4]; p->b.w[5] = q->w[5]; p->b.w[6] = q->w[6]; p->b.w[7] = q->w[7];
    p->b.w[8] = q->w[8]; p->b.w[9] = q->w[9]; p->b.w[10] = q->w[10];
    p->b.f[0] = q->f[0]; p->b.f[1] = q->f[1]; p->b.f[2] = q->f[2]; p->b.f[3] = q->f[3]; p->b.f[4] = q->f[4];
    if (changed) {
        float y;
        float x = *(float*)&p->b.w[8] + p->x;
        y = *(float*)&p->b.w[9] + p->y;
        p->out[0] = p->a[0] + x;
        p->out[1] = p->a[1] + x;
        p->out[2] = p->a[2] + y;
        p->out[3] = p->a[3] + y;
        p->out[4] = p->a[4] + x;
        p->out[5] = p->a[5] + x;
        p->out[6] = p->a[6] + y;
        p->out[7] = p->a[7] + y;
    }
}
}

