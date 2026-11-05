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

void ScnMdlMod::EnableAllNodeFlags(s8* flags) {
    nw4r::g3d::ResMdl model = m_resMdl;
    u32 count = model.GetResNodeNumEntries();
    for (u32 index = 0; index < count; ++index, ++flags) {
        if (*flags == 0) {
            *flags = 3;
        }
    }
}

void ScnMdlMod::EnableNodeFlag(s8* flags, u32 nodeId) {
    nw4r::g3d::ResMdl model = m_resMdl;
    nw4r::g3d::ResNode node = model.GetResNode(nodeId);
    s8 flag = 2;
    while (node.IsValid()) {
        u32 index = node.GetID();
        if (flags[index] >= 1) {
            break;
        }
        flags[index] = flag;
        flag = 3;
        node = node.GetParentNode();
    }
}

void ScnMdlMod::EnableNodeFlag(s8* flags, u32* nodeIds, int numNodeIds) {
    nw4r::g3d::ResMdl model = m_resMdl;
    u32* currentNodeId = nodeIds;
    u32 maxIndex = 0;
    for (int index = 0; index < numNodeIds; ++index) {
        nw4r::g3d::ResNode node = model.GetResNode(*currentNodeId);
        while (node.IsValid()) {
            u32 nodeIndex = node.GetID();
            if (flags[nodeIndex] >= 1) {
                break;
            }
            if (nodeIndex > maxIndex) {
                maxIndex = nodeIndex;
            }
            flags[nodeIndex] = 3;
            node = node.GetParentNode();
        }
        ++currentNodeId;
    }
    flags[maxIndex] = 2;
}
