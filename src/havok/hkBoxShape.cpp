// Havok translation unit hkBoxShape.o (main.dol 0x802CD884-0x802CE588).
// Functions in address order:
//   0x802CD884    32  finishLoadedObjecthkBoxShape
//   0x802CD8A4    20  cleanupLoadedObjecthkBoxShape
//   0x802CD8B8    92  __dt
//   0x802CD914    60  getVtablehkBoxShape
//   0x802CD950    96  calcStatistics
//   0x802CD9B0     8  getType
//   0x802CD9B8    36  getFirstVertex
//   0x802CD9DC   748  getAabb
//   0x802CDCC8   204  getSupportingVertex
//   0x802CDD94   300  convertVertexIdsToVertices
//   0x802CDEC0    20  getCollisionSpheresInfo
//   0x802CDED4   448  getCollisionSpheres
//   0x802CE094  1184  castRay
//   0x802CE534    84  __sinit_\hkBoxShape_cpp

#include <havok/hkBoxShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>
#include <havok/hkShapeRayCastInput.h>
#include <havok/hkMonitorStream.h>

void hkBoxShape::finishLoadedObjecthkBoxShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkBoxShape(flag);
}

void hkBoxShape::cleanupLoadedObjecthkBoxShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkBoxShape::~hkBoxShape() {
}

#pragma push
#pragma auto_inline off
const void* hkBoxShape::getVtablehkBoxShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x20] __attribute__((aligned(16)));
    hkBoxShape* p = ::new (buf) hkBoxShape(flag);
    return *(const void**)p;
}
#pragma pop

int hkBoxShape::getType() const {
    return HK_SHAPE_BOX;
}

#pragma scheduling off
void hkBoxShape::getFirstVertex(hkVector4& out) const {
    out = m_halfExtents;
}
#pragma scheduling reset

void hkBoxShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 8;
    out->m_flag = 1;
}

// The eight corners of the unit box, indexed by vertex id / 2: id bit 3 = -x, bit 2 = -y, bit 1 = -z.
static const hkReal s_boxVertexSigns[8][4] = {
    {1.0f, 1.0f, 1.0f, 0.0f},  {1.0f, 1.0f, -1.0f, 0.0f},  {1.0f, -1.0f, 1.0f, 0.0f},  {1.0f, -1.0f, -1.0f, 0.0f},
    {-1.0f, 1.0f, 1.0f, 0.0f}, {-1.0f, 1.0f, -1.0f, 0.0f}, {-1.0f, -1.0f, 1.0f, 0.0f}, {-1.0f, -1.0f, -1.0f, 0.0f},
};

// The corner on the side of the direction: the half extents with the direction's sign bits; w holds the vertex id
// (the negative-lane mask without the w bit).
// TODO: clean up: the sign copy works on the float bits through u32 views; a readable version would be an hkVector4
// helper (setFlipSign4) once its out-of-line form is known.
void hkBoxShape::getSupportingVertex(const hkVector4& direction, hkVector4& out) const {
    out = m_halfExtents;
    const u32* d = (const u32*)&direction;
    u32* o = (u32*)&out;
    o[0] ^= d[0] & 0x80000000;
    o[1] ^= d[1] & 0x80000000;
    o[2] ^= d[2] & 0x80000000;
    o[3] ^= d[3] & 0x80000000;
    out.setIntW(out.compareLessThanZero4() & 0xE);
}

// Each id selects a corner (half extents times the corner's signs); w keeps the id.
void hkBoxShape::convertVertexIdsToVertices(const u16* ids, int numIds, hkVector4* out) const {
    for (int i = numIds - 1; i >= 0; i--) {
        int id = *ids++;
        const hkVector4& sign = *(const hkVector4*)((const u8*)s_boxVertexSigns + id * 8);
        out->x = m_halfExtents.x * sign.x;
        out->y = m_halfExtents.y * sign.y;
        out->z = m_halfExtents.z * sign.z;
        out->w = m_halfExtents.w * sign.w;
        out->setIntW(id);
        out++;
    }
}

// Not yet decompiled in this unit:
//   0x802CD9DC   748  getAabb
//   0x802CDED4   448  getCollisionSpheres
//   0x802CE094  1184  castRay

static hkStaticTypeInfo hkBoxShapeTypeInfo("hkBoxShape", hkBoxShape::finishLoadedObjecthkBoxShape, hkBoxShape::cleanupLoadedObjecthkBoxShape,
                                      hkBoxShape::getVtablehkBoxShape());

// Statistics collector slots used by calcStatistics (vtable offsets 0x0C and 0x20); see hkCylinderShape.cpp.
// Shape from https://github.com/jj-link/brawl src/havok/fn_802CD950.cpp.
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void addObject(const char* name, int count, const void* object); // 0x0C
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();                                                    // 0x20
};

// TODO: cleanup (shape from jj-link/brawl fn_802CD950)
void hkBoxShape::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->addObject("BoxShape", 1, this);
    ((hkStatisticsCollectorIface*)collector)->unk20();
}

// Ray against the box expanded by the convex radius, with outcodes per side: the ray misses when both ends are
// outside the same face, or starts inside. The entering parameter grows and the leaving one shrinks over the faces
// the ray crosses; the normal is the axis of the last entered face.
hkBool hkBoxShape::castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const {
    HK_TIMER_BEGIN("rcBox");
    {
        hkVector4 radius;
        radius.setAll3(m_radius);
        hkVector4 halfExtents;
        halfExtents.setAdd4(m_halfExtents, radius);
        hkVector4 negHalfExtents;
        negHalfExtents.setNeg4(halfExtents);

        int outFromPos = halfExtents.compareLessEqual4(input.m_from) & 0xe;
        int outFromNeg = input.m_from.compareLessEqual4(negHalfExtents) & 0xe;
        int outToPos = halfExtents.compareLessEqual4(input.m_to) & 0xe;
        int outToNeg = input.m_to.compareLessEqual4(negHalfExtents) & 0xe;
        if ((outFromPos & outToPos) || (outFromNeg & outToNeg) || (outFromPos | outFromNeg) == 0) {
            goto rayMissed;
        }

        static const int axisBits[3] = {8, 4, 2};
        int bits[3] = {axisBits[0], axisBits[1], axisBits[2]};
        hkReal enter = 0.0f;
        hkReal leave = results.m_hitFraction;
        hkReal sign = 1.0f;
        hkVector4 normal;
        int outFrom = outFromPos;
        int outcode = outFromPos | outToPos;
        for (int side = 1; side >= 0; side--) {
            for (int i = 2; i >= 0; i--) {
                int bit = bits[i];
                if (outcode & bit) {
                    hkReal dFrom = sign * input.m_from(i) + negHalfExtents(i);
                    hkReal dTo = sign * input.m_to(i) + negHalfExtents(i);
                    hkReal t = dFrom / (dFrom - dTo);
                    if (outFrom & bit) {
                        if (t >= enter) {
                            enter = t;
                            normal.setZero4();
                            normal(i) = sign;
                        }
                    } else if (leave >= t) {
                        leave = t;
                    }
                    if (leave < enter) {
                        goto rayMissed;
                    }
                }
            }
            sign = -1.0f;
            outFrom = outFromNeg;
            outcode = outFromNeg | outToNeg;
        }

        results.m_normal = normal;
        results.m_hitFraction = enter;
        results.unk18[results.unk38] = 0xFFFFFFFF;
    }
    HK_TIMER_END();
    return true;

rayMissed:
    HK_TIMER_END();
    return false;
}
