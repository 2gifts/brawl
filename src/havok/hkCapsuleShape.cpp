// Havok translation unit hkCapsuleShape.o (main.dol 0x802CEB14-0x802D0144).
// Functions in address order:
//   0x802CEB14    32  finishLoadedObjecthkCapsuleShape
//   0x802CEB34    20  cleanupLoadedObjecthkCapsuleShape
//   0x802CEB48    92  __dt
//   0x802CEBA4    60  getVtablehkCapsuleShape
//   0x802CEBE0   136  __ct
//   0x802CEC68   200  getSupportingVertex
//   0x802CED30   288  convertVertexIdsToVertices
//   0x802CEE50    36  getFirstVertex
//   0x802CEE74     8  getNumVertices
//   0x802CEE7C    20  getCollisionSpheresInfo
//   0x802CEE90    72  getCollisionSpheres
//   0x802CEED8  1120  getAabb
//   0x802CF338   668  closestInfLineSegInfLineSeg
//   0x802CF5D4   352  closestPointLineSeg
//   0x802D0084     8  getType
//   0x802D008C   100  calcStatistics
//   0x802D00F0    84  __sinit_\hkCapsuleShape_cpp

#include <havok/hkCapsuleShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>
#include <havok/hkShapeRayCastInput.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkSphereShape.h>
#include <havok/hkCdPoint.h>


void hkCapsuleShape::finishLoadedObjecthkCapsuleShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkCapsuleShape(flag);
}

void hkCapsuleShape::cleanupLoadedObjecthkCapsuleShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkCapsuleShape::~hkCapsuleShape() {
}

#pragma push
#pragma auto_inline off
const void* hkCapsuleShape::getVtablehkCapsuleShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x30] __attribute__((aligned(16)));
    hkCapsuleShape* p = ::new (buf) hkCapsuleShape(flag);
    return *(const void**)p;
}
#pragma pop

hkCapsuleShape::hkCapsuleShape(const hkVector4& vertexA, const hkVector4& vertexB, hkReal radius)
    : hkConvexShape(radius) {
    m_vertexA = vertexA;
    m_vertexB = vertexB;
    m_vertexA.w = radius;
    m_vertexB.w = radius;
}

void hkCapsuleShape::convertVertexIdsToVertices(const u16* ids, int numIds, hkVector4* out) const {
    const hkVector4* vertices = &m_vertexA;
    for (int i = 0; i <= numIds - 1; i++) {
        int id = ids[i];
        out[i] = *(const hkVector4*)((const char*)vertices + id);
        out[i].setIntW(id);
    }
}

void hkCapsuleShape::getFirstVertex(hkVector4& out) const {
    out.x = m_vertexB.x;
    out.y = m_vertexB.y;
    out.z = m_vertexB.z;
    out.w = m_vertexB.w;
}

int hkCapsuleShape::getNumVertices() const {
    return 2;
}

void hkCapsuleShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 2;
    out->m_flag = 1;
}

// The two capsule end points, copied in order.
hkVector4* hkCapsuleShape::getCollisionSpheres(hkVector4* out) const {
    hkVector4* result = out;
    out[0] = m_vertexA;
    out[1] = m_vertexB;
    return result;
}

int hkCapsuleShape::getType() const {
    return HK_SHAPE_CAPSULE;
}

// Not yet decompiled in this unit:
//   0x802CEE90    72  getCollisionSpheres
//   0x802CEED8  1120  getAabb
//   0x802CF338   668  closestInfLineSegInfLineSeg
//   0x802CF5D4   352  closestPointLineSeg
//   0x802CF734  2384  castRay

static hkStaticTypeInfo hkCapsuleShapeTypeInfo("hkCapsuleShape", hkCapsuleShape::finishLoadedObjecthkCapsuleShape, hkCapsuleShape::cleanupLoadedObjecthkCapsuleShape,
                                      hkCapsuleShape::getVtablehkCapsuleShape());

// BYTES: the ray code is compiled with fused multiply-add contraction (as the rest of the unit after it).
#pragma fp_contract on

