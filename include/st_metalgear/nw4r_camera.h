#pragma once

// Local shadow: BrawlHeaders does not know nw4r::g3d::Camera / ScnRoot::GetCamera, which the stage uses to aim the camera of
// the scene at the search light (main.dol: GetCamera, SetPosture).

#include <mt/mt_vector.h>
#include <types.h>
#include <nw4r/g3d/g3d_camera.h>

namespace nw4r {
namespace g3d {

class ScnRoot {
public:
    Camera GetCamera(int index);
};

} // namespace g3d
} // namespace nw4r
