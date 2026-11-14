// Havok translation unit hkConvexVerticesShape.o (main.dol 0x802D1588-0x802D256C).
// Functions in address order:
//   0x802D1588    32  finishLoadedObjecthkConvexVerticesShape   [map: hkConvexVerticesShape__finishLoadedObjecthkConvexVerticesShape]
//   0x802D15A8    20  cleanupLoadedObjecthkConvexVerticesShape   [map: hkConvexVerticesShape__cleanupLoadedObjecthkConvexVerticesShape]
//   0x802D15BC   196  __dt   [map: hkConvexVerticesShape____dt]
//   0x802D1680    60  getVtablehkConvexVerticesShape   [map: hkConvexVerticesShape__getVtablehkConvexVerticesShape]
//   0x802D16BC     8  getType   [map: hkConvexVerticesShape__getType]
//   0x802D16C4    48  getFirstVertex   [map: hkConvexVerticesShape__getFirstVertex]
//   0x802D16F4    20  getCollisionSpheresInfo   [map: hkConvexVerticesShape__getCollisionSpheresInfo]
//   0x802D1708   452  getCollisionSpheres   [map: hkConvexVerticesShape__getCollisionSpheres]
//   0x802D18CC  1004  getAabb   [map: hkConvexVerticesShape__getAabb]
//   0x802D1CB8   800  getSupportingVertex   [map: hkConvexVerticesShape__getSupportingVertex]
//   0x802D1FD8   324  convertVertexIdsToVertices   [map: hkConvexVerticesShape__convertVertexIdsToVertices]
//   0x802D211C   776  castRay   [map: hkConvexVerticesShape__castRay]
//   0x802D2424   244  calcStatistics   [map: hkConvexVerticesShape__calcStatistics]
//   0x802D2518    84  __sinit_\hkConvexVerticesShape_cpp   [map: hkConvexVerticesShapecpp____sinit_]

#include <havok/hkConvexVerticesShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>
#include <havok/hkShapeRayCastInput.h>
#include <havok/hkMonitorStream.h>

void hkConvexVerticesShape::finishLoadedObjecthkConvexVerticesShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkConvexVerticesShape(flag);
}

void hkConvexVerticesShape::cleanupLoadedObjecthkConvexVerticesShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkConvexVerticesShape::~hkConvexVerticesShape() {
}

#pragma push
#pragma auto_inline off
const void* hkConvexVerticesShape::getVtablehkConvexVerticesShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x50] __attribute__((aligned(16)));
    hkConvexVerticesShape* p = ::new (buf) hkConvexVerticesShape(flag);
    return *(const void**)p;
}
#pragma pop

int hkConvexVerticesShape::getType() const {
    return HK_SHAPE_CONVEX_VERTICES;
}

void hkConvexVerticesShape::getFirstVertex(hkVector4& out) const {
    const hkConvexVerticesShapeFourVectors* v = (const hkConvexVerticesShapeFourVectors*)m_rotatedVertices.m_data;
    out.x = v->m_x.x;
    out.y = v->m_y.x;
    out.z = v->m_z.x;
    out.w = 1.0f;
}

void hkConvexVerticesShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = m_numVertices;
    out->m_flag = 1;
}

// Ray against the convex hull through its plane equations: the parameter where the ray enters the last front
// plane is the hit, unless the ray leaves a back plane first.
hkBool hkConvexVerticesShape::castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const {
    HK_TIMER_BEGIN("rcConvexVert");
    {
        hkReal enter = -1.0f;
        hkReal leave = results.m_hitFraction;
        hkVector4 normal;
        for (int i = m_planeEquations.getSize() - 1; i >= 0; i--) {
            const hkVector4& plane = m_planeEquations[i];
            hkReal distFrom = plane.dot3(input.m_from) + plane.w;
            hkReal distTo = plane.dot3(input.m_to) + plane.w;
            if (distFrom >= 0.0f) {
                if (distTo >= 0.0f) {
                    goto rayMissed;
                }
                hkReal t = distFrom / (distFrom - distTo);
                if (enter <= t) {
                    enter = t;
                    normal = plane;
                }
            } else {
                if (distTo < 0.0f) {
                    continue;
                }
                hkReal t = distFrom / (distFrom - distTo);
                if (leave >= t) {
                    leave = t;
                }
            }
            if (leave < enter) {
                goto rayMissed;
            }
        }
        if (enter < 0.0f) {
            goto rayMissed;
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

// Statistics collector slots used here (vtable offsets 0x0C, 0x10 and 0x20).
struct hkStatisticsCollectorIface {
    virtual void unk00();
    virtual void addObject(const char* name, int count, const void* object);                     // 0x0C
    virtual void addArrayBytes(const char* name, int type, const void* data, int used, int allocated); // 0x10
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void endObject();                                                                    // 0x20

    // Arrays that own their storage report their used and allocated bytes.
    template <typename T>
    void addArray(const char* name, int type, const hkArray<T>& array) {
        if ((array.m_capacityAndFlags & hkArrayBase::DONT_DEALLOCATE_FLAG) == 0) {
            addArrayBytes(name, type, array.m_data, array.m_size * sizeof(T), array.getCapacity() * sizeof(T));
        }
    }
};

void hkConvexVerticesShape::calcStatistics(hkStatisticsCollector* collector) const {
    hkStatisticsCollectorIface* c = (hkStatisticsCollectorIface*)collector;
    c->addObject("CvxVerts", 1, this);
    c->addArray("Verts", 1, m_rotatedVertices);
    c->addArray("Verts", 1, m_planeEquations);
    c->endObject();
}

// Not yet decompiled in this unit:
//   0x802D1708   452  getCollisionSpheres   [map: hkConvexVerticesShape__getCollisionSpheres]
//   0x802D18CC  1004  getAabb   [map: hkConvexVerticesShape__getAabb]
//   0x802D1CB8   800  getSupportingVertex   [map: hkConvexVerticesShape__getSupportingVertex]
//   0x802D1FD8   324  convertVertexIdsToVertices   [map: hkConvexVerticesShape__convertVertexIdsToVertices]

static hkStaticTypeInfo hkConvexVerticesShapeTypeInfo("hkConvexVerticesShape", hkConvexVerticesShape::finishLoadedObjecthkConvexVerticesShape, hkConvexVerticesShape::cleanupLoadedObjecthkConvexVerticesShape,
                                      hkConvexVerticesShape::getVtablehkConvexVerticesShape());

