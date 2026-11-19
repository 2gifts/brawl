#include <gf/gf_area.h>
#include <memory.h>
#include <so/area/so_area_instance.h>
#include <so/area/so_area_module_impl.h>
#include <st/st_area_manager.h>
#include <types.h>

extern stAreaManager* lbl_27_bss_5780;

void soAreaInstance::setOwnerId(int ownerId) {
    m_area->m_ownerId = ownerId;
}

void soAreaInstance::deactivate() {
    if (m_area->m_isRegist == true) {
        m_area->unRegist();
    }
}

bool soAreaInstance::isFlags(u16 mask) {
    return (m_data->m_shapeFlag.m_mask & mask) != 0;
}

u32 soAreaInstance::getId() {
    if (m_isCreated) {
        return m_area->m_id;
    }
    return 0;
}

void soAreaInstance::enable(bool isEnable, int frame) {
    if (isEnable) {
        m_area->m_isDrawFill = true;
    } else {
        m_area->m_isDrawFill = false;
    }
    m_isEnable = isEnable;
    m_enableFrame = frame;
}

void soAreaInstance::setWhole(bool isWhole) {
    if (!isWhole) {
        m_area->m_isDrawFill = false;
    }
}

void soAreaInstance::setCheckTargetGroup(int group) {
    m_area->m_checkTargetGroup = group;
}

void soAreaInstance::setCheckTargetLocalGroup(int group) {
    m_area->m_checkTargetLocalGroup = group;
}

void soAreaInstance::setTeamId(u32 teamId) {
    m_area->m_teamId = teamId;
}

u32 soAreaInstance::getTeamId() {
    return m_area->m_teamId;
}

void soAreaInstance::setShapeType(ShapeType shapeType) {
    m_shapeType = shapeType;
}

void soAreaInstance::destroy() {
    if (m_area != NULL) {
        if (m_isWindSet == true) {
            if (lbl_27_bss_5780 == NULL) {
                lbl_27_bss_5780 = new (Heaps::System) stAreaManager();
            }
            lbl_27_bss_5780->erase(m_area->m_id);
            m_isWindSet = false;
        }
        delete m_area;
        m_area = NULL;
    }
}

void soAreaInstance::setShape(const Vec2f& center, const Vec2f& size) {
    dynamic_cast<gfAreaRectangle&>(*m_area).setShape(center, size);
    unkC = center.m_x;
    unk10 = center.m_y;
    unk14 = size.m_x;
    unk18 = size.m_y;
}

void soAreaInstance::setShape(const Vec2f& center, float radius) {
    dynamic_cast<gfAreaCircle&>(*m_area).setShape(center, radius);
    unkC = center.m_x;
    unk10 = center.m_y;
    unk14 = radius;
}

void soAreaInstance::setWind(const stMovementData& data) {
    stMovementAreaInfoData info;
    info.m_areaId = m_area->m_id;
    info.m_data = data;
    if (lbl_27_bss_5780 == NULL) {
        lbl_27_bss_5780 = new (Heaps::System) stAreaManager();
    }
    lbl_27_bss_5780->setWind(&info);
    m_isWindSet = true;
}

void soAreaInstance::unsetWind() {
    if (m_isWindSet == true) {
        if (lbl_27_bss_5780 == NULL) {
            lbl_27_bss_5780 = new (Heaps::System) stAreaManager();
        }
        lbl_27_bss_5780->erase(m_area->m_id);
        m_isWindSet = false;
    }
}

// TODO: clean up, give variables real names
namespace jj_A6948 {
struct UnkObj27 {
    u32 m_00;
    u32 m_04;
    s32 m_08;
    u8 m_0c[0x1d - 0x0c];
    u8 m_1d_hi : 4;
    u8 m_1d_lo : 4;
    u8 m_1e[0x20 - 0x1e];
    const void* m_20;
};

extern "C" {
extern u8 lbl_27_data_157E0[];

void __ct__14soAreaInstanceFv(UnkObj27* obj) {
    obj->m_20 = lbl_27_data_157E0;
    obj->m_00 = 0;
    obj->m_04 = 0;
    obj->m_08 = -1;
    obj->m_1d_hi = 0;
}
}
}

