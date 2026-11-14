// Havok translation unit hkSphereShape.o (main.dol 0x802D5D88-0x802D6454).
// Functions in address order:
//   0x802D5D88    32  finishLoadedObjecthkSphereShape   [map: hkSphereShape__finishLoadedObjecthkSphereShape]
//   0x802D5DA8    20  cleanupLoadedObjecthkSphereShape   [map: hkSphereShape__cleanupLoadedObjecthkSphereShape]
//   0x802D5DBC    60  getVtablehkSphereShape   [map: hkSphereShape__getVtablehkSphereShape]
//   0x802D5DF8    36  __ct   [map: hkSphereShape____ct]
//   0x802D5E1C     8  getType   [map: hkSphereShape__getType]
//   0x802D5E24    24  getSupportingVertex   [map: hkSphereShape__getSupportingVertex]
//   0x802D5E3C   204  convertVertexIdsToVertices   [map: hkSphereShape__convertVertexIdsToVertices]
//   0x802D5F08    24  getFirstVertex   [map: hkSphereShape__getFirstVertex]
//   0x802D5F20     8  getNumVertices   [map: hkSphereShape__getNumVertices]
//   0x802D5F28   132  getAabb   [map: hkSphereShape__getAabb]
//   0x802D5FAC    16  getCollisionSpheresInfo   [map: hkSphereShape__getCollisionSpheresInfo]
//   0x802D5FBC    72  getCollisionSpheres   [map: hkSphereShape__getCollisionSpheres]
//   0x802D6004   920  castRay   [map: hkSphereShape__castRay]
//   0x802D639C   100  calcStatistics   [map: hkSphereShape__calcStatistics]
//   0x802D6400    84  __sinit_\hkSphereShape_cpp   [map: hkSphereShapecpp____sinit_]

#include <havok/hkSphereShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>
#include <havok/hkWorldCinfo.h>
#include <havok/hkShapeRayCastInput.h>
#include <havok/hkMonitorStream.h>

void hkSphereShape::finishLoadedObjecthkSphereShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkSphereShape(flag);
}

void hkSphereShape::cleanupLoadedObjecthkSphereShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

#pragma push
#pragma auto_inline off
const void* hkSphereShape::getVtablehkSphereShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x10] __attribute__((aligned(16)));
    hkSphereShape* p = ::new (buf) hkSphereShape(flag);
    return *(const void**)p;
}
#pragma pop

hkSphereShape::hkSphereShape(hkReal radius) : hkConvexShape(radius) {}

int hkSphereShape::getType() const {
    return HK_SHAPE_SPHERE;
}

void hkSphereShape::getSupportingVertex(const hkVector4& direction, hkVector4& out) const {
    out.setZero4();
}

// Every requested vertex is the sphere centre (the origin). The asm is an unrolled zero fill of numIds vectors.
void hkSphereShape::convertVertexIdsToVertices(const u16* ids, int numIds, hkVector4* out) const {
    for (int i = 0; i < numIds; i++) {
        out->w = 0.0f;
        out->z = 0.0f;
        out->y = 0.0f;
        out->x = 0.0f;
        out++;
    }
}

void hkSphereShape::getFirstVertex(hkVector4& out) const {
    out.setZero4();
}

int hkSphereShape::getNumVertices() const {
    return 1;
}

void hkSphereShape::getAabb(const hkTransform& xf, hkReal expansion, hkAabb& out) const {
    // HYPOTHESIS: the target also stores r into a dead 16-byte aligned stack vector (an hkVector4 extent splat in
    // the original, its stores scheduled among the lanes); that vector form (extent, setSub4/setAdd4) compiles to
    // the same instructions with the extent stores sunk to the end, and scores lower than this scalar form.
    hkReal r = expansion + m_radius;
    const hkVector4& t = xf.m_translation;
    out.m_min.set(t.x - r, t.y - r, t.z - r, t.w - r);
    out.m_max.set(t.x + r, t.y + r, t.z + r, t.w + r);
}

void hkSphereShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 1;
    out->m_flag = 1;
}

#pragma scheduling off
hkVector4* hkSphereShape::getCollisionSpheres(hkVector4* out) const {
    hkVector4 sphere;
    sphere.set(0.0f, 0.0f, 0.0f, m_radius);
    *out = sphere;
    return out;
}
#pragma scheduling reset

// Ray against the sphere around the origin: solves |from + t * dir|^2 = r^2 for the first t in [0, hitFraction).
// A start point far away (B^2 > 1000 A r^2) is first moved onto the closest point of the ray, for precision.
hkBool hkSphereShape::castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const {
    HK_TIMER_BEGIN("rcSphere");
    {
        hkReal radius2 = m_radius * m_radius;
        hkVector4 dir;
        dir.setSub4(input.m_to, input.m_from);
        hkReal B = dir.dot3(input.m_from);
        if (B >= 0.0f) {
            goto rayMissed;
        }
        hkReal A = dir.lengthSquared3();

        hkVector4 from;
        hkReal offset;
        if (B * B > 1000.0f * (A * radius2)) {
            if (A < radius2) {
                goto rayMissed;
            }
            offset = -B;
            hkReal t = offset / A;
            from.setInterpolate4(input.m_from, input.m_to, t);
            B = 0.0f;
        } else {
            offset = 0.0f;
            from = input.m_from;
        }

        hkReal C = from.lengthSquared3() - radius2;
        hkReal det = B * B - A * C;
        if (det <= 0.0f) {
            goto rayMissed;
        }
        hkReal t = -B - hkSimdReal(1.0f / hkVector4_sqrtInverse(det));
        t += offset;
        if (t >= results.m_hitFraction * A) {
            goto rayMissed;
        }
        if (t < 0.0f) {
            goto rayMissed;
        }
        t = t / A;
        results.m_hitFraction = t;
        results.m_normal.setInterpolate4(input.m_from, input.m_to, t);
        results.unk18[results.unk38] = 0xFFFFFFFF;
        results.m_normal.mul4(1.0f / m_radius);
    }
    HK_TIMER_END();
    return true;

rayMissed:
    HK_TIMER_END();
    return false;
}

// Statistics collector slots used by calcStatistics (vtable offsets 0x0C and 0x20).
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void addObject(const char* name, int count, const void* object); // 0x0C
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();                                                    // 0x20
};

void hkSphereShape::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->addObject("SphereShape", 1, this);
    ((hkStatisticsCollectorIface*)collector)->unk20();
}

static hkStaticTypeInfo hkSphereShapeTypeInfo("hkSphereShape", hkSphereShape::finishLoadedObjecthkSphereShape, hkSphereShape::cleanupLoadedObjecthkSphereShape,
                                      hkSphereShape::getVtablehkSphereShape());
