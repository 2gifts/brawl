#include <cm/cm_stage_param.h>
#include <nw4r/math/math_arithmetic.h>
#include <gf/gf_pad_system.h>
#include <cm/cm_controller_photo.h>
#include <gf/gf_pad_status.h>
#include <mt/mt_vector.h>
#include <string.h>
#include <types.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_subject.h>

void cmPhotoController::setSeamless(bool seamless)
{
    int result = 1;
    int active = seamless || unk60 > 0;
    if (active == 0) {
        if (unkB4 == 0) result = 0;
    }
    unk50 = result;
}

cmPhotoController::unk28_t cmPhotoController::store()
{
    unk28_t state;
    memset(&state, 0, sizeof(state));
    state.unk0.m_x = unk28_0.unk0;
    state.unk0.m_y = unk28_0.unk4;
    state.unk0.m_z = unk28_0.unk8;
    state.unkC.m_x = unk28_0.unkC;
    state.unkC.m_y = unk28_0.unk10;
    state.unkC.m_z = unk28_0.unk14;
    state.unk18 = unk28_0.unk18;
    state.unk1C.m_x = unk28_0.unk1C;
    state.unk1C.m_y = unk28_0.unk20;
    state.unk1C.m_z = unk28_0.unk24;
    return state;
}

void cmPhotoController::restore(unk28_t state)
{
    unk28_0.unk0 = state.unk0.m_x;
    unk28_0.unk4 = state.unk0.m_y;
    unk28_0.unk8 = state.unk0.m_z;
    unk28_0.unkC = state.unkC.m_x;
    unk28_0.unk10 = state.unkC.m_y;
    unk28_0.unk14 = state.unkC.m_z;
    unk28_0.unk18 = state.unk18;
    unk28_0.unk1C = state.unk1C.m_x;
    unk28_0.unk20 = state.unk1C.m_y;
    unk28_0.unk24 = state.unk1C.m_z;
}

extern const struct {
    u8 unk0;
    u8 unk1;
    u32 unk4[4];
} lbl_804078F8[5], lbl_8040795C[5];

cmPhotoController::unk14_t cmPhotoController::getControl(const gfPadStatus* pad, bool menu)
{
    unk14_t control;
    control.unk0 = 0;
    control.unk4.m_x = control.unk4.m_y = 0.0f;
    control.unkC.m_x = control.unkC.m_y = 0.0f;
    if (unk60 > 0) return control;
    const __typeof__(lbl_804078F8[0])* mapping = lbl_804078F8;
    if (menu) mapping = lbl_8040795C;
    for (int count = 0; count < 5; ++count, ++mapping) {
        u32 buttons = mapping->unk4[pad->m_controllerType];
        if (mapping->unk0) {
            if ((pad->m_buttonsCurrentFrame2.bits & buttons) == buttons &&
                (pad->m_buttonsPressedThisFrame.bits & buttons)) {
                control.unk0 |= mapping->unk1;
            }
        } else {
            if (buttons && (pad->m_buttonsCurrentFrame2.bits & buttons) == buttons) {
                control.unk0 |= mapping->unk1;
            }
        }
    }
    if (!menu) {
        getTrackingControl(pad, &control.unk4);
        getOrbitControl(pad, &control.unkC);
    } else {
        getOrbitControlWithMenu(pad, &control.unkC);
    }
    return control;
}

void cmPhotoController::update(float deltaFrame)
{
    switch (unkC) {
    case 0: updateShot(); break;
    case 1: updateMoveFig(); break;
    }
}

void cmPhotoController::setFigPosList(int figNo, const Vec3f* position)
{
    unk6C[figNo].unk0 = position->m_x;
    unk6C[figNo].unk4 = position->m_y;
    unk6C[figNo].unk8 = position->m_z;
    unk6C[figNo].unkC |= 0x80;
}

void cmPhotoController::removeFigPosList(int figNo)
{
    unk6C[figNo].unkC_0 = 0;
}

void cmPhotoController::updateControl(const gfPadStatus* pad, bool menu)
{
    unk14_t control = getControl(pad, menu);
    unk14_0 = control.unk0;
    __memcpy(&unk18, &control.unk4, sizeof(unk18));
    __memcpy(&unk20, &control.unkC, sizeof(unk20));
}

