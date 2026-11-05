// Havok translation unit hk_symmetric_agent21hk_multi_ray_convex_agent.o (main.dol 0x802BA204-0x802BAB24).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802BA204   352  linearCast   [map: hkSymmetricAgent_21hkMultiRayConvexAgent___linearCast]
//   0x802BA364    72  getPenetrations   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___getPenetrations]
//   0x802BA3AC    72  staticGetPenetrations   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___staticGetPenetrations]
//   0x802BA3F4    72  getClosestPoints   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___getClosestPoints]
//   0x802BA43C    72  staticGetClosestPoints   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___staticGetClosestPoints]
//   0x802BA484   352  staticLinearCast   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___staticLinearCast]
//   0x802BA5E4  1340  processCollision   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___processCollision]
//   0x802BAB20     4  updateShapeCollectionFilter   [map: hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent___updateShapeCollectionFilter]

#include <havok/hkMultiRayConvexAgent.h>
#include <havok/hkCollisionInput.h>

// The linear cast with the bodies swapped: the path is negated and each collector is wrapped in one that turns
// the points back.
void hkSymmetricAgent_21hkMultiRayConvexAgent_::linearCast(const hkCdBody& unk0, const hkCdBody& unk1, hkLinearCastCollisionInput& unk2, void* unk3, void* unk4) {
    const hkLinearCastCollisionInput& input = unk2;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, unk3);
    if (unk4 != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, unk4);
        hkMultiRayConvexAgent::linearCast(unk1, unk0, flippedInput, &flipped, &flippedStart);
    } else {
        hkMultiRayConvexAgent::linearCast(unk1, unk0, flippedInput, &flipped, 0);
    }
}

// Symmetric wrappers: the shapes are swapped and the collector is replaced by a flipping collector on the stack.
void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::getPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkMultiRayConvexAgent::getPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticGetPenetrations(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipBodyCollector flip(unk3);
    hkMultiRayConvexAgent::staticGetPenetrations(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::getClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkMultiRayConvexAgent::getClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticGetClosestPoints(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2, void* unk3) {
    hkSymmetricAgentFlipCollector flip(unk3);
    hkMultiRayConvexAgent::staticGetClosestPoints(unk1, unk0, unk2, &flip);
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::staticLinearCast(const hkCdBody& unk0, const hkCdBody& unk1, hkLinearCastCollisionInput& unk2, void* unk3,
                                                                          void* unk4) {
    const hkLinearCastCollisionInput& input = unk2;
    hkLinearCastCollisionInput flippedInput = input;
    flippedInput.m_path.setNeg4(input.m_path);
    hkSymmetricAgentFlipCastCollector flipped(input.m_path, unk3);
    if (unk4 != 0) {
        hkSymmetricAgentFlipCastCollector flippedStart(input.m_path, unk4);
        hkIterativeLinearCastAgent::staticLinearCast(unk1, unk0, flippedInput, &flipped, &flippedStart);
    } else {
        hkIterativeLinearCastAgent::staticLinearCast(unk1, unk0, flippedInput, &flipped, 0);
    }
}

void hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::updateShapeCollectionFilter(const hkCdBody& unk0, const hkCdBody& unk1, hkCollisionInput& unk2) {}

hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_::~hkSymmetricAgentLinearCast_21hkMultiRayConvexAgent_() {}
hkSymmetricAgent_21hkMultiRayConvexAgent_::~hkSymmetricAgent_21hkMultiRayConvexAgent_() {}
