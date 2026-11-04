#include <gf/gf_scn_mdl_mod.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/math/math_types.h>
#include <types.h>

ScnMdlMod::~ScnMdlMod() {}

void CalcWorldPartialScale(nw4r::math::MTX34* matrices, const u8* byteCode,
                          nw4r::g3d::ResMdl model, const nw4r::math::VEC3* scales) {
    if (byteCode || (byteCode = model.GetResByteCode("NodeTree"))) {
        while (*byteCode != 1) {
            if (*byteCode == 2) {
                u32 nodeIndex = (byteCode[1] << 8) + byteCode[2];
                if (1.0f != scales[nodeIndex].x ||
                    1.0f != scales[nodeIndex].y ||
                    1.0f != scales[nodeIndex].z) {
                    nw4r::g3d::ResNode node = model.GetResNode(nodeIndex);
                    u32 matrixIndex = node.IsValid() ? node->m_id : 0;
                    nw4r::math::MTX34Scale(&matrices[matrixIndex],
                                         &matrices[matrixIndex], &scales[nodeIndex]);
                }
            }
            byteCode += 5;
        }
    }
}
