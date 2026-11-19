#include <gr/collision/gr_collision_linework.h>
#include <so/ground/so_ground_tracer_impl.h>

bool soGroundTracerImpl::canStart(u32 arg0, u8 arg1, soGroundModule* groundModule, u32 index, float arg4) {
    grCollisionLineWork work;
    unk8 = 0xffff;
    unk10 = 0xffff;
    return getStartLineWorkId(&unk8, &work, arg0, arg1, groundModule, index, arg4);
}

Vec2f soGroundTracerImpl::getGroundMovementSpeed() {
    if (!isTraced()) {
        return Vec2f(0.0f, 0.0f);
    }
    return m_groundMovementSpeed;
}

#include <gr/collision/gr_collision.h>
#include <so/ground/so_ground_module_impl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/ut/ut_algorithm.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_coll2d.h>
// TODO: clean up, give variables real names
namespace jj_311F8 {
extern "C" {
extern const float lbl_27_rodata_450;
extern unsigned char lbl_27_data_31B8[];
}

struct Vec2 {
    float x, y;
    Vec2() {}
    Vec2(float a, float b) : x(a), y(b) {}
};

struct Tri {
    unsigned int x, y, z;
    Tri() {}
    Tri(unsigned int v) : x(v), y(v), z(v) {}
};

struct State {
    void* vtable;
    unsigned char flag;
    Tri abc;
    Vec2 first, second;
    unsigned char flag24, flag25;

    void init() {
        flag24 = 0;
        first.x = lbl_27_rodata_450;
        first.y = lbl_27_rodata_450;
        second.x = lbl_27_rodata_450;
        second.y = lbl_27_rodata_450;
        flag = 0;
        abc.x = 0xFFFF;
        vtable = lbl_27_data_31B8;
        abc = Tri(0xFFFF);
        Vec2 v1(lbl_27_rodata_450, lbl_27_rodata_450);
        first = v1;
        Vec2 v2(lbl_27_rodata_450, lbl_27_rodata_450);
        second = v2;
        flag24 = 0;
        flag25 = 0;
    }
};

extern "C" void __ct__18soGroundTracerImplFv(State* s) {
    s->init();
}
}

// TODO: clean up, give variables real names
namespace jj_31C88 {
typedef unsigned int u32;
typedef unsigned char u8;

extern "C" const float lbl_27_rodata_450;
extern "C" u32 getLineWork__11grCollisionFPUlP19grCollisionLineWork(void*, void*);

struct A { u8 pad[0x10]; };
struct B {
    u8 pad[0x54];
    u8 flags;
};
struct C {
    u8 pad[0x2C];
    u8 flags;
};

struct Bits {
    u8 hi : 1;
    u8 rest : 7;
};

struct Local {
    A* a;
    B* b;
    C* c;
    u8 byte;
    Bits bits;
};

struct Vec4 {
    float x, y, z, w;
};

struct Id {
    u32 v;
};

struct Self {
    u8 pad[8];
    Id i0;
    Id i1;
    Id i2;
    float f[4];
    u8 b24;
    u8 b25;
};

extern "C" u8 fn_27_31C88(Self* self) {
    int valid = 0;
    Local l;
    l.a = 0;
    l.b = 0;
    l.c = 0;
    l.byte = 0;
    l.bits.hi = 0;

    int ok;
    if (self->i0.v == 0xFFFF) {
        ok = 0;
    } else {
        Id id;
        id = self->i0;
        if (getLineWork__11grCollisionFPUlP19grCollisionLineWork(&id, &l.a) == 0) {
            ok = 0;
        } else {
            if (l.a && l.b && l.c)
                valid = 1;
            int enabled;
            if (valid && ((l.b->flags >> 4) & 1) && ((l.c->flags >> 7) & 1))
                enabled = 1;
            else
                enabled = 0;
            ok = enabled != 0;
        }
    }

    if (!ok) {
        Vec4 v;
        Id none;
        none.v = 0xFFFF;
        self->i0 = none;
        self->i1 = none;
        self->i2 = none;
        self->b24 = 0;
        v.x = lbl_27_rodata_450;
        v.y = lbl_27_rodata_450;
        self->f[0] = v.x;
        self->f[1] = v.y;
        v.z = lbl_27_rodata_450;
        v.w = lbl_27_rodata_450;
        self->f[2] = v.z;
        self->f[3] = v.w;
        self->b25 = 0;
    }
    return self->b24;
}
}