void cmPhotoController::updateLockFigNo(bool* seamless, int* figNo, const unk14_t* control)
{
    int action;
    if (control->unk0 & 0x10) action = 1;
    else if (control->unk0 & 4) action = 2;
    else {
        action = 0;
        if (control->unk0 & 8) action = 3;
    }
    switch (action) {
    case 0: break;
    case 1: *figNo = -1; break;
    case 2:
        if (*figNo < -1) *figNo = -1;
        do {
            ++*figNo;
            if (*figNo >= 4) {
                *figNo = -1;
                break;
            }
        } while (!unk6C[*figNo].unkC_0);
        break;
    case 3:
        if (*figNo < 0) *figNo = 4;
        do {
            --*figNo;
            if (*figNo < 0) {
                *figNo = -1;
                break;
            }
        } while (!unk6C[*figNo].unkC_0);
        break;
    }
}

void cmPhotoController::updateLockPlayerNo(bool* seamless, int* playerNo, const unk14_t* control)
{
    int action;
    if (control->unk0 & 0x10) action = 1;
    else if (control->unk0 & 4) action = 2;
    else {
        action = 0;
        if (control->unk0 & 8) action = 3;
    }
    if (*seamless && action) {
        *seamless = false;
        return;
    }
    switch (action) {
    case 0: break;
    case 1: *playerNo = -1; break;
    case 2: {
        cmSubject* subject;
        if (*playerNo < -1) *playerNo = -1;
        do {
            int player = ++*playerNo;
            if (player >= 7) {
                *playerNo = -1;
                break;
            }
            u32 disabled = CameraController::getInstance()->unk45;
            if (disabled & (1u << player)) {
                subject = 0;
            } else {
                subject = cmSubjectList::getSubjectByPlayerNo((u8)player);
                bool active = subject && subject->m_isLoupeTarget;
                switch (active) {
                case false: subject = 0; break;
                default: break;
                }
            }
        } while (!subject);
        break;
    }
    case 3: {
        cmSubject* subject;
        if (*playerNo < 0) *playerNo = 7;
        do {
            int player = --*playerNo;
            if (player < 0) {
                *playerNo = -1;
                break;
            }
            u32 disabled = CameraController::getInstance()->unk45;
            if (disabled & (1u << player)) {
                subject = 0;
            } else {
                subject = cmSubjectList::getSubjectByPlayerNo((u8)player);
                bool active = subject && subject->m_isLoupeTarget;
                switch (active) {
                case false: subject = 0; break;
                default: break;
                }
            }
        } while (!subject);
        break;
    }
    }
}

// TODO: clean up, give variables real names
extern "C" void fn_800A6CF4(void) {}

extern "C" void fn_800A7D5C(void) {}

// TODO: clean up, give variables real names
namespace jj_800A8DA8 {
struct A {
    int pad[2];
    int value;
    int pad2[13];
    int arg;
};

extern "C" void fn_800A709C(struct A*, int);

extern "C" void fn_800A8DA8(struct A* a, int v) {
    a->value = v;
    fn_800A709C(a, a->arg);
}
}

namespace jj_800A6B78 {
struct S { char p[0x14]; unsigned char f14; char q[3]; float f18, f1c, f20, f24; char r[0x18]; int f40; char s[0x74]; unsigned char fb8; };
struct Root { char p[0x465]; unsigned char f465; };
extern "C" void fn_800A6CF8(S*);
extern "C" void fn_800A709C(S*, int);
extern "C" const float lbl_805A205C;
extern Root* g_gfSceneRoot;
// TODO: clean up, give variables real names
extern "C" void fn_800A6B78(S* s) {
    fn_800A6CF8(s);
    s->f14 = 0;
    s->f1c = lbl_805A205C;
    s->f18 = lbl_805A205C;
    s->f24 = lbl_805A205C;
    s->f20 = lbl_805A205C;
    s->f40 = -1;
    fn_800A709C(s, -1);
    s->fb8 = 0;
    g_gfSceneRoot->f465 |= 0x20;
}
}

