// Havok translation unit hkHeightFieldAgent.o (main.dol 0x802B4524-0x802B7934).
// Functions in address order:
//   0x802B4524   592  __ct
//   0x802B4800   204  registerAgent
//   0x802B48CC   124  createHeightFieldBAgent
//   0x802B4948   132  createHeightFieldAAgent
//   0x802B49CC   148  __dt
//   0x802B4A60   168  cleanup
//   0x802B4B08    20  getPenetrations
//   0x802B4B1C  1904  staticGetPenetrations
//   0x802B528C    20  getClosestPoints
//   0x802B52A0  2224  staticGetClosestPoints
//   0x802B5B50  1296  staticLinearCast
//   0x802B60A0    24  linearCast
//   0x802B60B8   344  addRayHit
//   0x802B6210  2220  processCollision
//   0x802B6ABC   352  linearCast
//   0x802B6C1C    72  getPenetrations
//   0x802B6C64    72  staticGetPenetrations
//   0x802B6CAC    72  getClosestPoints
//   0x802B6CF4    72  staticGetClosestPoints
//   0x802B6D3C   352  staticLinearCast
//   0x802B6E9C  2552  processCollision
//   0x802B7894     4  updateShapeCollectionFilter
//   0x802B7898   156  __dt

#include <havok/hkHeightFieldAgent.h>
#include <havok/hkCollisionInput.h>
#include <havok/hkSymmetricAgentFlipCollectors.h>
#include <havok/hkBvTreeAgent.h>

#include <havok/hkSphereRepShape.h>

// HYPOTHESIS: the name; the only user of this writable .sdata word is the constructor below.
static u16 s_invalidContactPointId = 0xFFFF;

#pragma push
#pragma auto_inline off
hkHeightFieldAgent::hkHeightFieldAgent(const hkCdBody& bodyA, const hkCdBody& bodyB, hkCollisionInput& input,
                                       hkContactMgr* contactMgr)
    : hkCollisionAgent((int)contactMgr) {
    if (contactMgr != 0) {
        hkCollisionSpheresInfo info;
        ((const hkSphereRepShape*)bodyA.m_shape)->getCollisionSpheresInfo(&info);
        m_contactPointId.setSize(info.m_numSpheres, s_invalidContactPointId);
    }
}
#pragma pop

#include <havok/hkCollisionDispatcher.h>

#include <havok/hkCdPoint.h>
#include <havok/hkShapeRayCastCollector.h>
#include <havok/hkRotation.h>

// Turns the hits of the swept convex body's ray against the height field into collision points for a cd point
// collector. HYPOTHESIS: the member names (from their use in addRayHit).
struct hkHeightFieldRayForwardingCollector : hkShapeRayCastCollector {
    hkVector4 m_currentFrom;         // 0x10
    hkReal m_currentRadius;          // 0x20
    hkVector4 m_path;                // 0x30
    const hkCdBody* m_convexBody;    // 0x40
    hkCdPointCollector* m_collector; // 0x44

    virtual void addRayHit(const hkCdBody& body, const hkShapeRayCastOutput& hit);
};

// The hit point on the ray, moved back by the convex radius along the hit normal (rotated into world space); the
// normal's w is the hit fraction. The early-out fraction follows the wrapped collector's.
void hkHeightFieldRayForwardingCollector::addRayHit(const hkCdBody& body, const hkShapeRayCastOutput& hit) {
    hkCdPoint point;
    point.m_cdBodyA = m_convexBody;
    point.m_cdBodyB = &body;
    point.m_contact.m_position.setAddMul4(m_currentFrom, m_path, hit.m_hitFraction);
    point.m_contact.m_normal._setRotatedDir(body.getTransform().getRotation(), hit.m_normal);
    point.m_contact.m_position.setAddMul4(point.m_contact.m_position, point.m_contact.m_normal, -m_currentRadius);
    point.m_contact.m_normal.w = hit.m_hitFraction;
    m_collector->addCdPoint(&point);
    m_earlyOutHitFraction = m_collector->m_earlyOutDistance < m_earlyOutHitFraction ? m_collector->m_earlyOutDistance
                                                                                     : m_earlyOutHitFraction;
}