// A ray starting inside the capsule misses. Otherwise the ray is tested against the infinite cylinder around the
// axis: a hit between the end points is a body hit; anything else goes to the sphere of the nearer end.
hkBool hkCapsuleShape::castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const {
    HK_TIMER_BEGIN("rcCapsule");
    {
        hkVector4 closest;
        closestPointLineSeg(input.m_from, m_vertexA, m_vertexB, closest);
        hkVector4 fromToAxis;
        fromToAxis.setSub4(input.m_from, closest);
        if (fromToAxis.length3() < m_radius) {
            goto rayMissed;
        }
    }
    {
        hkVector4 rayDir;
        rayDir.setSub4(input.m_to, input.m_from);
        hkVector4 axis;
        axis.setSub4(m_vertexB, m_vertexA);
        hkReal distSquared = HK_REAL_MAX;
        hkReal rayT;
        hkReal axisT;
        hkVector4 rayPoint;
        hkVector4 axisPoint;
        closestInfLineSegInfLineSeg(input.m_from, rayDir, m_vertexA, axis, distSquared, rayT, axisT, rayPoint,
                                    axisPoint);
        if (distSquared > m_radius * m_radius) {
            goto rayMissed;
        }

        hkVector4 axisDir = axis;
        hkReal axisLength = 0.0f;
        if (axisDir.lengthSquared3() > 1.1920929e-07f) {
            axisLength = axisDir.length3();
            axisDir.setMul4(1.0f / axisLength, axisDir);
        } else {
            axisDir.setZero4();
        }

        // back from the closest approach to where the ray enters the cylinder
        hkVector4 perpendicular;
        perpendicular.setAddMul4(rayDir, axisDir, -rayDir.dot3(axisDir));
        hkReal halfChord = 1.0f / hkVector4_sqrtInverse(m_radius * m_radius - distSquared);
        hkReal perpLengthInv = 0.0f;
        hkReal perpLengthSq = perpendicular.lengthSquared3();
        if (0.0f != perpLengthSq) {
            perpLengthInv = hkVector4_sqrtInverse(perpLengthSq);
        }
        hkReal t = rayT - halfChord * perpLengthInv;
        if (t >= results.m_hitFraction) {
            goto rayMissed;
        }

        hkVector4 hit;
        hit.setInterpolate4(input.m_from, input.m_to, t);
        hkReal axisStart = m_vertexA.dot3(axisDir);
        hkReal hitAlong = hit.dot3(axisDir) - axisStart;
        if (t >= 0.0f && hitAlong > 0.0f && hitAlong < axisLength) {
            hkVector4 onAxis;
            onAxis.setInterpolate4(m_vertexA, m_vertexB, hitAlong / axisLength);
            hkVector4 normal;
            normal.setSub4(hit, onAxis);
            normal.normalize3();
            results.m_normal = normal;
            results.m_hitFraction = t;
            results.unk14 = HIT_BODY;
            results.unk18[results.unk38] = 0xFFFFFFFF;
            HK_TIMER_END();
            return true;
        }

        {
            hkReal fromAlong = input.m_from.dot3(axisDir) - axisStart;
            hkVector4 fromOnAxis;
            fromOnAxis.setInterpolate4(m_vertexA, m_vertexB, fromAlong / axisLength);
            hkVector4 fromToAxis;
            fromToAxis.setSub4(input.m_from, fromOnAxis);
            if (fromToAxis.length3() > m_radius && t < 0.0f) {
                goto rayMissed;
            }
        }

        // the end sphere on the side of the hit
        int cap = (hitAlong <= 0.0f) ? HIT_CAP0 : HIT_CAP1;
        const hkVector4& capCenter = (&m_vertexA)[cap];
        hkShapeRayCastInput sphereInput;
        sphereInput.m_from.setSub4(input.m_from, capCenter);
        sphereInput.m_to.setSub4(input.m_to, capCenter);
        sphereInput.unk20 = 0;
        sphereInput.unk24 = 0;
        hkSphereShape sphere(m_radius);
        HK_TIMER_END();
        if (sphere.castRay(sphereInput, results)) {
            results.unk14 = cap;
            return true;
        }
        return false;
    }

rayMissed:
    HK_TIMER_END();
    return false;
}

// Statistics collector slots used by calcStatistics (vtable offsets 0x0C and 0x20); see hkCylinderShape.cpp.
// Shape from https://github.com/jj-link/brawl src/havok/fn_802D008C.cpp.
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void addObject(const char* name, int count, const void* object); // 0x0C
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();                                                    // 0x20
};

// TODO: cleanup (shape from jj-link/brawl fn_802D008C)
void hkCapsuleShape::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->addObject("CapsuleShape", 1, this);
    ((hkStatisticsCollectorIface*)collector)->unk20();
}

// fn_802CEC68: body from https://github.com/jj-link/brawl src/havok/fn_802CEC68.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802CEC68 {
#pragma fp_contract on

extern "C" {

extern "C" float lbl_805A4068;

struct Vec4 {
    float x, y, z, w;
};

struct Seg {
    char pad[0x10];
    Vec4 a;
    Vec4 b;
};

struct Out {
    float x, y, z;
    union {
        float w;
        int id;
    };
};

// TODO: cleanup (shape from jj-link/brawl fn_802CEC68)
void fn_802CEC68(Seg* s, Vec4* dir, Out* out) {
    __declspec(align(16)) Vec4 d;
    d.x = s->b.x - s->a.x;
    d.y = s->b.y - s->a.y;
    d.z = s->b.z - s->a.z;
    d.w = s->b.w - s->a.w;
    float dot = d.x * dir->x + d.y * dir->y + d.z * dir->z;
    int id;
    if (dot < lbl_805A4068) {
        out->x = s->a.x;
        id = 0;
        out->y = s->a.y;
        out->z = s->a.z;
        out->w = s->a.w;
    } else {
        out->x = s->b.x;
        id = 0x10;
        out->y = s->b.y;
        out->z = s->b.z;
        out->w = s->b.w;
    }
    out->id = id;
}

}
} // namespace jj_fn_802CEC68

