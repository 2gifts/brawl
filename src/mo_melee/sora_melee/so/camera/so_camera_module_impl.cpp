#include <so/camera/so_camera_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_heap_module.h>
#include <so/so_module_accesser.h>
#include <st/stage.h>

soCameraModuleImpl::soCameraModuleImpl(soModuleAccesser* acc, soArray<soCameraSubject>* subjects, soSet<soCameraRange>* rangeSet,
                                       soSet<soCameraClipSphere>* clipSet, soEventObserverRegistrationDesc* regDesc) :
    soCameraModuleSimple(acc, subjects, rangeSet, clipSet, regDesc), m_cameraSubjectArray(subjects), m_cameraRangeSet(rangeSet),
    m_cameraClipSphereSet(clipSet)
{ }

soCameraModuleImpl::~soCameraModuleImpl() { }

void soCameraModuleImpl::setup() {
    if (m_cameraRangeSet != 0) {
        setup(m_cameraRangeSet);
    }
}

void soCameraModuleImpl::setup(soSet<soCameraRange>* cameraRangeSet) {
    s32 size = m_cameraSubjectArray->size();
    for (s32 i = 0; i < size; i++) {
        soHeapModule& heapModule = m_moduleAccesser->getHeapModule();
        soCameraSubject& subject = m_cameraSubjectArray->at(i);
        subject.construct(heapModule.getInstanceHeap(), &cameraRangeSet->elements()[i], 0);
    }
}

void soCameraModuleImpl::setTrainingRangeData(soCameraRange* range, u32 index) {
    m_cameraSubjectArray->at(index).m_trainingRangeData = range;
}

void soCameraModuleImpl::setZoomParam(soCameraZoom* zoom, u32 index) {
    m_cameraSubjectArray->at(index).m_zoomParam = zoom;
}

void soCameraModuleImpl::activate() {
    soCameraModuleSimple::activate();
    s32 size = m_cameraSubjectArray->size();
    for (s32 i = 0; i < size; i++) {
        m_cameraSubjectArray->at(i).enable();
    }
    resetAll();
}

void soCameraModuleImpl::deactivate() {
    soCameraModuleSimple::deactivate();
    s32 size = m_cameraSubjectArray->size();
    for (s32 i = 0; i < size; i++) {
        m_cameraSubjectArray->at(i).deactivate();
    }
}

void soCameraModuleImpl::resetAll() {
    s32 size = m_cameraSubjectArray->size();
    for (s32 i = 0; i < size; i++) {
        m_cameraSubjectArray->at(i).reset();
    }
}

void soCameraModuleImpl::update() {
    if (m_isWhole) {
        soCameraModuleSimple::update();
    }
    s32 size = m_cameraSubjectArray->size();
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    Vec3f prevPos = m_moduleAccesser->getPostureModule().getPrevPos();
    soCameraSubject* subject;
    bool isActive;
    for (s32 i = 0; i < size; i++) {
        subject = &m_cameraSubjectArray->at(i);
        isActive = false;
        if (bool(m_isWhole) == true && m_cameraSubjectArray->at(i).isEnable() == true) {
            isActive = true;
        }
        subject->update(&pos, &prevPos, m_moduleAccesser, isActive);
    }
}

void soCameraModuleImpl::updateForce(u32 index, Vec3f* pos, Vec3f* prevPos) {
    bool isWhole = m_isWhole;
    if (isWhole) {
        bool isActive = false;
        if (isWhole == true && m_cameraSubjectArray->at(index).isEnable() == true) {
            isActive = true;
        }
        m_cameraSubjectArray->at(index).update(pos, prevPos, m_moduleAccesser, isActive);
    }
}

void soCameraModuleImpl::setEnableCamera(bool enable, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            if (enable == true) {
                m_cameraSubjectArray->at(i).enable();
            } else {
                m_cameraSubjectArray->at(i).disable();
                m_cameraSubjectArray->at(i).sleep();
            }
        }
    } else if (enable == true) {
        m_cameraSubjectArray->at(index).enable();
    } else {
        m_cameraSubjectArray->at(index).disable();
        m_cameraSubjectArray->at(index).sleep();
    }
}