// Height field x convex pairs in both orders (shape types 18 and 21).
void hkHeightFieldAgent::registerAgent(hkCollisionDispatcher* dispatcher) {
    hkAgentFuncs af;
    af.create = (hkAgentFunc)&hkHeightFieldAgent::createHeightFieldAAgent;
    af.staticGetPenetrations = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetPenetrations;
    af.staticGetClosestPoints = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetClosestPoints;
    af.staticLinearCast = (hkAgentFunc)hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticLinearCast;
    af.symmetricA = true;
    af.symmetricB = true;
    dispatcher->registerCollisionAgent(&af, 0x12, 0x15);
    hkAgentFuncs bf;
    bf.create = (hkAgentFunc)&hkHeightFieldAgent::createHeightFieldBAgent;
    bf.staticGetPenetrations = (hkAgentFunc)hkHeightFieldAgent::staticGetPenetrations;
    bf.staticGetClosestPoints = (hkAgentFunc)hkHeightFieldAgent::staticGetClosestPoints;
    bf.staticLinearCast = (hkAgentFunc)hkHeightFieldAgent::staticLinearCast;
    bf.symmetricA = false;
    bf.symmetricB = true;
    dispatcher->registerCollisionAgent(&bf, 0x15, 0x12);
}

// TODO: cleanup (shape from jj-link/brawl fn_802B48CC)
hkCollisionAgent* hkHeightFieldAgent::createHeightFieldBAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                              hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkHeightFieldAgent(bodyA, bodyB, input, contactMgr);
}

// TODO: cleanup (shape from jj-link/brawl fn_802B4948)
hkCollisionAgent* hkHeightFieldAgent::createHeightFieldAAgent(const hkCdBody& bodyA, const hkCdBody& bodyB,
                                                              hkCollisionInput& input, hkContactMgr* contactMgr) {
    return new hkSymmetricAgent_18hkHeightFieldAgent_(bodyB, bodyA, input, contactMgr);
}

#pragma dont_inline on
void hkHeightFieldAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    staticGetPenetrations(a, b, c, target);
}

void hkHeightFieldAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    staticGetClosestPoints(a, b, c, target);
}

void hkHeightFieldAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    staticLinearCast(a, b, c, target, d);
}
#pragma dont_inline reset

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkHeightFieldAgent::getPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipBodyCollector wrap(target);
    hkHeightFieldAgent::staticGetPenetrations(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkHeightFieldAgent::getClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticGetClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {
    hkSymmetricAgentFlipCollector wrap(target);
    hkHeightFieldAgent::staticGetClosestPoints(b, a, c, &wrap);
}

void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::updateShapeCollectionFilter(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c) {}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_18hkHeightFieldAgent_::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkHeightFieldAgent::linearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkHeightFieldAgent::linearCast(b, a, flippedInput, &flipped, 0);
    }
}

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgentLinearCast_18hkHeightFieldAgent::staticLinearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {
    const hkLinearCastCollisionInput& input = c;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, target);
    if (d != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, d);
        hkHeightFieldAgent::staticLinearCast(b, a, flippedInput, &flipped, &flippedStart);
    } else {
        hkHeightFieldAgent::staticLinearCast(b, a, flippedInput, &flipped, 0);
    }
}

// fn_802B4A60: body from https://github.com/jj-link/brawl src/havok/fn_802B4A60.cpp (namespaced; labels as in our symbols).
#include <types.h>
namespace jj_fn_802B4A60 {
class Inner {
public:
    virtual void v0();
    virtual void v1();
    virtual void v2();
    virtual void v3();
    virtual void v4(unsigned int);
    virtual void v5();
    virtual void v6();
};

class Outer {
public:
    virtual void v0(int);
    virtual void v1();
    virtual void v2();
    char pad[4];
    Inner* m_inner;
    unsigned short* m_table;
    int m_count;
};

// TODO: cleanup (shape from jj-link/brawl fn_802B4A60)
extern "C" void fn_802B4A60(Outer* self) {
    if (self->m_inner != 0) {
        int off;
        int i;
        i = 0;
        off = 0;
        while (i < self->m_count) {
            unsigned int id = *(unsigned short*)((char*)self->m_table + off);
            if (id != 0xFFFF) {
                self->m_inner->v4(id);
            }
            off += 2;
            i++;
        }
    }
    if (self != 0) {
        self->v0(1);
    }
}
} // namespace jj_fn_802B4A60

