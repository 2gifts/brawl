// Havok translation unit hkGsk.o (main.dol 0x803183A0-0x8031C8CC).
// Not yet decompiled. Functions in address order:
//   0x803183A0   716  findClosestTriangleBackup
//   0x8031866C  1352  checkTriangleBoundaries
//   0x80318BB4    28  isNegative
//   0x80318BD0     8  setFeatureChange
//   0x80318BD8  1372  findClosestTriangle
//   0x80319134  1940  processEdgeTriangle
//   0x803198C8  1352  getClosestFeature
//   0x80319E10   880  transformPoints
//   0x8031A180  1612  reduceDimension
//   0x8031A7CC   724  process_point_edge
//   0x8031AAA0    36  __as
//   0x8031AAC4   128  hkGskCalcSupportDistSquared
//   0x8031AB44   312  exitAndExportCacheImpl
//   0x8031AC7C   208  convertFeatureToClosestDistance
//   0x8031AD4C    68  setAddMul4
//   0x8031AD90   868  getClosestPoint
//   0x8031B0F4  2312  handlePenetration
//   0x8031B9FC   932  reduceDimensionExtended
//   0x8031BDA0  2860  hkGskRecalcContactInternal

#include <havok/hkMath.h>
#include <havok/hkGsk.h>

void hkGsk::setFeatureChange(int value) {
    unk14 = value;
}

// The vertex id a simplex vertex keeps in its w word.
static inline u16 hkGsk_vertexId(const hkVector4& v) {
    return (u16)*(const int*)&v.w;
}

// Writes the simplex back to the cache: the dimensions, then each body's vertex ids sorted in descending order
// (B's after A's). The three-element sort swaps the last two back when exactly one swap happened, which keeps the
// winding of a triangle.
// TODO: clean up: the cache comes in as void* (the symbol name); its type is hkGskCache. Draft (35%): the target
// keeps each sort variable in one register across the swaps; ours splits the live ranges.
void hkGsk::exitAndExportCacheImpl(void* arg) {
    hkGskCache& cache = *(hkGskCache*)arg;
    int dimA = m_dimA;
    int dimB = m_dimB;
    cache.m_dimA = dimA;
    cache.m_dimB = dimB;
    {
        const hkVector4* v = verticesA();
        u16 v0 = hkGsk_vertexId(v[0]);
        if (dimA >= 2) {
            u16 v1 = hkGsk_vertexId(v[1]);
            int swaps = 0;
            if (v1 > v0) {
                u16 t = v1;
                v1 = v0;
                v0 = t;
                swaps = 1;
            }
            if (dimA > 2) {
                u16 v2 = hkGsk_vertexId(v[2]);
                if (v2 > v0) {
                    u16 t = v2;
                    v2 = v0;
                    v0 = t;
                    swaps++;
                }
                if (swaps == 1) {
                    u16 t = v2;
                    v2 = v1;
                    v1 = t;
                }
                cache.m_vertices[2] = v2;
            }
            cache.m_vertices[1] = v1;
        }
        cache.m_vertices[0] = v0;
    }
    {
        const hkVector4* v = verticesB();
        u16 v0 = hkGsk_vertexId(v[0]);
        if (m_dimB >= 2) {
            u16 v1 = hkGsk_vertexId(v[1]);
            int swaps = 0;
            if (v1 > v0) {
                u16 t = v1;
                v1 = v0;
                v0 = t;
                swaps = 1;
            }
            if (m_dimB > 2) {
                u16 v2 = hkGsk_vertexId(v[2]);
                if (v2 > v0) {
                    u16 t = v2;
                    v2 = v0;
                    v0 = t;
                    swaps++;
                }
                if (swaps == 1) {
                    u16 t = v2;
                    v2 = v1;
                    v1 = t;
                }
                cache.m_vertices[m_dimA + 2] = v2;
            }
            cache.m_vertices[m_dimA + 1] = v1;
        }
        cache.m_vertices[m_dimA] = v0;
    }
}