// TODO: clean up, give variables real names
namespace jj_A6978 {
extern "C" {
extern unsigned char lbl_27_data_157E0[];
extern void* lbl_27_bss_5780;

void* __nw__FUlQ25Heaps8HeapType(unsigned long, int);
void* __ct__13stAreaManagerFv(void*);
void erase__13stAreaManagerFUl(void*, void*);
void __dt__6gfAreaFv(void*, int);
void __dl__FPv(void*);
}

struct Inner {
    unsigned char pad[0x10];
    void* value;
};

struct Obj {
    unsigned char pad0[4];
    Inner* inner;
    unsigned char pad1[0x15];
    unsigned char flags;
    unsigned char pad2[2];
    void* vtable;
};

extern "C" Obj* __dt__14soAreaInstanceFv(Obj* self, int deleting) {
    if (self) {
        self->vtable = lbl_27_data_157E0;
        if (self->inner) {
            if ((unsigned int)((self->flags >> 5) & 1) == 1) {
                if (!lbl_27_bss_5780) {
                    void* p = __nw__FUlQ25Heaps8HeapType(0x54, 2);
                    if (p) {
                        p = __ct__13stAreaManagerFv(p);
                    }
                    lbl_27_bss_5780 = p;
                }
                erase__13stAreaManagerFUl(lbl_27_bss_5780, self->inner->value);
                self->flags &= ~0x20;
            }
            __dt__6gfAreaFv(self->inner, 1);
            self->inner = 0;
        }
        if (deleting > 0) {
            __dl__FPv(self);
        }
    }
    return self;
}
}

// TODO: clean up, give variables real names
namespace jj_A6A54 {
struct Vec2 {
    f32 x;
    f32 y;
};

struct Arg3 {
    u8 unk0;
    u8 unk1;
    u8 pad2[2];
    s32 unk4;
    s32 unk8;
};

struct Heap {
    u8 pad0[0x8];
    s32 unk8;
    s32 unkC;
    u8 pad10[0x8];
    s32 unk18;
    union {
        s32 unk1C;
        u8 unk1C_b[4];
        struct { u8 top : 1; u8 rest : 7; } flags;
        struct { unsigned int high : 24; unsigned int low : 8; } bits;
    };
    u8 pad20[0x24];
    void* unk44;
};

struct Ctx {
    Arg3* unk0;
    Heap* unk4;
    u8 pad8[0x15];
    u8 unk1D;
};

extern "C" {
void* __nw__FUlQ25Heaps8HeapType(unsigned long size, int heap);
void __ct__6gfAreaFRC5Vec2fb(Heap* p, void* q, unsigned int mode);
void setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(Heap* p, void* q, Vec2* v);
void setShape__12gfAreaCircleFRC5Vec2ff(Heap* p, void* q, float f);
void setEventReciver__6gfAreaFP18gfAreaEventReciver(Heap* p, void* q, s32 a, u8 b);
}

extern const f32 lbl_27_rodata_D18;
extern u8 __vt__15gfAreaRectangle[];
extern u8 __vt__12gfAreaCircle[];

inline void initCircle(Heap* h, void* arg, float value) {
    h->unk18 = 1;
    setShape__12gfAreaCircleFRC5Vec2ff(h, arg, value);
}

extern "C" void fn_27_A6A54(Ctx* self, void* arg1, u8 arg2, Arg3* arg3,
                              s32 arg4, void* arg5) {
    Vec2 v;
    Heap* h;
    s32 t = arg3->unk0;

    if (t == 1)
        goto block9;
    if (t < 1) {
        if (t >= 0)
            goto block6;
        goto tail;
    }
    if (t == 5)
        goto block9;
    if (t < 5)
        goto block6;
    goto tail;

block6:
    h = (Heap*)__nw__FUlQ25Heaps8HeapType(0x68, arg4);
    if (h != 0) {
        __ct__6gfAreaFRC5Vec2fb(h, arg1, 0);
        h->unk44 = __vt__15gfAreaRectangle;
        h->unk18 = 0;
        v.x = lbl_27_rodata_D18;
        v.y = lbl_27_rodata_D18;
        setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(h, arg1, &v);
    }
    self->unk4 = h;
    goto tail;

block9:
    h = (Heap*)__nw__FUlQ25Heaps8HeapType(0x54, arg4);
    if (h != 0) {
        __ct__6gfAreaFRC5Vec2fb(h, arg1, 0);
        h->unk44 = __vt__12gfAreaCircle;
        initCircle(h, arg1, lbl_27_rodata_D18);
    }
    self->unk4 = h;

tail:
    {
        u8 b = arg3->unk1;
        s32 a = arg3->unk4;
        h = self->unk4;
        h->bits.low = arg2;
        s32 c = arg3->unk8;
        self->unk4->unk1C_b[1] = b;
        self->unk4->unk8 = a;
        self->unk4->unkC = c;
        setEventReciver__6gfAreaFP18gfAreaEventReciver(self->unk4, arg5, a, b);
    }
    self->unk4->flags.top = 0;
    self->unk0 = arg3;
    self->unk1D |= 0x40;
}
}