// TODO: clean up, give variables real names
namespace jj_800A6AD4 {
extern "C" void* memset(void*, int, unsigned long);
extern "C" u8 lbl_80454F28[0x14];
extern "C" const float lbl_805A2058;
extern "C" const float lbl_805A205C;

struct Report {
    u32 f00;
    void* f04;
    u32 f08;
    u32 f0c;
    float f10;
    u8 pad14[0x30];
    float f44;
    float f48;
    float f4c;
    u8 f50;
    u8 pad51[3];
    float f54;
    float f58;
    float f5c;
    u32 f60;
    float f64;
    float f68;
    u8 buf6c[0x40];
    u32 fac;
    u32 fb0;
    u32 fb4;
    u8 fb8;
};

// TODO: clean up, give variables real names
extern "C" Report* fn_800A6AD4(Report* self) {
    self->f04 = lbl_80454F28;
    self->f00 = 0;
    self->f08 = 0;
    self->f0c = 0;
    self->f10 = lbl_805A2058;
    self->f44 = lbl_805A205C;
    self->f48 = lbl_805A205C;
    self->f4c = lbl_805A205C;
    self->f50 = 0;
    self->f54 = lbl_805A205C;
    self->f58 = lbl_805A205C;
    self->f5c = lbl_805A205C;
    self->f60 = 0;
    self->f64 = lbl_805A205C;
    self->f68 = lbl_805A205C;
    self->fb8 = 0;
    memset(self->buf6c, 0, 0x40);
    self->fb0 = 0;
    self->fac = 0;
    self->fb4 = 0;
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_800A6BF0 {
typedef unsigned char u8;
typedef unsigned short u16;

struct Camera {
    char pad0[0x60];
    float x, y, z;
    char pad6C[0x54];
    float a, b, c, d, e;
    char padD4[0x24];
    u16 mode;
    u16 flags;
};

struct Node {
    Node* next;
};

typedef void (Node::*Callback)(u8*);
extern Callback lbl_80454F10;

struct Scene {
    char pad[0x465];
    u8 flags;
};

extern Scene* g_gfSceneRoot;

struct Self {
    Camera* camera;
    char pad4[0xA8];
    Node* nodes;
    char padB0[8];
    u8 state;
    char padB9[3];
    float values[8];
};

extern "C" void fn_80018778(Camera*);

// TODO: clean up, give variables real names
extern "C" void fn_800A6BF0(Self* self) {
    Camera* camera = self->camera;
    camera->e = self->values[0];
    camera->x = self->values[1];
    camera->y = self->values[2];
    camera->z = self->values[3];
    camera->flags |= 2;
    camera->a = self->values[4];
    camera->b = self->values[5];
    camera->d = self->values[6];
    camera->flags |= 0x80;
    camera->c = self->values[7];
    camera->flags |= 0x40;
    camera->mode = 0xE1;
    fn_80018778(camera);

    u8 state = self->state;
    Callback callback = lbl_80454F10;
    for (Node* node = self->nodes; node; node = node->next)
        (node->*callback)(&state);

    self->state = 0;
    g_gfSceneRoot->flags &= ~0x20;
}
}

namespace jj_800A8C18 {
extern "C" {
extern void* memset(void*, int, unsigned long);
extern void fn_800A8DB4(void*, void*, void*, int);
extern float lbl_805A20C4;
extern float lbl_805A208C;
union FloatInt { float f; unsigned int i; };
struct Pair { FloatInt x, y; };
struct Result { unsigned char a; Pair p1, p2; };
struct Local {
    unsigned char r_a; char pad1[3];
    unsigned int r_x1, r_y1, r_x2, r_y2;
    unsigned char r_a2; char pad1b[3];
    FloatInt t_x1, t_y1, t_x2, t_y2;
    unsigned char acc_a; char pad2[3];
    Pair p1, p2;
};
inline float clamp(float v, float lo, float hi) {
    float t = nw4r::math::FSelect(v - lo, v, lo);
    return nw4r::math::FSelect(t - hi, hi, t);
}
// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_800A8C18(void* arg0, int arg1) {
    gfPadStatus padStatus;
    Local loc;
    memset(&loc.acc_a, 0, 0x14);
    for (int i = 0; i < 8; i++) {
        g_gfPadSystem->getSysPadStatus(i, &padStatus);
        if (padStatus.m_error != 0) continue;
        fn_800A8DB4(&loc.r_a, arg0, &padStatus, arg1);
        loc.r_a2 = loc.r_a;
        loc.t_x1.i = loc.r_x1;
        loc.t_y1.i = loc.r_y1;
        loc.t_x2.i = loc.r_x2;
        loc.t_y2.i = loc.r_y2;
        loc.acc_a |= loc.r_a;
        loc.p1.x.f += loc.t_x1.f;
        loc.p1.y.f += loc.t_y1.f;
        loc.p2.x.f += loc.t_x2.f;
        loc.p2.y.f += loc.t_y2.f;
    }
    loc.p1.x.f = clamp(loc.p1.x.f, lbl_805A20C4, lbl_805A208C);
    loc.p1.y.f = clamp(loc.p1.y.f, lbl_805A20C4, lbl_805A208C);
    loc.p2.x.f = clamp(loc.p2.x.f, lbl_805A20C4, lbl_805A208C);
    loc.p2.y.f = clamp(loc.p2.y.f, lbl_805A20C4, lbl_805A208C);
    *(Result*)((char*)arg0 + 0x14) = *(Result*)&loc.acc_a;
}
}
}

// TODO: clean up, give variables real names
namespace jj_800A6E78 {


struct Output {
    float f0;
    float f4;
    float f8;
    float fc;
    float f10;
    float f14;
    float f18;
    float f1c;
    float f20;
    float f24;
    float f28;
    float f2c;
    float f30;
    float f34;
    float f38;
    float f3c;
    float f40;
    float f44;
    float f48;
    float f4c;
    float f50;
    float f54;
};

struct Input {
    char pad0[8];
    int mode;
    char padc[0x44];
    unsigned char flag;
    char pad51[0x6f];
    float c0;
    float c4;
    char padc8[0x38];
    float f100;
};

extern "C" {
extern const float lbl_805A205C;
extern const float lbl_805A2064;
extern const float lbl_805A2068;
extern const float lbl_805A206C;
extern const float lbl_805A2070;
extern const float lbl_805A2074;
extern const float lbl_805A2078;
extern const float lbl_805A207C;
extern const float lbl_805A2080;
extern const float lbl_805A2084;
extern const float lbl_805A2088;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
void fn_800A6E78(Output* out, Input* in) {
    if (in->mode == 0 || in->mode == 2) {
        CameraController* c = CameraController::getInstance();
        cmStageParamPaused* p = &c->m_stageCameraParamPaused;

        out->f0 = lbl_805A2064 * (p->m_angle / lbl_805A2068);
        out->f4 = p->m_zoomDefault;
        out->f8 = p->m_zoomIn;
        out->fc = p->m_zoomIn;
        out->f10 = p->m_zoomOUt;
        out->f14 = lbl_805A2064 * (p->m_rotYMin / lbl_805A2068);
        out->f18 = lbl_805A2064 * (p->m_rotYMax / lbl_805A2068);
        out->f1c = lbl_805A2064 * (p->m_rotXMax / lbl_805A2068);
        out->f20 = lbl_805A2064 * (p->m_rotXMin / lbl_805A2068);
        out->f30 = *(float*)((char*)c + 0x100);
        out->f34 = *(float*)((char*)c + 0x104);
        out->f38 = *(float*)((char*)c + 0x108);
        out->f3c = *(float*)((char*)c + 0x10c);

        if (in->mode == 0) {
            out->f24 = p->m_centerPos.m_x;
            out->f28 = *(float*)((char*)c + 0xe8);
            out->f2c = *(float*)((char*)c + 0xec);
            float v = in->flag ? lbl_805A205C : p->m_offsetRange.m_12;
            out->f40 = *(float*)((char*)c + 0xf0);
            out->f44 = v;
            out->f48 = lbl_805A206C;
            float range4;
            float range8;
            range8 = p->m_offsetRange.m_8;
            range4 = p->m_offsetRange.m_4;
            out->f4c = range4;
            out->f50 = range8;
            out->f54 = lbl_805A2070;
        } else {
            out->f24 = in->c0;
            out->f28 = in->c4;
            out->f2c = lbl_805A205C;
            float range0;
            float range12;
            range12 = p->m_offsetRange.m_12;
            range0 = p->m_offsetRange.m_0;
            out->f40 = range0;
            out->f44 = range12;
            out->f48 = lbl_805A205C;
            float range4;
            float range8;
            range8 = p->m_offsetRange.m_8;
            range4 = p->m_offsetRange.m_4;
            out->f4c = range4;
            out->f50 = range8;
            out->f54 = lbl_805A205C;
            out->f10 = in->f100;
        }
    } else {
        out->f0 = lbl_805A2074;
        out->f4 = lbl_805A2068;
        out->f8 = lbl_805A2078;
        out->fc = lbl_805A207C;
        out->f10 = lbl_805A2080;
        out->f14 = lbl_805A205C;
        out->f18 = lbl_805A2084;
        out->f1c = lbl_805A2088;
        out->f20 = lbl_805A2088;
        out->f24 = lbl_805A205C;
        out->f28 = lbl_805A205C;
        out->f2c = lbl_805A205C;

        struct RangeTemp { char padding[0x2C]; cmStageParamPausedOffsetRange range; char tail[0x10]; } stack;
        float y = stack.range.m_12;
        float x = stack.range.m_0;
        out->f40 = x;
        out->f44 = y;
        out->f48 = lbl_805A206C;
        float range4;
        float range8;
        range8 = stack.range.m_8;
        range4 = stack.range.m_4;
        out->f4c = range4;
        out->f50 = range8;
        out->f54 = lbl_805A2070;
    }
}
}
}

#include <gf/gf_camera.h>
#include <stdlib.h>
// TODO: clean up, give variables real names
namespace jj_800A6A78 {
extern "C" char lbl_80454F5C[];

struct Obj {
    int unk0;
    int unk4;
    void* unk8;
};

extern "C" void __ct__15cmPhotoCallBackFv(Obj* p) {
    p->unk0 = 0;
    p->unk8 = lbl_80454F5C;
    p->unk4 = 0;
}
}

// TODO: clean up, give variables real names
namespace jj_800A6A94 {
extern "C" {
extern void __dl__FPv(void*);

void* __dt__15cmPhotoCallBackFv(void* arg0, int arg1) {
    if (arg0 != 0 && arg1 > 0) {
        __dl__FPv(arg0);
    }
    return arg0;
}
}
}

// TODO: clean up, give variables real names
namespace jj_800A6CF8 {
extern "C" double tan(double);
extern "C" const float lbl_805A2060;
struct State {
    gfCamera* camera;
    char pad[0xB8];
    float fov;
    Vec3f pos;
    float rx, ry, distance, rz;
    Vec3f center, vertical, horizontal;
    float depth;
};
extern "C" void fn_800A6CF8(State* s) {
    gfCamera* c = s->camera;
    s->fov = c->unkD0;
    s->pos = c->m_targetPos;
    s->rx = c->m_rot.m_x;
    s->ry = c->m_rot.m_y;
    float distance = c->unkCC;
    s->distance = distance;
    s->rz = c->m_rot.m_z;
    Vec3f center = c->m_centerPos;
    float aspect = *(float*)((char*)c + 0xE4);
    float height = distance * (float)tan(s->fov * lbl_805A2060);
    float width = height * aspect;
    Vec2f extentCopy;
    Vec2f extent;
    __memcpy(&extent.m_x, &width, sizeof(float));
    __memcpy(&extent.m_y, &height, sizeof(float));
    __memcpy(&extentCopy, &extent, sizeof(Vec2f));

    Vec3f vertical = s->pos;
    Vec3f horizontal = s->pos;
    Vec3f base = s->pos;
    vertical.m_x += extentCopy.m_y;
    horizontal.m_x += extentCopy.m_x;
    s->center = center;
    s->vertical = vertical;
    s->horizontal = horizontal;
    s->depth = c->unkCC;
}
}

// TODO: clean up, give variables real names
namespace jj_800A92C4 {


extern "C" const float lbl_805A205C;
extern "C" const float lbl_805A20E4;

struct Obj {
    char pad[8];
    int state;
};

struct Input {
    char pad[4];
    unsigned int buttons;
    char pad2[0x28];
    signed char x, y;
    char pad3[10];
    int type;
};

struct Vec {
    float x, y;
};

extern "C" void getOrbitControl__17cmPhotoControllerFPC11gfPadStatusP5Vec2f(Obj* self, Input* input, Vec* out, int y) {
    int x;

    if (self->state == 2) {
        out->y = lbl_805A205C;
        out->x = lbl_805A205C;
        return;
    }

    switch (input->type) {
    case 0:
        {
            unsigned char a = input->y;
            unsigned char b = input->x;
            x = (signed char)b;
            y = (signed char)a;
        }
        break;
    case 1:
        {
            unsigned char a = input->y;
            unsigned char b = input->x;
            x = (signed char)b;
            y = (signed char)a;
        }
        break;
    case 2:
        {
            unsigned int buttons = input->buttons;
            if (!(buttons & 0x300)) {
                x = 0;
                y = 0;
                if (buttons & 2) x = 127;
                if (buttons & 1) x = -127;
                if (buttons & 4) y = -127;
                if (buttons & 8) y = 127;
            } else {
                x = 0;
                y = 0;
            }
        }
        break;
    case 3:
        {
            unsigned char a = input->y;
            unsigned char b = input->x;
            x = (signed char)b;
            y = (signed char)a;
        }
        break;
    default:
        x = (int)input;
        break;
    }

    int xx = (signed char)x;
    int sign = xx >> 31;
    int v = ((sign ^ xx) - sign) - 30;
    float f;
    if (v > 0) {
        f = (float)v / lbl_805A20E4;
        if ((signed char)x < 0) f = -f;
    } else {
        f = lbl_805A205C;
    }
    out->x = f;

    xx = (signed char)y;
    sign = xx >> 31;
    v = ((sign ^ xx) - sign) - 30;
    if (v > 0) {
        f = (float)v / lbl_805A20E4;
        if ((signed char)y < 0) f = -f;
    } else {
        f = lbl_805A205C;
    }
    out->y = f;
}
}

// TODO: clean up, give variables real names
namespace jj_800A944C {
extern "C" const float lbl_805A205C;
extern "C" const float lbl_805A20E4;
struct Obj { char pad[8]; int state; };
struct Input { char pad[4]; unsigned int buttons; char pad2[0x2a]; signed char x, y; char pad3[8]; int type; };
struct Vec { float x, y; };
extern "C" void getOrbitControlWithMenu__17cmPhotoControllerFPC11gfPadStatusP5Vec2f(Obj* self, Input* input, Vec* out, int y) {
    int x;
    if (self->state == 2) {
        out->y = lbl_805A205C;
        out->x = lbl_805A205C;
        return;
    }
    switch (input->type) {
    case 0:
        { unsigned char a = input->y;
        unsigned char b = input->x;
        x = (signed char)b;
        y = (signed char)a; }
        break;
    case 1:
        { unsigned char a = input->y;
        unsigned char b = input->x;
        x = (signed char)b;
        y = (signed char)a; }
        break;
    case 2: {
        unsigned int buttons = input->buttons;
        if (buttons & 0x40) {
            x = 0;
            y = 0;
            if (buttons & 2) x = 127;
            if (buttons & 1) x = -127;
            if (buttons & 4) y = -127;
            if (buttons & 8) y = 127;
        } else {
            x = 0;
            y = 0;
        }
        break;
    }
    case 3: {
        unsigned int buttons = input->buttons;
        x = 0;
        y = 0;
        if (buttons & 2) x = 127;
        if (buttons & 1) x = -127;
        if (buttons & 4) y = -127;
        if (buttons & 8) y = 127;
        break;
    }
    default:
        x = (int)input;
        break;
    }
    int xx = (signed char)x;
    int sign = xx >> 31;
    int v = ((sign ^ xx) - sign) - 30;
    float f;
    if (v > 0) {
        f = (float)v / lbl_805A20E4;
        if ((signed char)x < 0) f = -f;
    } else f = lbl_805A205C;
    out->x = f;
    xx = (signed char)y;
    sign = xx >> 31;
    v = ((sign ^ xx) - sign) - 30;
    if (v > 0) {
        f = (float)v / lbl_805A20E4;
        if ((signed char)y < 0) f = -f;
    } else f = lbl_805A205C;
    out->y = f;
}
}

