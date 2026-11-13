// Havok translation unit hkGskCache.o (main.dol 0x8031C8CC-0x8031FEFC).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x8031C8CC  1176  initTriangle   [map: hkGskCache__initTriangle]
//   0x8031CD64   496  init   [map: hkGskCache__init]
//   0x8031CF54   436  findBestProjection   [map: hkConvexPenetrationUtil__findBestProjection]
//   0x8031D108   424  case0To1   [map: hkConvexPenetrationUtil__case0To1]
//   0x8031D2B0   364  case1To2   [map: hkConvexPenetrationUtil__case1To2]
//   0x8031D41C   676  case2To3   [map: hkConvexPenetrationUtil__case2To3]
//   0x8031D6C0   112  case3To4   [map: hkConvexPenetrationUtil__case3To4]
//   0x8031D730   460  getPointForSimplex   [map: hkConvexPenetrationUtil__getPointForSimplex]
//   0x8031D8FC   176  addPointToSimplex   [map: hkConvexPenetrationUtil__addPointToSimplex]
//   0x8031D9AC   556  tryToExpandPolytope   [map: hkConvexPenetrationUtil__tryToExpandPolytope]
//   0x8031DBD8   776  convertResultToFeature   [map: hkConvexPenetrationUtil__convertResultToFeature]
//   0x8031DEE0  2076  builtTetrahedron   [map: hkConvexPenetrationUtil__builtTetrahedron]
//   0x8031E6FC   836  calculatePenetrationDepth   [map: hkConvexPenetrationUtil__calculatePenetrationDepth]
//   0x8031EA40    76  rand01   [map: hkConvexPenetrationUtil__rand01]
//   0x8031EA8C   156  randomVector   [map: hkConvexPenetrationUtil__randomVector]
//   0x8031EB28   576  hkCalculatePenetrationDepth   [map: hkConvexShape__hkCalculatePenetrationDepth]
//   0x8031ED68  1632  hkCalcMultiPenetrationDepth   [map: hkTransform__hkCalcMultiPenetrationDepth]
//   0x8031F458   516  hkCvxCvxDistByHeuristicSamplingPerformSample   [map: hkConvexShape__hkCvxCvxDistByHeuristicSamplingPerformSample]
//   0x8031F65C  2208  hkCvxCvxDistByHeuristicSampling   [map: hkCvxCvxDistByHeuristicSampling__hkCvxCvxDistByHeuristicSampling]

// fn_8031D6C0: body from https://github.com/jj-link/brawl src/havok/fn_8031D6C0.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031D6C0 {
struct Obj {
    char pad[0x40];
    float threshold;
    char pad2[0x24];
    void* c;
};
struct Matrix { float m[4][4]; } __attribute__((aligned(16)));
extern "C" void fn_8031CF54(Obj*, void*, void*, Matrix*, float*);
extern "C" void fn_8031D8FC(Obj*, const Matrix*);
extern "C" int fn_8031D6C0(Obj* self, void* arg) {
    Matrix matrix;
    float value;
    fn_8031CF54(self, arg, self->c, &matrix, &value);
    if (value < self->threshold) return 1;
    fn_8031D8FC(self, &matrix);
    return 0;
}
} // namespace jj_fn_8031D6C0

// fn_8031EA8C: body from https://github.com/jj-link/brawl src/havok/fn_8031EA8C.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031EA8C {
extern "C" float fn_8031EA40(int*);

extern "C" void fn_8031EA8C(int arg0, float* arg1, float farg0, float farg1) {
    int sp8 = arg0;
    arg1[0] = (farg1 - farg0) * (farg0 + fn_8031EA40(&sp8));
    arg1[1] = (farg1 - farg0) * (farg0 + fn_8031EA40(&sp8));
    arg1[2] = (farg1 - farg0) * (farg0 + fn_8031EA40(&sp8));
}
} // namespace jj_fn_8031EA8C

// fn_8031D8FC: body from https://github.com/jj-link/brawl src/havok/fn_8031D8FC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031D8FC {
struct Vec4 {
    float x, y, z, w;
};

struct Obj {
    char pad[0x60];
    Vec4* a;
    Vec4* b;
    Vec4* c;
    int* index;
};

struct Matrix {
    float m[3][4];
};

extern "C" void fn_8031D8FC(Obj* self, const Matrix* src) {
    Vec4* e = &self->c[*self->index];
    e->x = src->m[0][0];
    e->y = src->m[0][1];
    e->z = src->m[0][2];
    e->w = src->m[0][3];

    e = &self->a[*self->index];
    e->x = src->m[1][0];
    e->y = src->m[1][1];
    e->z = src->m[1][2];
    e->w = src->m[1][3];

    e = &self->b[*self->index];
    e->x = src->m[2][0];
    e->y = src->m[2][1];
    e->z = src->m[2][2];
    e->w = src->m[2][3];

    ++*self->index;
}
} // namespace jj_fn_8031D8FC

// fn_8031D2B0: body from https://github.com/jj-link/brawl src/havok/fn_8031D2B0.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_8031D2B0 {
#pragma fp_contract on

struct Vec4 {
    float x, y, z, w;
};

struct Matrix {
    Vec4 row[4];
} __attribute__((aligned(16)));

struct Obj {
    char pad[0x68];
    Vec4* pos;
};

extern "C" void fn_8031D730(Obj* self, const Vec4* in, Matrix* out);
extern "C" void fn_8031D8FC(Obj* self, const Matrix* src);

extern "C" void fn_8031D2B0(Obj* self) {
    Matrix a;
    Matrix b;
    Vec4 d;
    Vec4 neg;
    Vec4 r;
    Vec4* m;
    float dp;
    float dq;

    m = self->pos;
    fn_8031D730(self, m, &a);
    d.x = a.row[0].x - self->pos->x;
    d.y = a.row[0].y - self->pos->y;
    d.z = a.row[0].z - self->pos->z;
    d.w = a.row[0].w - self->pos->w;
    dp = d.x * d.x + d.y * d.y + d.z * d.z;
    neg.x = -m->x;
    neg.y = -m->y;
    neg.z = -m->z;
    neg.w = -m->w;
    fn_8031D730(self, &neg, &b);
    r.x = b.row[0].x - self->pos->x;
    r.y = b.row[0].y - self->pos->y;
    r.z = b.row[0].z - self->pos->z;
    r.w = b.row[0].w - self->pos->w;
    dq = r.x * r.x + r.y * r.y + r.z * r.z;
    if (dp >= dq) {
        fn_8031D8FC(self, &a);
    } else {
        fn_8031D8FC(self, &b);
    }
}
} // namespace jj_fn_8031D2B0