// TODO: clean up, give variables real names
namespace jj_A6C80 {
struct Vec2 { float x, y; Vec2() {} Vec2(const Vec2& v) : x(v.x), y(v.y) {} Vec2& operator=(const Vec2& v) { float a=v.y; float b=v.x; x=b; y=a; return *this; } };
class Object_vptr_0 {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2(); virtual void slot3(); virtual Vec2 slot4();
};
struct Access { char pad[12]; Object_vptr_0* obj; };
struct Arg { char pad[0xd8]; Access* access; };
struct Data { unsigned char type; char pad; unsigned short flags; char pad4[12]; Vec2 a; Vec2 b; };
struct Target { char pad[0x1c]; union { unsigned char flags; struct { unsigned char high:1; unsigned char rest:7; } bits; }; char pad1d[3]; Vec2 pos; };
struct Self { Data* data; Target* target; int state; Vec2 a; Vec2 b; unsigned char type; union { unsigned char flags; struct { unsigned char high:1; unsigned char rest:7; } bits; }; };
extern "C" char __RTTI__12gfAreaCircle[], lbl_27_data_157C8[], __RTTI__6gfArea[];
extern "C" void* __dynamic_cast(void*, long, const void*, const void*, long);
extern "C" void setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(void*, Vec2*, Vec2*);
extern "C" void setShape__12gfAreaCircleFRC5Vec2ff(void*, Vec2*, float);
extern "C" void fn_27_A6C80(Self* self, Arg* arg) {
 Vec2 pos = arg->access->obj->slot4();
 self->a = self->data->a;
 self->b = self->data->b;
 self->type = self->data->type;
 Vec2& dest = (self->target->pos = pos);
 ((Target*)((char*)&dest - 0x20))->flags |= 0x20;
 switch (self->type) {
 case 0: {
  void* p = __dynamic_cast(self->target, 0x44, lbl_27_data_157C8, __RTTI__6gfArea, 0);
  if (!p) return;
  setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(p, &self->a, &self->b);
  break;
 }
 case 1: {
  void* p = __dynamic_cast(self->target, 0x44, __RTTI__12gfAreaCircle, __RTTI__6gfArea, 0);
  if (!p) return;
  setShape__12gfAreaCircleFRC5Vec2ff(p, &self->a, self->b.x);
  break;
 }
 }
 if (self->data->flags & 8) {
  self->target->bits.high = 0;
  self->bits.high = 0;
 } else {
  self->target->flags |= 0x80;
  self->flags |= 0x80;
 }
 self->state = -1;
}
}

// TODO: clean up, give variables real names
namespace jj_A6DEC {
struct Vec2 {
    float x, y;

    Vec2() {}
    Vec2(float x_, float y_) : x(x_), y(y_) {}
    Vec2(const Vec2& other) {
        __memcpy(this, &other, sizeof(Vec2));
    }

    void set(float px, float py) {
        x = px;
        y = py;
    }
};

struct Vec3 {
    float x, y, z;
};

struct Matrix {
    float m[3][4];
};

struct FlagBits {
    u8 active : 1;
    u8 reserved : 2;
    u8 initialized : 1;
    u8 other : 4;
};

#define V8(p) \
    virtual void p##0(); \
    virtual void p##1(); \
    virtual void p##2(); \
    virtual void p##3(); \
    virtual void p##4(); \
    virtual void p##5(); \
    virtual void p##6(); \
    virtual void p##7();

class Object_vptr_0 {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual Vec3 slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual float slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual Vec3 slot14(int);
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual float slot22();
};

class Model {
public:
    char pad[8];