// fn_802CF5D4: body from https://github.com/jj-link/brawl src/havok/fn_802CF5D4.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802CF5D4 {
#pragma fp_contract on
extern "C" float lbl_805A4068;
struct V4 { float x, y, z, w; } __attribute__((aligned(16)));

static inline float dot3(const V4& a, const V4& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

} // namespace jj_fn_802CF5D4

// TODO: cleanup (shape from jj-link/brawl fn_802CF5D4)
void hkCapsuleShape::closestPointLineSeg(const hkVector4& point, const hkVector4& segA, const hkVector4& segB,
                                         hkVector4& closest) {
    using namespace jj_fn_802CF5D4;
    const V4* a = (const V4*)&point;
    const V4* b = (const V4*)&segA;
    const V4* c = (const V4*)&segB;
    V4* out = (V4*)&closest;
    V4 d, e;
    d.x = b->x - a->x; d.y = b->y - a->y; d.z = b->z - a->z; d.w = b->w - a->w;
    e.x = c->x - b->x; e.y = c->y - b->y; e.z = c->z - b->z; e.w = c->w - b->w;
    float p = dot3(e, d);
    p = -p;
    float l = dot3(e, e);
    if (p <= lbl_805A4068) {
        out->x = b->x; out->y = b->y; out->z = b->z; out->w = b->w;
    } else if (p >= l) {
        out->x = c->x; out->y = c->y; out->z = c->z; out->w = c->w;
    } else {
        p /= l;
        out->x = p * e.x + b->x; out->y = p * e.y + b->y;
        out->z = p * e.z + b->z; out->w = p * e.w + b->w;
    }
}

// TODO: clean up, give variables real names
namespace jj_802CEED8 {
#pragma fp_contract on
extern float lbl_805A4068;
struct V { float x,y,z,w; } __attribute__((aligned(16)));
struct M { V a,b,c,d; };
struct Bounds { float p[3]; float center; float a[4]; float b[3]; };
inline void copyMatrix(M* n,const M* mat) {
 V md,mc,mb,ma;
 ma.x=mat->a.x; ma.y=mat->a.y; ma.z=mat->a.z; ma.w=mat->a.w;
 mb.x=mat->b.x; mb.y=mat->b.y; mb.z=mat->b.z; mb.w=mat->b.w;
 mc.x=mat->c.x; mc.y=mat->c.y; mc.z=mat->c.z; mc.w=mat->c.w;
 md.x=mat->d.x; md.y=mat->d.y; md.z=mat->d.z; md.w=mat->d.w;
 n->a.x=ma.x; n->a.y=ma.y; n->a.z=ma.z; n->a.w=ma.w;
 n->b.x=mb.x; n->b.y=mb.y; n->b.z=mb.z; n->b.w=mb.w;
 n->c.x=mc.x; n->c.y=mc.y; n->c.z=mc.z; n->c.w=mc.w;
 n->d.x=md.x; n->d.y=md.y; n->d.z=md.z; n->d.w=md.w;
}
extern "C" void fn_802CEED8(const Bounds* bd, const M* mat, float* out, float off) {
 V dv;
 V result[2];
 M n;
 float d=off+bd->center;
 dv.x=d; dv.y=d; dv.z=d; dv.w=d;
 copyMatrix(&n,mat);
 const float* input=bd->a;
 for(int i=0;i<2;i++) {
 int j=i*4;
 float* dst=(float*)result+j;
 const float* src=input+j;
 float x=src[0],y=src[1],z=src[2];
 dst[0]=n.d.x+(x*n.a.x+y*n.b.x+z*n.c.x);
 dst[1]=n.d.y+(x*n.a.y+y*n.b.y+z*n.c.y);
 dst[2]=n.d.z+(x*n.a.z+y*n.b.z+z*n.c.z);
 dst[3]=lbl_805A4068;
 }
 V& pa=result[0];
 V& pb=result[1];
 out[0]=pa.x<pb.x?pa.x:pb.x;
 out[1]=pa.y<pb.y?pa.y:pb.y;
 out[2]=pa.z<pb.z?pa.z:pb.z;
 out[3]=pa.w<pb.w?pa.w:pb.w;
 out[0]-=dv.x; out[1]-=dv.y; out[2]-=dv.z; out[3]-=dv.w;
 out[4]=pa.x>pb.x?pa.x:pb.x;
 out[5]=pa.y>pb.y?pa.y:pb.y;
 out[6]=pa.z>pb.z?pa.z:pb.z;
 out[7]=pa.w>pb.w?pa.w:pb.w;
 out[4]+=dv.x; out[5]+=dv.y; out[6]+=dv.z; out[7]+=dv.w;
}
}

