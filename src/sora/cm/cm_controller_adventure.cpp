#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_adventure.h>
#include <types.h>

float lbl_80454DD8[8] = { 0.05f, 0.05f, 0.2f, 0.3f, 0.5f, 1.0f, 0.0f, 0.0f };

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0290[0x8] __attribute__((aligned(4))) = { 0 };

void cmAdventureController::setTargetWeightTable(float* table) {
    float f0, f1, f2, f3, f4, f5;
    f5 = table[0];
    f4 = table[1];
    f3 = table[2];
    f2 = table[3];
    f1 = table[4];
    f0 = table[5];
    lbl_80454DD8[0] = f5;
    lbl_80454DD8[1] = f4;
    lbl_80454DD8[2] = f3;
    lbl_80454DD8[3] = f2;
    lbl_80454DD8[4] = f1;
    lbl_80454DD8[5] = f0;
}

void cmAdventureController::resetTarget() {
    searchSubject();
    Vec3f target1 = getTargetPos();
    _5C = target1;
    Vec3f target2 = getTargetPos();
    _50 = target2;
    Vec3f target3 = getTargetPos();
    _2C = target3;
    Vec3f zero(0.0f, 0.0f, 0.0f);
    _80 = zero;
}

cmAdventureFreeController::cmAdventureFreeController() {
    unkB_3 = 1;
}

void cmAdventureController::initialize() {
    _1C = 120.0f;
}

// TODO: clean up, give variables real names
extern "C" void fn_800A5F6C(void) {}

// TODO: clean up, give variables real names
namespace jj_800A59D0 {
struct Foo {
    char pad[0x12c];
    float x;
};

extern "C" void fn_800A59D0(Foo* a, const float* b) {
    a->x = *b;
}
}

namespace jj_800A59DC {
struct Obj {
    char pad[0x18];
    float a;
};

extern "C" {
// TODO: clean up, give variables real names
void fn_800A59DC(Obj* dst, const float* src) {
    dst->a = src[0];
}
}
}

namespace jj_800A41E4 {
// TODO: clean up, give variables real names
extern "C" void fn_800A41E4(void *dst, const void *src)
{
    const unsigned int *s = (const unsigned int *)src;
    unsigned int *d = (unsigned int *)dst;
    d[0] = s[0x11C / 4];
    d[1] = s[0x120 / 4];
    d[2] = s[0x124 / 4];
}
}

namespace jj_800A5990 {
extern "C" float lbl_805A1F90;
extern "C" float lbl_805A1FF8;

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800A5990(char* arg0, float* arg1)
{
    char* r5 = *(char**)arg0;
    if (r5 != 0) {
        float a = *arg1;
        float f1 = lbl_805A1FF8 * a;
        *(float*)(arg0 + 0x134) = lbl_805A1F90;
        *(float*)(arg0 + 0x130) = f1;
        *(float*)(r5 + 0xC0) = f1;
        *(float*)(r5 + 0xC4) = *(float*)(arg0 + 0x134);
    }
}
}