    V8(a)
    V8(b)
    V8(c)
    V8(d)
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual Vec3 slot36(void*, int);
    virtual Vec3 slot37(void*, Vec3*, int, int);
    virtual void slot38();
    virtual void slot39();
    virtual void slot40(void*, Matrix*, int);
};

class Graphics {
public:
    char pad[8];

    V8(a)
    V8(b)
    V8(c)
    V8(d)
    V8(e)
    V8(f)
    V8(g)
    V8(h)
    V8(i)
    V8(j)
    V8(k)
    V8(l)
    virtual void slot96();
    virtual void slot97();
    virtual void slot98();
    virtual void slot99();
    virtual void slot100();
    virtual void slot101(float*, float*, int);
};

struct Shape {
    u8 pad0[2];
    u16 flags;
    u8 pad1[8];
    void* resource;
    u8 pad2[8];
    float width, height;
};

struct Destination {
    u8 pad[0x1c];
    u8 flags;
    u8 pad1[3];
    float x, y;

    void setPosition(float px, float py) {
        x = px;
        y = py;
        flags |= 0x20;
    }
};

struct Object {
    Shape* shape;
    Destination* destination;
    int count;
    float x, y;
    Vec2 size;
    u8 state, flags;
};

struct Modules {
    u8 pad0[4];
    Model* model;
    u8 pad1[4];
    Object_vptr_0* posture;
    Graphics* graphics;
};

struct Fighter {
    u8 pad[0xd8];
    Modules* modules;
};

extern "C" {
    void* __dynamic_cast(void*, long, const void*, const void*, long);
    void regist__6gfAreaFv(void*);
    void setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(void*, Vec2*, Vec2*);
    void setShape__12gfAreaCircleFRC5Vec2ff(void*, float, Vec2*);
    void rot__5Vec2fCFfP5Vec2f(Vec2*, Vec2*, float);
    void getScalingFactor__FP6MatrixP5Vec3f(Matrix*, Vec3*);

    extern float lbl_27_rodata_D18[];
    extern char __RTTI__12gfAreaCircle[];
    extern char lbl_27_data_157C8[];
    extern char __RTTI__6gfArea[];
}

extern "C" void fn_27_A6DEC(Object* self, Fighter* fighter, int enabled) {
    u16 bits;
    float* constants = lbl_27_rodata_D18;
    int count = self->count;

    if (count >= 0) {
        count--;
        self->count = count;
        if (count < 0) {
            ((FlagBits*)&self->destination->flags)->active = 0;
            ((FlagBits*)&self->flags)->active = 0;
            self->count = -1;
            return;
        }
    }

    bits = self->shape->flags;
    if (!(bits & 2)) {
        int hasTarget = bits & 1;
        Vec3 pos;

        if (hasTarget) {
            pos = fighter->modules->model->slot36(self->shape->resource, 0);
        } else {
            pos = fighter->modules->posture->slot4();
        }

        self->destination->setPosition(pos.x, pos.y);

        switch (self->state) {
        case 0: {
            Vec2 v(self->x, self->y);
            Vec2 p = self->size;

            if ((u32)((self->flags >> 4) & 1) == 1) {
                v.x = constants[1];
            } else if (bits & 4) {
                v.x *= fighter->modules->posture->slot9();
            }

            if (bits & 0x200) {
                if (hasTarget) {
                    Vec3 point = {v.x, v.y, constants[1]};
                    point = fighter->modules->model->slot37(
                        self->shape->resource, &point, 0, 0);
                    v.set(point.x - pos.x, point.y - pos.y);
                } else {
                    const Vec3& rotation =
                        fighter->modules->posture->slot14(0);
                    rot__5Vec2fCFfP5Vec2f(&v, &v, constants[2] * rotation.x);
                }
            }

            if (!(bits & 0x40)) {
                if (hasTarget) {
                    Matrix matrix;
                    Vec3 scale;
                    fighter->modules->model->slot40(
                        self->shape->resource, &matrix, 0);
                    getScalingFactor__FP6MatrixP5Vec3f(&matrix, &scale);
                    if (constants[0] != scale.x) {
                        p.x *= scale.x;
                        p.y *= scale.x;
                        v.x *= scale.x;
                        v.y *= scale.x;
                    }
                } else {
                    float scale = fighter->modules->posture->slot22();
                    if (constants[0] != scale) {
                        p.x *= scale;
                        p.y *= scale;
                        v.x *= scale;
                        v.y *= scale;
                    }
                }
            }

            setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(
                __dynamic_cast(self->destination, 0x44,
                               lbl_27_data_157C8, __RTTI__6gfArea, 0),
                &v, &p);
            break;
        }

        case 1: {
            Vec2 v(self->x, self->y);
            float radius = self->size.x;

            if ((u32)((self->flags >> 4) & 1) == 1) {
                v.x = constants[1];
            } else if (bits & 4) {
                v.x *= fighter->modules->posture->slot9();
            }

            if (bits & 0x200) {
                if (hasTarget) {
                    Vec3 point = {v.x, v.y, constants[1]};
                    point = fighter->modules->model->slot37(
                        self->shape->resource, &point, 0, 0);
                    v.set(point.x - pos.x, point.y - pos.y);
                } else {
                    const Vec3& rotation =
                        fighter->modules->posture->slot14(0);
                    rot__5Vec2fCFfP5Vec2f(&v, &v, constants[2] * rotation.x);
                }
            }

            if (!(bits & 0x40)) {
                if (hasTarget) {
                    Matrix matrix;
                    Vec3 scale;
                    fighter->modules->model->slot40(
                        self->shape->resource, &matrix, 0);
                    getScalingFactor__FP6MatrixP5Vec3f(&matrix, &scale);
                    if (constants[0] != scale.x) {
                        radius *= scale.x;
                        v.x *= scale.x;
                        v.y *= scale.x;
                    }
                } else {
                    float scale = fighter->modules->posture->slot22();
                    if (constants[0] != scale) {
                        radius *= scale;
                        v.x *= scale;
                        v.y *= scale;
                    }
                }
            }

            setShape__12gfAreaCircleFRC5Vec2ff(
                __dynamic_cast(self->destination, 0x44,
                               __RTTI__12gfAreaCircle, __RTTI__6gfArea, 0),
                radius, &v);
            break;
        }

        case 2:
        case 3:
        case 4: {
            float first[14];
            float bounds[14];
            Vec2 p;
            Vec2 v;

            fighter->modules->graphics->slot101(first, bounds, 0);

            if (self->state == 2) {
                p.set(self->shape->width + (bounds[8] - bounds[6]),
                      self->shape->height + (bounds[3] - bounds[5]));
                v.set(constants[3] * (bounds[8] + bounds[6]) - pos.x,
                      constants[3] * (bounds[3] + bounds[5]) - pos.y);
            } else if (self->state == 3) {
                float height =
                    self->shape->height + (bounds[3] - bounds[7]);
                p.set(self->shape->width + (bounds[8] - bounds[6]), height);
                v.set(constants[3] * (bounds[8] + bounds[6]) - pos.x,
                      bounds[7] + constants[3] * height - pos.y);
            } else if (self->state == 4) {
                float height =
                    self->shape->height + (bounds[7] - bounds[5]);
                p.set(self->shape->width + (bounds[8] - bounds[6]), height);
                v.set(constants[3] * (bounds[8] + bounds[6]) - pos.x,
                      bounds[7] - constants[3] * height - pos.y);
            }

            setShape__15gfAreaRectangleFRC5Vec2fRC5Vec2f(
                __dynamic_cast(self->destination, 0x44,
                               lbl_27_data_157C8, __RTTI__6gfArea, 0),
                &v, &p);
            break;
        }
        }
    }

    if (enabled && ((FlagBits*)&self->flags)->active) {
        if (!((FlagBits*)&self->destination->flags)->initialized) {
            regist__6gfAreaFv(self->destination);
        }
        self->destination->flags |= 0x80;
    } else {
        ((FlagBits*)&self->destination->flags)->active = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_A75A4 {
extern "C" unsigned char fn_27_A75A4(unsigned int *p)
{
    return ((unsigned char *)*p)[1];
}
}

