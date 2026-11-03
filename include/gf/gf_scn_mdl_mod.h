#pragma once

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

void CalcWorldPartialScale(nw4r::math::MTX34* matrices, const u8* byteCode,
                          nw4r::g3d::ResMdl model, const nw4r::math::VEC3* scales);