void soCameraModuleImpl::setEnableUpdatePos(bool enable, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).m_isEnableUpdatePos = enable;
        }
    } else {
        m_cameraSubjectArray->at(index).m_isEnableUpdatePos = enable;
    }
}

void soCameraModuleImpl::setEnableInterpolatePos(bool enable, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).m_isEnableInterpolatePos = enable;
        }
    } else {
        m_cameraSubjectArray->at(index).m_isEnableInterpolatePos = enable;
    }
}

soCameraRange* soCameraModuleImpl::getCameraRangeData(u32 index) {
    return &m_cameraRangeSet->elements()[index];
}

void soCameraModuleImpl::resetCameraRange(u32 index) {
    m_cameraSubjectArray->at(index).resetRange();
}

void soCameraModuleImpl::addCameraRangeOffset(Vec3f* offset, u32 index) {
    m_cameraSubjectArray->at(index).addOffset(offset);
}

void soCameraModuleImpl::addCameraRangeRect(Rect2D* rect, u32 index) {
    m_cameraSubjectArray->at(index).addRect(rect);
}

void soCameraModuleImpl::addCameraRangeZoom(float zoom, u32 index) {
    m_cameraSubjectArray->at(index).addZoom(zoom);
}

void soCameraModuleImpl::unRegistCamera(u32 index) {
    m_cameraSubjectArray->at(index).unRegist();
}

void soCameraModuleImpl::renderDebug() {
    renderDebugClip();
}

void soCameraModuleImpl::zoomIn(short frame, u32 index, Vec2f* pos, float zoom) {
    m_cameraSubjectArray->at(index).zoomIn(frame, *pos, zoom);
}

void soCameraModuleImpl::zoomOut() {
    soCameraSubject::zoomOut();
}

void soCameraModuleImpl::setPri(u8 pri, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).setPri(pri);
        }
    } else {
        m_cameraSubjectArray->at(index).setPri(pri);
    }
}

u8 soCameraModuleImpl::getPri(u32 index) {
    return m_cameraSubjectArray->at(index).getPri();
}

void soCameraModuleImpl::setAdvPri(u8 pri, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).setAdvPri(pri);
        }
    } else {
        m_cameraSubjectArray->at(index).setAdvPri(pri);
    }
}

u8 soCameraModuleImpl::getAdvPri(u32 index) {
    return m_cameraSubjectArray->at(index).getAdvPri();
}

void soCameraModuleImpl::setPlayerNo(u8 playerNo, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).setPlayerNo(playerNo);
        }
    } else {
        m_cameraSubjectArray->at(index).setPlayerNo(playerNo);
    }
}

u8 soCameraModuleImpl::getPlayerNo(u32 index) {
    return m_cameraSubjectArray->at(index).getPlayerNo();
}

void soCameraModuleImpl::setDemoDistScaleMul(float scale, u32 index) {
    m_cameraSubjectArray->at(index).m_demoDistScaleMul = scale;
}

void soCameraModuleImpl::setDamage(short damage, u32 index) {
    m_cameraSubjectArray->at(index).getInstance()->m_damage = damage;
}

void soCameraModuleImpl::setDamageFly(u32 index) {
    cmSubject* subject = m_cameraSubjectArray->at(index).getInstance();
    subject->m_isDamageFly = true;
}

void soCameraModuleImpl::exitDamageFly(u32 index) {
    cmSubject* subject = m_cameraSubjectArray->at(index).getInstance();
    subject->m_isDamageFly = false;
}

void soCameraModuleImpl::setRun(u32 index) {
    cmSubject* subject = m_cameraSubjectArray->at(index).getInstance();
    subject->m_isRun = true;
}

void soCameraModuleImpl::exitRun(u32 index) {
    cmSubject* subject = m_cameraSubjectArray->at(index).getInstance();
    subject->m_isRun = false;
}

void soCameraModuleImpl::getSubjectRange(Rect2D* outRange, u32 index) {
    cmSubject* subject = m_cameraSubjectArray->at(index).getInstance();
    outRange->m_left = subject->m_60.m_left;
    outRange->m_right = subject->m_60.m_right;
    outRange->m_up = subject->m_60.m_up;
    outRange->m_down = subject->m_60.m_down;
}