namespace jj_800A4200 {


struct CameraThing {
    char pad[0x60];
    float x;
    float y;
    float z;
    char pad6c[0x8e];
    u16 flags;
};

extern "C" CameraController* getInstance__16CameraControllerFv();

// TODO: clean up, replace the raw offset accesses with a struct with named fields and give variables real names
extern "C" void fn_800A4200(void* self, const void* pos) {
    CameraController* cc;
    CameraThing* t;
    *(float*)((char*)self + 0x11c) = *(const float*)pos;
    *(float*)((char*)self + 0x120) = *((const float*)pos + 1);
    *(float*)((char*)self + 0x124) = *((const float*)pos + 2);
    cc = getInstance__16CameraControllerFv();
    if ((int)cc->m_currentControllerKind == CameraController::Adventure) {
        t = *(CameraThing**)self;
        t->x = *(float*)((char*)self + 0x11c);
        t->y = *(float*)((char*)self + 0x120);
        t->z = *(float*)((char*)self + 0x124);
        t->flags |= 2;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_800A58C0 {
extern "C" const float lbl_805A1F90;
namespace init { extern "C" float lbl_805A1F90; }
struct Vec2 { float x, y; }; struct Vec3 { float x,y,z; };
struct Obj { char pad0[11]; unsigned char flags; char pad1[0x20]; Vec3 a,b,c,d,e; char pad2[0x18]; Vec3 zero; char pad3[0x5c]; float offsetX,offsetY; char pad4[0x20]; Vec3 f,g; };
// TODO: clean up, give variables real names
extern "C" void fn_800A58C0(Obj* p, Vec2* input, int useOffset, float scale) {
 float y=input->y; float x=input->x;
 struct Position { Vec2 xy; float z; } pos={{x,y},init::lbl_805A1F90};
 if(useOffset){pos.xy.x += p->offsetX * scale; pos.xy.y += p->offsetY;}
 Vec3& value=*reinterpret_cast<Vec3*>(&pos);
 p->e=value; p->d=value; p->b=value; p->c=value; p->a=value; p->f=value; p->g=value;
 Vec3 zero={lbl_805A1F90,lbl_805A1F90,lbl_805A1F90}; p->zero=zero; p->flags |= 4;
}
}

// TODO: clean up, give variables real names
namespace jj_800A4BE4 {
typedef unsigned short u16;
typedef unsigned int u32;

struct Vec3 { float x, y, z; };
struct Vec2 {
    float x, y;
    inline void set(const Vec3* p) {
        float a, b;
        b = p->y;
        a = p->x;
        x = a;
        y = b;
    }
};
struct Box { Vec2 min, max; };
struct Polygon {
    u16 count;
    u16 pad;
    Vec3* points;
    Vec3 min, max;
    char rest[0x20];
};
struct Mesh { u32 count; Polygon* polygons; };
struct Object { char pad[0x98]; float width, height; Mesh* mesh; };

extern "C" int fn_80041D64(Box*, Box*);
extern "C" int fn_80041978(Box*, Box*);
extern "C" void fn_800A46B4(Object*, Box*, Vec2*, Vec2*);
extern "C" float lbl_805A1FA0;
extern "C" float lbl_805A1FCC;

// TODO: clean up, give variables real names
inline void makeBox(Box* box, float left, float bottom, float right, float top) {
    float margin = lbl_805A1FCC;
    float bottomFinal = bottom;
    float leftFinal = left - margin;
    box->min.y = bottomFinal;
    box->min.x = leftFinal;
    float topFinal = margin + top;
    box->max.x = margin + right;
    box->max.y = topFinal;
}

// TODO: clean up, give variables real names
extern "C" void fn_800A4BE4(Object* self, Vec2* arg) {
    Box box;
    Box other;
    Box edge;
    Vec2 pos;
    Vec2 half;
    float x, y;
    float hx = lbl_805A1FA0 * self->width;
    float hy = lbl_805A1FA0 * self->height;
    x = arg->x;
    y = arg->y;
    pos.x = x;
    pos.y = y;
    makeBox(&box, x - hx, (y - hy) - lbl_805A1FCC, x + hx, y + hy);
    {
        Polygon* poly;
        int outerOffset;
        int innerOffset;
        Mesh* mesh;
        u32 i;
        u32 j;
        mesh = self->mesh;
        if (mesh) {
            half.x = hx;
            half.y = hy;
            i = 0;
            outerOffset = 0;
            for (; i < mesh->count; outerOffset += 0x40, ++i) {
                poly = (Polygon*)((char*)mesh->polygons + outerOffset);
                other.min.set(&poly->min);
                other.max.set(&poly->max);
                if (fn_80041D64(&box, &other)) {
                    j = 0;
                    innerOffset = 0;
                    for (; j < (u16)*(short*)poly; innerOffset += 0xC, ++j) {
                        if (j == (u32)(poly->count - 1)) {
                            Vec3* p;
                            Vec3* items = poly->points;
                            p = (Vec3*)((char*)items + innerOffset);
                            edge.min.set(p);
                            edge.max.set(items);
                        } else {
                            Vec3* p = (Vec3*)((char*)poly->points + innerOffset);
                            edge.min.set(p);
                            edge.max.set(p + 1);
                        }
                        if (fn_80041978(&box, &edge))
                            fn_800A46B4(self, &edge, &pos, &half);
                    }
                }
            }
            arg->x = pos.x;
            arg->y = pos.y;
        }
    }
}
}

#include <nw4r/ut/ut_LinkList.h>
// TODO: clean up, give variables real names
namespace jj_800A4154 {

struct Node {
    Node* next;
    u32 pad4;
    u32 flags;
    u8 padC[2];
    u16 s14;
};

struct Head {
    u32 count;
    Node head;
};

extern "C" Head lbl_8049DEDC;

struct Object {
    u8 pad0[0xB];
    u8 bit7 : 1;
    u8 bit6 : 1;
    u8 rest : 6;
    u16 id;
    u8 padE[0xD6];
    Node* node;
};

inline Node* findNode(Head* head) {
    nw4r::ut::LinkList<Node, 0>* list =
        (nw4r::ut::LinkList<Node, 0>*)head;
    Node* found = 0;

    if (head->count != 0) {
        nw4r::ut::LinkList<Node, 0>::Iterator it = list->GetBeginIter();
        for (; it != list->GetEndIter(); it++) {
            u32 f = it->flags;
            if (((f >> 16) & 1) && ((f >> 23) & 7) == 5) {
                found = &*it;
                break;
            }
        }
    }
    return found;
}

extern "C" void searchSubject__21cmAdventureControllerFv(Object* p) {
    Node* found = findNode(&lbl_8049DEDC);
    p->node = found;
    p->id = 0;
    p->bit6 = p->bit7;
    p->bit7 = 0;

    if (found) {
        p->id = found->s14;
        p->bit7 = (found->flags >> 15) & 1;
    }
}
}

