#pragma once

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <sr/sr_common.h>
#include <types.h>

// Native construction and lifecycle calls establish a plain 0x28-byte object.
// HYPOTHESIS: member meanings and create's original reference qualifiers remain
// unresolved; these declarations preserve the observed pointer argument ABI.
// create selects each resource by animationName, or index zero when it is null.
class soAnimObj {
    u32 unk0[10];
public:
    soAnimObj();
    ~soAnimObj();
    void create(const nw4r::g3d::ResFile&, const nw4r::g3d::ResMdl&, Heaps::HeapType, const char* animationName);
    void set(nw4r::g3d::ScnMdl*);
    void remove();
    void destroy();
    void setFrame(float);
};
static_assert(sizeof(soAnimObj) == 0x28, "Animation object extent");
