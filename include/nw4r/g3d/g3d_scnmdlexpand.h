#pragma once

// A group of scene objects that are attached to the nodes of a model (as in the nw4r library: ScnMdlExpand derives from
// ScnGroup). Only the members that the game calls are declared.

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <types.h>

struct MEMAllocator;

namespace nw4r {
namespace g3d {
class ScnMdlExpand : public ScnGroup {
public:
    // Adds an object at the end of the group, attached to a node of the model.
    bool PushBack(ScnObj* obj, const char* nodeName);
    // Makes a group for at most maxChildren objects on the model (size: where the size of the group goes, or NULL).
    static ScnMdlExpand* Construct(MEMAllocator* allocator, u32* size, int maxChildren, ScnMdl* model);

    ScnMdl* m_scnMdl;   // 0xE8 the model that the objects are attached to
    u32* m_nodeIDs;     // 0xEC the node of each object
};
static_assert(sizeof(ScnMdlExpand) == 0xF0, "Class is wrong size!");
}
}