Vec3f soCameraModuleImpl::getSubjectPos(u32 index) {
    return m_cameraSubjectArray->at(index).getInstance()->m_pos;
}

float soCameraModuleImpl::getSubjectOffsetY(u32 index) {
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    soCameraSubject& subject = m_cameraSubjectArray->at(index);
    float scale = posture.getScale();
    return scale * subject.getCenterY();
}

soCameraSubject* soCameraModuleImpl::getSubject(u32 index) {
    return &m_cameraSubjectArray->at(index);
}

bool soCameraModuleImpl::isLocked() {
    return g_Stage->isCameraLocked();
}

soCameraClipSphere* soCameraModuleImpl::getClipSphere(u32 index) {
    return &m_cameraClipSphereSet->elements()[index];
}

void soCameraModuleImpl::getCameraRangeGlobalRect(Rect2D* outRange, u32 index) {
    m_cameraSubjectArray->at(index).getCameraRangeGlobalRect(outRange);
}

void soCameraModuleImpl::setCameraRangeGlobalRect(Rect2D* range, soModuleAccesser* moduleAccesser, u32 index) {
    m_cameraSubjectArray->at(index).setCameraRangeGlobalRect(range, moduleAccesser);
}

void soCameraModuleImpl::setOffsetGlobalRectCenter(Rect2D* range, soModuleAccesser* moduleAccesser, u32 index) {
    m_cameraSubjectArray->at(index).setOffsetGlobalRectCenter(range, moduleAccesser);
}

#include <gf/gf_camera.h>
#include <cm/cm_camera_controller.h>
#include <mt/mt_common.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/ut/ut_algorithm.h>
// TODO: clean up, give variables real names
namespace jj_B2248 {
extern "C" void construct__15soCameraSubjectFQ25Heaps8HeapTypeP13soCameraRangeUl(void* self, void* a, int b, int c);

class VA {
public:
    virtual void v0();
    virtual void* v1(int idx, void* p);
    virtual void v2();
    virtual int v3();
};

class VB {
public:
    virtual void v0();
    virtual void v1();
    virtual void* v2();
};

struct Inner { char pad[0xC0]; VB* unkC0; };
struct Mid { char pad[0xD8]; Inner* unkD8; };
struct Self { char pad0[0x10]; Mid* unk10; char pad1[4]; VA* unk18; };

extern "C" void fn_27_B2248(Self* self, int* arg1) {
    int count = self->unk18->v3();
    VB* b;
    void* obj;
    int off;
    int i;
    i = 0;
    off = 0;
    while (i < count) {
        Inner* d = self->unk10->unkD8;
        b = d->unkC0;
        obj = self->unk18->v1(i, d);
        void* p = b->v2();
        construct__15soCameraSubjectFQ25Heaps8HeapTypeP13soCameraRangeUl(obj, p, *arg1 + off, 0);
        off += 0x14;
        i += 1;
    }
}
}

// TODO: clean up, give variables real names
namespace jj_B2B64 {
struct Pair {
    u32 a;
    u32 b;
};

struct Inner {
    virtual void vf0();
    virtual void* vf1(u32 x, float y);
};

struct Outer {
    char pad[0x18];
    Inner* m_inner;
};

extern "C" void zoomIn__15soCameraSubjectFs5Vec2ff(void* obj, float f, u32 a, Pair* p);

extern "C" void fn_27_B2B64(Outer* self, u32 a, u32 b, const Pair* p, float f) {
    Pair tmp = *p;
    void* ret = self->m_inner->vf1(b, f);
    zoomIn__15soCameraSubjectFs5Vec2ff(ret, f, a, &tmp);
}
}

// TODO: clean up, give variables real names
namespace jj_B2C90 {
struct Outer;

class Interface {
public:
    virtual void unused();
    virtual Outer* get();
};

struct Object {
    unsigned char pad[0x18];
    Interface* interface;
};

extern "C" unsigned int getPri__15soCameraSubjectFv(Outer*);

extern "C" unsigned int fn_27_B2C90(Object p)
{
    return getPri__15soCameraSubjectFv(p.interface->get());
}
}

