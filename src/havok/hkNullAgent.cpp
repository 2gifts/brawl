// Havok translation unit hkNullAgent.o (main.dol 0x802C0590-0x802C068C).
// Functions in address order:
//   0x802C0590    32  __ct
//   0x802C05B0     4  staticGetClosestPoints
//   0x802C05B4     4  staticGetPenetrations
//   0x802C05B8     4  staticLinearCast
//   0x802C05BC    12  createNullAgent
//   0x802C05C8    12  getNullAgent
//   0x802C05D4     4  cleanup
//   0x802C05D8     4  processCollision
//   0x802C05DC     4  linearCast
//   0x802C05E0     4  getClosestPoints
//   0x802C05E4     4  getPenetrations
//   0x802C05E8    92  __dt
//   0x802C0644    72  __sinit_\hkNullAgent_cpp

#include <havok/hkNullAgent.h>

#pragma push
#pragma auto_inline off
hkNullAgent::hkNullAgent() {}
#pragma pop

static hkNullAgent s_nullAgent;

void hkNullAgent::staticGetClosestPoints() {}
void hkNullAgent::staticGetPenetrations() {}
void hkNullAgent::staticLinearCast() {}

hkNullAgent* hkNullAgent::createNullAgent() {
    return &s_nullAgent;
}

hkNullAgent* hkNullAgent::getNullAgent() {
    return &s_nullAgent;
}

void hkNullAgent::cleanup() {}
void hkNullAgent::processCollision(const hkCdBody& a, const hkCdBody& b, hkProcessCollisionInput& c, void* d) {}
void hkNullAgent::linearCast(const hkCdBody& a, const hkCdBody& b, hkLinearCastCollisionInput& c, void* target, void* d) {}
void hkNullAgent::getClosestPoints(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {}
void hkNullAgent::getPenetrations(const hkCdBody& a, const hkCdBody& b, hkCollisionInput& c, void* target) {}

hkNullAgent::~hkNullAgent() {}
