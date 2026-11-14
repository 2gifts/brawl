// Havok translation unit hkCylinderShape.o (main.dol 0x802D2628-0x802D4688).
// Functions in address order:
//   0x802D2628    52  finishLoadedObjecthkCylinderShape   [map: hkCylinderShape__finishLoadedObjecthkCylinderShape]
//   0x802D265C    20  cleanupLoadedObjecthkCylinderShape   [map: hkCylinderShape__cleanupLoadedObjecthkCylinderShape]
//   0x802D2670    92  __dt   [map: hkCylinderShape____dt]
//   0x802D26CC    72  getVtablehkCylinderShape   [map: hkCylinderShape__getVtablehkCylinderShape]
//   0x802D2714   132  minValueRoundedUpTo1   [map: hkCylinderShape__minValueRoundedUpTo1]
//   0x802D2798     4  assertRoundUpThreshold   [map: hkCylinderShape__assertRoundUpThreshold]
//   0x802D279C    96  __ct   [map: hkCylinderShape____ct]
//   0x802D27FC     8  getCylinderRadius   [map: hkCylinderShape__getCylinderRadius]
//   0x802D2804  1080  getSupportingVertex   [map: hkCylinderShape__getSupportingVertex]
//   0x802D2C3C   740  convertVertexIdsToVertices   [map: hkCylinderShape__convertVertexIdsToVertices]
//   0x802D2F20    36  getFirstVertex   [map: hkCylinderShape__getFirstVertex]
//   0x802D2F44     8  getNumVertices   [map: hkCylinderShape__getNumVertices]
//   0x802D2F4C    20  getCollisionSpheresInfo   [map: hkCylinderShape__getCollisionSpheresInfo]
//   0x802D2F60  1304  getCollisionSpheres   [map: hkCylinderShape__getCollisionSpheres]
//   0x802D3478  1720  getAabb   [map: hkCylinderShape__getAabb]
//   0x802D45C8     8  getType   [map: hkCylinderShape__getType]
//   0x802D45D0   100  calcStatistics   [map: hkCylinderShape__calcStatistics]

#include <havok/hkCylinderShape.h>
#include <havok/hkRegistry.h>
#include <havok/hkShapeType.h>
#include <havok/hkMath.h>
#include <havok/hkShapeRayCastInput.h>
#include <havok/hkMonitorStream.h>
#include <havok/hkCapsuleShape.h>
#include <havok/hkCdPoint.h>

hkReal hkCylinderShape::s_intRoundUpThreshold = -1.0f;

void hkCylinderShape::finishLoadedObjecthkCylinderShape(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkCylinderShape(flag);
}

void hkCylinderShape::cleanupLoadedObjecthkCylinderShape(void* p) {
    ((hkBaseObject*)p)->~hkBaseObject();
}

hkCylinderShape::~hkCylinderShape() {
}

#pragma push
#pragma auto_inline off
const void* hkCylinderShape::getVtablehkCylinderShape() {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    char buf[0x60] __attribute__((aligned(16)));
    ::new (buf) hkCylinderShape(flag);
    return *(const void**)buf;
}
#pragma pop

// BYTES: not an inline candidate (the constructor calls it).
#pragma push
#pragma auto_inline off
hkReal hkCylinderShape::minValueRoundedUpTo1() {
    for (hkReal x = 0.0f; x < 1.1f; x += 0.01f) {
        if (hkMath::hkFloatToInt(x)) {
            return x;
        }
    }
    return 1.0f;
}
#pragma pop

void hkCylinderShape::assertRoundUpThreshold() {
}

// BYTES: called out of line by the loaded-object helpers above.
#pragma push
#pragma auto_inline off
hkCylinderShape::hkCylinderShape(hkFinishLoadedObjectFlag flag) {
    if (s_intRoundUpThreshold < 0.0f) {
        s_intRoundUpThreshold = 1.0f - minValueRoundedUpTo1();
    }
}
#pragma pop

hkReal hkCylinderShape::getCylinderRadius() const {
    return m_cylRadius;
}

#pragma scheduling off
void hkCylinderShape::getFirstVertex(hkVector4& out) const {
    out = m_vertexB;
}
#pragma scheduling reset

int hkCylinderShape::getNumVertices() const {
    return -1;
}

void hkCylinderShape::getCollisionSpheresInfo(hkCollisionSpheresInfo* out) const {
    out->m_numSpheres = 18;
    out->m_flag = 1;
}

int hkCylinderShape::getType() const {
    return HK_SHAPE_CYLINDER;
}

// BYTES: the ray code is compiled with fused multiply-add contraction.
#pragma fp_contract on

