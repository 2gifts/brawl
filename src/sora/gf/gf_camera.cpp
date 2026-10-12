#include <gf/gf_camera.h>
#include <sr/sr_common.h>
#include <types.h>

extern gfCameraManager* lbl_8059FFC0;
extern "C" void fn_80019FEC();

gfCameraManager* gfCameraManager::getManager() {
    // MATCH-ONLY: the original branches on the same null test twice; the repeated test keeps the second branch
    if (lbl_8059FFC0 == NULL && lbl_8059FFC0 == NULL && operator new(sizeof(gfCameraManager), Heaps::SystemFW) != NULL) {
        fn_80019FEC();
    }
    return lbl_8059FFC0;
}