// The plane gives the normal and the distance (its w). The point: the single vertex of A, the single vertex of B
// moved along the plane by the distance, or the point kept at 0x130 (weights 1, 0 and 0.5).
void hkGsk::convertFeatureToClosestDistance(const hkVector4& plane, hkGskClosestDistance& out) const {
    out.m_distance = plane.w;
    out.m_normal = plane;
    if (m_dimA == 1) {
        out.m_position = vectorAt(0x20);
        out.m_weight = 1.0f;
    } else if (m_dimB == 1) {
        out.m_position.setAddMul4(vectorAt(0xA0), plane, plane.w);
        out.m_weight = 0.0f;
    } else {
        out.m_position = vectorAt(0x130);
        out.m_weight = 0.5f;
    }
}

// Signed squared distance of the support point difference a - b along the direction.
hkReal hkGskCalcSupportDistSquared(const hkVector4& direction, const hkVector4& a, const hkVector4& b) {
    hkVector4 diff;
    diff.setSub4(a, b);
    hkReal dist = diff.dot3(direction);
    hkReal absDist = (hkReal)__fabs(dist);
    return dist * absDist;
}

// Weak inline copies emitted in this unit; extern "C" placeholders under their fn_ names until the
// instantiating callers are written (shape from https://github.com/jj-link/brawl src/havok/fn_<addr>.cpp).
// hkCdVertex::operator=
// TODO: cleanup (shape from jj-link/brawl fn_8031AAA0)
extern "C" void fn_8031AAA0(float* d, const float* s) {
    d[0] = s[0];
    d[1] = s[1];
    d[2] = s[2];
    d[3] = s[3];
}

// Shape from https://github.com/jj-link/brawl src/havok/fn_80318BB4.cpp.
// hkMath::isNegative-style sign mask (returns 8 for a negative value).
// TODO: cleanup (shape from jj-link/brawl fn_80318BB4)
extern "C" int fn_80318BB4(float f) {
    if (f < 0.0f) {
        return 8;
    }
    return 0;
}

// TODO: cleanup (shape from jj-link/brawl fn_80319E10)
// fn_80319E10: body from https://github.com/jj-link/brawl src/havok/fn_80319E10.cpp (namespaced; callees rebound to our C++ names).
#include <types.h>
namespace jj_fn_80319E10 {
#pragma fp_contract on

struct V { float x,y,z,w; } __attribute__((aligned(16)));
struct M {
    V a,b,c,d;
    M() {}
    M(const M& o) { *this = o; }
    M& operator=(const M& o) {
        a.x=o.a.x; a.y=o.a.y; a.z=o.a.z; a.w=o.a.w;
        b.x=o.b.x; b.y=o.b.y; b.z=o.b.z; b.w=o.b.w;
        c.x=o.c.x; c.y=o.c.y; c.z=o.c.z; c.w=o.c.w;
        d.x=o.d.x; d.y=o.d.y; d.z=o.d.z; d.w=o.d.w;
        return *this;
    }
};
extern "C" const f32 lbl_805A46B8;

extern "C" void fn_80319E10(const M* mat, const float* in, int count, float* out) {
    M n;
    M m = *mat;
    n = m;
    for (int i = 0; i < count; ++i) {
        int j = i * 4;
        float* dst = out + j;
        const float* src = in + j;
        float z = src[2], y = src[1], x = src[0];
        dst[0] = n.d.x + (x*n.a.x + y*n.b.x + z*n.c.x);
        dst[1] = n.d.y + (x*n.a.y + y*n.b.y + z*n.c.y);
        dst[2] = n.d.z + (x*n.a.z + y*n.b.z + z*n.c.z);
        dst[3] = lbl_805A46B8;
    }
}
} // namespace jj_fn_80319E10

