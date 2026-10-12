#include <types.h>
#include <cm/cm_controller_anm.h>
#include <nw4r/g3d/g3d_anmscn.h>

struct cmAnimationControllerAnm {
    char unk0[0xC];
    nw4r::g3d::G3dObj* unkC;
    nw4r::g3d::G3dObj* unk10;
};

void cmAnimationController::releaseScnAnmRes() {
    cmAnimationControllerAnm* self = (cmAnimationControllerAnm*) this;
    if (self->unk10 != NULL) {
        self->unk10->Destroy();
    }
    if (self->unkC != NULL) {
        self->unkC->Destroy();
    }
    self->unkC = NULL;
    self->unk10 = NULL;
}