// TODO: clean up, give variables real names
namespace jj_B2D80 {
typedef unsigned int u32;

struct Outer;

struct Interface {
    virtual void unused();
    virtual Outer* get();
};

struct Object {
    char pad[0x18];
    Interface* object;
};

extern "C" u32 getAdvPri__15soCameraSubjectFv(Outer*);

extern "C" u32 fn_27_B2D80(Object self) {
    return getAdvPri__15soCameraSubjectFv(self.object->get());
}
}

// TODO: clean up, give variables real names
namespace jj_B2E70 {
struct Outer;
class Interface {
public:
    virtual void unused();
    virtual Outer* get();
};
struct Object {
    unsigned char pad[0x18];
    Interface* interface;
};
extern "C" unsigned int getPlayerNo__15soCameraSubjectFv(Outer*);
extern "C" unsigned int fn_27_B2E70(Object p)
{
    return getPlayerNo__15soCameraSubjectFv(p.interface->get());
}
}

// TODO: clean up, give variables real names
namespace jj_B302C {
extern "C" float* getInstance__15soCameraSubjectFv(void* p);

class soCallbackBase {
public:
    virtual void f0();
    virtual void* f1(u32 arg);
};

struct soCallbackOwner {
    u8 pad[0x18];
    soCallbackBase* callback;
};

struct Vec4 { float x, y, z, w; };

extern "C" void fn_27_B302C(soCallbackOwner* __restrict self, Vec4* out, u32 arg) {
    float* work = (float*)getInstance__15soCameraSubjectFv(self->callback->f1(arg));
    out->x = work[15];
    out->y = work[16];
    out->z = work[17];
    out->w = work[18];
}
}

// TODO: clean up, give variables real names
namespace jj_B31A4 {


struct Vec3 {
    float x, y, z;

