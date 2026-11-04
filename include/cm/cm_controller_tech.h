#pragma once

#include <StaticAssert.h>
#include <gf/gf_camera_controller.h>
#include <mt/mt_vector.h>
#include <types.h>

class cmSubject;

class cmTechController : public gfCameraController {
public:
    void* m_target; // HYPOTHESIS: the object start() receives; resetTarget clears it
    u16 unkC;
    u16 unkE;
    u8 unk10;
    bool unk11_80 : 1;
    bool unk11_40 : 1;
    float unk14;
    Vec3f unk18;
    Vec3f unk24;
    Vec3f unk30;
    Vec3f unk3C;
    Vec3f unk48;
    float unk54;

    cmTechController();
    ~cmTechController();
    void resetTarget();
    // HYPOTHESIS: reference qualifiers and float parameter names follow the native register use.
    void start(cmSubject* subject, u16 frame, const Vec2f& rotation, const Vec2f& targetRotation, float zoom, float distance);
    virtual void update(float);
};
static_assert(sizeof(cmTechController) == 0x58, "Class is wrong size!");
