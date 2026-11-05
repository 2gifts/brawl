#pragma once

#include <havok/hkCollisionAgent.h>

// Base of the convex pair agents (box-box, capsule, sphere, triangle, GSK, multi-ray convex, multi-sphere triangle):
// their vtables share one linear cast slot, hkIterativeLinearCastAgent::linearCast (0x802B7A14, in hkCollisionAgent.o;
// staticLinearCast at 0x802B7FD0), and their constructors' exception actions destroy this base through its own weak
// destructor copy (0x802A0E20), not hkCollisionAgent's (0x802A0DC4, the one in hkCollisionAgent's vtable).
// No vtable of this class is in the image: its vptr store is always dead.
// BYTES: the extra level keeps hkCollisionAgent's vptr store in the constructors that run in the no-dead-store mode
// (hkGskBaseAgent).
struct hkIterativeLinearCastAgent : hkCollisionAgent {
    hkIterativeLinearCastAgent() {}
    explicit hkIterativeLinearCastAgent(int unk8Value) : hkCollisionAgent(unk8Value) {}
    virtual ~hkIterativeLinearCastAgent() {}
};