// The cylinder is the inner segment (the axis ends pulled in by the convex radius) grown by the cylinder radius
// plus the convex radius. A ray starting inside misses; otherwise the infinite cylinder is tested first (a hit
// between the end planes is a body hit) and the end caps after it.
hkBool hkCylinderShape::castRay(const hkShapeRayCastInput& input, hkShapeRayCastOutput& results) const {
    HK_TIMER_BEGIN("rcCylinder");
    {
        hkVector4 offset;
        offset.setCross(m_perpendicular2, m_perpendicular1);
        offset.setMul4(m_radius, offset);
        hkVector4 start;
        start.setAdd4(m_vertexA, offset);
        hkVector4 end;
        end.setSub4(m_vertexB, offset);
        hkReal totalRadius = m_radius + m_cylRadius;

        {
            hkVector4 closest;
            hkCapsuleShape::closestPointLineSeg(input.m_from, start, end, closest);
            hkVector4 fromToAxis;
            fromToAxis.setSub4(input.m_from, closest);
            if (fromToAxis.length3() < totalRadius) {
                // within the radius: inside unless the start is beyond one of the end planes
                hkVector4 axis;
                axis.setSub4(end, start);
                hkVector4 fromStart;
                fromStart.setSub4(input.m_from, start);
                if (axis.dot3(fromStart) > 0.0f) {
                    hkVector4 fromEnd;
                    fromEnd.setSub4(input.m_from, end);
                    if (axis.dot3(fromEnd) < 0.0f) {
                        goto rayMissed;
                    }
                }
            }
        }

        hkVector4 rayDir;
        rayDir.setSub4(input.m_to, input.m_from);
        hkVector4 axis;
        axis.setSub4(end, start);
        hkReal distSquared = HK_REAL_MAX;
        hkReal rayT;
        hkReal axisT;
        hkVector4 rayPoint;
        hkVector4 axisPoint;
        hkCapsuleShape::closestInfLineSegInfLineSeg(input.m_from, rayDir, start, axis, distSquared, rayT, axisT,
                                                    rayPoint, axisPoint);
        if (distSquared > totalRadius * totalRadius) {
            goto rayMissed;
        }
        hkVector4 axisDir = axis;
        if (axisDir.lengthSquared3() <= 1.1920929e-07f) {
            goto rayMissed;
        }
        hkReal axisLength = axisDir.length3();
        axisDir.setMul4(1.0f / axisLength, axisDir);

        // back from the closest approach to where the ray enters the infinite cylinder
        hkVector4 perpendicular;
        perpendicular.setAddMul4(rayDir, axisDir, -rayDir.dot3(axisDir));
        hkReal halfChord = 1.0f / hkVector4_sqrtInverse(totalRadius * totalRadius - distSquared);
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
        hkReal startAlong = start.dot3(axisDir);
        hkReal hitAlong = hit.dot3(axisDir) - startAlong;
        if (t >= 0.0f && hitAlong > 0.0f && hitAlong < axisLength) {
            hkVector4 onAxis;
            onAxis.setInterpolate4(start, end, hitAlong / axisLength);
            hkVector4 normal;
            normal.setSub4(hit, onAxis);
            normal.normalize3();
            results.m_normal = normal;
            results.m_hitFraction = t;
            results.unk18[results.unk38] = 0xFFFFFFFF;
            HK_TIMER_END();
            return true;
        }

        // the end cap on the side of the ray start
        {
            hkReal fromAlong = input.m_from.dot3(axisDir);
            hkVector4 capCenter;
            hkReal side;
            if (fromAlong < startAlong) {
                capCenter = start;
                side = -1.0f;
            } else if (fromAlong > startAlong + axisLength) {
                capCenter = end;
                side = 1.0f;
            } else {
                goto rayMissed;
            }
            hkVector4 toCap;
            toCap.setSub4(capCenter, input.m_from);
            hkReal num = -1.0f * (side * toCap.dot3(axisDir));
            if (num < 0.0f) {
                goto rayMissed;
            }
            hkReal denom = (-1.0f * side) * rayDir.dot3(axisDir);
            if (num >= denom * results.m_hitFraction) {
                goto rayMissed;
            }
            hkReal capT = num / denom;
            hkVector4 capHit;
            capHit.setInterpolate4(input.m_from, input.m_to, capT);
            hkVector4 onCap;
            onCap.setSub4(capHit, capCenter);
            if (onCap.lengthSquared3() > totalRadius * totalRadius) {
                goto rayMissed;
            }
            results.m_normal.setMul4(side, axisDir);
            results.m_hitFraction = capT;
            results.unk18[results.unk38] = 0xFFFFFFFF;
            HK_TIMER_END();
            return true;
        }
    }

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

void hkCylinderShape::calcStatistics(hkStatisticsCollector* collector) const {
    ((hkStatisticsCollectorIface*)collector)->addObject("Cylinder", 1, this);
    ((hkStatisticsCollectorIface*)collector)->unk20();
}

// Not yet decompiled in this unit:
//   0x802D2714   132  minValueRoundedUpTo1   [map: hkCylinderShape__minValueRoundedUpTo1]
//   0x802D2798     4  assertRoundUpThreshold   [map: hkCylinderShape__assertRoundUpThreshold]
//   0x802D279C    96  __ct   [map: hkCylinderShape____ct]
//   0x802D2804  1080  getSupportingVertex   [map: hkCylinderShape__getSupportingVertex]
//   0x802D2C3C   740  convertVertexIdsToVertices   [map: hkCylinderShape__convertVertexIdsToVertices]
//   0x802D2F60  1304  getCollisionSpheres   [map: hkCylinderShape__getCollisionSpheres]
//   0x802D3478  1720  getAabb   [map: hkCylinderShape__getAabb]
//   0x802D45D0   100  calcStatistics   [map: hkCylinderShape__calcStatistics]

static hkStaticTypeInfo hkCylinderShapeTypeInfo("hkCylinderShape", hkCylinderShape::finishLoadedObjecthkCylinderShape, hkCylinderShape::cleanupLoadedObjecthkCylinderShape,
                                      hkCylinderShape::getVtablehkCylinderShape());