// TODO: clean up, give variables real names
namespace jj_31E40 {
#pragma fp_contract off

struct LineId {
    union {
        u32 value;
        struct {
            u16 a;
            u16 b;
        };
    };

    LineId() { value = 0xffff; }
    LineId(const u32& v) : value(v) {}
};

class TraceState : public soNullable {
public:
    virtual void __dt__18soGroundTracerImplFv();
    virtual bool fn_27_31C88();
    virtual void canStart__18soGroundTracerImplFUlUcP14soGroundModuleUlf();
    virtual void fn_27_31E40();

    u32 current;
    u32 next;
    u32 other;
    Vec2f target;
    Vec2f movement;
    u8 traced;
    u8 mode;
};

struct GroundData {
    u8 pad[0x54];
    u8 flags;
};

struct Entry {
    grCollisionLine* line;
    GroundData* ground;
    grCollision* collision;
    u8 byte;
    u8 high : 1;
    u8 rest : 7;

    Entry() : line(0), ground(0), collision(0), byte(0), high(0) {}

    bool valid() const {
        return line && ground && collision;
    }

    int enabled() const {
        bool v = valid();
        int result;
        if (v && ((ground->flags >> 4) & 1) &&
            ((*(u8*)((char*)collision + 0x2c) >> 7) & 1))
            result = 1;
        else
            result = 0;
        return result;
    }
};

extern const float lbl_27_rodata_450[];

extern "C" {
u32 getLineWork__11grCollisionFPUlP19grCollisionLineWork(LineId*, Entry*);
s32 getStartLineWorkId__18soGroundTracerImplFPUlP19grCollisionLineWorkUlUcP14soGroundModuleUlf(TraceState*, u32*, Entry*, u32, u32, soGroundModule*, u32, float);
u32 getNextLineWork__19grCollisionLineWorkCFRC5Vec2fP19grCollisionLineWork(Entry*, Vec2f*, Entry*);
u16 getLineIndex__11grCollisionFP15grCollisionLine(grCollision*, grCollisionLine*);
void calcLineVector__11grCollisionFRCUlRC5Vec2fP5Vec2f(LineId*, const Vec2f*, Vec2f*);
Vec2f fn_27_312B0(TraceState*, const Vec2f*, float);
}

#pragma inline all
inline bool loadTraceCurrent(TraceState* self, Entry* entry) {
    if (self->current == 0xffff) return false;
    LineId id(self->current);
    if (!getLineWork__11grCollisionFPUlP19grCollisionLineWork(&id, entry)) return false;
    return entry->enabled() != 0;
}
inline u32 makeTraceId(Entry* context) {
    LineId id;
    id.a = context->collision->m_index;
    id.b = getLineIndex__11grCollisionFP15grCollisionLine(context->collision, context->line);
    return id.value;
}
inline float traceLenSq(const Vec2f& v) {
    return v.m_x*v.m_x+v.m_y*v.m_y;
}
inline float traceSqrt(float sq, const float* c) {
    return ((float)(double)nw4r::math::FAbs(sq)<=c[8]) ? c[0] : sq*rsqrtf(sq);
}
inline float traceLen(const Vec2f& v, const float* c) {
    return traceSqrt(traceLenSq(v),c);
}
inline bool traceWithin(const Vec2f& v, float size, const float* c) {
    return traceLen(v,c)<=size;
}
inline Vec2f traceCenter(TraceState* self, Vec2f pos, float t) {
    Vec2f off;
    Vec2f::copy(off,fn_27_312B0(self,&pos,t));
    Vec2f r;
    Vec2f::copy(r,self->target+off);
    return r;
}
inline float traceSize(soGroundModule* ground, const float* c) {
    clRhombus2D a,b;
    ground->getRhombus(&a,&b,0);
    Vec2f d;
    Vec2f::copy(d,b.m_downPos-a.m_downPos);
    return traceLen(d,c);
}
inline bool traceNear(TraceState* self, soPostureModule* posture, soGroundModule* ground, u32 index, float t, const float* c) {
    Vec3f p=posture->getPos();
    Vec2f initial(p.m_x,p.m_y);
    Vec2f adjusted;
    if(self->mode) adjusted=initial;
    else {
        Vec2f movement;
        Vec2f::copy(movement,ground->getDownPos(index)-ground->getCenterPos(index));
        adjusted=initial-movement;
    }
    Vec2f center;
    Vec2f::copy(center,traceCenter(self,ground->getCenterPos(index),t));
    return traceWithin(adjusted-center,traceSize(ground,c),c);
}
extern "C" int fn_27_31E40(
    TraceState* self,
    u32 a,
    u32 b,
    soPostureModule* posture,
    soGroundModule* ground,
    u32 index,
    u8 mode,
    float f1,
    float f2,
    float f3
) {
    const float* c = lbl_27_rodata_450;

    if (self->fn_27_31C88() == 1)
        return 0;

    self->mode = mode;

    Entry entry;
    if (!loadTraceCurrent(self, &entry)) {
        self->current = 0xffff;
        self->other = 0xffff;
        if (!getStartLineWorkId__18soGroundTracerImplFPUlP19grCollisionLineWorkUlUcP14soGroundModuleUlf(self, &self->current, &entry, a, b, ground, index, f2))
            return 0;
    }

    self->next = 0xffff;
    u32 open = 0;
    Entry next;
    Entry cur = entry;
    clSegment2D seg;
    cur.collision->getSegment(&seg, cur.line);

    Vec2f pos(c[0], c[0]);
    if (c[9] == f3)
        pos = seg.m_p1Pos;
    else
        pos = seg.m_p0Pos;

    if (getNextLineWork__19grCollisionLineWorkCFRC5Vec2fP19grCollisionLineWork(&cur, &pos, &next) != 1)
        goto collision_failed;

    int var_r3_2 = 0;
    int var_r5 = 0;
    int enabled = 0;

    if (next.line != 0 && next.ground != 0)
        var_r5 = 1;
    if (var_r5 != 0 && next.collision != 0)
        var_r3_2 = 1;
    if (var_r3_2 != 0 && ((next.ground->flags >> 4) & 1) &&
        ((*(u8*)((char*)next.collision + 0x2c) >> 7) & 1))
        enabled = 1;
    else
        enabled = 0;

    if (enabled)
        goto collision_make_id;
    open = 1;
    goto collision_done;

collision_failed:
    open = 1;
    goto collision_done;

collision_make_id:
    self->next = makeTraceId(&next);

collision_done:
    self->target = pos;
    calcLineVector__11grCollisionFRCUlRC5Vec2fP5Vec2f(&LineId(self->current), &ground->getCenterPos(index), &self->movement);
    self->traced = 1;

    if (open == 1) {
        if (traceNear(self, posture, ground, index, f1, c) == true)
            return 1;
    }

    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_325B8 {
#pragma fp_contract off
#pragma inline all
extern "C" {
Vec2f fn_27_312B0(void*, const Vec2f*, float);
extern const float lbl_27_rodata_450[];
}
#define V10(p) virtual void v##p##0(); virtual void v##p##1(); virtual void v##p##2(); virtual void v##p##3(); virtual void v##p##4(); virtual void v##p##5(); virtual void v##p##6(); virtual void v##p##7(); virtual void v##p##8(); virtual void v##p##9();
namespace matching {
struct GroundBase { u32 unk0, unk4; };
class Ground : public GroundBase {
public:
V10(0) V10(1) V10(2) V10(3) V10(4) V10(5) V10(6) V10(7)
virtual void v80(); virtual void v81(); virtual void v82();
virtual Vec2f getPosA(int);
virtual void v84(); virtual void v85();
virtual Vec2f getPosB(int);
virtual void v87(); virtual void v88(); virtual void v89();
V10(9)
virtual void v100();
virtual void getRhombus(clRhombus2D*,clRhombus2D*,int);
};
struct Id { u32 v; };
class soNullable {
public:
virtual void isNull();
virtual void __dt__18soGroundTracerImplFv();
virtual int fn_27_31C88();
virtual void canStart__18soGroundTracerImplFUlUcP14soGroundModuleUlf();
virtual void fn_27_31E40();
virtual void fn_27_32460();
virtual Vec2f fn_27_325B8();
virtual void fn_27_327D8();
virtual void fn_27_32E84();
virtual void fn_27_332E8();
virtual void fn_27_33E20();
virtual void fn_27_33EA8();
virtual void getGroundMovementSpeed__18soGroundTracerImplFv();
virtual float fn_27_340BC(Ground*,int,int);
u32 unk4;
Id id;
u8 padC[8];
Vec2f position;
u8 pad1C[9],flag;
};
}
using namespace matching;
inline Vec2f calcCenter(soNullable* self, Vec2f pos, float t) {
Vec2f off;
Vec2f::copy(off,fn_27_312B0(self,&pos,t));
Vec2f r;
Vec2f::copy(r,self->position+off);
return r;
}
extern "C" Vec2f fn_27_325B8(soNullable* self,Ground* ground,int index,float t) {
Vec2f center;
Vec2f movement;
gfTaskScheduler* scheduler=gfTaskScheduler::getInstance();
bool mode;
if(scheduler == 0) mode=false;
else mode=(s32)((*(u32*)scheduler<<8)&0xff800000)>>24>=6;
if(!mode) {
Vec2f zero(lbl_27_rodata_450[0],lbl_27_rodata_450[0]);
return zero;
}
if(!self->fn_27_31C88()) {
Vec2f zero(lbl_27_rodata_450[0],lbl_27_rodata_450[0]);
return zero;
}
Vec2f::copy(center,calcCenter(self,ground->getPosB(index),t));
if(self->flag) return center;
Vec2f::copy(movement,ground->getPosA(index)-ground->getPosB(index));
return center+movement;
}
}

// TODO: clean up, give variables real names
namespace jj_327D8 {
#pragma fp_contract off
#pragma inline all
extern "C" {
Vec2f fn_27_312B0(void*, const Vec2f*, float);
u32 getLineWork__11grCollisionFPUlP19grCollisionLineWork(void*,void*);
u32 getNearSegmetIndex__19grCollisionLineWorkCFRC5Vec2f(void*,void*);
void getSegment__11grCollisionFP11clSegment2DP15grCollisionLine(void*,void*,void*);
void projection1__11clSegment2DCFRC5Vec2fR5Vec2f(clSegment2D*,Vec2f*,Vec2f*);
float getProjectionT__11clSegment2DCFRC5Vec2f(clSegment2D*,Vec2f*);
extern const float lbl_27_rodata_450[];
}
#define V10(p) virtual void v##p##0(); virtual void v##p##1(); virtual void v##p##2(); virtual void v##p##3(); virtual void v##p##4(); virtual void v##p##5(); virtual void v##p##6(); virtual void v##p##7(); virtual void v##p##8(); virtual void v##p##9();
namespace matching {
struct GroundBase { u32 unk0, unk4; };
class Ground : public GroundBase {
public:
V10(0) V10(1) V10(2) V10(3) V10(4) V10(5) V10(6) V10(7)
virtual void v80(); virtual void v81(); virtual void v82();
virtual Vec2f getPosA(int);
virtual void v84(); virtual void v85();
virtual Vec2f getPosB(int);
virtual void v87(); virtual void v88(); virtual void v89();
V10(9)
virtual void v100();
virtual void getRhombus(clRhombus2D*,clRhombus2D*,int);
};
struct Id { u32 v; };
class soNullable {
public:
virtual void isNull();
virtual void __dt__18soGroundTracerImplFv();
virtual int fn_27_31C88();
virtual void canStart__18soGroundTracerImplFUlUcP14soGroundModuleUlf();
virtual void fn_27_31E40();
virtual void fn_27_32460();
virtual void fn_27_325B8();
virtual void fn_27_327D8();
virtual void fn_27_32E84();
virtual void fn_27_332E8();
virtual void fn_27_33E20();
virtual void fn_27_33EA8();
virtual void getGroundMovementSpeed__18soGroundTracerImplFv();
virtual float fn_27_340BC(Ground*,int,int);
u32 unk4;
Id id;
u8 padC[8];
Vec2f position;
u8 pad1C[9],flag;
};
class Object_vptr_0 {
public:
virtual void slot0();
virtual void slot1();
virtual void slot2();
virtual void slot3();
virtual Vec3f slot4();
virtual void slot5();
virtual void slot6();
virtual void slot7();
virtual void slot8();
virtual void slot9();
virtual void slot10();
virtual void slot11();
virtual void slot12();
virtual void slot13();
};
}
using namespace matching;
struct Info {
void* line; void* data; void* collision;
u8 tail; u8 high:1; u8 rest:7;
};
inline float lenSq(const Vec2f& v) {
return v.m_x*v.m_x+v.m_y*v.m_y;
}
inline float sqrtSafe(float sq,const float* k) {
return ((float)(double)nw4r::math::FAbs(sq)<=k[8]) ? k[0] : sq*rsqrtf(sq);
}
inline float len(const Vec2f& v,const float* k) {
return sqrtSafe(lenSq(v),k);
}
inline bool isWithin(const Vec2f& v,float size,const float* k) {
return len(v,k)<=size;
}
inline Vec2f calcCenter(soNullable* self, Vec2f pos, float t) {
Vec2f off;
Vec2f::copy(off,fn_27_312B0(self,&pos,t));
Vec2f r;
Vec2f::copy(r,self->position+off);
return r;
}
inline float getSize(Ground* ground,const float* k) {
clRhombus2D a,b;
ground->getRhombus(&a,&b,0);
Vec2f d;
Vec2f::copy(d,b.m_downPos-a.m_downPos);
return len(d,k);
}
inline bool checkNear(soNullable* self,Object_vptr_0* src,Ground* ground,int index,float t,const float* k) {
Vec3f p=src->slot4();
Vec2f initial(p.m_x,p.m_y);
Vec2f adjusted;
if(self->flag) adjusted=initial;
else {
Vec2f movement;
Vec2f::copy(movement,ground->getPosA(index)-ground->getPosB(index));
adjusted=initial-movement;
}
Vec2f center;
Vec2f::copy(center,calcCenter(self,ground->getPosB(index),t));
return isWithin(adjusted-center,getSize(ground,k),k);
}
extern "C" int fn_27_327D8(soNullable* self,Object_vptr_0* src,Ground* ground,int index,float t) {
const float* k=lbl_27_rodata_450;
gfTaskScheduler* scheduler=gfTaskScheduler::getInstance();
bool mode;
if(scheduler == 0) mode=false;
else mode=(s32)((*(u32*)scheduler<<8)&0xff800000)>>24>=6;
if(!mode) return 0;
if(!self->fn_27_31C88())return 0;
if(checkNear(self,src,ground,index,t,k)==true)return 1;
float angle=self->fn_27_340BC(ground,index,0);
float l=getSize(ground,k);
if((float)(double)nw4r::math::FAbs(t-angle)>=l)return 0;
int valid=0;
Info info;
info.line=0; info.data=0; info.collision=0; info.tail=0; info.high=0;
int ok;
if(self->id.v==0xffff)ok=0;
else {
Id id;
id=self->id;
if(!getLineWork__11grCollisionFPUlP19grCollisionLineWork(&id,&info))ok=0;
else {
if(info.line&&info.data&&info.collision)valid=1;
int enabled;
if(valid&&((*(u8*)((char*)info.data+0x54)>>4)&1)&&((*(u8*)((char*)info.collision+0x2c)>>7)&1))enabled=1;
else enabled=0;
ok=enabled!=0;
}
}
if(!ok)return 0;
Vec2f point;
Vec2f::copy(point,ground->getPosB(index));
clSegment2D seg;
getSegment__11grCollisionFP11clSegment2DP15grCollisionLine(info.collision,&seg,info.line);
int dir=getNearSegmetIndex__19grCollisionLineWorkCFRC5Vec2f(&info,&self->position);
Vec2f intersection;
Vec2f end;
Vec2f::copy(end,calcCenter(self,point,t));
projection1__11clSegment2DCFRC5Vec2fR5Vec2f(&seg,&end,&intersection);
if(!getNearSegmetIndex__19grCollisionLineWorkCFRC5Vec2f(&info,&end)) {
seg.m_p0Pos=intersection;
} else {
seg.m_p1Pos=intersection;
}
float res=getProjectionT__11clSegment2DCFRC5Vec2f(&seg,&point);
if(res>k[0]&&res<k[9])return 0;
if(dir==0&&res<=k[0])return 1;
if((u32)dir==1&&res>=k[9])return 1;
return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_332E8 {
#pragma fp_contract off
#pragma inline all

extern const float lbl_27_rodata_450;
extern const float lbl_27_rodata_470;

struct TraceState : soGroundTracer {
    u32 current;
    u32 next;
    u32 previous;
    Vec2f position;
    Vec2f movement;
    u8 flag24;
    u8 flag25;
};

struct CollisionContext {
    grCollisionLine* line;
    grCollisionJoint* joint;
    grCollision* collision;
    u8 byte;
    u8 high : 1;
    u8 rest : 7;
    CollisionContext() {
        line = 0;
        joint = 0;
        collision = 0;
        byte = 0;
        high = 0;
    }
    bool valid() const { return line && joint && collision; }
    int enabled() const {
        if (valid() && joint->m_0x54_4 &&
            ((*(u8*)((char*)collision + 0x2c) >> 7) & 1))
            return 1;
        return 0;
    }
};

extern "C" int getStartLineWorkId__18soGroundTracerImplFPUlP19grCollisionLineWorkUlUcP14soGroundModuleUlf(TraceState*, u32*, CollisionContext*, u32, u32, soGroundModule*, u32, float);
extern "C" int getLineWork__11grCollisionFPUlP19grCollisionLineWork(u32*, CollisionContext*);
extern "C" int getFarSegmetIndex__19grCollisionLineWorkCFRC5Vec2f(CollisionContext*, Vec2f*);
extern "C" Vec2f fn_27_312B0(TraceState*, const Vec2f*, float);
extern "C" Vec2f getFarSegmetPos__19grCollisionLineWorkCFRC5Vec2f(CollisionContext*, Vec2f*);
extern "C" u32 getNextLineWork__19grCollisionLineWorkCFRC5Vec2fP19grCollisionLineWork(CollisionContext*, Vec2f*, CollisionContext*);
extern "C" u16 getLineIndex__11grCollisionFP15grCollisionLine(grCollision*, grCollisionLine*);
extern "C" void calcLineVector__11grCollisionFRCUlRC5Vec2fP5Vec2f(u32*, Vec2f*, Vec2f*);

static inline void updateMovement(TraceState* self, Vec2f center, u32& id) {
    id = self->current;
    calcLineVector__11grCollisionFRCUlRC5Vec2fP5Vec2f(&id, &center, &self->movement);
}

struct LineId { u32 v; };
static inline bool loadCurrent(TraceState* self, CollisionContext* work) {
    if (self->current == 0xffff) return false;
    LineId local;
    local = *(LineId*)&self->current;
    if (!getLineWork__11grCollisionFPUlP19grCollisionLineWork(&local.v, work)) return false;
    return work->enabled() != 0;
}
static inline bool loadNext(TraceState* self, CollisionContext* work) {
    if (self->next == 0xffff) return false;
    LineId local;
    local = *(LineId*)&self->next;
    if (!getLineWork__11grCollisionFPUlP19grCollisionLineWork(&local.v, work)) return false;
    return work->enabled() != 0;
}
static inline u32 makeId(CollisionContext* context) {
    union { u32 id; struct { u16 collision, line; } parts; } id;
    id.id = 0xffff;
    id.parts.collision = context->collision->m_index;
    id.parts.line = getLineIndex__11grCollisionFP15grCollisionLine(context->collision, context->line);
    return id.id;
}
static inline float lenSq(const Vec2f& v) {
    return v.m_x * v.m_x + v.m_y * v.m_y;
}
static inline float sqrtSafe(float square) {
    return ((float)(double)nw4r::ut::Abs(square) <= lbl_27_rodata_470)
        ? lbl_27_rodata_450 : square * rsqrtf(square);
}
static inline float length(const Vec2f& v) {
    return sqrtSafe(lenSq(v));
}
static inline Vec2f calcCenter(TraceState* self, Vec2f pos, float value) {
    Vec2f off;
    Vec2f::copy(off, fn_27_312B0(self, &pos, value));
    Vec2f result;
    Vec2f::copy(result, self->position + off);
    return result;
}
static inline float getSize(soGroundModule* ground) {
    clRhombus2D current, old;
    ground->getRhombus(&current, &old, 0);
    Vec2f d;
    Vec2f::copy(d, old.m_downPos - current.m_downPos);
    return length(d);
}
static inline bool isWithin(const Vec2f& v, float size) {
    return length(v) <= size;
}
static inline bool arrived(TraceState* self, soPostureModule* posture, soGroundModule* ground, u32 index, float value) {
    Vec3f pos3 = posture->getPos();
    Vec2f pos(pos3.m_x, pos3.m_y);
    Vec2f previous;
    if (self->flag25) previous = pos;
    else {
        Vec2f movement;
        Vec2f::copy(movement, ground->getDownPos(index) - ground->getCenterPos(index));
        previous = pos - movement;
    }
    Vec2f center;
    Vec2f::copy(center, calcCenter(self, ground->getCenterPos(index), value));
    return isWithin(previous - center, getSize(ground));
}

extern "C" int fn_27_332E8(TraceState* self, u32 line, u32 arg, soPostureModule* posture, soGroundModule* ground, u32 index, float value, float amount) {
    gfTaskScheduler* scheduler = gfTaskScheduler::getInstance();
    bool active;
    if (scheduler == 0) active = false;
    else active = (s32)((*(u32*)scheduler << 8) & 0xff800000) >> 24 >= 6;
    if (!active) return 0;
    if (!self->isTraced()) return 0;
    u32 failed = 0;
    CollisionContext work;
    if (line) {
        u32 id = 0xffff;
        if (!getStartLineWorkId__18soGroundTracerImplFPUlP19grCollisionLineWorkUlUcP14soGroundModuleUlf(self, &id, &work, line, arg, ground, index, amount)) return 0;
        CollisionContext oldWork;
        if (!loadCurrent(self, &oldWork)) return 0;
        int side = getFarSegmetIndex__19grCollisionLineWorkCFRC5Vec2f(&oldWork, &self->position);
        clSegment2D segment;
        work.collision->getSegment(&segment, work.line);
        if (!side) self->position = segment.m_p0Pos;
        else self->position = segment.m_p1Pos;
        self->previous = self->current;
        self->current = id;
    } else {
        if (loadNext(self, &work) == 1) {
            self->previous = self->current;
            self->current = self->next;
        } else failed = 1;
        if (failed == 1) {
            if (arrived(self, posture, ground, index, value) == 1) return 1;
            return 0;
        }
    }
    if (!loadCurrent(self, &work)) return 0;
    self->next = 0xffff;
    u32 movementId;
    CollisionContext nextWork;
    CollisionContext copy = work;
    Vec2f point;
    Vec2f::copy(point, self->position);
    point = getFarSegmetPos__19grCollisionLineWorkCFRC5Vec2f(&copy, &point);
    if (getNextLineWork__19grCollisionLineWorkCFRC5Vec2fP19grCollisionLineWork(&copy, &point, &nextWork) == 1) {
        bool valid = nextWork.line && nextWork.joint && nextWork.collision;
        if (valid && nextWork.enabled()) goto next_valid;
        failed = 1;
        goto next_done;
    }
    failed = 1;
    goto next_done;
next_valid:
    self->next = makeId(&nextWork);
next_done:
    self->position = point;
    updateMovement(self, ground->getCenterPos(index), movementId);
    if (failed == 1) {
        if (arrived(self, posture, ground, index, value) == 1) return 1;
    }
    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_33E20 {
extern "C" const float lbl_27_rodata_450;

struct Vec4 {
    float x, y, z, w;
};

class S {
public:
    virtual void v0();
    virtual void v1();
    virtual int isActive();

    unsigned char pad[4]; // 0x4
    int ints[3];          // 0x8
    float f[4];           // 0x14
    unsigned char b24;
    unsigned char b25;
};

extern "C" void fn_27_33E20(S* self) {
    if (self->isActive()) {
        Vec4 local;
        self->ints[0] = 0xFFFF;
        self->ints[1] = 0xFFFF;
        self->ints[2] = 0xFFFF;
        self->b24 = 0;
        local.x = lbl_27_rodata_450;
        local.y = lbl_27_rodata_450;
        self->f[0] = local.x;
        self->f[1] = local.y;
        local.z = lbl_27_rodata_450;
        local.w = lbl_27_rodata_450;
        self->f[2] = local.z;
        self->f[3] = local.w;
        self->b25 = 0;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_33EA8 {
struct Vec2 {
    f32 x;
    f32 y;
};

struct QBits {
    u8 hi : 1;
    u8 rest : 7;
};

struct Line {
    u32 a;
    void* b;
    void* c;
    u8 d;
    QBits q;

    Line() {
        a = 0;
        b = 0;
        c = 0;
        d = 0;
        q.hi = 0;
    }
};

class soNullable {
public:
    virtual void isNull();
    virtual void __dt__18soGroundTracerImplFv();
    virtual int fn_27_31C88();
    virtual void canStart__18soGroundTracerImplFUlUcP14soGroundModuleUlf();
    virtual void fn_27_31E40();
    virtual void fn_27_32460();
    virtual void fn_27_325B8();
    virtual void fn_27_327D8();
    virtual void fn_27_32E84();
    virtual void fn_27_332E8();
    virtual void fn_27_33E20();

    u8 pad_4[4];
    u32 unk8;
    u8 pad_c[8];
    Vec2 pos;
    Vec2 vel;
};

struct Obj2Prefix {
    u32 pad[2];
};

class Obj2 : public Obj2Prefix {
public:
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
    virtual void slot23();
    virtual void slot24();
    virtual void slot25();
    virtual void slot26();
    virtual void slot27();
    virtual void slot28();
    virtual void slot29();
    virtual void slot30();
    virtual void slot31();
    virtual void slot32();
    virtual void slot33();
    virtual void slot34();
    virtual void slot35();
    virtual void slot36();
    virtual void slot37();
    virtual void slot38();
    virtual void slot39();
    virtual void slot40();
    virtual void slot41();
    virtual void slot42();
    virtual void slot43();
    virtual void slot44();
    virtual void slot45();
    virtual void slot46();
    virtual void slot47();
    virtual void slot48();
    virtual void slot49();
    virtual void slot50();
    virtual void slot51();
    virtual void slot52();
    virtual void slot53();
    virtual void slot54();
    virtual void slot55();
    virtual void slot56();
    virtual void slot57();
    virtual void slot58();
    virtual void slot59();
    virtual void slot60();
    virtual void slot61();
    virtual void slot62();
    virtual void slot63();
    virtual void slot64();
    virtual void slot65();
    virtual void slot66();
    virtual void slot67();
    virtual void slot68();
    virtual void slot69();
    virtual void slot70();
    virtual void slot71();
    virtual void slot72();
    virtual void slot73();
    virtual void slot74();
    virtual void slot75();
    virtual void slot76();
    virtual void slot77();
    virtual void slot78();
    virtual void slot79();
    virtual void slot80();
    virtual void slot81();
    virtual void slot82();
    virtual void slot83();
    virtual void slot84();
    virtual void slot85();
    virtual Vec2 slot86(s32);

    u8 pad[8];
};

struct Id {
    u32 v;
};

struct IdRead {
    u8 pad[8];
    Id value;
};

extern "C" u32 getLineWork__11grCollisionFPUlP19grCollisionLineWork(void*, void*);
extern "C" void calcLineVector__11grCollisionFRCUlRC5Vec2fP5Vec2f(u32*, void*, f32*);
extern "C" Vec2 getNearSegmetPos__19grCollisionLineWorkCFRC5Vec2f(void*, void*);
extern "C" const f32 lbl_27_rodata_450;

extern "C" void fn_27_33EA8(soNullable* self, u32 arg1, Obj2* arg2, s32 arg3, u32 arg4) {
    u32 contextId;
    u32 queryId;

    if (self->fn_27_31C88() != 0) {
        bool flag = false;
        Line l;
        s32 ok;

        if (((IdRead*)self)->value.v == 0xFFFF) {
            ok = 0;
        } else {
            Id id;
            id = ((IdRead*)self)->value;
            if (getLineWork__11grCollisionFPUlP19grCollisionLineWork(&id, &l) == 0) {
                ok = 0;
            } else {
                flag = (l.a != 0) && (l.b != 0) && (l.c != 0);
                int result;
                if (flag && ((((u8*)l.b)[0x54] >> 4) & 1) &&
                    ((((u8*)l.c)[0x2C] >> 7) & 1)) {
                    result = 1;
                } else {
                    result = 0;
                }
                ok = result != 0;
            }
        }

        if (!ok) {
            self->fn_27_33E20();
            return;
        }

        if (arg1 != 1) {
            Vec2 zero = {lbl_27_rodata_450, lbl_27_rodata_450};
            self->vel = zero;

            Vec2 r = arg2->slot86(arg3);
            contextId = self->unk8;
            calcLineVector__11grCollisionFRCUlRC5Vec2fP5Vec2f(&contextId, &r, &self->vel.x);

            Vec2 p = getNearSegmetPos__19grCollisionLineWorkCFRC5Vec2f(&l, &self->pos);
            self->pos = p;
        }
    }
}
}

