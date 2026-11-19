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
extern void* lbl_805A0278;

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

extern "C" void fn_8009F78C(void*);
extern "C" void changeInput__16CameraControllerFQ216CameraController4Kind(CameraController*, int);
extern const f32 lbl_805A1DA0;
extern const f32 lbl_805A1DB8;

// TODO: clean up, give variables real names
extern "C" void fn_8009C61C(CameraController* self) {
    self->lowerFlags = 0;
    self->unk45 = 0;
    void* controller = self->unk58;
    self->unk188 = 0;
    self->unk190 = lbl_805A1DB8;
    self->unk18C = lbl_805A1DA0;
    if (controller != 0) {
        fn_8009F78C(controller);
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
float fn_8004E94C(void*);
void fn_8009D744(void*, float);
void fn_8009EC5C(float);
void* fn_8009E3F4(void*, float);
void fn_8009EA18(void*, void*);
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_8009CA88(CameraSelf* self) {
    s32 r30 = (*(u32*)((char*)g_gfApplication + 0xEC) >> 27) != 0;
    if ((self->flags44 >> 7) & 1) {
        f32 f31 = fn_8004E94C(g_GameGlobal);
        if (r30 == 0) fn_8009EC5C(f31);
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
extern char lbl_80454BF0[];
extern char lbl_8049DEDC[];
extern void* lbl_805A0278;
void __dl__FPv(void*);
void __dt__6gfTaskFv(void*, int);
void __dt__22cmMeleeFixedControllerFv(void*, int);
void* fn_800A9E0C(void*, int);
void* fn_800AAA98(void*, int);
void fn_8015C304(void*);
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
            fn_800A9E0C(self->unk78, 1);
            self->unk78 = 0;
        }
        if (self->unk5C != 0) {
            __dt__22cmMeleeFixedControllerFv(self->unk5C, 1);
            self->unk5C = 0;
        }
        if (self->unk80 != 0) {
            fn_800AAA98(self->unk80, 1);
            self->unk80 = 0;
        }
        fn_8015C304(lbl_8049DEDC);
        lbl_805A0278 = 0;
        __dt__6gfTaskFv(self, 0);
        if (flag > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

