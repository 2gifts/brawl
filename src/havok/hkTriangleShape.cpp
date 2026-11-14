// Havok translation unit hkTriangleShape.o (main.dol 0x802D650C-0x802D7260).
// Functions in address order:
//   0x802D650C    32  finishLoadedObjecthkTriangleShape   [map: hkTriangleShape__finishLoadedObjecthkTriangleShape]
//   0x802D652C    20  cleanupLoadedObjecthkTriangleShape   [map: hkTriangleShape__cleanupLoadedObjecthkTriangleShape]
//   0x802D6540    92  __dt   [map: hkTriangleShape____dt]
//   0x802D659C    60  getVtablehkTriangleShape   [map: hkTriangleShape__getVtablehkTriangleShape]
//   0x802D65D8     8  getType   [map: hkTriangleShape__getType]
//   0x802D65E0   216  getSupportingVertex   [map: hkTriangleShape__getSupportingVertex]
//   0x802D66B8   288  convertVertexIdsToVertices   [map: hkTriangleShape__convertVertexIdsToVertices]
//   0x802D67D8    36  getFirstVertex   [map: hkTriangleShape__getFirstVertex]
//   0x802D67FC     8  getNumVertices   [map: hkTriangleShape__getNumVertices]
//   0x802D6804   708  getAabb   [map: hkTriangleShape__getAabb]
//   0x802D6AC8    20  getCollisionSpheresInfo   [map: hkTriangleShape__getCollisionSpheresInfo]
//   0x802D6ADC   128  getCollisionSpheres   [map: hkTriangleShape__getCollisionSpheres]
//   0x802D6B5C  1612  castRay   [map: hkTriangleShape__castRay]
//   0x802D71A8   100  calcStatistics   [map: hkTriangleShape__calcStatistics]
//   0x802D720C    84  __sinit_\hkTriangleShape_cpp   [map: hkTriangleShapecpp____sinit_]

#include <havok/hkTriangleShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>
#include <havok/hkShapeRayCastInput.h>
#include <havok/hkMonitorStream.h>

void hkTriangleShape::finishLoadedObjecthkTriangleShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkTriangleShape(flag);
}

void hkTriangleShape::cleanupLoadedObjecthkTriangleShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkTriangleShape::~hkTriangleShape() {
}

#pragma push
#pragma auto_inline off
const void* hkTriangleShape::getVtablehkTriangleShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x40] __attribute__((aligned(16)));
    hkTriangleShape* p = ::new (buf) hkTriangleShape(flag);
    return *(const void**)p;
}
#pragma pop

int hkTriangleShape::getType() const {
    return HK_SHAPE_TRIANGLE;
}

#pragma scheduling off
void hkTriangleShape::getFirstVertex(hkVector4& out) const {
    out.x = m_vertexA.x;
    out.y = m_vertexA.y;
    out.z = m_vertexA.z;
    out.w = m_vertexA.w;
}
#pragma scheduling reset

int hkTriangleShape::getNumVertices() const {
    return 3;
}

void hkTriangleShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 3;
    out->m_flag = 1;
}

// Ray against the triangle's plane, then the hit point against the three edges (each edge with the hit point must
// wind the same way as the triangle, up to a small tolerance scaled by the normal's length). The reported normal
// faces the ray's start.
hkBool hkTriangleShape::castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const {
    HK_TIMER_BEGIN("rcTriangle");
    hkVector4 edge1;
    edge1.setSub4(m_vertexB, m_vertexA);
    hkVector4 edge2;
    edge2.setSub4(m_vertexC, m_vertexA);
    hkVector4 normal;
    normal.setCross(edge1, edge2);
    hkReal planeDist = m_vertexA.dot3(normal);
    hkReal distFrom = normal.dot3(input.m_from) - planeDist;
    hkReal distTo = normal.dot3(input.m_to) - planeDist;
    if (distFrom * distTo >= 0.0f) {
        // both ends on one side; a ray starting in the plane may still leave it
        if (0.0f != distFrom || 0.0f == distTo) {
            HK_TIMER_END();
            return false;
        }
    }
    {
        hkReal t = distFrom / (distFrom - distTo);
        if (t < results.m_hitFraction) {
            hkVector4 hit;
            hit.setInterpolate4(input.m_from, input.m_to, t);
            hkReal tolerance = normal.lengthSquared3() * -0.0001f;
            hkVector4 toA;
            toA.setSub4(m_vertexA, hit);
            hkVector4 toB;
            toB.setSub4(m_vertexB, hit);
            hkVector4 sideAB;
            sideAB.setCross(toA, toB);
            if (sideAB.dot3(normal) >= tolerance) {
                hkVector4 toC;
                toC.setSub4(m_vertexC, hit);
                hkVector4 sideBC;
                sideBC.setCross(toB, toC);
                if (sideBC.dot3(normal) >= tolerance) {
                    hkVector4 sideCA;
                    sideCA.setCross(toC, toA);
                    if (sideCA.dot3(normal) >= tolerance) {
                        results.m_hitFraction = t;
                        if (distFrom > 0.0f) {
                            results.m_normal = normal;
                        } else {
                            results.m_normal.setNeg4(normal);
                        }
                        results.m_normal.normalize3();
                        results.unk18[results.unk38] = 0xFFFFFFFF;
                        HK_TIMER_END();
                        return true;
                    }
                }
            }
        }
    }
    HK_TIMER_END();
    return false;
}

// Not yet decompiled in this unit:
//   0x802D65E0   216  getSupportingVertex   [map: hkTriangleShape__getSupportingVertex]
//   0x802D66B8   288  convertVertexIdsToVertices   [map: hkTriangleShape__convertVertexIdsToVertices]
//   0x802D6804   708  getAabb   [map: hkTriangleShape__getAabb]
//   0x802D6ADC   128  getCollisionSpheres   [map: hkTriangleShape__getCollisionSpheres]

static hkStaticTypeInfo hkTriangleShapeTypeInfo("hkTriangleShape", hkTriangleShape::finishLoadedObjecthkTriangleShape, hkTriangleShape::cleanupLoadedObjecthkTriangleShape,
                                      hkTriangleShape::getVtablehkTriangleShape());

// Statistics collector slots used by calcStatistics (vtable offsets 0x0C and 0x20); see hkCylinderShape.cpp.
// Shape from https://github.com/jj-link/brawl src/havok/fn_802D71A8.cpp.
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void addObject(const char* name, int count, const void* object); // 0x0C
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();                                                    // 0x20
};

// TODO: cleanup (shape from jj-link/brawl fn_802D71A8)
void hkTriangleShape::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->addObject("TriangleShp", 1, this);
    ((hkStatisticsCollectorIface*)collector)->unk20();
}

// fn_802D6ADC: body from https://github.com/jj-link/brawl src/havok/fn_802D6ADC.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802D6ADC {
// TODO: cleanup (shape from jj-link/brawl fn_802D6ADC)
extern "C" void* fn_802D6ADC(void* src, void* dst)
{
    float* in = (float*)src;
    float* out = (float*)dst;
    out[0] = in[4];
    out[1] = in[5];
    out[2] = in[6];
    out[3] = in[7];
    out[3] = in[3];
    out[4] = in[8];
    out[5] = in[9];
    out[6] = in[10];
    out[7] = in[11];
    out[7] = in[3];
    out[8] = in[12];
    out[9] = in[13];
    out[10] = in[14];
    out[11] = in[15];
    out[11] = in[3];
    return dst;
}
} // namespace jj_fn_802D6ADC
