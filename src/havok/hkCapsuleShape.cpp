// Havok translation unit hkCapsuleShape.o (main.dol 0x802CEB14-0x802D0144).
// Functions in address order:
//   0x802CEB14    32  finishLoadedObjecthkCapsuleShape   [map: hkCapsuleShape__finishLoadedObjecthkCapsuleShape]
//   0x802CEB34    20  cleanupLoadedObjecthkCapsuleShape   [map: hkCapsuleShape__cleanupLoadedObjecthkCapsuleShape]
//   0x802CEB48    92  __dt   [map: hkCapsuleShape____dt]
//   0x802CEBA4    60  getVtablehkCapsuleShape   [map: hkCapsuleShape__getVtablehkCapsuleShape]
//   0x802CEBE0   136  __ct   [map: hkCapsuleShape____ct]
//   0x802CEC68   200  getSupportingVertex   [map: hkCapsuleShape__getSupportingVertex]
//   0x802CED30   288  convertVertexIdsToVertices   [map: hkCapsuleShape__convertVertexIdsToVertices]
//   0x802CEE50    36  getFirstVertex   [map: hkCapsuleShape__getFirstVertex]
//   0x802CEE74     8  getNumVertices   [map: hkCapsuleShape__getNumVertices]
//   0x802CEE7C    20  getCollisionSpheresInfo   [map: hkCapsuleShape__getCollisionSpheresInfo]
//   0x802CEE90    72  getCollisionSpheres   [map: hkCapsuleShape__getCollisionSpheres]
//   0x802CEED8  1120  getAabb   [map: hkCapsuleShape__getAabb]
//   0x802CF338   668  closestInfLineSegInfLineSeg   [map: hkCapsuleShape__closestInfLineSegInfLineSeg]
//   0x802CF5D4   352  closestPointLineSeg   [map: hkCapsuleShape__closestPointLineSeg]
//   0x802D0084     8  getType   [map: hkCapsuleShape__getType]
//   0x802D008C   100  calcStatistics   [map: hkCapsuleShape__calcStatistics]
//   0x802D00F0    84  __sinit_\hkCapsuleShape_cpp   [map: hkCapsuleShapecpp____sinit_]

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
    while (--numIds >= 0) {
        u16 id = *ids;
        *out = *(const hkVector4*)((const char*)vertices + id);
        out->setIntW(id);
        out++;
        ids++;
    }
}

#pragma scheduling off
void hkCapsuleShape::getFirstVertex(hkVector4& out) const {
    out.x = m_vertexB.x;
    out.y = m_vertexB.y;
    out.z = m_vertexB.z;
    out.w = m_vertexB.w;
}
#pragma scheduling reset

int hkCapsuleShape::getNumVertices() const {
    return 2;
}

void hkCapsuleShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 2;
    out->m_flag = 1;
}

// The two capsule end points, copied in order.
#pragma scheduling off
hkVector4* hkCapsuleShape::getCollisionSpheres(hkVector4* out) const {
    hkVector4* result = out;
    out[0] = m_vertexA;
    out[1] = m_vertexB;
    return result;
}
#pragma scheduling reset

int hkCapsuleShape::getType() const {
    return HK_SHAPE_CAPSULE;
}

// Not yet decompiled in this unit:
//   0x802CEE90    72  getCollisionSpheres   [map: hkCapsuleShape__getCollisionSpheres]
//   0x802CEED8  1120  getAabb   [map: hkCapsuleShape__getAabb]
//   0x802CF338   668  closestInfLineSegInfLineSeg   [map: hkCapsuleShape__closestInfLineSegInfLineSeg]
//   0x802CF5D4   352  closestPointLineSeg   [map: hkCapsuleShape__closestPointLineSeg]
//   0x802CF734  2384  castRay   [map: hkCapsuleShape__castRay]

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