// TODO: clean up, give variables real names
namespace jj_8031AD90 {
#pragma fp_contract on
#pragma opt_dead_assignments off
#pragma opt_unroll_loops off
struct Vec { float v[4]; Vec(); } __attribute__((aligned(16)));
struct State {
 int countA, countB, limitA, limitB;
 char flag; char pad[3]; int changed; char pad1[8];
 Vec a[4]; Vec unused[4]; Vec b[4]; Vec localB[4];
 Vec dir; float pad3[16]; int status;
 State() : flag(0) {}
};
inline Vec::Vec() {}
class Object_vptr_0 {
public:
 virtual void slot0(); virtual void slot1(); virtual void slot2();
 virtual void slot3(); virtual void slot4(); virtual void slot5();
 virtual void slot6(); virtual void slot7(); virtual void slot8();
 virtual void slot9(); virtual void slot10();
 virtual void slot11(void*, int, Vec*);
 char pad[8]; float radius;
};
struct Body {
 void* transform; float* mat;
 Object_vptr_0* shape_a; Object_vptr_0* shape_b; float threshold;
};
struct Flags { char pad[8]; signed char f0, f1, f2, f3; };
extern "C" int fn_803198C8(State*, Object_vptr_0*, Object_vptr_0*, void*, Vec*);
extern "C" void fn_8031AB44(State*, void*);
extern const float lbl_805A46B8;
inline Object_vptr_0* prepareFirstShape(State* state, Object_vptr_0* shape, int count) {
 state->countA = count;
 return shape;
}
inline void addScaled(Vec* out, float t, const Vec& direction, const float* base) {
 float x = t * direction.v[0] + base[0];
 float y = t * direction.v[1] + base[1];
 float z = t * direction.v[2] + base[2];
 float w = t * direction.v[3] + base[3];
 out->v[0] = x; out->v[1] = y; out->v[2] = z; out->v[3] = w;
}
extern "C" int fn_8031AD90(Body* __restrict body, Flags* flags, Vec* out1, Vec* out2) {
 Vec v;
 Vec result;
 State state;
 Object_vptr_0* shapeB = body->shape_b;
 Object_vptr_0* shapeA = prepareFirstShape(&state, body->shape_a, flags->f0);
 state.countB = flags->f1;
 state.limitA = flags->f2;
 state.limitB = flags->f3;
 state.changed = 0;
 shapeA->slot11(flags, state.countA, state.a);
 shapeB->slot11((char*)flags + state.countA * 2, state.countB, state.localB);
 fn_803198C8(&state, body->shape_a, body->shape_b, body->transform, &v);
 if (state.changed) fn_8031AB44(&state, flags);
 Object_vptr_0* first = body->shape_a;
 Object_vptr_0* second = body->shape_b;
 float* m = body->mat;
 float w_a = first->radius;
 float x = v.v[0], y = v.v[1], z = v.v[2];
 float w_b = second->radius;
 float diff = (v.v[3] - w_a) - w_b;
 out1->v[0] = z * m[8] + (x * m[0] + y * m[4]);
 out1->v[1] = z * m[9] + (x * m[1] + y * m[5]);
 out1->v[2] = z * m[10] + (x * m[2] + y * m[6]);
 out1->v[3] = diff;
 if (diff < body->threshold) {
  if (state.countA == 1) {
   float t = second->radius - v.v[3];
   addScaled(&result, t, v, state.a[0].v);
  } else if (state.countB == 1) {
   float t = second->radius;
   addScaled(&result, t, v, state.b[0].v);
  } else {
   float t = second->radius - v.v[3];
   addScaled(&result, t, v, state.pad3);
  }
  m = body->mat;
  float pz = result.v[2], py = result.v[1], px = result.v[0];
  out2->v[0] = m[12] + (pz * m[8] + (px * m[0] + py * m[4]));
  out2->v[1] = m[13] + (pz * m[9] + (px * m[1] + py * m[5]));
  out2->v[2] = m[14] + (pz * m[10] + (px * m[2] + py * m[6]));
  out2->v[3] = lbl_805A46B8;
  return 0;
 }
 return 1;
}
}

// TODO: clean up, give variables real names
namespace jj_8031AD4C {
#pragma fp_contract on

extern "C" void setAddMul4__9hkVector4FRC9hkVector4RC9hkVector4f(float* dest, const float* src1, const float* src2, float scale) {
    dest[0] = src1[0] + scale * src2[0];
    dest[1] = src1[1] + scale * src2[1];
    dest[2] = src1[2] + scale * src2[2];
    dest[3] = src1[3] + scale * src2[3];
}
}

