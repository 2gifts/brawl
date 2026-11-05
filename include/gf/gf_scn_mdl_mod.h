#pragma once

#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

namespace nw4r {
namespace g3d {
class ResMdl;
}
namespace math {
struct MTX34;
struct VEC3;
}
}

class ScnMdlMod : public nw4r::g3d::ScnMdl {
public:
    // HYPOTHESIS: retain G3dObj's allocator-owned storage policy in this shadow hierarchy.
    static void operator delete(void*) {}
    virtual ~ScnMdlMod();
    void EnableAllNodeFlags(s8* flags);
    void EnableNodeFlag(s8* flags, u32 nodeId);
    void EnableNodeFlag(s8* flags, u32* nodeIds, int numNodeIds);
};

void CalcWorldPartialScale(nw4r::math::MTX34* matrices, const u8* byteCode,
                          nw4r::g3d::ResMdl model, const nw4r::math::VEC3* scales);