    Vec3() {}
    Vec3(float c, float b, float a) : x(a), y(b), z(c) {}
};

struct Vec4 {
    float x, y, z, w;
};

struct Matrix {
    float m[3][4];
};

struct Entry {
    void* p;
    float x, y, z, w;
};

struct List {
    Entry* items;
    unsigned int count;
};

struct Obj {
    char pad[8];

    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual Vec3 get(void*, const Vec3*, int, int);
    virtual void v40();
    virtual void v41();
    virtual void matrix(void*, Matrix*, int);
};

struct Link {
    char pad[4];
    Obj* obj;
};

struct Holder {
    char pad[0xd8];
    Link* link;
};

struct Mgr {
    char pad0[0x10];
    Holder* holder;
    char pad1[0xc];
    List* list;
};

extern "C" {
extern const float lbl_27_rodata_DC0[];
void* getCameraInternal__16CameraControllerFv(void*);
void getScalingFactor__FP6MatrixP5Vec3f(const Matrix*, Vec3*);
unsigned int isClipIn__8gfCameraFPCQ28gfCamera10ClipSphere(void*, Vec4*, float, float);
}

extern "C" int fn_27_B31A4(Mgr* self, int mode) {
    const float* k = lbl_27_rodata_DC0;
    Entry* e;
    int off;
    unsigned int count;
    void* mgr;
    unsigned int i;
    List* list = self->list;

    if (!list)
        return 1;

    count = list->count;
    if (!mode) {
        mgr = gfCameraManager::getManager();
    } else {
        CameraController* cam = CameraController::getInstance();
        mgr = getCameraInternal__16CameraControllerFv(cam);
    }

    float min;
    float eps;
    eps = k[1];
    i = 0;
    min = k[2];
    off = 0;

    for (; i < count; off += 0x14, i++) {
        e = (Entry*)((char*)self->list->items + off);
        Matrix mat;
        Vec4 p;
        Vec3 d;
        float vz = e->z;
        float vy = e->y;
        float vx = e->x;
        Vec3 pos = self->holder->link->obj->get(
            e->p, &Vec3(vz, vy, vx), 0, 0);
        float radius = e->w;
        self->holder->link->obj->matrix(e->p, &mat, 0);
        getScalingFactor__FP6MatrixP5Vec3f(&mat, &d);

        float len2 = d.z * d.z + (d.x * d.x + d.y * d.y);
        float len;
        if ((float)(double)nw4r::math::FAbs(len2) <= eps)
            len = k[0];
        else
            len = len2 * rsqrtf(len2);

        radius *= len;
        if (radius < min)
            radius = min;

        float z = pos.z;
        float y = pos.y;
        float x = pos.x;
        p.y = y;
        p.x = x;
        p.z = z;
        p.w = radius;

        if (isClipIn__8gfCameraFPCQ28gfCamera10ClipSphere(mgr, &p, z, y) == 1U)
            return 1;
    }

    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_B3398 {
struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(float c, float b, float a) : x(a), y(b), z(c) {}
};

struct Vec4 { float x, y, z, w; };
struct Matrix { float m[3][4]; };
struct Entry { void* p; float x, y, z, w; };
struct List { Entry* items; unsigned int count; };

struct Obj {
    char pad[8];
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual Vec3 get(void*, const Vec3*, int, int);
    virtual void v40();
    virtual void v41();
    virtual void matrix(void*, Matrix*, int);
};

struct Link { char pad[4]; Obj* obj; };
struct Holder { char pad[0xd8]; Link* link; };
struct Mgr {
    char pad0[0x10];
    Holder* holder;
    char pad1[0xc];
    List* list;
};

typedef Vec3 (*Fn9C)(void*, void*, const Vec3*, int, int);
typedef void (*FnA8)(void*, void*, Matrix*, int);

extern "C" {
extern const float lbl_27_rodata_DC0[];
void* getManager__15gfCameraManagerFv(Mgr*);
void* getInstance__16CameraControllerFv();
void* getCameraInternal__16CameraControllerFv(void*);
void getScalingFactor__FP6MatrixP5Vec3f(const Matrix*, Vec3*);
float rsqrtf__Ff(float);
unsigned int isClipInAll__8gfCameraFPCQ28gfCamera10ClipSphere(void*, Vec4*, float, float);
}

extern "C" int fn_27_B3398(Mgr* self, int mode) {
    const float* k = lbl_27_rodata_DC0;
    Entry* e;
    int off;
    unsigned int count;
    void* mgr;
    unsigned int i;
    List* list = self->list;

    if (!list) return 1;

    count = list->count;
    if (!mode) {
        mgr = getManager__15gfCameraManagerFv(self);
    } else {
        void* cam = getInstance__16CameraControllerFv();
        mgr = getCameraInternal__16CameraControllerFv(cam);
    }

    float min;
    float eps;
    eps = k[1];
    i = 0;
    min = k[2];
    off = 0;

    for (; i < count; off += 0x14, i++) {
        e = (Entry*)((char*)self->list->items + off);
        Matrix mat;
        Vec4 p;
        Vec3 d;
        float vz = e->z;
        float vy = e->y;
        float vx = e->x;
        Vec3 pos = self->holder->link->obj->get(
            e->p, &Vec3(vz, vy, vx), 0, 0);
        float radius = e->w;
        self->holder->link->obj->matrix(e->p, &mat, 0);
        getScalingFactor__FP6MatrixP5Vec3f(&mat, &d);

        float len2 = d.z * d.z + (d.x * d.x + d.y * d.y);
        float len;
        if ((float)__fabs(len2) <= eps) len = k[0];
        else len = len2 * rsqrtf__Ff(len2);

        radius *= len;
        if (radius < min) radius = min;

        float z = pos.z;
        float y = pos.y;
        float x = pos.x;
        p.y = y;
        p.x = x;
        p.z = z;
        p.w = radius;

        if (isClipInAll__8gfCameraFPCQ28gfCamera10ClipSphere(mgr, &p, z, y) == 1U) return 1;
    }

    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_B358C {

struct Vec3 {
    float x, y, z;
    Vec3() {}
    Vec3(float c, float b, float a) : x(a), y(b), z(c) {}
};

struct Vec4 { float x, y, z, w; };
struct Matrix { float m[3][4]; };
struct Entry { void* p; float x, y, z, w; };
struct List { Entry* items; unsigned int count; };

struct Obj {
    char pad[8];
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4();
    virtual void v5();
    virtual void v6();
    virtual void v7();
    virtual void v8();
    virtual void v9();
    virtual void v10();
    virtual void v11();
    virtual void v12();
    virtual void v13();
    virtual void v14();
    virtual void v15();
    virtual void v16();
    virtual void v17();
    virtual void v18();
    virtual void v19();
    virtual void v20();
    virtual void v21();
    virtual void v22();
    virtual void v23();
    virtual void v24();
    virtual void v25();
    virtual void v26();
    virtual void v27();
    virtual void v28();
    virtual void v29();
    virtual void v30();
    virtual void v31();
    virtual void v32();
    virtual void v33();
    virtual void v34();
    virtual void v35();
    virtual void v36();
    virtual Vec3 get(void*, const Vec3*, int, int);
    virtual void v40();
    virtual void v41();
    virtual void matrix(void*, Matrix*, int);
};

struct Link { char pad[4]; Obj* obj; };
struct Holder { char pad[0xd8]; Link* link; };
struct Mgr {
    char pad0[0x10];
    Holder* holder;
    char pad1[0xc];
    List* list;
};

extern "C" {
extern const float lbl_27_rodata_DC0[];
void* getManager__15gfCameraManagerFv();
void* getInstance__16CameraControllerFv();
void* getCameraInternal__16CameraControllerFv(void*);
void getScalingFactor__FP6MatrixP5Vec3f(const Matrix*, Vec3*);
float rsqrtf__Ff(float);
u8 getClipOutStatus__8gfCameraFPCQ28gfCamera10ClipSphere(void*, Vec4*, float, float);
}

extern "C" u8 fn_27_B358C(Mgr* self, int mode) {
    const float* k = lbl_27_rodata_DC0;
    Entry* e;
    int off;
    unsigned int count;
    void* mgr;
    unsigned int i;
    List* list = self->list;

    if (!list) return 0xff;

    count = list->count;
    if (!mode) {
        mgr = getManager__15gfCameraManagerFv();
    } else {
        void* cam = getInstance__16CameraControllerFv();
        mgr = getCameraInternal__16CameraControllerFv(cam);
    }

    float min;
    float eps;
    eps = k[1];
    i = 0;
    min = k[2];
    off = 0;

    for (; i < count; off += 0x14, i++) {
        e = (Entry*)((char*)self->list->items + off);
        Matrix mat;
        Vec4 p;
        Vec3 d;
        float vz = e->z;
        float vy = e->y;
        float vx = e->x;
        Vec3 pos = self->holder->link->obj->get(
            e->p, &Vec3(vz, vy, vx), 0, 0);
        float radius = e->w;
        self->holder->link->obj->matrix(e->p, &mat, 0);
        getScalingFactor__FP6MatrixP5Vec3f(&mat, &d);

        float len2 = d.z * d.z + (d.x * d.x + d.y * d.y);
        float len;
        if ((float)__fabs(len2) <= eps) len = k[0];
        else len = len2 * rsqrtf__Ff(len2);

        radius *= len;
        if (radius < min) radius = min;

        float z = pos.z;
        p.y = pos.y;
        p.x = pos.x;
        p.z = z;
        p.w = radius;

        u8 result = getClipOutStatus__8gfCameraFPCQ28gfCamera10ClipSphere(mgr, &p, pos.z, pos.y);
        if (result) return result;
    }

    return 0;
}
}

// TODO: clean up, give variables real names
namespace jj_B3968 {
class FlagArray {
public:
    virtual ~FlagArray();
    virtual void* at(unsigned int index);
};
struct Work {
    unsigned int padding[6];
    FlagArray* flags;
};
extern "C" void getCameraRangeGlobalRect__15soCameraSubjectFP6Rect2D(void*, const void*);
extern "C" void fn_27_B3968(Work* __restrict work, const void* value, unsigned int index) {
    FlagArray* flags = work->flags;
    getCameraRangeGlobalRect__15soCameraSubjectFP6Rect2D(flags->at(index), value);
}
}

// TODO: clean up, give variables real names
namespace jj_B4688 {
extern "C" void fn_27_B3A60(void*);

extern "C" void fn_27_B4688(void* self) {
    fn_27_B3A60(reinterpret_cast<u8*>(self) - 0x4);
}
}

