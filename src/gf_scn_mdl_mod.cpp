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

// TODO: clean up, give variables real names
extern "C" void fn_8004755C(void) {}

extern "C" void fn_80047FC8(void) {}

// TODO: clean up, give variables real names
namespace jj_8004810C {
extern "C" {

extern unsigned char lbl_8040ABD8[];

unsigned char* fn_8004810C(void) {
    return lbl_8040ABD8;
}

}
}

namespace jj_80047C0C {
// TODO: clean up, give variables real names
extern "C" int fn_80047C0C(void* self, int off)
{
    return (signed char)((unsigned char*)self)[off] >= 2;
}
}

namespace jj_800480DC {
class Inner {
public:
    virtual void f0();
    virtual void f1();
    virtual void f2();
    virtual char* f3();
};

// TODO: clean up, give variables real names
extern "C" char* fn_800480DC(Inner* self) {
    return self->f3() + 4;
}
}

namespace jj_8004805C {
#pragma optimization_level 1

extern "C" {
extern unsigned char lbl_8040ABD8[0x10];
extern unsigned char lbl_8040ABC0[0x18];
extern unsigned char lbl_8040AB90[0x10];
extern unsigned char lbl_8040AB80[0x10];
extern unsigned char lbl_8040AA68[0x10];

// TODO: clean up, give variables real names
int fn_8004805C(void* unused, unsigned int* p) {
    if (*p == (unsigned int)lbl_8040ABD8) return 1;
    unsigned int v = *p;
    if (v == (unsigned int)lbl_8040ABC0) return 1;
    if (v == (unsigned int)lbl_8040AB90) return 1;
    if (v == (unsigned int)lbl_8040AB80) return 1;
    return (unsigned int)__cntlzw((unsigned int)lbl_8040AA68 - v) >> 5;
}
}
}

